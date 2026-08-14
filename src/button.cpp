// button.cpp
#include "button.h"

void ButtonManager::begin(uint8_t p) {
    pin = p;
    pinMode(pin, INPUT_PULLUP); // internal pull-up - button just needs to short pin to GND, no resistor needed
    lastRawState = lastStableState = digitalRead(pin);
}

bool ButtonManager::wasPressed() {
    bool raw = digitalRead(pin);

    if (raw != lastRawState) {
        lastChangeTime = millis();
        lastRawState = raw;
    }

    // Only trust the new state once it's held steady past the debounce window
    if (millis() - lastChangeTime > DEBOUNCE_MS && raw != lastStableState) {
        lastStableState = raw;
        if (lastStableState == LOW) { // LOW = pressed, since pull-up idles HIGH
            return true;
        }
    }
    return false;
}