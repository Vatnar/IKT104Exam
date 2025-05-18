#pragma once
#include "DFRobot_RGBLCD1602/DFRobot_RGBLCD1602.h"
#include "Structs.h"

class Display
{
public:
    void EventLoop();
    Display(TempHumid & tempHumid, Location &coordinate, Datetime &datetime,
    Weather &weather, RSSStream &rssstream, TempLocationChange &tlc, EditAlarm &editAlarm, AlarmData &alarmData);   // Constructor tar imot referanse
    void SetThreadPointer(std::unique_ptr<Thread> thread); // Needed since display is instantiated at the same time as thread.
private:
    I2C lcdI2C; 
    DFRobot_RGBLCD1602 lcd;

   static constexpr uint32_t FLAG_STOP = 1U << 0;
   
   struct ClockData {
    const char* day;
    int date;
    const char* month;
    int hour;
    int minute;
};

    
    void initDisplay();

    void displayStartup();        // The functions are set as private because they
    void m_displayDateTime();       // are only being used by the public methods in the class
    void displayTempHum();
    void displayWeather();
    void displayNews();
    void displayAlarm();
    void setLocation();

    void scrollText();
    void updateTime();



    std::unique_ptr<Thread> m_threadPtr; // Needed to move ownership
    Mutex m_threadPtrMutex;
    TempHumid &m_tempHumid;
    Location &m_location;
    Datetime &m_datetime;
    Weather &m_weather;
    RSSStream &m_rssstream;
    TempLocationChange &m_tlc;
    ClockData m_clock;
    AlarmData &m_alarmData;
    EditAlarm &m_editAlarm;
};