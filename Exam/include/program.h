#pragma once
#include <mbed.h>
#include "Input.h"
#include "States.h"
#include "display.h"
#include "API.h"
#include "sensor.h"
#include "structs.h"
#include "alarm.h"


class Program {
    public:
    Program();
    int ProgramLoop();
    private:
    void startup(ButtonState &buttonState);
    void showalarm(ButtonState &buttonState);
    void editenabled(ButtonState &buttonState);
    void edithour(ButtonState &buttonState);
    void editminute(ButtonState &buttonState);
    void temphumid(ButtonState &buttonState);
    void weather(ButtonState &buttonState);
    void setloc(ButtonState &buttonState);
    void news(ButtonState &buttonState);


    private:
    State m_state;
    Display m_display;
    Thread m_displayStartupThread;
    Thread m_displayThread;


    TempHumid m_tempHumid;
    Sensor m_sensor;

    Datetime m_datetime;
    Weather m_weather;
    Coordinate m_coordinate;

    
    API m_API;
    Thread m_APIStartupThread;
    Thread m_APIThread;

    Input m_input;
    Thread m_inputThread;




    Timeout m_alarmTimeout;

    bool timedOut = false;
};