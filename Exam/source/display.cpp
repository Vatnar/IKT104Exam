#include "mbed-os/mbed.h"
#include "DFRobot_RGBLCD1602/DFRobot_RGBLCD1602.h"
#include "display.h"
#include "States.h"

void Display::Init() {
    I2C lcdI2C(D14, D15); 
    DFRobot_RGBLCD1602 lcd(&lcdI2C); 

    thread_sleep_for(80);               // Trenger sleep for å initialisere LCD-displayet
    lcd.clear();
    lcd.display();
}

void Display::EventLoop() {
    State state;                // Instansierer State-klassen

    while (true) {
        #define ANY 0xFFFFFFFF
        state = (State)ThisThread::flags_wait_any(ANY, false);      // Dette vil vente til en state er sendt, og behandle den
        
        switch (state) {
            case State::STARTUP:        m_displayStartup();     break;
            case State::SHOWALARM:      m_displayAlarm();       break;
            case State::DATETIME:       m_displayDateTime();    break;
            case State::TEMPHUMID:      m_displayTempHum();     break;
            case State::WEATHER:        m_displayWeather();     break;
            case State::NEWS:           m_displayNews();        break;
            case State::EDITHOUR:       m_editHour();           break;
            case State::EDITMINUTE:     m_editMinute();         break;
            case State::SETLOC:         m_setLocation();        break;
        }
    }
}

void m_displayStartup() {
    
}