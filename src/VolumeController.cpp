#include "VolumeController.h"

VolumeController::VolumeController(controller_gains_t gains) 
    : Controller(gains)
{
}
VolumeController::~VolumeController()
{
}
motor_t VolumeController::control(double input_flow)
{
    double output = this->compute(input_flow);
    motor_t motor;
    motor.rotation_direction = true;
    motor.duty_cycle = output;
    return motor;
}
bool VolumeController::set_flow_rate_ref(double tidal_volume_ref,double period)
{
    if(tidal_volume_ref >= MIN_TIDAL_VOLUME && tidal_volume_ref <= MAX_TIDAL_VOLUME){
        m_flow_rate_ref = tidal_volume_ref/period;
        this->set_setpoint(m_flow_rate_ref);
    }
    return (tidal_volume_ref >= MIN_TIDAL_VOLUME && tidal_volume_ref <= MAX_TIDAL_VOLUME);
}
double VolumeController::get_flow_rate_ref(void)
{
    return m_flow_rate_ref;
}
