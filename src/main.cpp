#include <Arduino.h>
#include "display.h"
#include "wifi_clock.h"
#include "orientation.h"
#include "rotation_animator.h"
#include "button.h"
#include "pomodoro.h"

DisplayManager display;
WifiClockManager wifiClock;
OrientationManager orientation;
RotationAnimator animator;
ButtonManager button;
PomodoroManager pomodoro;

Face lastRotationFace = Face::POS_X; // tracks last SIDE face we animated to

enum class AppMode { CLOCK, POMODORO };
AppMode mode = AppMode::CLOCK;

#define BUTTON_PIN 4  // this is the pin the button is connected to 

// Hardcoded per your plan - these will correspond to numbers printed on
// the 4 side faces once the case is designed/printed.
unsigned long durationForFace(Face f) {
    switch (f) {
        case Face::POS_X: return 15UL * 60;
        case Face::NEG_X: return 30UL * 60;
        case Face::POS_Y: return 45UL * 60;
        case Face::NEG_Y: return 60UL * 60;
        default: return 15UL * 60;
    }
}

String formatTime(int h, int m, int s) {
  char buf[9];
  sprintf(buf, "%02d:%02d:%02d", h, m, s);
  return String(buf);
}

String formatCountdown(unsigned long totalSeconds) {
  int m = totalSeconds / 60;
  int s = totalSeconds % 60;
  char buf[6];
  sprintf(buf, "%02d:%02d", m, s);
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

  button.begin(BUTTON_PIN);

  display.clearScreen();
  Serial.println("Clock started.");
}

void loop() {
  orientation.update();
  Face target = orientation.getTargetFace();

  // --- Button: switch between clock and pomodoro mode ---
  if (button.wasPressed()) {
    if (mode == AppMode::CLOCK) {
      mode = AppMode::POMODORO;
      pomodoro.start(durationForFace(target)); // duration locked in at the moment you press
    } else {
      mode = AppMode::CLOCK;
      pomodoro.reset();
    }
  }

  // --- Z-face pause/resume (only matters in pomodoro mode) ---
  if (mode == AppMode::POMODORO) {
    if (target == Face::POS_Z || target == Face::NEG_Z) {
      if (pomodoro.isRunning()) pomodoro.pause();
    } else {
      if (!pomodoro.isRunning()) pomodoro.resume();
    }
  }

  // --- Rotation logic - same in both modes ---
  if (orientation.isSideFace(target)) {
    if (target != lastRotationFace) {
      animator.setTarget(orientation.angleForFace(target));
      lastRotationFace = target;
    }
  }
  animator.update();

  // --- Decide what string to show based on mode ---
  static String lastDisplayStr = "";
  String displayStr;
  int h, m, s;

  if (mode == AppMode::CLOCK) {
    if (wifiClock.getTime(h, m, s)) {
      displayStr = formatTime(h, m, s);
    }
  } else {
    displayStr = formatCountdown(pomodoro.getRemainingSeconds());
  }

  if (displayStr != lastDisplayStr || !animator.isSettled()) {
    display.renderRotatedTime(displayStr, animator.getCurrentAngle());
    lastDisplayStr = displayStr;
  }

  delay(20);
}