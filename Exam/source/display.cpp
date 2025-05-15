#include "mbed-os/mbed.h"
#include "display.h"
#include "States.h"
#include "Logger.h"
#include <string>
#include "structs.h"
#include <chrono>
using namespace std::chrono_literals;

constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)

Display::Display(TempHumid &tempHumid) : lcdI2C(D14, D15), lcd(&lcdI2C), m_tempHumid(tempHumid) {
    lcd.init();
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
            case State::EDITENABLED:    m_editEnabled();        break;
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
void Display::m_displayStartup() {
    
    // Unix epoch time
    lcd.setCursor(0,0);
    lcd.printf("Unix epoch time:");
    lcd.setCursor(0, 1);
    lcd.printf("1234567890");
    ThisThread::sleep_for(2s);
    
    // Latitude longitude
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.printf("Lat: 58.3405");
    lcd.setCursor(0, 1);
    lcd.printf("Lon:  8.5934");
    ThisThread::sleep_for(2s);

    // City
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.printf("City:");
    lcd.setCursor(0, 1);
    lcd.printf("Grimstad");
    ThisThread::sleep_for(2s);
}

void Display::m_displayAlarm() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.printf("Alarm      7:30");
    lcd.setCursor(0, 1);
    lcd.printf("---------------");
}

void Display::m_displayDateTime() {
    
}

void Display::m_editEnabled() {

}

void Display::m_displayTempHum() {

    lcd.clear();
    lcd.setCursor(0,0);

    m_tempHumid.mutex.lock(); // Lås mutex for å lese sikkert
    float temp = m_tempHumid.temp;
    float humid = m_tempHumid.humid;
    m_tempHumid.mutex.unlock();

    lcd.printf("Temp: %.1f C", temp);
    lcd.setCursor(0, 1);
    lcd.printf("Humid: %.1f%%", humid);
}

void Display::m_displayWeather() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.printf("Broken clouds");
    lcd.setCursor(0, 1);
    lcd.printf("7 degrees");
}

void Display::m_displayNews() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.printf("BBC");
    m_scrollText("The past, present and future walked into a bar, it was tense");
}

void Display::m_editHour() {

}

void Display::m_editMinute() {

}

void Display::m_setLocation() {
    
}

// Oppdatert m_scrollText med sømløs looping, padding og 200ms speed
void Display::m_scrollText(const std::string& text) {
    constexpr size_t windowSize = 16;
    const size_t length = text.length();

    // Padding mellom slutt og start
    const std::string buffer = text + "        ";
    const size_t totalLength = buffer.length();

    size_t pos = 0;
    while (true) {
        std::string window;
        if (pos + windowSize <= totalLength) {
            // Ingen wrapping
            window = buffer.substr(pos, windowSize);
        } else {
            // Wrap around fra slutt til begynnelse
            size_t firstLen = totalLength - pos;
            window = buffer.substr(pos, firstLen)
                   + buffer.substr(0, windowSize - firstLen);
        }

        // Vis vindu
        lcd.setCursor(0, 1);
        lcd.printf("%-*s", windowSize, window.c_str());

        // Vent 100ms
        ThisThread::sleep_for(200ms);

        // Avbryt om FLAG_STOP
        uint32_t flags = ThisThread::flags_get();
        if (flags & FLAG_STOP) {
            ThisThread::flags_clear(FLAG_STOP);
            break;
        }

        // Neste posisjon
        pos = (pos + 1) % totalLength;
    }
}