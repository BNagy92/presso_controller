#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <PID_v1.h>
#include "temp_measurment.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define RELAY_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define ADC_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"

#define SSR_RELAY_PIN 23

#define TMP_ADC_PIN 13

#define N_MEAS (100)

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
unsigned long windowStartTime;
int WindowSize = 5000;
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

void setup() {
  //initialize the variables we're linked to

  
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode (LED_BUILTIN, OUTPUT);
  pinMode (SSR_RELAY_PIN, OUTPUT);
  init_temp(TMP_ADC_PIN);
  windowStartTime = millis();
  setupBLE();
  //analogSetAttenuation(TMP_ADC_PIN, 6db);
  Input = (double) ADC_LUT[(uint32_t) analogRead(TMP_ADC_PIN)];
  Setpoint = 100;
  //turn the PID on
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetMode(AUTOMATIC);
}


const double R=100.0;

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("Csumi!");
  

  Input = measure_temp(TMP_ADC_PIN);
  myPID.Compute();
  unsigned long now = millis();
  if (now - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (Output > now - windowStartTime) digitalWrite(SSR_RELAY_PIN, HIGH);
  else digitalWrite(SSR_RELAY_PIN, LOW);
  //char tmp_str[10];
  //String(tmp).toCharArray(tmp_str,10);
  //pADCCharacteristic->setValue(tmp_str);
  delay(500);
}
