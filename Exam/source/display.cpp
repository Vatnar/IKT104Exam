#include "mbed-os/mbed.h"
#include "display.h"
#include "States.h"
#include "Logger.h"
#include <string>

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
    lcd.setCursor(0,0);
    lcd.printf("Lat: 58.3405");
    lcd.setCursor(0, 1);
    lcd.printf("Lon:  8.5934");
    ThisThread::sleep_for(2s);

    // City
    lcd.setCursor(0,0);
    lcd.printf("City:");
    lcd.setCursor(0, 1);
    lcd.printf("Grimstad");
    ThisThread::sleep_for(2s);
}

void Display::m_displayAlarm() {
    lcd.setCursor(0,0);
    lcd.printf("Alarm     7:30");
    lcd.setCursor(0, 1);
    lcd.printf("--------------");
}

void Display::m_displayDateTime() {
    
}

void Display::m_editEnabled() {

}

void Display::m_displayTempHum() {
    lcd.setCursor(0,0);
    lcd.printf("Temp: 21.4 C");
    lcd.setCursor(0, 1);
    lcd.printf("Humidity: 69%");
}

void Display::m_displayWeather() {
    lcd.setCursor(0,0);
    lcd.printf("Broken clouds");
    lcd.setCursor(0, 1);
    lcd.printf("7 degrees");
}

void Display::m_displayNews() {
    lcd.setCursor(0,0);
    lcd.printf("BBC");
    lcd.setCursor(0, 1);
    m_scrollText("The past, present and future walked into a bar, it was tense");
}

void Display::m_editHour() {

}

void Display::m_editMinute() {

}

void Display::m_setLocation() {
    
}

void Display::m_scrollText(const std::string& text) {
    int windowSize = 16; // antall kolonner
    int lengde = text.length();

    // Hvis teksten er kortere enn displayet, bare vis den
    if (lengde <= windowSize) {

        lcd.printf("%-*s", windowSize, text.c_str()); // fyll med mellomrom
        return;
    }

    for (int i = 0; i <= lengde - windowSize; i++) {
        
        lcd.printf("%.*s", windowSize, text.c_str() + i); // vis 16 tegn fra posisjon i
        ThisThread::sleep_for(50ms);
    }
}