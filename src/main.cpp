#include <Arduino.h>
#include "display.h"
#include "wifi_clock.h"
#include "orientation.h"

DisplayManager display;
WifiClockManager wifiClock;
OrientationManager orientation;

String formatTime(int h, int m, int s) {
  char buf[9];
  sprintf(buf, "%02d:%02d:%02d", h, m, s);
  return String(buf);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  display.begin();
  display.showMessage("Connecting...");

  bool ok = wifiClock.begin();
  
  if (!ok) {
    Serial.println("WiFi/NTP setup failed - check credentials/signal.");
  }

  if (!orientation.begin()) {
    Serial.println("MPU6050 not found - check wiring!");
  }

  display.clearScreen();
  Serial.println("Clock started.");
}

void loop() {
  int h, m, s;
  if (wifiClock.getTime(h, m, s)) {
    display.drawCenteredTime(formatTime(h, m, s));
  }

  orientation.update();
  orientation.printDebug();

  delay(1000);
}