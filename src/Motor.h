#ifndef _MOTOR_H_
#define _MOTOR_H_
#include "Configs.h"

class Motor{
    motor_t m_motor;
    TaskHandle_t m_motor_task_handler;
    SemaphoreHandle_t m_mutex_motor;
    const int PWM_FREQ_MOTOR = 1000;                            // this variable is used to define the time period 
    const int PWM_RESOLUTION_MOTOR = 8;                         // this will define the resolution of the signal which is 8 in this case
    const int PWM_UPCHANNEL_MOTOR = 0;                            // this variable is used to select the channel number
    const int PWM_UPGPIO_MOTOR = 15;                              // GPIO to which we want to attach this channel signal
    const int PWM_DOWNCHANNEL_MOTOR = 1;
    const int PWM_DOWNGPIO_MOTOR = 2;
        static void vTaskMotor(void*);
    public:
    Motor();
    ~Motor();
    void actuate_motor(motor_t);
    motor_t get_motor_state();
    int init_motor();
};

#endif
