#pragma once
#include <Arduino.h>

class WifiClockManager {
public:
    // Call once in setup(): brings up the WiFiManager captive portal if
    // needed, connects to WiFi, and syncs time from NTP
    bool begin();

    // Call any time to get the current local hour/min/sec
    // Returns false if time isn't synced yet
    bool getTime(int &hour, int &minute, int &second);

private:
    bool timeSynced = false;
};