#ifndef SFM3000_H
#define SFM3000_H

#define I2C_ADDR          0x40        // SFM3300-D I2C Address
#define FLOW_BYTES        2           // Number of Bytes of the Flow Value
#define SERIAL_BYTES      4           // Number of Bytes of the Serial Number
#define CONTINUOUS_READ   0x1000      // Command for Continuous Reading
#define SERIAL_NUM        0x31AE      // Command to Get the Serial Number
#define SOFT_RESET        0x2000      // Command for Soft Reset
#define OFFSET            32768.0     // Offset Flow Value 
#define SCALE_FACTOR      120.0       // Air and N2
 
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif
 
#include <Wire.h>

struct Readings {
  uint32_t read_value;
  bool crc8(uint8_t [], uint8_t, uint8_t);
  void set_value(uint8_t nbytes);
  double to_flow();
};

class SFM3000 {
  public:
    SFM3000(void);
    ~SFM3000(){}
    void read(uint8_t nbytes);
    void send(uint16_t command);
    double get_flow();
  private:
    Readings *r;
    double flow; 
    uint32_t serial;

};

 
#endif
