#include "Parameters.h"


Parameters::Parameters()
{
    m_ie_ratio = DEFAULT_IE_RATIO;
    m_bpm = DEFAULT_BPM;
    m_cycles_time.th = TH;
    m_cycles_time = calculate_cycles_time();
    m_tidal_volume_ref = DEFAULT_TIDAL_VOLUME;
    m_flow_rate_ref = DEFAULT_FLOW_RATE;
    m_pressure_ref = DEFAULT_PRESSURE;
}
Parameters::~Parameters()
{ 
}
cycle_times_t Parameters::calculate_cycles_time()
{
    cycle_times_t cyl_times;
    cyl_times.period = 60.0 / m_bpm;
    cyl_times.tin = m_cycles_time.period / (1 + m_ie_ratio);
    cyl_times.tex = m_cycles_time.period - m_cycles_time.tin - m_cycles_time.th;
    return cyl_times;
}
bool Parameters::set_tidal_volume_ref(double tidal_volume_ref)
{
    if(m_tidal_volume_ref >= MIN_TIDAL_VOLUME && m_tidal_volume_ref <= MAX_TIDAL_VOLUME){
        m_tidal_volume_ref = tidal_volume_ref;
        m_flow_rate_ref = tidal_volume_ref/m_cycles_time.period;
    }
    return (m_tidal_volume_ref >= MIN_TIDAL_VOLUME && m_tidal_volume_ref <= MAX_TIDAL_VOLUME);
}
bool Parameters::set_pressure_ref(double pressure_ref)
{
    if(m_pressure_ref >= MIN_PRESSURE && m_pressure_ref <= MAX_PRESSURE){
        m_pressure_ref = pressure_ref;
    }
    return (m_pressure_ref >= MIN_PRESSURE && m_pressure_ref <= MAX_PRESSURE);
}
bool Parameters::set_bpm(uint16_t bpm)
{
    if(bpm >= MIN_BPM && bpm <= MAX_BPM){
        m_bpm = bpm;
        m_cycles_time = calculate_cycles_time();
    }
    return (bpm >= MIN_BPM && bpm <= MAX_BPM);
}
bool Parameters::set_ie_ratio(double ie_ratio)
{
    if(ie_ratio >= MIN_IE_RATIO && ie_ratio <= MAX_IE_RATIO){
        m_ie_ratio = ie_ratio;
        m_cycles_time = calculate_cycles_time();
    }
    return (ie_ratio >= MIN_IE_RATIO && ie_ratio <= MAX_IE_RATIO);
}
bool Parameters::set_peep(double peep)
{
    if(peep >= MIN_PEEP && peep <= MAX_PEEP)
        m_peep = peep;
    
    return (peep >= MIN_PEEP && peep <= MAX_PEEP);
}
void Parameters::set_th_time(double th){
    m_cycles_time.th = th;
    m_cycles_time = calculate_cycles_time();
}
double Parameters::get_flow_rate_ref(void)
{
    return m_flow_rate_ref;
}
double Parameters::get_pressure_ref(void)
{
    return m_pressure_ref;
}
cycle_times_t Parameters::get_cycles_time(void)
{
    return m_cycles_time;
}
double Parameters::get_peep(void)
{
    return m_peep;
}
double Parameters::get_tidal_volume_ref(void)
{
    return m_tidal_volume_ref;
}
