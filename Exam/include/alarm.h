#pragma once

#include "structs.h"
#include <functional>
#include <chrono>
#include "mbed.h"

class AlarmManager {
public:
    using clock      = std::chrono::system_clock;
    using time_point = clock::time_point;

    AlarmManager(AlarmData& alarmData, Timeout &alarm);

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


    // Spørringer
    bool isActive()   const;
    bool isSnoozed()  const;
    bool isEnabled()  const;
    std::pair<int,int> getTime() const;

    void triggerAlarm();

        void autoMute();
    void scheduleNextAlarm();

    AlarmData      &m_alarmData;
    Timeout         m_triggerTimeout, m_muteTimeout, m_snoozeTimeout;


    Timeout &m_alarm;
    PwmOut          m_buzzer;

private:

};
