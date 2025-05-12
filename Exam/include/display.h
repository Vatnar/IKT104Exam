#pragma once
#include "DFRobot_RGBLCD1602/DFRobot_RGBLCD1602.h"

class Display
{
public:
    Display();
    void EventLoop();

private:
    I2C lcdI2C; 
    DFRobot_RGBLCD1602 lcd; 
    
    void m_displayStartup();        // The functions are set as private because they
    void m_displayDateTime();       // are only being used by the public methods in the class
    void m_displayTempHum();
    void m_displayWeather();
    void m_displayNews();
    void m_displayAlarm();
    void m_editHour();
    void m_editMinute();
    void m_setLocation();

    
};