#include "display.h"
#include <Fonts/FreeSansBold24pt7b.h>

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

// Constructor: builds tft and canvas with their real settings.
// This colon syntax is called an "initializer list" - it's how you pass
// constructor arguments to member objects that need them at creation time.
DisplayManager::DisplayManager()
    : tft(TFT_CS, TFT_DC, TFT_RST), canvas(BOX_W, BOX_H) {
}

void DisplayManager::begin() {
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(GC9A01A_BLACK);

    tft.setFont(&FreeSansBold24pt7b);
    tft.setTextColor(GC9A01A_WHITE);

    canvas.setFont(&FreeSansBold24pt7b);
    canvas.setTextColor(GC9A01A_WHITE);
}

void DisplayManager::showMessage(String msg) {
    tft.setCursor(SCREEN_CENTER_X - 60, SCREEN_CENTER_Y);
    tft.print(msg);
}

void DisplayManager::drawCenteredTime(String timeStr) {
    int16_t x1, y1;
    uint16_t w, h;

    canvas.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);

    canvas.fillScreen(GC9A01A_BLACK);

    int16_t cursorX = (BOX_W / 2) - (w / 2) - x1;
    int16_t cursorY = (BOX_H / 2) - (h / 2) - y1;

    canvas.setCursor(cursorX, cursorY);
    canvas.print(timeStr);

    tft.drawRGBBitmap(BOX_X, BOX_Y, canvas.getBuffer(), BOX_W, BOX_H);
}
void DisplayManager::clearScreen() {
    tft.fillScreen(GC9A01A_BLACK);
}