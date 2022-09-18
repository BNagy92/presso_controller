#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "temp_measurment.h"
#include "globals.h"
#include "pid_controller.h"
//#include "relay_control.h"

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
struct pid_struct pid_c1;
struct pid_struct *pid_c1_ptr = &pid_c1;
void setup() {

  temp_c1.pin = TMP_ADC_PIN;
  temp_c1.Temp = (double*) malloc(sizeof(double));
  temp_c1.MUTEX_MEAS_OUT_C1 = xSemaphoreCreateMutex();
  pid_c1.Temp = temp_c1.Temp;
  pid_c1.MUTEX_MEAS_OUT_C1 = temp_c1.MUTEX_MEAS_OUT_C1;
  // MUTEX_PID_OUT_C1 = xSemaphoreCreateMutex();
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode (LED_BUILTIN, OUTPUT);
  pinMode (SSR_RELAY_PIN, OUTPUT);
  init_temp(TMP_ADC_PIN);
  init_pid();
  setupBLE();
}

void loop() {

  xTaskCreate(
    measure_temp,    // Function that should be called
    "TempMeasure_C1",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    temp_c1_ptr,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
    /*
  xTaskCreate(
    RelayControl,    // Function that should be called
    "RelayControl_C1",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );*/
  Serial.println(*temp_c1.Temp);

  //char tmp_str[10];
  //String(tmp).toCharArray(tmp_str,10);
  //pADCCharacteristic->setValue(tmp_str);
  delay(1000);
}
