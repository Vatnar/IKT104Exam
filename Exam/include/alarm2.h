#pragma once

#include "structs.h"
#include <functional>
#include <chrono>
#include "mbed.h"

class AlarmManager {
public:
    using clock      = std::chrono::system_clock;
    using time_point = clock::time_point;

    AlarmManager(AlarmData& alarmData, PinName buzzerPin);

    // Starter alarm­-scheduling
    void start();

    // Sett alarmtid
    void setTime(int h, int m);
    // Slå alarm på/av
    void enable();
    void disable();

    // Snooze og mute
    void snooze();
    void mute();

    // Callbacks
    void onTrigger(std::function<void()> cb);
    void onStop   (std::function<void()> cb);

    // Spørringer
    bool isActive()   const;
    bool isSnoozed()  const;
    bool isEnabled()  const;
    std::pair<int,int> getTime() const;

    void triggerAlarm();



private:
    void autoMute();
    void scheduleNextTrigger();

    AlarmData&      m_alarmData;
    Timeout         m_triggerTimeout, m_muteTimeout, m_snoozeTimeout;
    PwmOut          m_buzzer;

    std::function<void()> _cbTrigger;
    std::function<void()> _cbStop;
};
