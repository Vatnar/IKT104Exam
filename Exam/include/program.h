#pragma once
#include <mbed.h>
#include "Input.h"
#include "States.h"
#include "display.h"
#include "API.h"
#include "sensor.h"
#include "structs.h"


class Program {
    public:
    Program();
    int ProgramLoop();
    ButtonState waitForSingleButtonPress();
    private:
    void startup();
    void showalarm();
    void editenabled();
    void edithour();
    void editminute();
    void temphumid( );
    void weather( );
    void setloc();
    void locleft();
    void locright();
    void locup();
    void locdown();
    void news( );


  private:
    
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
    
    
};