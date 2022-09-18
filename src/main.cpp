#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <PID_v1.h>
#include "temp_measurment.h"
#include "globals.h"
//#include "relay_control.h"




//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
class BLERelayCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("\r\nNew Relay Control Message: ");
        //for (int i = 0; i < value.length(); i++)
        //  Serial.print(value[i]);
        //Serial.println();

        if (value.compare("on")){
          //Serial.println("On");
          digitalWrite(LED_BUILTIN, LOW);
          digitalWrite(SSR_RELAY_PIN, LOW);
        }
        if (value.compare("off")){
          //Serial.println("Off");
          digitalWrite(LED_BUILTIN, HIGH);
          digitalWrite(SSR_RELAY_PIN, HIGH);
        }

      }
    }
};
BLECharacteristic *pADCCharacteristic;
BLECharacteristic *pRelayCharacteristic;
void setupBLE(){
    BLEDevice::init("ESP32 BLE Relay Control");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pRelayCharacteristic = pService->createCharacteristic(
                                         RELAY_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pADCCharacteristic = pService->createCharacteristic(
                                         ADC_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ
                                       );
  
  pRelayCharacteristic->setCallbacks(new BLERelayCallbacks());
  pRelayCharacteristic->setValue("Write On or Off");
  pADCCharacteristic->setCallbacks(new BLERelayCallbacks());
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}
struct temp_struct temp_c1;
struct temp_struct *temp_c1_ptr = &temp_c1;
void setup() {
  //initialize the variables we're linked to
  
  temp_c1.pin = TMP_ADC_PIN;
  temp_c1.Temp = 0;
  temp_c1.MUTEX_MEAS_OUT_C1 = xSemaphoreCreateMutex();

  // MUTEX_PID_OUT_C1 = xSemaphoreCreateMutex();
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode (LED_BUILTIN, OUTPUT);
  pinMode (SSR_RELAY_PIN, OUTPUT);
  init_temp(TMP_ADC_PIN);
  setupBLE();
  Input = (double) ADC_LUT[(uint32_t) analogRead(TMP_ADC_PIN)];
  Setpoint = 100;
  //turn the PID on
  myPID.SetOutputLimits(0, 100);
  myPID.SetMode(AUTOMATIC);
}


const double R=100.0;

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("Csumi!");
  /*
  xTaskCreate(
    RelayControl,    // Function that should be called
    "RelayControl_C1",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );*/
  xTaskCreate(
    measure_temp,    // Function that should be called
    "TempMeasure_C1",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    temp_c1_ptr,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
  Input = temp_c1.Temp;
  Serial.println(Input);
  myPID.Compute();

  //char tmp_str[10];
  //String(tmp).toCharArray(tmp_str,10);
  //pADCCharacteristic->setValue(tmp_str);
  delay(1000);
}
