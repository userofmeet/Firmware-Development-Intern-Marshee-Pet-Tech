// This file contains a sample code to check the simulation for checking the microcontroller and QMI8658 IMU 
// which is intergrated in the controller itself. You can upload it using the Arduino IDE and select the module 
// as ESP32-S3 controller. The credits for this code goes to Swetha Kanathi and the firmware team.

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"
#include <ArduinoBLE.h>  // Include BLE library

#define USE_WIRE
#define SENSOR_SDA 11
#define SENSOR_SCL 10
#define SENSOR_IRQ -1
#define IMU_CS 5

#define MOTION_THRESHOLD_ACC 2.0f
#define MOTION_THRESHOLD_GYRO 1.5f
#define LOW_PASS_ALPHA 0.1f // Low-pass filter coefficient

SensorQMI8658 qmi;
IMUdata acc;
IMUdata gyr;

float accMagFiltered = 0;
float gyroMagFiltered = 0;

// Calibration offsets
float accOffsetX = 0, accOffsetY = 0, accOffsetZ = 0;
float gyrOffsetX = 0, gyrOffsetY = 0, gyrOffsetZ = 0;

// Declare BLE service and characteristics
BLEService motionService("180A");  
BLECharacteristic motionCharacteristic("2A57", BLERead | BLENotify, 512);  
BLECharacteristic sensorDataCharacteristic("2A58", BLERead | BLENotify, 512);  

float calculateMagnitude(float x, float y, float z) {
    return sqrt(x * x + y * y + z * z);
}

float lowPassFilter(float currentValue, float previousValue) {
    return LOW_PASS_ALPHA * currentValue + (1 - LOW_PASS_ALPHA) * previousValue;
}

bool isInMotion(float accMag, float gyroMag) {
    return (accMag > MOTION_THRESHOLD_ACC || gyroMag > MOTION_THRESHOLD_GYRO);
}

void calibrateSensors() {
    Serial.println("Calibrating sensors... Hold the sensor still for 5 seconds.");
    float accSumX = 0, accSumY = 0, accSumZ = 0;
    float gyrSumX = 0, gyrSumY = 0, gyrSumZ = 0;

    for (int i = 0; i < 100; i++) {
        if (qmi.getAccelerometer(acc.x, acc.y, acc.z) && qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
            accSumX += acc.x;
            accSumY += acc.y;
            accSumZ += acc.z;
            gyrSumX += gyr.x;
            gyrSumY += gyr.y;
            gyrSumZ += gyr.z;
        }
        delay(20);
    }

    accOffsetX = accSumX / 100;
    accOffsetY = accSumY / 100;
    accOffsetZ = accSumZ / 100;
    gyrOffsetX = gyrSumX / 100;
    gyrOffsetY = gyrSumY / 100;
    gyrOffsetZ = gyrSumZ / 100;

    Serial.println("Calibration complete.");
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Initialize BLE
    if (!BLE.begin()) {
        Serial.println("Failed to initialize BLE!");
        while (1);
    }

    BLE.setLocalName("MotionSensor");
    BLE.setAdvertisedService(motionService);
    motionService.addCharacteristic(motionCharacteristic);
    motionService.addCharacteristic(sensorDataCharacteristic);
    BLE.addService(motionService);

    motionCharacteristic.writeValue("Initializing...");
    sensorDataCharacteristic.writeValue("Waiting for data...");

    BLE.advertise();
    Serial.println("BLE Initialized. Advertising...");

#ifdef USE_WIRE
    if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
        Serial.println("Failed to find QMI8658!");
        while (1) delay(1000);
    }
#else
    if (!qmi.begin(IMU_CS)) {
        Serial.println("Failed to find QMI8658!");
        while (1) delay(1000);
    }
#endif

    Serial.print("Device ID:");
    Serial.println(qmi.getChipID(), HEX);

    qmi.configAccelerometer(
        SensorQMI8658::ACC_RANGE_8G,
        SensorQMI8658::ACC_ODR_1000Hz,
        SensorQMI8658::LPF_MODE_0
    );

    qmi.configGyroscope(
        SensorQMI8658::GYR_RANGE_64DPS,
        SensorQMI8658::GYR_ODR_896_8Hz,
        SensorQMI8658::LPF_MODE_3
    );

    qmi.enableGyroscope();
    qmi.enableAccelerometer();
    qmi.dumpCtrlRegister();

    Serial.println("Sensor Initialized. Starting motion detection...");

    calibrateSensors();
}

void loop() {
    BLE.poll();  // Handle BLE events

    if (qmi.getDataReady()) {
        float accMag = 0;
        float gyroMag = 0;

        if (qmi.getAccelerometer(acc.x, acc.y, acc.z) && qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
            // Apply calibration offsets
            acc.x -= accOffsetX;
            acc.y -= accOffsetY;
            acc.z -= accOffsetZ;
            gyr.x -= gyrOffsetX;
            gyr.y -= gyrOffsetY;
            gyr.z -= gyrOffsetZ;

            // Apply low-pass filtering
            acc.x = lowPassFilter(acc.x, accMagFiltered);
            acc.y = lowPassFilter(acc.y, accMagFiltered);
            acc.z = lowPassFilter(acc.z, accMagFiltered);

            gyr.x = lowPassFilter(gyr.x, gyroMagFiltered);
            gyr.y = lowPassFilter(gyr.y, gyroMagFiltered);
            gyr.z = lowPassFilter(gyr.z, gyroMagFiltered);

            accMag = calculateMagnitude(acc.x, acc.y, acc.z);
            gyroMag = calculateMagnitude(gyr.x, gyr.y, gyr.z);

            bool motionState = isInMotion(accMag, gyroMag);

            if (motionState) {
                Serial.println("STATE: Motion Detected");
                Serial.printf("ACC: %.2f, %.2f, %.2f (mag: %.2f)\n", acc.x, acc.y, acc.z, accMag);
                Serial.printf("GYRO: %.2f, %.2f, %.2f (mag: %.2f)\n", gyr.x, gyr.y, gyr.z, gyroMag);

                motionCharacteristic.writeValue("STATE: Motion Detected");

                char sensorData[100];  // Buffer for sensor data transmission
                snprintf(sensorData, sizeof(sensorData), 
                         "ACC: %.2f, %.2f, %.2f (mag: %.2f)\nGYRO: %.2f, %.2f, %.2f (mag: %.2f)", 
                         acc.x, acc.y, acc.z, accMag, gyr.x, gyr.y, gyr.z, gyroMag);
                sensorDataCharacteristic.writeValue(sensorData);
            } else {
                Serial.println("STATE: Resting");
                motionCharacteristic.writeValue("STATE: Resting");

                char sensorData[100];  // Buffer for sensor data transmission
                snprintf(sensorData, sizeof(sensorData), 
                         "ACC: %.2f, %.2f, %.2f (mag: %.2f)\nGYRO: %.2f, %.2f, %.2f (mag: %.2f)", 
                         acc.x, acc.y, acc.z, accMag, gyr.x, gyr.y, gyr.z, gyroMag);
                sensorDataCharacteristic.writeValue(sensorData);
            }
        }
    }
    delay(100);
