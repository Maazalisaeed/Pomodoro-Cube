#pragma once
#include <Arduino.h>

enum class PomodoroPhase { WORK, SHORT_BREAK, LONG_BREAK };

class PomodoroManager {
public:
    void startCycle(int levelIndex); // 0=15min, 1=30min, 2=45min, 3=60min work level
    void update();                   // call every loop - auto-advances phases when one finishes
    void pause();
    void resume();
    void reset();
    bool isRunning();
    unsigned long getRemainingSeconds();
    PomodoroPhase getPhase();
    int getCompletedWorkCount(); // how many work sessions done so far this cycle (0-4)
    unsigned long getPhaseDurationSeconds() const { return durationSeconds; } // needed to compute ring fraction

    
private:
    static const unsigned long WORK_MIN[4];
    static const unsigned long SHORT_BREAK_MIN[4];
    static const unsigned long LONG_BREAK_MIN[4];
    
    int levelIndex = 0;
    PomodoroPhase phase = PomodoroPhase::WORK;
    int completedWorkCount = 0;

    unsigned long durationSeconds = 0;
    unsigned long startedAtMs = 0;
    unsigned long pausedRemainingSeconds = 0;
    bool running = false;

    void beginPhase(PomodoroPhase p);
};