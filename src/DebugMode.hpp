#ifndef DEBUG_MODE_H
#define DEBUG_MODE_H

#include<Arduino_FreeRTOS.h>
#include "Sensoring.hpp"
#include "Configs.h"
#include "Parameters.h"
#include "VolumeController.h"
#include "PressureController.h"
#include "Controller.h"
#include "Motor.h"

class DebugMode
{
    public:
        /*constructor*/
        DebugMode(Parameters *, Sensoring *, Motor *);
        /*destructor*/
        ~DebugMode();
        StaticTask_t xTaskBuffer;
        StackType_t xStack[ 254 ];
        /*function to setup debug mode*/
        uint8_t setup();

        /*function to start debug operation*/
        void start(ControlModeType con_mode);

        /*function to stop debug operation*/
        void stop();

        /*function that returns the current operation mode state*/
        BreathingStateType get_current_state();

    private:
        /*parameters object pointer */
        Parameters *m_param;
        /*sensoring object pointer*/
        Sensoring *m_sensoring;
        /*motor object pointer*/
        Motor* m_motor;

        /*current breathing stage*/
        BreathingStateType m_breathing_state;
        /*next breathing stage*/
        BreathingStateType m_breathing_next_state; 

        /*current operation mode*/
        ControlModeType m_con_mode;

        /*Controlle modes pointers*/
        PressureController *m_pressure_controller;
        VolumeController *m_flow_controller;
        Controller *m_controller;

        /*setup freertos components*/
        uint8_t setup_threading();

        /*control timer handler*/
        TimerHandle_t control_timer_handler;
        /*SW timer to perform a control iteration at each CONTROL_FREQUENCY*/
        static void control_timer_task(void *pParameters);

        /*function to start control*/
        void start_control(Controller *curr_controller);
        /*function to stop control*/
        void stop_control(Controller *curr_controller);

        /*breathing states timer handler*/
        TimerHandle_t breathing_timer_handler;
        /*SW timer to perform the different breathing states (inspiration, hold, expiration)*/
        static void breathing_timer_task(void *pParameters);

        /*breathing state machine taks handler*/
        TaskHandle_t breathing_task_handler;
        /*task to run the breathing state machine*/
        static void breathing_state_machine_task(void *pParameters);

        /*function that updates the time counting of the breathings timer according to breathing 
        state (inspiration, hold, expiration) and starts timer*/
        void update_start_breathing_timer();
        /*function that stops the breathings timer*/
        void stop_breathing_timer();

        /*Array of pointers to functions*/
        void (*do_state_machine[ControlModeType::N_CONTROL_MODES])(DebugMode *p_debug_mode);
        /*state machine functions*/
        static void do_state_machine_pressure(DebugMode *p_debug_mode);
        static void do_state_machine_flow(DebugMode *p_debug_mode);   
};


#endif      //DEBUG_MODE_H
