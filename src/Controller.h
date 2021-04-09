#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include <PID_v1.h>
#include <Arduino.h>
#include "Parameters.h"

class Controller{
    private:  
        double m_setpoint;
        double m_output;
        double m_input;
        controller_gains_t m_gains;
    protected:
        PID * m_pid; 
    public:
        Controller(controller_gains_t);
        virtual ~Controller();
        void set_setpoint(double);
        double get_last_output(void);
        controller_gains_t get_gains(void);
        void set_gains(controller_gains_t);
        virtual motor_t control(double) = 0;
        void switch_controller(bool);
    protected:
        double compute(double);  
};

#endif
