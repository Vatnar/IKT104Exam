#include "mbed-os/mbed.h"
#include "display.h"
#include "States.h"
#include "Logger.h"

constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)


Display::Display(): lcdI2C(D14, D15), lcd(&lcdI2C) {
    thread_sleep_for(80);               // Trenger sleep for å initialisere LCD-displayet
    lcd.clear();
    lcd.display();
}

void Display::EventLoop() {
    State state = State::STARTUP;                // Instansierer State-klassen
    int32_t flags = 0;

    while (true) {

        LOG("[Info] Current display state %d", state);

        flags = ThisThread::flags_wait_any(ANYSTATE);
        if (flags < 0) {
            LOG("[Error] osThreadFlagsWait returned error: 0x%08x\n", (uint32_t)flags);
            continue;  
        }

        if (__builtin_popcount((uint32_t)flags) != 1) {
            LOG("[Error] Multiple or zero flags set: 0x%08x\n", (uint32_t)flags);
            continue;
        }

        state = static_cast<State>(flags);
        switch (state) {
            case State::STARTUP:        m_displayStartup();     break;
            case State::SHOWALARM:      m_displayAlarm();       break;
            //TODO tror datetime skal vises konstant på toppen - Peter
            // case State::DATETIME:       m_displayDateTime();    break;
            case State::EDITENABLED:     m_editEnabled();        break;
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
void Display::m_editEnabled(){};
void Display::m_displayTempHum() {}
void Display::m_displayWeather() {}
void Display::m_displayNews() {}
void Display::m_editHour() {}
void Display::m_editMinute() {}
void Display::m_setLocation() {}