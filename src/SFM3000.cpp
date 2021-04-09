#include "SFM3000.h"

bool Readings::crc8(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum){

  uint8_t crc = 0; // calculated checksum
  // calculates 8-Bit checksum with given polynomial
  for(uint8_t byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
  {
    crc ^= (data[byteCtr]);
    for(uint8_t bit = 8; bit > 0; --bit)
    {
      if(crc & 0x80) crc = (crc << 1) ^ 0x131;
      else           crc = (crc << 1);
    }
  }
  // verify checksum
  if(crc != checksum) return false;
  return true;
}

void Readings::set_value(uint8_t nbytes){

    uint8_t read_bytes[6] = {0,0,0,0,0,0};
    uint8_t first[2] = {0,0};
    uint8_t second[2] = {0,0};
    uint8_t nCRC = nbytes / 2; 

    Wire.requestFrom(I2C_ADDR, nbytes + nCRC);
    for (uint8_t cnt = 0; cnt < nbytes + nCRC ; cnt++)
        read_bytes[cnt] = Wire.read();
    memcpy(first, read_bytes, 2*sizeof(uint8_t));
    memcpy(second, read_bytes+3, 2*sizeof(uint8_t));

    if (crc8(first, 2, read_bytes[2]))
        read_value = (first[0] << 8) | first[1];

    if ((nbytes == SERIAL_BYTES) && crc8(second, 2, read_bytes[5]))
        read_value = (read_value << 16) | (second[0] << 8) | second[1];
}

double Readings::to_flow(){
    return ( (uint16_t(read_value & 0xFFFF) - OFFSET) / SCALE_FACTOR );
}

SFM3000::SFM3000(){ 
    r = new Readings;
    flow = 0.0;
    serial = 0;
	Wire.begin();
}

void SFM3000::send(uint16_t command){
    Wire.beginTransmission(I2C_ADDR);
	Wire.write(uint8_t((command >> 8) & 0xFF));  
	Wire.write(uint8_t(command & 0xFF));
	Wire.endTransmission();
}

void SFM3000::read(uint8_t nbytes){
    r->set_value(nbytes);
    if(nbytes == FLOW_BYTES)
        flow = r->to_flow();
    else
        serial = r->read_value;
}

double SFM3000::get_flow(){
    return flow;
}
