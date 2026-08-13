#include <Arduino.h>
#include "display.h"
#include "wifi_clock.h"
#include "orientation.h"
#include "rotation_animator.h"

DisplayManager display;
WifiClockManager wifiClock;
OrientationManager orientation;
RotationAnimator animator;

int lastKnownTarget = 0;

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
  static String lastTimeStr = "";
  String timeStr;
  if (wifiClock.getTime(h, m, s)) {
    timeStr = formatTime(h, m, s);
  }

  orientation.update();

  int newTarget = orientation.getTargetOrientation();
  if (newTarget != lastKnownTarget) {
    animator.setTarget((float)newTarget);
    lastKnownTarget = newTarget;
  }

  animator.update();

  if (timeStr != lastTimeStr || !animator.isSettled()) {
    display.renderRotatedTime(timeStr, animator.getCurrentAngle());
    lastTimeStr = timeStr;
  }

  delay(20);
}