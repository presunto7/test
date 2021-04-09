#include "ABP.h"
#include <Wire.h>

ABP::ABP(int i2c_adress)
{
  this->m_i2c_adress = i2c_adress;
  this->m_pressure_psi = 0;
  this->m_pressure_cmh2o = 0;
  Wire.begin();
}
ABP::~ABP()
{

}
void ABP::read_sensor()
{
  uint16_t data = 0;
  data = request_data();
  this->m_pressure_psi = calculate_pressure(data);
  this->m_pressure_cmh2o = convert_psi_to_cho(this->m_pressure_psi);
  return;
}

uint16_t ABP::request_data()
{
  uint16_t data = 0;
  uint8_t index = 0;
  Wire.requestFrom(m_i2c_adress,ABP_NUM_BYTES);
  while(Wire.available())
  { 
    uint8_t c = Wire.read();
    data = (data << (index * 8)) | c ;
    index = index + 1;
  }
  return data;
}

double ABP::calculate_pressure(uint16_t rev_data)
{
    double pressure = 0;
    uint16_t data = rev_data;
    double out = 0;
#ifdef PERCENTAGE //if percentage is defined, calculate correspondent output percentage to convert into pressure
    double perc = convert_out_hex_to_perc(data);
    Serial.println(perc, DEC);
    Serial.print("\r\n");
    out=perc;
#else //if PERCENTAGE is not defined use data as output
    out = data;
#endif //end ifdef PERCENTAGE
    pressure = (((out - ABP_OUT_MIN) * (ABP_P_RANGE)) / ABP_OUT_MAX ) + ABP_P_MIN;
    return pressure; 
}

#ifdef PERCENTAGE
double ABP::convert_out_hex_to_perc(uint16_t data){
  return double((100.0/ABP_OUT_MAX_HEX)*data);
}
#endif

double ABP::convert_psi_to_cho(double pressure)
{ 
  return pressure * PSI_TO_CMCH2O;
}
