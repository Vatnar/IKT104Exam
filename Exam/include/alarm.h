#pragma once

#include "structs.h"
#include <functional>
#include <chrono>
#include "mbed.h"

class Alarm {
public:
    using clock      = std::chrono::system_clock;
    using time_point = clock::time_point;

    Alarm(AlarmData& alarmData, Timeout &alarm,Timeout &autoMute, Datetime &datetime);

    void scheduleNextAlarm();
    void snooze();
    void startAutoMute();
    void stopAutoMute();
    void triggerAlarmCB();
    PwmOut  m_buzzer;

private:

    void autoMuteCB();

    AlarmData &m_alarmData;
    Datetime &m_datetime;

    Timeout &m_alarm;
    Timeout &m_autoMute;


};
