#include "clock.h"

ClockManager::ClockManager(int startHour, int startMinute, int startSecond) {
  _startHour = startHour;
  _startMinute = startMinute;
  _startSecond = startSecond;
}

void ClockManager::begin() {
  _bootMillis = millis();
  _currentHour = _startHour;
  _currentMinute = _startMinute;
  _currentSecond = _startSecond;
}

void ClockManager::update() {
  unsigned long elapsedSeconds = (millis() - _bootMillis) / 1000;

  unsigned long totalSeconds = 
      (_startHour * 3600UL) + (_startMinute * 60UL) + _startSecond + elapsedSeconds;

  // Wrap around every 24 hours (86400 seconds)
  totalSeconds = totalSeconds % 86400UL;

  _currentHour = totalSeconds / 3600;
  _currentMinute = (totalSeconds % 3600) / 60;
  _currentSecond = totalSeconds % 60;
}

int ClockManager::getHour() {
  return _currentHour;
}

int ClockManager::getMinute() {
  return _currentMinute;
}

int ClockManager::getSecond() {
  return _currentSecond;
}

String ClockManager::getTimeString() {
  char buf[9]; // "HH:MM:SS\0"
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", _currentHour, _currentMinute, _currentSecond);
  return String(buf);
}