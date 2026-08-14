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


private:
    void drawRotated(float angleDeg);
    Adafruit_GC9A01A tft;
    GFXcanvas16 canvas;
    GFXcanvas16 renderBuf; // full RENDER_SIZE buffer - built in RAM, pushed to screen in one shot

    // Screen + text box layout constants
    static const int SCREEN_CENTER_X = 120;
    static const int SCREEN_CENTER_Y = 120;
    static const int BOX_W = 200;
    static const int BOX_H = 80;
    static const int BOX_X = SCREEN_CENTER_X - BOX_W / 2;
    static const int BOX_Y = SCREEN_CENTER_Y - BOX_H / 2;
    static const int RENDER_SIZE = 220; // big enough to hold the 200x80 text at any rotation angle
    static const int RENDER_X = SCREEN_CENTER_X - RENDER_SIZE / 2;
    static const int RENDER_Y = SCREEN_CENTER_Y - RENDER_SIZE / 2;
public:
    // Constructor: sets up the tft and canvas objects with the right pins/size
    DisplayManager();

};
