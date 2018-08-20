/*
   Special thanks to Neil Kolban for his BLE library: https://github.com/nkolban/ESP32_BLE_Arduino
   Our work is built over his library.

  BLEMystique Heart Rate Monitor module by Pentester Academy.

  Terms of use:
  - This code must only be used for non-commercial purpose.
  - This repository (https://github.com/pentesteracademy/blemystique) must be mentioned as a reference.  
   
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// Hardcoded services and characterstics for Heart Rate Monitor profile
#define SERVICE_UUID        "0000180D-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID1 "00002A37-0000-1000-8000-00805F9B34FB"
#define DESCRIPTOR_UUID     "00002902-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID2 "00002A39-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID3 "00002A38-0000-1000-8000-00805F9B34FB"

// Loggging module
class MyCallbacks: public BLECharacteristicCallbacks {

    // When a characteristic is written
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i],HEX);
          Serial.print(" ");

        Serial.println();
        Serial.println("*********");
      }
    }

    // When a characteristic is read
    void onRead(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.println("############");
        Serial.print("Read characteristics value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i], HEX);
          Serial.print(" ");
          
        Serial.println();
        Serial.println("##########");
    }
};


BLECharacteristic *pCharacteristic1;
// Min and max heart rate to simulate
int min_rate=74;
int max_rate = 78;
uint8_t a[] = { 0x00, 0x4f, 0x00, 0x8f, 0x00, 0x00, 0x00, 0x00};


void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("Heart Rate Monitor");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Characteristic
  pCharacteristic1 = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID1,
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE |
                                         BLECharacteristic::PROPERTY_READ
                                       );
  pCharacteristic1->setValue(a,8);

  // Descriptor
  pCharacteristic1->addDescriptor(new BLE2902());

  pCharacteristic1->setCallbacks(new MyCallbacks());
 
  // Characteristic 2
  BLECharacteristic *pCharacteristic2 = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID2,
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_READ
                                       );
  uint8_t c[] = { 0x01 };
  pCharacteristic2->setValue(c, 1);

  pCharacteristic2->setCallbacks(new MyCallbacks());
  
  // Characteristic 3
  BLECharacteristic *pCharacteristic3 = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID3,
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_READ
                                       );
                                     
  uint8_t d[] = { 0x02 };
  pCharacteristic3->setValue(d, 1);

  pCharacteristic3->setCallbacks(new MyCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  
  Serial.println("Heart Rate Monitor up");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);

  // Changing the heart rate value
  uint8_t number = random(min_rate, max_rate);
  a[1] = number;  
  pCharacteristic1->setValue(a, 8);
  pCharacteristic1->notify();
  Serial.printf("Set heart rate to: %d\n",number);
}
