#include "pomodoro.h"

const unsigned long PomodoroManager::WORK_MIN[4]        = {1, 30, 45, 60};
const unsigned long PomodoroManager::SHORT_BREAK_MIN[4] = {5, 7, 10, 15};
const unsigned long PomodoroManager::LONG_BREAK_MIN[4]  = {15, 20, 30, 60};

void PomodoroManager::startCycle(int idx) {
    levelIndex = idx;
    completedWorkCount = 0;
    beginPhase(PomodoroPhase::WORK);
}

void PomodoroManager::beginPhase(PomodoroPhase p) {
    phase = p;
    unsigned long minutes;
    switch (p) {
        case PomodoroPhase::WORK:        minutes = WORK_MIN[levelIndex]; break;
        case PomodoroPhase::SHORT_BREAK: minutes = SHORT_BREAK_MIN[levelIndex]; break;
        case PomodoroPhase::LONG_BREAK:  minutes = LONG_BREAK_MIN[levelIndex]; break;
    }
    durationSeconds = minutes * 60UL;
    startedAtMs = millis();
    running = true;
}

void PomodoroManager::update() {
    if (!running) return;
    if (getRemainingSeconds() == 0) {
        // Current phase just hit zero - decide what comes next
        if (phase == PomodoroPhase::WORK) {
            completedWorkCount++;
            if (completedWorkCount >= 4) {
                beginPhase(PomodoroPhase::LONG_BREAK);
            } else {
                beginPhase(PomodoroPhase::SHORT_BREAK);
            }
        } else if (phase == PomodoroPhase::SHORT_BREAK) {
            beginPhase(PomodoroPhase::WORK);
        } else { // LONG_BREAK just finished - full cycle done, start a fresh one
            completedWorkCount = 0;
            beginPhase(PomodoroPhase::WORK);
        }
    }
}

void PomodoroManager::pause() {
    if (running) {
        unsigned long elapsed = (millis() - startedAtMs) / 1000;
        pausedRemainingSeconds = (elapsed < durationSeconds) ? (durationSeconds - elapsed) : 0;
        running = false;
    }
}

void PomodoroManager::resume() {
    if (!running) {
        durationSeconds = pausedRemainingSeconds;
        startedAtMs = millis();
        running = true;
    }
}

void PomodoroManager::reset() {
    running = false;
    phase = PomodoroPhase::WORK;
    completedWorkCount = 0;
    durationSeconds = 0;
    pausedRemainingSeconds = 0;
}

bool PomodoroManager::isRunning() { return running; }

unsigned long PomodoroManager::getRemainingSeconds() {
    if (!running) return pausedRemainingSeconds;
    unsigned long elapsed = (millis() - startedAtMs) / 1000;
    return (elapsed >= durationSeconds) ? 0 : durationSeconds - elapsed;
}

PomodoroPhase PomodoroManager::getPhase() { return phase; }
int PomodoroManager::getCompletedWorkCount() { return completedWorkCount; }