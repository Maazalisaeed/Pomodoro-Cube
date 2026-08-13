#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include "wifi_clock.h"

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

#define SCREEN_CENTER_X 120
#define SCREEN_CENTER_Y 120


Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);
WifiClockManager wifiClock;

void drawCenteredTime(String timeStr) {
  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);

  tft.fillRect(SCREEN_CENTER_X - 100, SCREEN_CENTER_Y - 40,
               200, 80, GC9A01A_BLACK);

  int16_t cursorX = SCREEN_CENTER_X - (w / 2) - x1;
  int16_t cursorY = SCREEN_CENTER_Y - (h / 2) - y1;

  tft.setCursor(cursorX, cursorY);
  tft.print(timeStr);
}

// Turns raw hour/minute/second numbers into a "HH:MM:SS" string with
// leading zeros, e.g. 9,5,3 -> "09:05:03"
String formatTime(int h, int m, int s) {
  char buf[9]; // "HH:MM:SS" + null terminator = 9 characters
  sprintf(buf, "%02d:%02d:%02d", h, m, s);
  return String(buf);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(GC9A01A_BLACK);

  tft.setFont(&FreeSansBold24pt7b);
  tft.setTextColor(GC9A01A_WHITE);

  // Show something on screen while we wait for WiFi/NTP
  tft.setCursor(SCREEN_CENTER_X - 60, SCREEN_CENTER_Y);
  tft.print("Connecting...");

  bool ok = wifiClock.begin();
  if (!ok) {
    Serial.println("WiFi/NTP setup failed - check credentials/signal.");
  }

  tft.fillScreen(GC9A01A_BLACK); // clear the "Connecting..." message
  Serial.println("Clock started.");
}

void loop() {
  int h, m, s;
  if (wifiClock.getTime(h, m, s)) {
    drawCenteredTime(formatTime(h, m, s));
  }

  delay(200);
}