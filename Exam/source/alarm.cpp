#include "alarm.h"
#include <ctime>

using namespace std::chrono_literals;

AlarmManager::AlarmManager(AlarmData& alarmData, Timeout &alarm)
    : m_alarmData(alarmData),
    m_alarm(alarm),
      m_buzzer(D13)
{
    m_buzzer.write(0);
}

void AlarmManager::start() {
    scheduleNextAlarm();
}

// Setter alarmtid
void AlarmManager::setTime(int h, int m) {
     
    m_alarmData.hour   = h;
    m_alarmData.minute = m;
         // <<< Legg til
}


// Slå alarm på
void AlarmManager::enable() {
     
    m_alarmData.enabled = true;
         // <<< Legg til
    scheduleNextAlarm();
}


// Slå alarm av
void AlarmManager::disable() {
     
    m_alarmData.enabled = false;
         // <<< Legg til

    m_triggerTimeout.detach();
    m_muteTimeout.detach();
    m_snoozeTimeout.detach();
}


// Snooze i 5 minutter
void AlarmManager::snooze() {
     
    if (!m_alarmData.active) {
           // <<< Husk unlock før early return
        return;
    }
    m_alarmData.active  = false;
    m_alarmData.snoozed = true;
         // <<< Legg til

    m_snoozeTimeout.attach(callback(this, &AlarmManager::triggerAlarm), 5min);
}


// Mute alarm (stopper aktiv eller snoozed)
void AlarmManager::mute() {

    if (m_alarmData.enabled) {
    scheduleNextAlarm();
    }


     
    m_alarmData.active  = false;
    m_alarmData.snoozed = false;
      

    m_buzzer.write(0);

    scheduleNextAlarm();
}


bool AlarmManager::isActive() const {
     
    bool v = m_alarmData.active;
        // <<< Legg til
    return v;
}

bool AlarmManager::isSnoozed() const {
     
    bool v = m_alarmData.snoozed;
        // <<< Legg til
    return v;
}

bool AlarmManager::isEnabled() const {
     
    bool v = m_alarmData.enabled;
        // <<< Legg til
    return v;
}

std::pair<int,int> AlarmManager::getTime() const {
     
    auto p = std::make_pair(m_alarmData.hour, m_alarmData.minute);
        // <<< Legg til
    return p;
}

// Intern: beregn neste trigger basert på nåtid + alarmtid
void AlarmManager::scheduleNextAlarm() {
        
     
    if (!m_alarmData.enabled) {
     
    return;
    }
    
    auto now = clock::now();
    time_t t = clock::to_time_t(now);
    std::tm tm   = *std::localtime(&t);

    int &h = m_alarmData.hour;
    int &m = m_alarmData.minute;
     
    tm.tm_hour = h;
    tm.tm_min  = m;
    tm.tm_sec  = 0;
    
    auto timeringing = clock::from_time_t(std::mktime(&tm));
    if (timeringing <= now) {
        timeringing += 24h;
    }
    auto delay = timeringing - now;


    m_triggerTimeout.attach(callback(this, &AlarmManager::triggerAlarm), delay);
}

// Internt: kalles når alarmtid nås
void AlarmManager::triggerAlarm() {

    m_alarmData.active  = true;
    m_alarmData.snoozed = false;    
    
    // Start buzzer
    m_buzzer.period(0.001);
    m_buzzer.write(0.9);    

    // Sett auto-mute etter 10 minutter
    m_muteTimeout.attach(callback(this, &AlarmManager::autoMute), 10min);
}

// Intern: auto-mute etter utløpstid
void AlarmManager::autoMute() {
    
    m_alarmData.active = false;
    
    m_buzzer.write(0.0f);
        
    scheduleNextAlarm();
}
