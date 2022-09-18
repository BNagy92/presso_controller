#include "relay_control.h"

void RelayControl(void *params){
  for(;;){ // infinite loop
    struct relay_struct *relay_struct_ptr = (relay_struct*) params;
    if (relay_struct_ptr->MUTEX_PID_OUT_C1 != NULL){
        if ( xSemaphoreTake(relay_struct_ptr->MUTEX_PID_OUT_C1, 10) == pdTRUE )
        { // enter critical section
            uint32_t duty_cycle = *relay_struct_ptr->pwm_out;
            xSemaphoreGive(relay_struct_ptr->MUTEX_PID_OUT_C1);
            unsigned long windowStartTime = millis();
            unsigned long now = millis();
            while(now - windowStartTime < RELAY_CYCLE){
                now = millis();
                if ((double)(now - windowStartTime) < (double)((RELAY_CYCLE / 100) * duty_cycle) )
                    digitalWrite(relay_struct_ptr->RELAY_PIN, HIGH);
                else 
                    digitalWrite(relay_struct_ptr->RELAY_PIN, LOW);
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
            digitalWrite(relay_struct_ptr->RELAY_PIN, LOW);
        }
        else
            digitalWrite(relay_struct_ptr->RELAY_PIN, LOW);
    }
    else
        digitalWrite(relay_struct_ptr->RELAY_PIN, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
}
}