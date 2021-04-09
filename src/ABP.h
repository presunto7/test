#ifndef _CONFIGURATION_ABP_H_
#define _CONFIGURATION_ABP_H_
#include <Arduino.h>

#define ABP_I2C_ADDR 0x28
//#define PERCENTAGE
#define ABP_NUM_BYTES 2
#define ABP_OUT_MAX_HEX 0x3FFF
//units in psi 
#define ABP_P_MAX 1
#define ABP_P_MIN 0 
#define ABP_P_RANGE ABP_P_MAX - ABP_P_MIN
#define PSI_TO_CMCH2O 70.30696
#ifdef PERCENTAGE
#define ABP_OUT_MIN 10.0 //%
#define ABP_OUT_MAX 80.0 //%
#else
#define ABP_OUT_MIN 1638 //Digital
#define ABP_OUT_MAX 13104 //Digital
#endif


class ABP {
    public:
        ABP(int);
        ~ABP();
        double get_air_pressure(void) {return this->m_pressure_cmh2o;}
        double get_air_pressure_psi(void) {return this->m_pressure_psi;}
        void read_sensor();
    private:
        int m_i2c_adress;
        double m_pressure_psi;
        double m_pressure_cmh2o;
        uint16_t request_data(void);
        double calculate_pressure(uint16_t rev_data);
#ifdef PERCENTAGE
        double convert_out_hex_to_perc(uint16_t data);
#endif
        double convert_psi_to_cho(double pressure);
};

#endif
