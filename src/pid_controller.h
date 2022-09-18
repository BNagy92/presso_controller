#include <Arduino.h>
#include <mutex>
#include <PID_v1.h>
#include <iostream>
#include "globals.h"

double Setpoint, Input, Output;
double Kp=2, Ki=5, Kd=1;
extern PID myPID;

struct pid_struct {
   double *Temp;
   SemaphoreHandle_t MUTEX_MEAS_OUT_C1 = NULL;
   uint32_t * pwm_out;
   SemaphoreHandle_t MUTEX_PID_OUT_C1 = NULL;
};

void init_pid();
void calc_pid(void *params);