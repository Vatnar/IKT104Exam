#pragma once

#include "Structs.h"
#include <functional>
#include <chrono>
#include "mbed.h"

class Alarm {
public:
    using clock      = std::chrono::system_clock;
    using time_point = clock::time_point;

    // Access to referenced shared structs
    Alarm(AlarmData& alarmData, Timeout &alarm,Timeout &autoMute, Datetime &datetime);

    
    void ScheduleNextAlarm(); // Schedule a new alarm based on the alarmData
    void Snooze(); // Snoozes the ACTIVE alarm by 5m
    void StartAutoMute(); // Mutes the ACTIVE alarm after 10m of ringing
    void StopAutoMute();  // Stops the automute timer, (needed for snoozing

    PwmOut  m_buzzer;

  private:
    
        
    void autoMuteCB();    // mutes alarm
    void TriggerAlarmCB(); // The actual ringing caused by a timeout expiring
  

    AlarmData &m_alarmData;
    Datetime &m_datetime;

    Timeout &m_alarm;
    Timeout &m_autoMute;
};
