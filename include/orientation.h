#pragma once
#include <Wire.h>
#include <MPU6050_light.h>

class OrientationManager {
public:
    bool begin();
    void update();   // call every loop - reads sensor
    void printDebug(); // prints current values to Serial

private:
    MPU6050 mpu{Wire};
};