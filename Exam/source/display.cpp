#include "mbed-os/mbed.h"
#include "display.h"
#include "States.h"

Display::Display(): lcdI2C(D14, D15), lcd(&lcdI2C) {}

void Display::Init() {
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
            //TODO tror datetime skal vises konstant på toppen - Peter
            // case State::DATETIME:       m_displayDateTime();    break;
            case State::TEMPHUMID:      m_displayTempHum();     break;
            case State::WEATHER:        m_displayWeather();     break;
            case State::NEWS:           m_displayNews();        break;
            case State::EDITHOUR:       m_editHour();           break;
            case State::EDITMINUTE:     m_editMinute();         break;
            case State::SETLOC:         m_setLocation();        break;
        }
    }
}
// Disse må eksistere for at man kan kunne bygge - Peter
void Display::m_displayStartup() {}
void Display::m_displayAlarm() {}
void Display::m_displayDateTime() {}
void Display::m_displayTempHum() {}
void Display::m_displayWeather() {}
void Display::m_displayNews() {}
void Display::m_editHour() {}
void Display::m_editMinute() {}
void Display::m_setLocation() {}