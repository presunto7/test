#ifndef _PRESSURE_CONTROLLER_H_
#define _PRESSURE_CONTROLLER_H_
#include "Controller.h"

class PressureController : public Controller{
    private:
        double m_pressure_ref;
    public:
        PressureController(controller_gains_t);
        ~PressureController();
        motor_t control(double);
        double get_pressure_ref(void);
        bool set_pressure_ref(double);
};
#endif
