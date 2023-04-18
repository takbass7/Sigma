#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "Sigma.h"

#define SPEED 115200
#define LED1 12
#define LED2 14
#define LED3 13

#define SERVICE_UUID        "9ab83c41-05ac-4f2a-bede-3dee193f24b0"
#define CHARACTERISTIC_UUID "af0357e2-be4a-4c3e-bba6-070c7aa15a45"

#define BLE_NAME "MKII"

//--- Global Varible Define ---
BLEServer* pServer = NULL;
BLEService* pService = NULL;
BLECharacteristic* pCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
Sigma* sg = NULL;

//--- Server Call Back Class ---
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        BLEDevice::startAdvertising();
        Serial.println("on connect");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("on disconnect");
    }
};


class MyCallbacks: public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.print("Client Write: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();
      }
    }

    void onRead(BLECharacteristic *pCharacteristic) {
        Serial.print("Client Read ...");
        //std::string value = pCharacteristic->getValue();
        //pCharacteristic->setValue("R " + value);
    }

    void onNotify(BLECharacteristic* pCharacteristic) {
        Serial.print("onNotify ...");



    }

    void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) {
        Serial.print("onStatus ...");
    }

};

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3,OUTPUT);

  sg = new Sigma();
  sg->println("BLE Client V 2.0");

  //Serial.begin(SPEED);
  //Serial.println("BLE Server V 2.0");

  //--- Bluetooth BLE----
  BLEDevice::init(BLE_NAME);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                          BLECharacteristic::PROPERTY_READ |
                                          BLECharacteristic::PROPERTY_WRITE |
                                          BLECharacteristic::PROPERTY_NOTIFY
                                       );

  // Create a BLE Descriptor
  BLEDescriptor *pStatusDescriptor = new BLE2902();
  pCharacteristic->addDescriptor(pStatusDescriptor);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Z");
  pService->start();


  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  //pAdvertising->setScanResponse(true);
  pAdvertising->setScanResponse(false);
  //pAdvertising->setMinPreferred(0x0);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  //pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Wait for Client...");
}

void loop() {
    // notify changed value

    if (deviceConnected) {
        if ( pCharacteristic) {
            Serial.print("=");
            //pCharacteristic->setValue("A");
            pCharacteristic->setValue((uint8_t*)&value, 4);
            pCharacteristic->notify( );
            value++;
        }

        delay(1000); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
        Serial.print("+");
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
        Serial.print("*");
    }

    digitalWrite(LED3,HIGH);
    delay(1000);
    digitalWrite(LED3,LOW);
    delay(1000);
    digitalWrite(LED1,HIGH);
    delay(1000);
    digitalWrite(LED1,LOW);
    delay(1000);
    digitalWrite(LED2,HIGH);
    delay(1000);
    digitalWrite(LED2,LOW);
    delay(1000);
    Serial.println("suki desu.");

}

void serialEvent() {
  //--- If Connected then Ready For Input
  //if (connected) { sg->readln(); }
}