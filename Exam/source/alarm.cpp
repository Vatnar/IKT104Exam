#include "alarm.h"
#include <ctime>
#include "Logger.h"

constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
#define LINE() LINE_IF(LOG_ENABLED)


using namespace std::chrono_literals;

Alarm::Alarm(AlarmData& alarmData, Timeout &alarm,Timeout &autoMute, Datetime &datetime)
    : m_alarmData(alarmData),
    m_alarm(alarm),
    m_autoMute(autoMute),
    m_datetime(datetime),
      m_buzzer(D13)
{
    m_buzzer.write(0);
}



void Alarm::scheduleNextAlarm() {
    if (!m_alarmData.enabled) {
        // LOG("Alarm disabled\n");
        return;
    }

    using namespace std::chrono;

    time_t current_time = time(nullptr);
    struct tm *now = localtime(&current_time);

    struct tm alarm_time = *now;
    alarm_time.tm_hour = m_alarmData.hour-m_datetime.offset-1;
    alarm_time.tm_min = m_alarmData.minute;
    alarm_time.tm_sec = 0;

    time_t alarm_epoch = mktime(&alarm_time);
    double diff = difftime(alarm_epoch, current_time);
    if (diff <= 0) diff += 24 * 3600;


    // Convert diff seconds to chrono duration
    auto delay = duration<double>(diff);

    LOG("Scheduling alarm in %.2f seconds\n", diff);

    // If m_alarmTimeout.attach expects a duration in seconds or milliseconds,
    // convert accordingly. Example assumes attach takes seconds as float:
    m_alarm.detach();
    m_alarm.attach(callback(this, &Alarm::triggerAlarmCB), delay.count());
}

void Alarm::snooze(){
  auto delay = 5s;
    m_alarm.detach();
    m_alarm.attach(callback( this, &Alarm::triggerAlarmCB), delay.count());
}


// Internt: kalles når alarmtid nås
void Alarm::triggerAlarmCB() {

    m_alarmData.active  = true;
    m_alarmData.snoozed = false;    
    
    // Start buzzer
    m_buzzer.period(0.001);
    m_buzzer.write(0.9);    
}

void Alarm::startAutoMute() {
  m_alarmData.automute = true;
  auto delay = 10s;
  m_autoMute.detach();
    m_autoMute.attach(callback(this, &Alarm::autoMuteCB), delay.count());
}
void Alarm::stopAutoMute() {
  m_alarmData.automute = false;
  m_autoMute.detach();
}
// Intern: auto-mute etter utløpstid
void Alarm::autoMuteCB() {
    
    m_alarmData.active = false;
    m_buzzer.write(0.0f);
        
}
