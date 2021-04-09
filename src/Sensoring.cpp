#include "Sensoring.hpp"


Sensoring::Sensoring(): pressure_sensor(ABP_I2C_ADDR)
{
    error_codes.f_errors = 0;
    current_volume = 0;
}

Sensoring::~Sensoring(){}

uint8_t Sensoring::setupSensoring()
{
    /*Start sensors here*/
    flow_sensor.send(CONTINUOUS_READ);

       return  startThreads();
}

uint8_t Sensoring::startThreads()
{ 
    sensors_data_mutex = xSemaphoreCreateMutex();
    if(sensors_data_mutex == NULL)
        return error_t :: SENS_SETUP;
    
    if((xTaskCreate(&Sensoring::acquireSensorsDataTask, "at", 256, this, 1, &acquisitions_task)) != pdPASS)
        return error_t :: SENS_SETUP;

    acquisitions_timer = xTimerCreate(
                                        "AcquireSensorsDataTrigger", 
                                        pdMS_TO_TICKS((1.0/ACQUISITION_FREQUENCY)*1000), 
                                        pdTRUE, 
                                        this, 
                                        Sensoring::acquireSensorsDataTrigger
                                        ); 
    
    if(acquisitions_timer == NULL)
        return error_t :: SENS_SETUP;

    if(xTimerStart(acquisitions_timer, 0) != pdPASS)
        return  error_t :: SENS_SETUP;
    return error_t :: NO_ERROR;
}

void Sensoring::acquireSensorsDataTrigger(void *pParameters)
{
    Sensoring* sensoring = reinterpret_cast<Sensoring*>(pParameters); 

    xTaskNotifyGive(sensoring->acquisitions_task);
}

void Sensoring::acquireSensorsDataTask(void *pParameters)
{
    Sensoring* sensoring = reinterpret_cast<Sensoring*>(pParameters); 
	
    while(1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        xSemaphoreTake(sensoring->sensors_data_mutex, portMAX_DELAY);

        /*Update sensors data here*/
        sensoring->flow_sensor.read(FLOW_BYTES);
        sensoring->current_flow = sensoring->flow_sensor.get_flow();
        sensoring->pressure_sensor.read_sensor();
        sensoring->current_pressure = sensoring->pressure_sensor.get_air_pressure_psi();

        sensoring->current_volume += (sensoring->current_flow/60.0) * (1.0/ACQUISITION_FREQUENCY);
        Serial.print(sensoring->current_volume , 1);
        Serial.print("\t");
        Serial.print(sensoring->current_pressure , 1);
        Serial.print("\t");
        Serial.println(sensoring->current_flow, 1);
        
        if(sensoring->current_volume < 0)
            sensoring->current_volume = 0;

        xSemaphoreGive(sensoring->sensors_data_mutex);
    }
}

double Sensoring::getCurrentFlow()
{
    xSemaphoreTake(sensors_data_mutex, portMAX_DELAY);
    return current_flow;
    xSemaphoreGive(sensors_data_mutex);
}

double Sensoring::getCurrentPressure()
{
    xSemaphoreTake(sensors_data_mutex, portMAX_DELAY);
    return current_pressure;
    xSemaphoreGive(sensors_data_mutex);
}

double Sensoring::getCurrentVolume()
{
    xSemaphoreTake(sensors_data_mutex, portMAX_DELAY);
    return current_volume;
    xSemaphoreGive(sensors_data_mutex);
}
