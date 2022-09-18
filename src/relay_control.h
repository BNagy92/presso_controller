#include <Arduino.h>
#include <mutex>
#include "globals.h"

#define RELAY_CYCLE 500 //the period time of relay in ms 

void RelayControl(uint32_t RELAY_PIN, uint32_t * pwm_out);