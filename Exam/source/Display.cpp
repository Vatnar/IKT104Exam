#include "mbed-os/mbed.h"
#include "Display.h"
#include "States.h"
#include "Logger.h"
#include <string>
#include "Structs.h"
#include <chrono>
#include <ctime>

using namespace std::chrono_literals;

constexpr bool LOG_ENABLED = false;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)

Display::Display(TempHumid & tempHumid, Location & coordinate,
        Datetime & datetime, Weather &weather, RSSStream &rssstream, TempLocationChange &tlc, EditAlarm &editAlarm, AlarmData &alarmData): lcdI2C(D14, D15), lcd( & lcdI2C),
         m_tempHumid(tempHumid), m_location(coordinate), m_datetime(datetime), m_weather(weather), m_rssstream(rssstream), m_tlc(tlc), m_editAlarm(editAlarm), m_alarmData(alarmData) {
        lcd.init();
        thread_sleep_for(80); // Trenger sleep for å initialisere LCD-displayet
        lcd.clear();
        lcd.display();
    }
    void Display::SetThreadPointer(std::unique_ptr<Thread> thread) {
      m_threadPtrMutex.lock();
      m_threadPtr = nullptr;
      m_threadPtr = std::move(thread);
      m_threadPtrMutex.unlock();
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

        // Checks if flag count is valid
        if (__builtin_popcount((uint32_t)flags) != 1) {
            LOG("[Error] Multiple or zero flags set: 0x%08x\n", (uint32_t)flags);
            continue;
        }
        m_threadPtrMutex.lock();
        if (m_threadPtr) {
            LOG("[DEBUG] Stopping existing thread due to state change");
            // m_threadPtr->flags_set(FLAG_STOP);
            // m_threadPtr->join();
            m_threadPtr.reset();
        }
        m_threadPtrMutex.unlock();


state = static_cast<State>(flags);
switch (state) {
            case State::STARTUP:        displayStartup();     break;
            case State::SHOWALARM:      displayAlarm();       break;
            case State::EDITALARM:      displayAlarm();       break;
            case State::TEMPHUMID:      displayTempHum();     break;
            case State::WEATHER:        displayWeather();     break;
            case State::NEWS:           displayNews();        break;
            case State::SETLOC:         setLocation();        break;
        }
    }
}

void Display::displayStartup() {

    LOG("[DEBUG] DISPLAYING STARTUP");
    // Unix epoch time
    lcd.setCursor(0,0);
    lcd.printf("Unix epoch time:");
    lcd.setCursor(0, 1);
  // Må vente på at startuppen er ferdig først

    lcd.printf("%d", m_datetime.timestamp);

    ThisThread::sleep_for(2s);
    
    // Latitude longitude
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("Lat: %f", m_location.latitude);
    lcd.setCursor(0, 1);
    lcd.printf("Lon:  %f", m_location.longitude);
    
    ThisThread::sleep_for(2s);

    // City
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.printf("City:");
    lcd.setCursor(0, 1);
    lcd.printf("Grimstad");
    ThisThread::sleep_for(2s);
}

void Display::displayAlarm() {
    LOG("[DEBUG] DISPLAYING ALARM");

    updateTime();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("%s %02d %s %02d:%02d", m_clock.day, m_clock.date, m_clock.month, m_clock.hour, m_clock.minute);

    lcd.setCursor(0, 1);

    if (m_editAlarm.editing)
        lcd.printf("Alarm (E)  %02i:%02i", m_editAlarm.hour, m_editAlarm.minute);
    else {
        if (m_alarmData.snoozed)
            lcd.printf("Alarm (S)  %02i:%02i", m_alarmData.hour, m_alarmData.minute);
        else if (m_alarmData.active)
            lcd.printf("Alarm (A)  %02i:%02i", m_alarmData.hour, m_alarmData.minute);
        else if (m_alarmData.enabled)
            lcd.printf("Alarm      %02i:%02i", m_alarmData.hour, m_alarmData.minute);
        else 
            lcd.printf("%-16s", "Alarm OFF");
    }
}

void Display::displayTempHum() {
    LOG("[DEBUG] DISPLAYING TEMPHUM");
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

void Display::displayWeather() {
    lcd.clear();
    lcd.setCursor(0,0);
    
     m_weather.mutex.lock();
    lcd.printf("%s", m_weather.description.c_str());
    lcd.setCursor(0, 1);
    lcd.printf("%.1f degrees C", m_weather.temp);
     m_weather.mutex.unlock();
}

void Display::displayNews() {
      lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("CNN");

    if (m_threadPtr) {
        LOG("[DEBUG] Sending stop flag to scroll thread");
        m_threadPtr->flags_set(FLAG_STOP);
        m_threadPtr->join();
        m_threadPtr.reset();
    }
    // Make a new thread for scrolling since display still needs to handle input event
    auto newThread = std::make_unique<Thread>();
    newThread->start([this] {
        this->scrollText();
    });

    SetThreadPointer(std::move(newThread));
}

// Displays coordinates
void Display::setLocation() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.printf("%s", m_tlc.latitude.c_str());
    lcd.setCursor(0, 1);
    lcd.printf("%s", m_tlc.longitude.c_str());
}

// Oppdatert m_scrollText med sømløs looping, padding og 200ms speed
void Display::scrollText() {
    std::string text = m_rssstream.rss;
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

// Helper for updating time for display
void Display::updateTime() {
    time_t now = time(NULL);
    now += m_datetime.offset * 3600 * 2;  // Juster med offset i sekunder
    struct tm *localTime = localtime(&now);

    const char* days[]   = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    m_clock.day    = days[localTime->tm_wday];
    m_clock.date   = localTime->tm_mday;
    m_clock.month  = months[localTime->tm_mon];
    m_clock.hour   = localTime->tm_hour;
    m_clock.minute = localTime->tm_min;
}