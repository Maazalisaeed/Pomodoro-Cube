#include "orientation.h"

bool OrientationManager::begin() {
    Serial.println("Orientation: starting Wire.begin");
    Wire.begin(1, 2); // SDA=GPIO1, SCL=GPIO2
    Wire.setTimeOut(1000); // ms - prevents infinite hang, forces a real failure instead
    Serial.println("Orientation: Wire.begin done, calling mpu.begin()");
    
    byte status = mpu.begin();
    Serial.print("MPU6050 status: ");
    Serial.println(status);

    if (status != 0) {
        return false; // init failed
    }

    Serial.println("Calibrating gyro/accel offsets, keep cube still...");
    mpu.calcOffsets(); // takes ~1-2 sec, cube must be still and level
    Serial.println("Done calibrating.");

    return true;
}

float OrientationManager::readRawAngleDeg() {
    // atan2 gives -180..180, normalize to 0..359.9
    float deg = atan2(mpu.getAccX(), mpu.getAccY()) * 180.0f / PI;
    if (deg < 0) deg += 360.0f;
    return deg;
}

float OrientationManager::angleDiff(float a, float b) {
    float diff = fmod(a - b + 540.0f, 360.0f) - 180.0f;
    return diff;
}

void OrientationManager::update() {
    mpu.update();

    float ax = mpu.getAccX();
    float ay = mpu.getAccY();
    float tiltMagnitude = sqrt(ax * ax + ay * ay);

    // When nearly flat, X/Y are too small to give a reliable angle - skip this cycle entirely
    static constexpr float MIN_TILT = 0.15f;
    if (tiltMagnitude < MIN_TILT) {
        return; // hold whatever target we already have, don't even consider changing it
    }

    float raw = readRawAngleDeg();
    float distFromCurrent = fabs(angleDiff(raw, currentTarget));

    if (distFromCurrent <= 45.0f + HYSTERESIS_DEG) {
        candidateTarget = currentTarget;
        return;
    }

    int nearestBucket = ((int)roundf(raw / 90.0f) % 4) * 90;
    if (nearestBucket < 0) nearestBucket += 360;

    if (nearestBucket != candidateTarget) {
        candidateTarget = nearestBucket;
        candidateSince = millis();
    } else if (millis() - candidateSince >= CONFIRM_MS) {
        currentTarget = candidateTarget;
    }
}

int OrientationManager::getTargetOrientation() {
    return currentTarget;
}
void OrientationManager::printDebug() {
    Serial.print("AccX: ");
    Serial.print(mpu.getAccX());
    Serial.print("  AccY: ");
    Serial.print(mpu.getAccY());
    Serial.print("  AccZ: ");
    Serial.print(mpu.getAccZ());
    Serial.print("  -> target: ");
    Serial.println(getTargetOrientation());
}