#ifndef _VOLUME_CONTROLLER_H_
#define _VOLUME_CONTROLLER_H_
#include "Controller.h"

class VolumeController : public Controller{
    private:
        double m_flow_rate_ref; 
    public:
        VolumeController(controller_gains_t);
        ~VolumeController();
        motor_t control(double);
        double get_flow_rate_ref(void);
        bool set_flow_rate_ref(double, double);
};

#endif
