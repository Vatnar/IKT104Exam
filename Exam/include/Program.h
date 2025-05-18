#pragma once
#include <mbed.h>
#include "Input.h"
#include "States.h"
#include "Display.h"
#include "API.h"
#include "Sensor.h"
#include "Structs.h"
#include "Alarm.h"


class Program {
public:
    Program();
    int ProgramLoop();
    
private:
    ButtonState waitForSingleButtonPress();
    void startup();
    void handleAlarmActive();
    void showalarm();
    void editenabled();
    void temphumid();
    void weather();
    void news();


    void setloc();
    void locleft();
    void locright();
    void locup();
    void locdown();

    void editAlarm();
    void alarmUp();
    void alarmDown();
    void alarmLeft();
    void alarmRight();
    void toggleAlarm();
    
    State m_state;
    Display m_display;
    Thread m_displayThread;

    TempHumid m_tempHumid;
    Sensor m_sensor;
    
    Datetime m_datetime;
    Weather m_weather;
    RSSStream m_rssstream;
    
    API m_API;
    Thread m_APIStartupThread;

    Input m_input;
    Thread m_inputThread;


    // Coordinate stuff
    Location m_location;
    TempLocationChange m_tlc;
    
    Alarm m_alarm;
    AlarmData m_alarmData;
    Timeout m_alarmTimeout;
    Timeout m_autoMute;

    EditAlarm m_editAlarm;

        
};