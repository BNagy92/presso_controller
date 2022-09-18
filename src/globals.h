#include <Arduino.h>
#include <mutex>


#ifndef GLOBALS_H
#define GLOBALS_H

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define RELAY_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define ADC_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define SSR_RELAY_PIN 23
#define TMP_ADC_PIN 13
extern SemaphoreHandle_t MUTEX_PID_OUT_C1 = NULL;
extern uint32_t PID_OUT_C1;  // Declare the global variable as extern

#endif