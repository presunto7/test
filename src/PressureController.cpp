#include "PressureController.h"
#include "Configs.h"

PressureController::PressureController(controller_gains_t gains = {0,0,0}) : 
Controller(gains)
{
    m_pid->SetOutputLimits(PRESSURE_CONTROLLER_PID_OUTPUT_MIN,PRESSURE_CONTROLLER_PID_OUTPUT_MAX);
}

PressureController::~PressureController()
{
}

motor_t PressureController::control(double input_pressure){
    int output = this->compute(input_pressure);
    motor_t motor;
    if (output < 0){
        motor.rotation_direction = false;
        motor.duty_cycle = output*(-1);
    }
    else{
        motor.rotation_direction = true;
        motor.duty_cycle = output;
    }
    return motor;
}
bool PressureController::set_pressure_ref(double pressure_ref)
{
    if(m_pressure_ref >= MIN_PRESSURE && m_pressure_ref <= MAX_PRESSURE){
        m_pressure_ref = pressure_ref;
        this->set_setpoint(m_pressure_ref);
    }
    return (m_pressure_ref >= MIN_PRESSURE && m_pressure_ref <= MAX_PRESSURE);
}
double PressureController::get_pressure_ref(void)
{
    return m_pressure_ref;
}
