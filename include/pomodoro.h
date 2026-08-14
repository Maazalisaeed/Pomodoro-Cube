// pomodoro.h
#pragma once
#include <Arduino.h>

class PomodoroManager {
public:
    void start(unsigned long durationSeconds);
    void pause();
    void resume();
    void reset();
    bool isRunning();
    unsigned long getRemainingSeconds();

private:
    unsigned long durationSeconds = 0;
    unsigned long startedAtMs = 0;
    unsigned long pausedRemainingSeconds = 0;
    bool running = false;
};