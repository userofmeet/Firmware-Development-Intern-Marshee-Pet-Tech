#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "esp_gap_ble_api.h"

BLEServer* pServerGlobal = nullptr;
BLECharacteristic* pCharacteristicTX;
bool deviceConnected = false;
int lastRSSI = -100;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class CharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) override {
    String value = pCharacteristic->getValue(); 

    if (value.length() > 0) {
      Serial.print("Received from client: ");
      Serial.println(value);
    }

    Serial.print("Last known RSSI: ");
    Serial.println(lastRSSI);
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println("Client connected");
  }

  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("Client disconnected");
    pServer->startAdvertising();
  }
};

void gapCallback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
  if (event == ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT) {
    if (param->read_rssi_cmpl.status == ESP_BT_STATUS_SUCCESS) {
      lastRSSI = param->read_rssi_cmpl.rssi;
      Serial.print("Updated RSSI: ");
      Serial.println(lastRSSI);
    } else {
      Serial.println("Failed to read RSSI");
    }
  }
}

void setup() {
  Serial.begin(115200);

  BLEDevice::init("ESP32_GPS_BLE");
  BLEDevice::setCustomGapHandler(gapCallback);  
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  pServerGlobal = pServer;

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristicTX = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  BLECharacteristic* pCharacteristicRX = pService->createCharacteristic(
                                           CHARACTERISTIC_UUID_RX,
                                           BLECharacteristic::PROPERTY_WRITE
                                         );

  pCharacteristicRX->setCallbacks(new CharacteristicCallbacks());

  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE Server ready & advertising...");
}

unsigned long lastRSSITime = 0;

void loop() {
  if (deviceConnected && (millis() - lastRSSITime > 5000)) {  
    lastRSSITime = millis();
    esp_ble_gap_read_rssi(BLEDevice::getAddress().getNative()[0]);  
  }
}
