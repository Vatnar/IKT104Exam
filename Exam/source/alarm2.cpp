#include "alarm2.h"
#include <ctime>

using namespace std::chrono_literals;

AlarmManager::AlarmManager(AlarmData& alarmData, PinName D13)
    : m_alarmData(alarmData),
      m_buzzer(D13)
{
    // Sørg for at buzzer starter stille
    m_buzzer.write(0.0f);
}

// Starter scheduling av neste alarm
void AlarmManager::start() {
    scheduleNextTrigger();
}

// Setter alarmtid
void AlarmManager::setTime(int h, int m) {
    m_alarmData.mutex.lock();
    m_alarmData.hour   = h;
    m_alarmData.minute = m;
    m_alarmData.mutex.unlock();    // <<< Legg til
}


// Slå alarm på
void AlarmManager::enable() {
    m_alarmData.mutex.lock();
    m_alarmData.enabled = true;
    m_alarmData.mutex.unlock();    // <<< Legg til
    scheduleNextTrigger();
}


// Slå alarm av
void AlarmManager::disable() {
    m_alarmData.mutex.lock();
    m_alarmData.enabled = false;
    m_alarmData.mutex.unlock();    // <<< Legg til

    m_triggerTimeout.detach();
    m_muteTimeout.detach();
    m_snoozeTimeout.detach();
}


// Snooze i 5 minutter
void AlarmManager::snooze() {
    m_alarmData.mutex.lock();
    if (!m_alarmData.active) {
        m_alarmData.mutex.unlock();  // <<< Husk unlock før early return
        return;
    }
    m_alarmData.active  = false;
    m_alarmData.snoozed = true;
    m_alarmData.mutex.unlock();    // <<< Legg til

    m_snoozeTimeout.attach(callback(this, &AlarmManager::triggerAlarm), 5min);
}


// Mute alarm (stopper aktiv eller snoozed)
void AlarmManager::mute() {

    m_alarmData.mutex.lock();
    m_alarmData.active  = false;
    m_alarmData.snoozed = false;
    m_alarmData.mutex.unlock(); 

    m_buzzer.write(0.0f);
    // ny, mer eksplisitt sjekk og kall
    if (_cbStop) {
        _cbStop();
    }
    scheduleNextTrigger();
}

// Registrer callback for når alarm trigger
void AlarmManager::onTrigger(std::function<void()> cb) {
    _cbTrigger = std::move(cb);
}

// Registrer callback for når alarm stopper
void AlarmManager::onStop(std::function<void()> cb) {
    _cbStop = std::move(cb);
}

bool AlarmManager::isActive() const {
    m_alarmData.mutex.lock();
    bool v = m_alarmData.active;
    m_alarmData.mutex.unlock();   // <<< Legg til
    return v;
}

bool AlarmManager::isSnoozed() const {
    m_alarmData.mutex.lock();
    bool v = m_alarmData.snoozed;
    m_alarmData.mutex.unlock();   // <<< Legg til
    return v;
}

bool AlarmManager::isEnabled() const {
    m_alarmData.mutex.lock();
    bool v = m_alarmData.enabled;
    m_alarmData.mutex.unlock();   // <<< Legg til
    return v;
}

std::pair<int,int> AlarmManager::getTime() const {
    m_alarmData.mutex.lock();
    auto p = std::make_pair(m_alarmData.hour, m_alarmData.minute);
    m_alarmData.mutex.unlock();   // <<< Legg til
    return p;
}

// Intern: beregn neste trigger basert på nåtid + alarmtid
void AlarmManager::scheduleNextTrigger() {
    auto now = clock::now();
    std::time_t t = clock::to_time_t(now);
    std::tm tm   = *std::localtime(&t);

    int h, m;
    m_alarmData.mutex.lock();
    h = m_alarmData.hour;
    m = m_alarmData.minute;
    m_alarmData.mutex.unlock();

    tm.tm_hour = h;
    tm.tm_min  = m;

    tm.tm_sec  = 0;
    auto then = clock::from_time_t(std::mktime(&tm));
    if (then <= now) {
        then += 24h;
    }
    auto delay = then - now;
    m_triggerTimeout.attach(callback(this, &AlarmManager::triggerAlarm), delay);
}

// Internt: kalles når alarmtid nås
void AlarmManager::triggerAlarm() {

    /*m_alarmData.mutex.lock();
    m_alarmData.active  = true;
    m_alarmData.snoozed = false;
    m_alarmData.mutex.unlock();*/
    
    
    // Start buzzer
    m_buzzer.period(0.001f);
    m_buzzer.write(0.9);    

    // Sett auto-mute etter 10 minutter
    m_muteTimeout.attach(callback(this, &AlarmManager::autoMute), 10min);
}

// Intern: auto-mute etter utløpstid
void AlarmManager::autoMute() {
    {
        m_alarmData.mutex.lock();
        m_alarmData.active = false;
        m_alarmData.mutex.unlock();
    }
    m_buzzer.write(0.0f);
        
    // ny, mer eksplisitt sjekk og kall
    if (_cbStop) {
        _cbStop();
    }
    scheduleNextTrigger();
}
