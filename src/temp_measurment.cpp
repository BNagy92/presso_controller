#include "temp_measurment.h"

void init_temp(int pin)
{
adcAttachPin(pin);
analogSetPinAttenuation(pin, ADC_6db);
};
double measure_temp(int pin)
{
  uint32_t tmp_buff = 0;
  uint32_t tmp2;
  /*for(int i=0;i<100;i++){
    tmp2 = ADC_LUT[(uint32_t) analogRead(pin)];
    tmp_buff= tmp2 + tmp_buff;
    delay(1);
  }*/
  for(int i=0;i<100;i++){
    tmp_buff = ADC_LUT[(uint32_t) analogRead(pin)] + tmp_buff;
    delay(1);
  }
  Serial.println(analogRead(pin));
  double avg_adc = (double) tmp_buff/100.0;
  double u = (avg_adc/4095.0)*1.75;
  double i = u/100.0;
  double rptc = (3.3-u)/i;
  double Rt = (rptc/100-1.0)/0.00385-12;
  Serial.println(Rt);
  return Rt;
};