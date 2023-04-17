#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SPEED 115200
#define LED1 12
#define LED2 14
#define LED3 13

#define SERVICE_UUID        "9ab83c41-05ac-4f2a-bede-3dee193f24b0"
#define CHARACTERISTIC_UUID "af0357e2-be4a-4c3e-bba6-070c7aa15a45"

#define BLE_NAME "MKII"

//--- Global Varible Define ---
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

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
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
      }
    }

    void onRead(BLECharacteristic *pCharacteristic) {
        Serial.print("Read ...");
    }
};

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3,OUTPUT);

  Serial.begin(SPEED);
  Serial.println("BLE Server V 2.0");
  
  //--- Bluetooth BLE----
  BLEDevice::init(BLE_NAME);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
 /*  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                          BLECharacteristic::PROPERTY_READ |
                                          BLECharacteristic::PROPERTY_WRITE |
                                          BLECharacteristic::PROPERTY_NOTIFY |
                                          BLECharacteristic::PROPERTY_INDICATE
                                       ); */

BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                          BLECharacteristic::PROPERTY_READ |
                                          BLECharacteristic::PROPERTY_WRITE 
                                       );

  // Create a BLE Descriptor                                     
  pCharacteristic->addDescriptor(new BLE2902());

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello Takky Sama");

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
        //pCharacteristic->setValue((uint8_t*)&value, 4);
        //pCharacteristic->notify();
        value++;
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
        //Serial.print("+");
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