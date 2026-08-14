// pomodoro.cpp
#include "pomodoro.h"

void PomodoroManager::start(unsigned long duration) {
    durationSeconds = duration;
    startedAtMs = millis();
    running = true;
}

void PomodoroManager::pause() {
    if (running) {
        unsigned long elapsed = (millis() - startedAtMs) / 1000;
        pausedRemainingSeconds = (elapsed < durationSeconds) ? (durationSeconds - elapsed) : 0;
        running = false;
    }
}

void PomodoroManager::resume() {
    if (!running && pausedRemainingSeconds > 0) {
        durationSeconds = pausedRemainingSeconds;
        startedAtMs = millis();
        running = true;
    }
}

void PomodoroManager::reset() {
    running = false;
    durationSeconds = 0;
    pausedRemainingSeconds = 0;
}

bool PomodoroManager::isRunning() { return running; }

unsigned long PomodoroManager::getRemainingSeconds() {
    if (!running) return pausedRemainingSeconds;
    unsigned long elapsed = (millis() - startedAtMs) / 1000;
    return (elapsed >= durationSeconds) ? 0 : durationSeconds - elapsed;
}