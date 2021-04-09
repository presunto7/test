#include "DebugMode.hpp"


DebugMode::DebugMode(Parameters *param, Sensoring *sensoring, Motor *motor) : 
m_param(param),
m_sensoring(sensoring),
m_motor(motor),
m_breathing_state(BreathingStateType::IDLE)
{
        do_state_machine[ControlModeType::PRESSURE] = &DebugMode::do_state_machine_pressure;
        do_state_machine[ControlModeType::FLOW] = &DebugMode::do_state_machine_flow;

        m_flow_controller = new VolumeController(
                                                    controller_gains_t
                                                    {
                                                        .kp = VOLUME_KP, 
                                                        .ki = VOLUME_KI, 
                                                        .kd = VOLUME_KD
                                                    }
                                                );
        m_pressure_controller = new PressureController(
                                                        controller_gains_t
                                                        {
                                                            .kp = PRESSURE_KP, 
                                                            .ki = PRESSURE_KI, 
                                                            .kd = PRESSURE_KD
                                                        }
                                                    );
}


DebugMode::~DebugMode()
{
    delete m_flow_controller;
    delete m_pressure_controller;
}


uint8_t DebugMode::setup()
{
    return setup_threading();
}


uint8_t DebugMode::setup_threading()
{
  Serial.println("control timer inicia ");
  breathing_task_handler = xTaskCreateStatic(&DebugMode::breathing_state_machine_task, "bt", 254, this, 2,xStack, &xTaskBuffer) ;
   if( breathing_task_handler != pdPASS)
      return error_t :: OP_MODE_SETUP;
    control_timer_handler = xTimerCreate(
                                        "control_timer_task", 
                                        pdMS_TO_TICKS((1.0/CONTROL_FREQUENCY)*1000), 
                                        pdTRUE, 
                                        this, 
                                        DebugMode::control_timer_task
                                        ); 
                                             Serial.println("control timer done");
    if(control_timer_handler == NULL)
        return error_t :: OP_MODE_SETUP;

    breathing_timer_handler = xTimerCreate(
                                        "breathing_timer_task", 
                                        0,           // !for now it is null. Then each state will configure its own needed time!
                                        pdTRUE, 
                                        this, 
                                        DebugMode::breathing_timer_task
                                        ); 
                                        Serial.println("breathing timer done");
    if(breathing_timer_handler == NULL)
        return error_t :: OP_MODE_SETUP;
    

    return error_t :: NO_ERROR;
}


void DebugMode::start(ControlModeType con_mode)
{
    m_con_mode = con_mode;

    switch (m_con_mode)
    {
        case ControlModeType::FLOW:
            m_controller = m_flow_controller;
            break;

        case ControlModeType::PRESSURE:
            m_controller = m_pressure_controller;
            break;          

        default:
            break;
    }
    xTaskNotifyGive(breathing_task_handler);
}

void DebugMode::stop_breathing_timer()
{
    xTimerStop(breathing_timer_handler, portMAX_DELAY);      // HANDLE SE DER ERRO ??
}


BreathingStateType DebugMode::get_current_state()
{
    return m_breathing_state;
}


void DebugMode::stop()
{
    while( (m_breathing_state != BreathingStateType::STOPPING) && (m_breathing_state != BreathingStateType::IDLE))
    {
        m_breathing_next_state = BreathingStateType::STOPPING;
        xTaskNotifyGive(breathing_task_handler);
    }
}


void DebugMode::start_control(Controller *current_controller)
{
    current_controller->switch_controller(true);
    xTimerReset(control_timer_handler, portMAX_DELAY);      // HANDLE SE DER ERRO ??
}


void DebugMode::stop_control(Controller *current_controller)
{
    current_controller->switch_controller(false);
    xTimerStop(control_timer_handler, portMAX_DELAY);      // HANDLE SE DER ERRO ??
}


void DebugMode::update_start_breathing_timer()
{
    switch (m_breathing_state)
    {
        case BreathingStateType::INSPIRATION:
            xTimerChangePeriod(breathing_timer_handler, pdMS_TO_TICKS((1.0/m_param->get_cycles_time().tin)*1000), portMAX_DELAY);
            break;

        case BreathingStateType::HOLD:
            xTimerChangePeriod(breathing_timer_handler, pdMS_TO_TICKS((1.0/m_param->get_cycles_time().th)*1000), portMAX_DELAY);
            break;

        case BreathingStateType::EXPIRATION:
            xTimerChangePeriod(breathing_timer_handler, pdMS_TO_TICKS((1.0/m_param->get_cycles_time().tex)*1000), portMAX_DELAY);
            break;        
        default:
            break;
    }
    
    xTimerReset(breathing_timer_handler, portMAX_DELAY);
}


void DebugMode::control_timer_task(void *pParameters)
{
    DebugMode* p_debug_mode = reinterpret_cast<DebugMode*>(pParameters); 

    motor_t current_motor_control;
    double read_sensor;

    switch (p_debug_mode->m_con_mode)
    {
        case ControlModeType::FLOW:
            read_sensor = p_debug_mode->m_sensoring->getCurrentFlow();
            break;

        case ControlModeType::PRESSURE:
            read_sensor = p_debug_mode->m_sensoring->getCurrentPressure();
            break;
        default:
            break;
    }
    current_motor_control = p_debug_mode->m_controller->control(read_sensor);
    p_debug_mode->m_motor->actuate_motor(current_motor_control);
}


void DebugMode::breathing_timer_task(void *pParameters)
{
    DebugMode* p_debug_mode = reinterpret_cast<DebugMode*>(pParameters); 

    p_debug_mode->stop_breathing_timer();

    xTaskNotifyGive(p_debug_mode->breathing_task_handler);
}


