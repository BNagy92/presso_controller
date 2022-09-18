#include "Globals.h"

uint32_t PID_OUT_C1 = 0;
SemaphoreHandle_t MUTEX_PID_OUT_C1 = xSemaphoreCreateMutex();
struct RELAY_C1_PARAMS
{
uint32_t RELAY_PIN;
uint32_t * pwm_out;
} RELAY_C1_PARAMS;