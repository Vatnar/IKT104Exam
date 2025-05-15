#pragma once
#include <mbed.h>
#include "Input.h"
#include "States.h"
#include "display.h"
#include "API.h"
#include "sensor.h"


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


    temphumidstruct m_tempHumid;
    Sensor m_sensor;

    Datetime m_datetime;
    API m_API;
    Thread m_APIStartupThread;
    Thread m_APIThread;

    Input m_input;
    Thread m_inputThread;

};