void DebugMode::breathing_state_machine_task(void *pParameters)
{
    DebugMode* p_debug_mode = reinterpret_cast<DebugMode*>(pParameters); 

    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        p_debug_mode->m_breathing_state = BreathingStateType::INSPIRATION;

        (*p_debug_mode->do_state_machine[p_debug_mode->m_con_mode])(p_debug_mode);   
    }
}


void DebugMode::do_state_machine_flow(DebugMode * p_debug_mode)
{
    
    while(p_debug_mode->m_breathing_state != BreathingStateType::IDLE)
    {
        switch(p_debug_mode->m_breathing_state)
        {
            case BreathingStateType::INSPIRATION:
                /*setup state in here*/
                    p_debug_mode->m_breathing_next_state = BreathingStateType::HOLD;

                    p_debug_mode->m_flow_controller->set_flow_rate_ref(
                                                        p_debug_mode->m_param->get_tidal_volume_ref(), 
                                                        p_debug_mode->m_param->get_cycles_time().period
                                                      );

                    p_debug_mode->start_control(p_debug_mode->m_flow_controller);
                    p_debug_mode->update_start_breathing_timer();
                    
                /*wait state to end here*/
                    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    //remove this if not necessary

                /*do something before state is done*/
                    p_debug_mode->stop_control(p_debug_mode->m_flow_controller);
                    p_debug_mode->m_breathing_state = p_debug_mode->m_breathing_next_state;
                break;

            case BreathingStateType::HOLD:
                /*setup state in here*/
                    p_debug_mode->m_breathing_next_state = BreathingStateType::EXPIRATION;
                    p_debug_mode->update_start_breathing_timer();  
                    p_debug_mode->m_motor->actuate_motor({false,0});
                /*wait state to end here*/
                    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    //remove this if not necessary

                /*do something before state is done*/
                    p_debug_mode->m_breathing_state = p_debug_mode->m_breathing_next_state;
                break;

            case BreathingStateType::EXPIRATION:
                /*setup state in here*/
                    p_debug_mode->m_breathing_next_state = BreathingStateType::INSPIRATION;
                    p_debug_mode->update_start_breathing_timer(); 
                    p_debug_mode->m_pressure_controller->set_pressure_ref(p_debug_mode->m_param->get_peep());
                    p_debug_mode->start_control(p_debug_mode->m_pressure_controller);
                /*wait state to end here*/
                    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    //remove this if not necessary
                /*do something before state is done*/
                    p_debug_mode->stop_control(p_debug_mode->m_pressure_controller);
                    p_debug_mode->m_breathing_state = p_debug_mode->m_breathing_next_state;
                break;

            case BreathingStateType::STOPPING:
                /*setup state in here*/
                    p_debug_mode->m_breathing_next_state = BreathingStateType::IDLE;
                    p_debug_mode->m_motor->actuate_motor({false,0});
                //TODO: homing
                /*wait state to end here*/
                    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    //remove this if not necessary

                /*do something before state is done*/
                    p_debug_mode->m_breathing_state = p_debug_mode->m_breathing_next_state;
                break;

            default:
                break;
        }

    }
    
}


void DebugMode::do_state_machine_pressure(DebugMode * p_debug_mode)
{
   
    while(p_debug_mode->m_breathing_state != BreathingStateType::IDLE)
    {
        switch(p_debug_mode->m_breathing_state)
        {
            case BreathingStateType::INSPIRATION:
                /*setup state in here*/
                    p_debug_mode->m_breathing_next_state = BreathingStateType::HOLD;

                    p_debug_mode->m_pressure_controller->set_pressure_ref(p_debug_mode->m_param->get_pressure_ref());

                    p_debug_mode->start_control(p_debug_mode->m_pressure_controller);
                    p_debug_mode->update_start_breathing_timer();
                    
                /*wait state to end here*/
                    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    //remove this if not necessary
                    p_debug_mode->m_breathing_next_state = p_debug_mode->m_breathing_next_state;
                break;

            case BreathingStateType::HOLD:
                /*setup state in here*/
                    p_debug_mode->m_breathing_next_state = BreathingStateType::EXPIRATION;
                    // TODO: hold motor in current position
                    p_debug_mode->update_start_breathing_timer();  

                /*wait state to end here*/
                    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    //remove this if not necessary
                /*do something before state is done*/
                    p_debug_mode->stop_control(p_debug_mode->m_pressure_controller);
                    p_debug_mode->m_breathing_state = p_debug_mode->m_breathing_next_state;
                break;

            case BreathingStateType::EXPIRATION:
                /*setup state in here*/
                    p_debug_mode->m_breathing_next_state = BreathingStateType::INSPIRATION;
                    // TODO: release motor until PEEP and hold it
                    p_debug_mode->update_start_breathing_timer(); 
                    p_debug_mode->m_pressure_controller->set_pressure_ref(p_debug_mode->m_param->get_peep());
                    p_debug_mode->start_control(p_debug_mode->m_pressure_controller);
                /*wait state to end here*/
                    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    //remove this if not necessary

                /*do something before state is done*/
                    p_debug_mode->stop_control(p_debug_mode->m_pressure_controller);
                    p_debug_mode->m_breathing_state = p_debug_mode->m_breathing_next_state;
                break;

            case BreathingStateType::STOPPING:
            default:
                /*setup state in here*/
                    p_debug_mode->m_breathing_next_state = BreathingStateType::IDLE;
                    p_debug_mode->m_motor->actuate_motor({false,0});
                    // TODO: homing

                /*wait state to end here*/
                    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    //remove this if not necessary

                /*do something before state is done*/
                    p_debug_mode->m_breathing_state = p_debug_mode->m_breathing_next_state;
                break;
        }
    }
}
