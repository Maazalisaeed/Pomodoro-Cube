#pragma once
#include <Wire.h>
#include <MPU6050_light.h>

class OrientationManager {
public:
    bool begin();
    void update();          // call every loop - reads sensor + updates target
    void printDebug();      // prints raw + target values to Serial

    int getTargetOrientation(); // returns 0, 90, 180, or 270

private:
    MPU6050 mpu{Wire};

    int currentTarget = 0;
    int candidateTarget = 0;
    unsigned long candidateSince = 0;

    static constexpr float HYSTERESIS_DEG = 20.0f;
    static constexpr unsigned long CONFIRM_MS = 150;

    float readRawAngleDeg();
    static float angleDiff(float a, float b);
};