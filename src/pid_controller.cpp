#include "pid_controller.h"
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
void init_pid(){  
  Input = 0;
  Setpoint = 100;
  myPID.SetOutputLimits(0, 100);
  myPID.SetMode(AUTOMATIC);
};
void calc_pid(void *params){
  struct pid_struct * pid_d;
  pid_d = (pid_struct *) params;
  for(;;){
  Input = *pid_d->Temp;
  myPID.Compute();
  vTaskDelay(200 / portTICK_PERIOD_MS);
  };
};