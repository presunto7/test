#include "Motor.h" 
#include <Arduino.h> 

Motor::Motor()
{
    //ledcSetup(PWM_UPCHANNEL_MOTOR, PWM_FREQ_MOTOR, PWM_RESOLUTION_MOTOR);     // Configure PWM functionalities
   // ledcAttachPin(PWM_UPGPIO_MOTOR, PWM_UPCHANNEL_MOTOR);                       // Attach the channel to the GPIO to be controlled
    //ledcSetup(PWM_DOWNCHANNEL_MOTOR, PWM_FREQ_MOTOR, PWM_RESOLUTION_MOTOR);     // Configure PWM functionalities
    //ledcAttachPin(PWM_DOWNGPIO_MOTOR, PWM_DOWNCHANNEL_MOTOR);     
}
Motor::~Motor()
{

}
void Motor::actuate_motor(motor_t motor)
{
    xSemaphoreTake(m_mutex_motor,portMAX_DELAY);
    m_motor = motor;
    xSemaphoreGive(m_mutex_motor);
    xTaskNotifyGive(m_motor_task_handler);
}

motor_t Motor::get_motor_state()
{
    xSemaphoreTake(m_mutex_motor,portMAX_DELAY);
    motor_t motor = m_motor;
    xSemaphoreGive(m_mutex_motor);
    return motor;
}

void  Motor::vTaskMotor(void* pvParameters)
{
    Motor* p_motor = reinterpret_cast<Motor*>(pvParameters);
    motor_t motor;
    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        xSemaphoreTake(p_motor->m_mutex_motor,portMAX_DELAY);
        motor = p_motor->m_motor;
        xSemaphoreGive(p_motor->m_mutex_motor);
        if(motor.rotation_direction){
           // ledcWrite (p_motor->PWM_UPCHANNEL_MOTOR,motor.duty_cycle);
           // ledcWrite (p_motor->PWM_DOWNCHANNEL_MOTOR, 0);
        }else{
           // ledcWrite (p_motor->PWM_UPCHANNEL_MOTOR, 0);
           // ledcWrite (p_motor->PWM_DOWNCHANNEL_MOTOR, motor.duty_cycle);
        }
    }
}

int Motor::init_motor()
{
    if( (xTaskCreate(&Motor::vTaskMotor, "vTaskMotor", 200, this, 1, &m_motor_task_handler)) != pdPASS)
        return error_t :: MOTOR_SETUP;
    m_mutex_motor = xSemaphoreCreateMutex();
    if(m_mutex_motor == NULL)
        return error_t :: MOTOR_SETUP;
    return  error_t :: NO_ERROR;
}
