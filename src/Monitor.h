#ifndef _MONITOR_H_
#define _MONITOR_H_
#include "Configs.h"
#include "Sensoring.hpp"
#include "Parameters.h"
#include "DebugMode.hpp"


class Monitor{
    errors_t m_errors;
    TaskHandle_t m_monitor_task_handler;
    SemaphoreHandle_t m_mutex_monitor;
    Sensoring* m_sensoring;
    Parameters* m_param;
    DebugMode* m_debug;
    static Monitor * m_monitor;
    private:
    Monitor(Sensoring*,Parameters*,DebugMode*);
    ~Monitor();
    static void vTaskMonitor(void*);
    int init_monitor();
    void check_errors();
    public:
    static Monitor *getInstance(Sensoring*,Parameters*,DebugMode*);
    errors_t get_current_errors();
};
#endif
