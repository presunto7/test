#include "Controller.h"
#include "Configs.h"

Controller::Controller(controller_gains_t gains)
{
    m_pid = new PID(&m_input, &m_output, &m_setpoint, gains.kp, gains.ki, gains.kd, DIRECT);
    m_gains = gains;
}
Controller::~Controller()
{ 
    delete m_pid; 
}
void Controller::set_setpoint(double setpoint) 
{ 
    this->m_setpoint = setpoint; 
}
double Controller::get_last_output(void)
{ 
    return m_output;
}
controller_gains_t Controller::get_gains(void)
{
    return m_gains;
}
void Controller::set_gains(controller_gains_t gains)
{
    m_gains = gains;
}
double Controller::compute(double input)
{
    m_input = input;
    m_pid->Compute(); 
    return (int) m_output;
}
void Controller::switch_controller(bool state)
{
    m_output = 0;
    m_input = 0;
    m_pid->SetMode(state);
}
