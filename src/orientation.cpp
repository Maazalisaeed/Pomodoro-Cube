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

void OrientationManager::update() {
    mpu.update();
}

void OrientationManager::printDebug() {
    Serial.print("AngleX: ");
    Serial.print(mpu.getAngleX());
    Serial.print("  AngleY: ");
    Serial.print(mpu.getAngleY());
    Serial.print("  AngleZ: ");
    Serial.println(mpu.getAngleZ());

    Serial.print("AccX: ");
    Serial.print(mpu.getAccX());
    Serial.print("  AccY: ");
    Serial.print(mpu.getAccY());
    Serial.print("  AccZ: ");
    Serial.println(mpu.getAccZ());
}