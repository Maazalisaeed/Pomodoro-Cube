#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

class DisplayManager {
public:
    // Call once in setup(): initializes the physical screen and prepares
    // the off-screen buffer
    void begin();

    // Draws a status message directly to the screen (used before WiFi/time
    // is ready - no buffer needed since it's a one-off, not repeated draws)
    void showMessage(String msg);

    // Draws the given time string flicker-free using the off-screen buffer
    void drawCenteredTime(String timeStr);
    void clearScreen();
    void renderRotatedTime(String timeStr, float angleDeg); // new: rotated version of drawCenteredTime
    void renderFrame(String text, float angleDeg, float ringFraction, uint16_t ringColor, bool dashedRing);

private:
    void drawRotated(float angleDeg);// this function deal with rotation
    void drawRingInto(GFXcanvas16 &buf, float fraction, uint16_t color); // this function deal with the drawig the ring aroudn the pomodoro timer
    void drawDashedRingInto(GFXcanvas16 &buf, int numDashes, float dashWidthDeg, uint16_t color);// this function deal with drawing a dashed ring around clock face

    Adafruit_GC9A01A tft;
    GFXcanvas16 canvas;
    GFXcanvas16 renderBuf;

    static const int SCREEN_CENTER_X = 120;
    static const int SCREEN_CENTER_Y = 120;
    static const int BOX_W = 200;
    static const int BOX_H = 80;
    static const int BOX_X = SCREEN_CENTER_X - BOX_W / 2;
    static const int BOX_Y = SCREEN_CENTER_Y - BOX_H / 2;
    static const int RENDER_SIZE = 240;  // CHANGED from 220 to 240 - full screen now
    static const int RENDER_X = SCREEN_CENTER_X - RENDER_SIZE / 2;
    static const int RENDER_Y = SCREEN_CENTER_Y - RENDER_SIZE / 2;
    static const int RING_THICKNESS = 6;    // NEW
    static const int RING_MAX_RADIUS = 118; // NEW
public:
    // Constructor: sets up the tft and canvas objects with the right pins/size
    DisplayManager();

};
