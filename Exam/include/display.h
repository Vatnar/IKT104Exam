#include "DFRobot_RGBLCD1602/DFRobot_RGBLCD1602.h"

class Display
{
public:
    void Init();
    void EventLoop();

private:
    I2C lcdI2C(D14, D15); 
    DFRobot_RGBLCD1602 lcd(&lcdI2C); 
    
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