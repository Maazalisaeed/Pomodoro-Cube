#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>

class ClockManager {
  public:
    // Constructor: pass in the hard-set starting time (24hr format)
    ClockManager(int startHour, int startMinute, int startSecond);

    // Call once in setup() - records boot reference point
    void begin();

    // Call every loop() - recalculates current time based on elapsed millis()
    void update();

    // Getters for current time
    int getHour();
    int getMinute();
    int getSecond();

    // Returns formatted string like "14:32:07"
    String getTimeString();

  private:
    int _startHour;
    int _startMinute;
    int _startSecond;

    unsigned long _bootMillis;  // millis() value when begin() was called

    int _currentHour;
    int _currentMinute;
    int _currentSecond;
};

#endif