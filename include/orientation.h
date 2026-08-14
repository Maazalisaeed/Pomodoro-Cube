#pragma once
#include <Wire.h>
#include <MPU6050_light.h>

enum class Face { POS_X, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z };

class OrientationManager {
public:
    bool begin();
    void update();
    void printDebug();

    Face getTargetFace();
    static const char* faceName(Face f);
    bool isSideFace(Face f);       // true for +X/-X/+Y/-Y (the 4 rotation faces)
    float angleForFace(Face f);    // only meaningful for side faces

private:
    MPU6050 mpu{Wire};

    Face currentTarget = Face::POS_Z;
    Face candidateTarget = Face::POS_Z;
    unsigned long candidateSince = 0;

    // How much stronger a new axis must be than the current target's axis
    // before we even consider switching. This is the hysteresis margin.
    static constexpr float SWITCH_MARGIN = 0.25f;
    static constexpr unsigned long CONFIRM_MS = 150;

    Face rawDominantFace(float ax, float ay, float az);
    float magnitudeForFace(Face f, float ax, float ay, float az);
};