  #include <BLEDevice.h>
  #include <BLEUtils.h>
  #include <BLEServer.h>

  #define SERVICE_UUID "0000180A-0000-1000-8000-00805f9b34fb"
  #define CHARACTERISTIC_UUID "00002A58-0000-1000-8000-00805f9b34fb"

  BLECharacteristic *pCharacteristic;

  class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) override {
      String incomingValue = String((char*)pCharacteristic->getValue().c_str()); 
      // Receive the custom text from the user.
      //  We can edit the code such that the application being used can pre fetch the GPS coordinates and send to the device via BLE instead of the user inputs 

      if (incomingValue.length() > 0) {
        String response = "ESP32 says: " + incomingValue;
        pCharacteristic->setValue(response);
        pCharacteristic->notify();
      }
    }
  };

  class ServerCallbacks : public BLEServerCallbacks {
    void onDisconnect(BLEServer* pServer) override {
      Serial.println("Client disconnected");
      BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
      pAdvertising->start(); 
      Serial.println("Advertising again...");
    }
  };

  void setup() {
    BLEDevice::init("MotionSensor");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE |
      BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->setCallbacks(new CharacteristicCallbacks());
    pCharacteristic->setValue("ESP32 Ready to communicate...");
    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->start();
    pServer->setCallbacks(new ServerCallbacks());
  }

  void loop() {
  }
