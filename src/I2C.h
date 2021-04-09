#ifndef _I2C_H_
#define _I2C_H_
#include <Wire.h>
#include <Arduino.h>
#include "Configs.h"
class I2C{
    static SemaphoreHandle_t m_i2c_mutex;
    void i2c_take_mutex (void);
    void i2c_release_mutex(void);
    public:
    I2C();
    ~I2C();
    bool i2c_init();
    void i2c_write(uint8_t,uint8_t*);
    uint8_t i2c_read(uint8_t,uint8_t,uint8_t*);
};
#endif
