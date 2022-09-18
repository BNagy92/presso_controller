#include "relay_control.h"

void RelayControl(uint32_t RELAY_PIN, uint32_t * pwm_out){
  for(;;){ // infinite loop
    if (MUTEX_PID_OUT_C1 != NULL){
        if ( xSemaphoreTake(MUTEX_PID_OUT_C1, portTICK_PERIOD_US(10)) == pdTRUE )
        { // enter critical section
            uint32_t duty_cycle = *pwm_out;
            xSemaphoreGive(MUTEX_PID_OUT_C1);
            unsigned long windowStartTime = millis();
            unsigned long now = millis();
            while(now - windowStartTime < RELAY_CYCLE){
                now = millis();
                if ((double)(now - windowStartTime) < (double)((RELAY_CYCLE / 100) * duty_cycle) )
                    digitalWrite(RELAY_PIN, HIGH);
                else 
                    digitalWrite(RELAY_PIN, LOW);
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
            digitalWrite(RELAY_PIN, LOW);
        }
        else
            digitalWrite(RELAY_PIN, LOW);
    }
    else
        digitalWrite(RELAY_PIN, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
}
}