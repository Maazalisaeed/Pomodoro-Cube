#pragma once
#include <Arduino.h>

class RotationAnimator {
public:
    void begin(float startAngle);
    void setTarget(float newTarget);
    void update();
    float getCurrentAngle() const { return current; }
    bool isSettled() const;

private:
    float current = 0, velocity = 0, target = 0;
    unsigned long lastUpdateMs = 0;

    static constexpr float STIFFNESS = 180.0f;
    static constexpr float DAMPING   = 14.0f;
};