#ifndef SENSORING_H
#define SENSORING_H

#include <Arduino.h>
/*Include sensors driver here*/
#include "SFM3000.h"
#include "ABP.h"
#include "Configs.h"

#include "Arduino.h"
#define ACQUISITION_FREQUENCY 1000
#include<Arduino_FreeRTOS.h>

class Sensoring
{
    uint8_t startThreads();
    static void acquireSensorsDataTask(void *pParameters);
    static void acquireSensorsDataTrigger(void *pParameters);
    TaskHandle_t acquisitions_task;
    TimerHandle_t acquisitions_timer;
    SemaphoreHandle_t sensors_data_mutex;

    /*Instantiate sensors object here*/
    SFM3000 flow_sensor;
    ABP pressure_sensor;
    double current_flow;
    double current_pressure;
    double current_volume;
    
    errors_t error_codes; 

    public:
        Sensoring();
        ~Sensoring();

        uint8_t setupSensoring();
        double getCurrentFlow();
        double getCurrentPressure();
        double getCurrentVolume();
};

#endif      //SENSORING_H
