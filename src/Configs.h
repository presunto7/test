#ifndef _CONFIGS_H_
#define _CONFIGS_H_
#include <Arduino.h>
#include<Arduino_FreeRTOS.h>
#include <timers.h> 
#include <semphr.h> 
/****************************/
/*      CONTROL VALUES      */
/****************************/
#define CONTROL_FREQUENCY 100  //Hz
#define VOLUME_KP 1 
#define VOLUME_KI 0
#define VOLUME_KD 0
#define PRESSURE_KP 1 
#define PRESSURE_KI 0
#define PRESSURE_KD 0
#define PRESSURE_CONTROLLER_PID_OUTPUT_MAX 255
#define PRESSURE_CONTROLLER_PID_OUTPUT_MIN -255
/****************************/
/*      DEFAULT VALUES      */
/****************************/
#define DEFAULT_PEEP 4
#define DEFAULT_BPM 10
#define DEFAULT_TIDAL_VOLUME 560
#define DEFAULT_PRESSURE 10
#define DEFAULT_IE_RATIO (1/2)
#define DEFAULT_FLOW_RATE (1/2)
/****************************/
/*      MAX/MIN VALUES      */
/****************************/
#define MAX_BPM 30
#define MIN_BPM 8
#define MAX_IE_RATIO 4
#define MIN_IE_RATIO 1
#define MAX_TIDAL_VOLUME 200
#define MIN_TIDAL_VOLUME 800
#define MAX_PRESSURE 40
#define MIN_PRESSURE DEFAULT_PEEP 
#define MAX_PEEP 30
#define MIN_PEEP 3 
#define TH 0.15
#define PIP 40
#define MAX_VOLUME 40
#define MAX_PRESS 40

/*operation modes*/
enum OperationModeType {DEBUG, ASSISTED, ACTIVE, N_OPERATION_MODES};
/*operation modes names*/
const char* const op_mode_names[] = {"debug", "assisted", "active"};
/*control modes */
enum ControlModeType {PRESSURE, FLOW, N_CONTROL_MODES};
/*control modes names*/
const char* const con_mode_names[] = {"pressure", "flow"};
/*breathing cycle states*/
enum BreathingStateType {IDLE, INSPIRATION, HOLD, EXPIRATION, STOPPING, N_BREATHING_STATES};
/*breathing cycle states names*/
const char* const breathing_states_names[] = {"iddle", "inspiration", "hold", "expiration", "stopping"};


#define MONITOR_TASK_FREQ 50
#define MONITOR_WINDOW_SIZE 5
typedef struct cycle_times_t{ 
  double tin; 
  double tex;
  double th;
  double period;
}cycle_times_t;

typedef struct motor_t{
  bool rotation_direction;
  uint8_t duty_cycle;
}motor_t;

typedef struct controller_gains_t{
  double kp; 
  double ki;
  double kd;
}controller_gains_t;

typedef struct error_t{
  enum errorsType {
                        NO_ERROR,
                        VOLUME_ERROR,
                        PRESSURE_ERROR,
                        PEEP_ERROR,
                        PIP_ERROR,
                        SENS_SETUP,
                        OP_MODE_SETUP,
                        MOTOR_SETUP,
                        MONITOR_SETUP};

  uint8_t f_error_aux;
  uint32_t f_errors;
  
}errors_t;

#endif
