#pragma once

#include "Structs.h"
#include <functional>
#include <chrono>
#include "mbed.h"

class Alarm {
public:
    using clock      = std::chrono::system_clock;
    using time_point = clock::time_point;

    Alarm(AlarmData& alarmData, Timeout &alarm,Timeout &autoMute, Datetime &datetime);

    void ScheduleNextAlarm();
    void Snooze();
    void StartAutoMute();
    void StopAutoMute();
    void TriggerAlarmCB();
    PwmOut  m_buzzer;

private:

    void autoMuteCB();

    AlarmData &m_alarmData;
    Datetime &m_datetime;

    Timeout &m_alarm;
    Timeout &m_autoMute;


};
