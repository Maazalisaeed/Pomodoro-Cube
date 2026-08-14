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

Face OrientationManager::rawDominantFace(float ax, float ay, float az) {
    float absX = fabs(ax), absY = fabs(ay), absZ = fabs(az);

    if (absX > absY && absX > absZ) {
        return ax > 0 ? Face::POS_X : Face::NEG_X;
    } else if (absY > absX && absY > absZ) {
        return ay > 0 ? Face::POS_Y : Face::NEG_Y;
    } else {
        return az > 0 ? Face::POS_Z : Face::NEG_Z;
    }
}

float OrientationManager::magnitudeForFace(Face f, float ax, float ay, float az) {
    switch (f) {
        case Face::POS_X: return ax;
        case Face::NEG_X: return -ax;
        case Face::POS_Y: return ay;
        case Face::NEG_Y: return -ay;
        case Face::POS_Z: return az;
        case Face::NEG_Z: return -az;
    }
    return 0;
}

void OrientationManager::update() {
    mpu.update();

    float ax = mpu.getAccX();
    float ay = mpu.getAccY();
    float az = mpu.getAccZ();

    Serial.print("AccX: "); Serial.print(ax);
    Serial.print("  AccY: "); Serial.print(ay);
    Serial.print("  AccZ: "); Serial.println(az);

    Face rawFace = rawDominantFace(ax, ay, az);

    float currentStrength = magnitudeForFace(currentTarget, ax, ay, az);
    float rawStrength = magnitudeForFace(rawFace, ax, ay, az);

    if (rawFace == currentTarget) {
        candidateTarget = currentTarget;
        return;
    }

    if (rawStrength > currentStrength + SWITCH_MARGIN) {
        if (rawFace != candidateTarget) {
            candidateTarget = rawFace;
            candidateSince = millis();
        } else if (millis() - candidateSince >= CONFIRM_MS) {
            currentTarget = candidateTarget;
        }
    } else {
        candidateTarget = currentTarget;
    }
}

Face OrientationManager::getTargetFace() {
    return currentTarget;
}

const char* OrientationManager::faceName(Face f) {
    switch (f) {
        case Face::POS_X: return "+X";
        case Face::NEG_X: return "-X";
        case Face::POS_Y: return "+Y";
        case Face::NEG_Y: return "-Y";
        case Face::POS_Z: return "+Z";
        case Face::NEG_Z: return "-Z";
    }
    return "?";
}

void OrientationManager::printDebug() {
    float ax = mpu.getAccX();
    float ay = mpu.getAccY();
    float az = mpu.getAccZ();

    
}

bool OrientationManager::isSideFace(Face f) {
    return f == Face::POS_X || f == Face::NEG_X ||
           f == Face::POS_Y || f == Face::NEG_Y;
}

float OrientationManager::angleForFace(Face f) {
    switch (f) {
        case Face::POS_X: return 0.0f;
        case Face::POS_Y: return 90.0f;
        case Face::NEG_X: return 180.0f;
        case Face::NEG_Y: return 270.0f;
        default: return 0.0f;
    }
}