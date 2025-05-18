#pragma once
#include "DFRobot_RGBLCD1602/DFRobot_RGBLCD1602.h"
#include "Structs.h"

class Display
{
public:
  // Dependency injection, give references to shared structs
    Display(TempHumid &tempHumid, Location &coordinate, Datetime &datetime,
          Weather &weather, RSSStream &rssstream, TempLocationChange &tlc,
          EditAlarm &editAlarm, AlarmData &alarmData);

  // Display loop that uses thread flags
    void EventLoop();
                               
    void SetThreadPointer(std::unique_ptr<Thread> thread); // Needed since display is instantiated at the same time as thread.
private:
    I2C lcdI2C; 
    DFRobot_RGBLCD1602 lcd;

   static constexpr uint32_t FLAG_STOP = 1U << 0;

   // Internal struct definition for drawing system time
   struct ClockData {
    const char* day;
    int date;
    const char* month;
    int hour;
    int minute;
   };
   // Shows startup information fetched by the API class
   void displayStartup();

   // Different views that can be shown
    void displayTempHum(); // Updates live
    void displayWeather(); // Updates every 15min
    void displayNews();    // Updates every 15min
    void displayAlarm();   // CLock updates live

    // Updates live
    void setLocation();


    // Used internally for displaying the RSS feed
    void scrollText();
    // Helper function for displayAlarm()
    void updateTime();



    std::unique_ptr<Thread> m_threadPtr; // Needed to move ownership
    Mutex m_threadPtrMutex;

    // References to shared data structs in program class
    TempHumid &m_tempHumid;
    Location &m_location;
    Datetime &m_datetime;
    Weather &m_weather;
    RSSStream &m_rssstream;
    TempLocationChange &m_tlc;

    AlarmData &m_alarmData;
    EditAlarm &m_editAlarm;

    ClockData m_clock;
};