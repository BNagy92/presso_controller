#include <Arduino.h>
#include <mutex>
#include "globals.h"

#define RELAY_CYCLE 500 //the period time of relay in ms 
struct relay_struct {
   uint32_t RELAY_PIN;
   uint32_t * pwm_out;
   SemaphoreHandle_t MUTEX_PID_OUT_C1 = NULL;
};
void RelayControl(void *params);