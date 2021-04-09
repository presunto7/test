#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_
#include <Arduino.h>
#include "Configs.h"

class Parameters
{
  private:
    uint16_t m_bpm;
    double m_ie_ratio;
    cycle_times_t m_cycles_time;
    double m_tidal_volume_ref;
    double m_flow_rate_ref;
    double m_pressure_ref; 
    double m_peep;
  private:
    cycle_times_t calculate_cycles_time();
  public:
    Parameters();
    ~Parameters();
    bool set_tidal_volume_ref(double);
    bool set_pressure_ref(double);
    bool set_bpm(uint16_t);
    bool set_ie_ratio(double);
    bool set_peep(double);
    void set_th_time(double);
    double get_tidal_volume_ref(void);
    double get_pressure_ref(void);
    double get_flow_rate_ref(void);
    double get_peep(void);
    
    cycle_times_t get_cycles_time(void);
};

#endif
