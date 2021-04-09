    #include "I2C.h"
    #include "Configs.h"
    void I2C::i2c_take_mutex(void)
    {
        xSemaphoreTake(m_i2c_mutex, portMAX_DELAY);
    }
    void I2C::i2c_release_mutex(void)
    {
        xSemaphoreGive(m_i2c_mutex);
    }
    I2C::I2C()
    {

    }
    I2C::~I2C()
    {

    }
    bool I2C::i2c_init()
    {
        if(m_i2c_mutex != NULL)
        {
            m_i2c_mutex = xSemaphoreCreateMutex();
            if(m_i2c_mutex == NULL)
            {
              return false;  
            }
        }
        Wire.begin();
        return true;
    }
    void I2C::i2c_write(uint8_t addr,uint8_t* arr)
    {
        int size_arr = *(&arr + 1) - arr;
        i2c_take_mutex();
        Wire.beginTransmission(addr);
        for(int i=0; i< size_arr; i++)
        {
            Wire.write(arr[i]);  
        }
	    Wire.endTransmission();
        i2c_release_mutex();
    }

    uint8_t I2C::i2c_read(uint8_t addr,uint8_t bytes,uint8_t* arr)
    {
        int count = 0;
        int size_arr = *(&arr + 1) - arr;
        i2c_take_mutex();
        if(size_arr >= bytes)
        {
             Wire.requestFrom(addr, bytes);
            for (int i = 0; i < bytes ; i++)
            {
                if(Wire.available())
                {
                    arr[i] = Wire.read();
                    count ++;
                }
            }
        }
        i2c_release_mutex();
        return count;
    }
