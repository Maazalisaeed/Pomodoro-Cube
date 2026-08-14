#include <Arduino.h>
#include "display.h"
#include "wifi_clock.h"
#include "orientation.h"
#include "rotation_animator.h"

DisplayManager display;
WifiClockManager wifiClock;
OrientationManager orientation;
RotationAnimator animator;

Face lastRotationFace = Face::POS_X; // tracks last SIDE face we animated to

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

  Face target = orientation.getTargetFace();

  if (orientation.isSideFace(target)) {
    if (target != lastRotationFace) {
      animator.setTarget(orientation.angleForFace(target));
      lastRotationFace = target;
    }
  }
  // else: Z+ or Z- - do nothing for now. This is intentionally a stub -
  // this is where the pomodoro pause/start logic will hook in later.

  animator.update();

  if (timeStr != lastTimeStr || !animator.isSettled()) {
    display.renderRotatedTime(timeStr, animator.getCurrentAngle());
    lastTimeStr = timeStr;
  }

  delay(20);
}