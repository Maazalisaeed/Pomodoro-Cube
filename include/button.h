// button.h
#pragma once
#include <Arduino.h>

class ButtonManager {
public:
    void begin(uint8_t pin);
    bool wasPressed(); // true exactly once per clean press

private:
    uint8_t pin;
    bool lastRawState = true;
    bool lastStableState = true;
    unsigned long lastChangeTime = 0;
    static constexpr unsigned long DEBOUNCE_MS = 30;
};