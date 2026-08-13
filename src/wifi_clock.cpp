#include "wifi_clock.h"
#include <WiFiManager.h>
#include <time.h>

bool WifiClockManager::begin() {
    WiFiManager wm;

    // autoConnect tries saved WiFi credentials first. If that fails
    // (no saved network, or it's unreachable), it starts a hotspot
    // named "PomodoroCube-Setup" and blocks here until the user
    // connects a phone to it and submits real WiFi credentials.
    bool connected = wm.autoConnect("PomodoroCube-Setup");

    if (!connected) {
        Serial.println("WiFiManager failed to connect");
        return false;
    }
    Serial.println("WiFi connected");

    // Mountain Time: UTC-7, with DST rules baked in
    configTzTime("MST7MDT,M3.2.0,M11.1.0", "pool.ntp.org", "time.nist.gov");

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10000)) { // 10 second timeout
        Serial.println("Failed to get NTP time");
        return false;
    }

    timeSynced = true;
    return true;
}

bool WifiClockManager::getTime(int &hour, int &minute, int &second) {
    if (!timeSynced) return false;

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return false;

    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
    second = timeinfo.tm_sec;
    return true;
}