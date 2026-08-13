#include "rotation_animator.h"
#include <math.h>

void RotationAnimator::begin(float startAngle) {
    current = target = startAngle;
    lastUpdateMs = millis();
}

void RotationAnimator::setTarget(float newTarget) {
    while (newTarget - current > 180) newTarget -= 360;
    while (newTarget - current < -180) newTarget += 360;
    target = newTarget;
}

void RotationAnimator::update() {
    unsigned long now = millis();
    float dt = (now - lastUpdateMs) / 1000.0f;
    lastUpdateMs = now;
    if (dt > 0.05f) dt = 0.05f;

    float error = target - current;
    float accel = STIFFNESS * error - DAMPING * velocity;
    velocity += accel * dt;
    current  += velocity * dt;
}

bool RotationAnimator::isSettled() const {
    return fabs(target - current) < 0.5f && fabs(velocity) < 1.0f;
}