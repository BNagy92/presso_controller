#include "temp_measurment.h"

void init_temp(int pin)
{
adcAttachPin(pin);
analogSetPinAttenuation(pin, ADC_6db);
};
void measure_temp(void *params)
{
  struct temp_struct *temp_struct_ptr = (temp_struct*) params;
  for(;;){
    uint32_t tmp_buff = 0;
    uint32_t tmp2;
    /*for(int i=0;i<100;i++){
      tmp2 = ADC_LUT[(uint32_t) analogRead(pin)];
      tmp_buff= tmp2 + tmp_buff;
      delay(1);
    }*/
    for(int i=0;i<100;i++){
      tmp_buff = (uint32_t) analogRead(temp_struct_ptr->pin) + tmp_buff;
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    //Serial.println(analogRead(temp_struct_ptr->pin));
    double avg_adc = (double) tmp_buff/100.0;
    double u = (avg_adc/4095.0)*1.75;
    double i = u/100.0;
    double rptc = (3.3-u)/i;
    double Rt = (rptc/100-1.0)/0.00385-12;
    if ( xSemaphoreTake(temp_struct_ptr->MUTEX_MEAS_OUT_C1, 10) == pdTRUE ){
      *temp_struct_ptr->Temp = Rt;
      xSemaphoreGive(temp_struct_ptr->MUTEX_MEAS_OUT_C1);
    }
    
  }
};