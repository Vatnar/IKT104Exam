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
    // Initializes the program and member variables and starts the startupsequence and 15 minute interval for updating rss and weather
    Program();

    // Handles input, and most program logic in an infinite loop
    int ProgramLoop();

  private:
    // Helper function for getting input
    ButtonState waitForSingleButtonPress();


    void startup();

    // Runs instead of normal input handling whenever the alarm is currently ringing (active)
    void handleAlarmActive();

    // Different views to choose from
    void showalarm();
    void temphumid();
    void weather();
    void news();

    // Setting coordinates manually
    void setloc();
    void locleft();
    void locright();
    void locup();
    void locdown();


    // Editing alarm
    void editAlarm();
    void alarmUp();
    void alarmDown();
    void alarmLeft();
    void alarmRight();
    void toggleAlarm();


    // Internal states in program
    State m_state;

    // Instanitation of classes
    Input m_input;
    Display m_display;
    Sensor m_sensor;
    API m_API;
    Alarm m_alarm;


    // Concurrent threads to run
    Thread m_displayThread;
    Thread m_APIStartupThread;
    Thread m_inputThread;


    // Data structs
    TempHumid m_tempHumid;
    Datetime m_datetime;
    Weather m_weather;
    RSSStream m_rssstream;

    // Coordinate structs
    Location m_location;
    TempLocationChange m_tlc;

    // Structs for Alarm
    AlarmData m_alarmData;
    EditAlarm m_editAlarm;

    // Timeouts for alarm
    Timeout m_alarmTimeout;
    Timeout m_autoMute;
};