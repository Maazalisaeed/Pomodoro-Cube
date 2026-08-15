#include "display.h"
#include <Fonts/FreeSansBold24pt7b.h>

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

// Constructor: builds tft and canvas with their real settings.
// This colon syntax is called an "initializer list" - it's how you pass
// constructor arguments to member objects that need them at creation time.
DisplayManager::DisplayManager()
    : tft(TFT_CS, TFT_DC, TFT_RST), canvas(BOX_W, BOX_H), renderBuf(RENDER_SIZE, RENDER_SIZE) {
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
void DisplayManager::renderRotatedTime(String timeStr, float angleDeg) {
    int16_t x1, y1;
    uint16_t w, h;

    canvas.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
    canvas.fillScreen(GC9A01A_BLACK);

    int16_t cursorX = (BOX_W / 2) - (w / 2) - x1;
    int16_t cursorY = (BOX_H / 2) - (h / 2) - y1;

    canvas.setCursor(cursorX, cursorY);
    canvas.print(timeStr);

    drawRotated(angleDeg);
}

void DisplayManager::drawRotated(float angleDeg) {
    float rad = angleDeg * PI / 180.0f;
    float cosA = cos(rad);
    float sinA = sin(rad);

    int srcCx = BOX_W / 2, srcCy = BOX_H / 2;
    int dstCx = RENDER_SIZE / 2, dstCy = RENDER_SIZE / 2;

    uint16_t *srcBuf = canvas.getBuffer();

    renderBuf.fillScreen(GC9A01A_BLACK); // clears in RAM - cheap, no SPI involved

    for (int y = 0; y < RENDER_SIZE; y++) {
        for (int x = 0; x < RENDER_SIZE; x++) {
            int dx = x - dstCx;
            int dy = y - dstCy;
            int srcX = (int)roundf(dx * cosA - dy * sinA) + srcCx;
            int srcY = (int)roundf(dx * sinA + dy * cosA) + srcCy;

            if (srcX >= 0 && srcX < BOX_W && srcY >= 0 && srcY < BOX_H) {
                renderBuf.drawPixel(x, y, srcBuf[srcY * BOX_W + srcX]); // RAM write, not SPI - very fast
            }
        }
    }

    // The ONE actual SPI transfer for this whole frame - this is what kills the jitter
    tft.drawRGBBitmap(RENDER_X, RENDER_Y, renderBuf.getBuffer(), RENDER_SIZE, RENDER_SIZE);
}
void DisplayManager::drawRingInto(GFXcanvas16 &buf, float fraction, uint16_t color) {
    if (fraction < 0) fraction = 0;
    if (fraction > 1) fraction = 1;

    int cx = RENDER_SIZE / 2, cy = RENDER_SIZE / 2;
    int outerR = RING_MAX_RADIUS;       // fixed size now - stays put at the rim
    int innerR = outerR - RING_THICKNESS;

    float sweepDeg = fraction * 360.0f; // how much of the circle is still "remaining"

    for (int y = -outerR; y <= outerR; y++) {
        for (int x = -outerR; x <= outerR; x++) {
            float r = sqrtf((float)(x * x + y * y));
            if (r < innerR || r > outerR) continue; // only care about the ring band

            // Angle measured clockwise from the top (12 o'clock), like a clock face
            float clockAngle = atan2f((float)x, (float)-y) * 180.0f / PI;
            if (clockAngle < 0) clockAngle += 360.0f;

            if (clockAngle < sweepDeg) {
                buf.drawPixel(cx + x, cy + y, color);
            }
        }
    }
}

void DisplayManager::renderFrame(String text, float angleDeg, float ringFraction, uint16_t ringColor, bool dashedRing) {
    int16_t x1, y1;
    uint16_t w, h;

    canvas.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    canvas.fillScreen(GC9A01A_BLACK);
    int16_t cursorX = (BOX_W / 2) - (w / 2) - x1;
    int16_t cursorY = (BOX_H / 2) - (h / 2) - y1;
    canvas.setCursor(cursorX, cursorY);
    canvas.print(text);

    float rad = angleDeg * PI / 180.0f;
    float cosA = cos(rad);
    float sinA = sin(rad);
    int srcCx = BOX_W / 2, srcCy = BOX_H / 2;
    int dstCx = RENDER_SIZE / 2, dstCy = RENDER_SIZE / 2;
    uint16_t *srcBuf = canvas.getBuffer();

    renderBuf.fillScreen(GC9A01A_BLACK);

    if (dashedRing) {
        drawDashedRingInto(renderBuf, 11, 15.0f, ringColor); // 11 dashes, ~9 deg wide each
    } else {
        drawRingInto(renderBuf, ringFraction, ringColor);
    }

    for (int y = 0; y < RENDER_SIZE; y++) {
        for (int x = 0; x < RENDER_SIZE; x++) {
            int dx = x - dstCx;
            int dy = y - dstCy;
            int srcX = (int)roundf(dx * cosA - dy * sinA) + srcCx;
            int srcY = (int)roundf(dx * sinA + dy * cosA) + srcCy;

            if (srcX >= 0 && srcX < BOX_W && srcY >= 0 && srcY < BOX_H) {
                uint16_t color = srcBuf[srcY * BOX_W + srcX];
                if (color != GC9A01A_BLACK) {
                    renderBuf.drawPixel(x, y, color);
                }
            }
        }
    }

    tft.drawRGBBitmap(RENDER_X, RENDER_Y, renderBuf.getBuffer(), RENDER_SIZE, RENDER_SIZE);
}
void DisplayManager::drawDashedRingInto(GFXcanvas16 &buf, int numDashes, float dashWidthDeg, uint16_t color) {
    int cx = RENDER_SIZE / 2, cy = RENDER_SIZE / 2;
    int outerR = RING_MAX_RADIUS;
    int innerR = outerR - RING_THICKNESS;

    float slotDeg = 360.0f / numDashes; // spacing between dash centers - ~32.7 deg for 11 dashes

    for (int y = -outerR; y <= outerR; y++) {
        for (int x = -outerR; x <= outerR; x++) {
            float r = sqrtf((float)(x * x + y * y));
            if (r < innerR || r > outerR) continue;

            float clockAngle = atan2f((float)x, (float)-y) * 180.0f / PI;
            if (clockAngle < 0) clockAngle += 360.0f;

            // Where does this pixel fall inside its own 32.7-degree slot?
            float posInSlot = fmodf(clockAngle, slotDeg);
            float slotCenter = slotDeg / 2.0f;
            float halfDash = dashWidthDeg / 2.0f;

            // Only draw if close to the CENTER of the slot - leaves a gap on both sides
            if (fabsf(posInSlot - slotCenter) < halfDash) {
                buf.drawPixel(cx + x, cy + y, color);
            }
        }
    }
}