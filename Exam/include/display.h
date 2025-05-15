#pragma once
#include "DFRobot_RGBLCD1602/DFRobot_RGBLCD1602.h"
#include "structs.h"

class Display
{
public:
    void EventLoop();
    Display(TempHumid &tempHumid);   // Constructor tar imot referanse
    /*Display(Datetime &datetime);
    Display(Coordinate &coordinate);
    Display(Weather &weather);*/

private:
    I2C lcdI2C; 
    DFRobot_RGBLCD1602 lcd;

   static constexpr uint32_t FLAG_STOP = 1U << 0;
    
    void m_initDisplay();

    void m_displayStartup();        // The functions are set as private because they
    void m_displayDateTime();       // are only being used by the public methods in the class
    void m_displayTempHum();
    void m_editEnabled();
    void m_displayWeather();
    void m_displayNews();
    void m_displayAlarm();
    void m_editHour();
    void m_editMinute();
    void m_setLocation();

    void m_scrollText(const std::string& tekst);

    TempHumid &m_tempHumid;     // Referanse til shared struct
    /*Datetime &m_datetime;
    Weather &m_weather;
    Coordinate &m_coordinate;*/
};