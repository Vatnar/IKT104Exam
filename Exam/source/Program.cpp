#include "Program.h"
#include <mbed.h>
#include <stdio.h>
#include <string>
#include "Logger.h"
#include "API.h"
#include "Structs.h"
#include "Sensor.h"
#include "Display.h"

    constexpr bool LOG_ENABLED = false;

    #define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
    #define LINE() LINE_IF(LOG_ENABLED)


Program::Program()
    : m_API(m_datetime, m_weather, m_location, m_rssstream),
      m_sensor(m_tempHumid), 
      m_display(m_tempHumid, m_location, m_datetime, m_weather, m_rssstream, m_tlc, m_editAlarm, m_alarmData),
      m_alarm(m_alarmData, m_alarmTimeout, m_autoMute, m_datetime)
      {

    LINE();
    LINE();
    LINE();
    printf("-----------------------------------------\n");
    printf("             ALARM app                   \n\n");
    LINE();




        m_state = State::STARTUP;   // set initial state

        // Initializing data structs

        m_datetime.mutex.lock();
        m_datetime.code = NSAPI_ERROR_NO_MEMORY;
        m_datetime.offset = 0;
        m_datetime.timestamp = 0;
        m_datetime.mutex.unlock();

        m_location.mutex.lock();
        m_location.latitude = 0;
        m_location.longitude = 0;
        m_location.mutex.unlock();

        m_weather.mutex.lock();
        m_weather.description = "";
        m_weather.temp = 0.0;
        m_weather.mutex.unlock();

        m_alarmData.mutex.lock();
        m_alarmData.active = false;
        m_alarmData.enabled = false;
        m_alarmData.snoozed = false;
        m_alarmData.hour = 0;
        m_alarmData.minute = 0;
        m_alarmData.automute = false;
        m_alarmData.mutex.unlock();

        m_tlc.locationChanging = false;
        m_tlc.latitudeChanging = true;
        m_tlc.pos = 0;

        m_editAlarm.editing = false;
        m_editAlarm.hour = m_alarmData.hour;
        m_editAlarm.minute = m_alarmData.minute;
        m_editAlarm.pos = 0;


    LOG("[INFO] Starting API startup thread");
    m_APIStartupThread.start([this]() { m_API.StartUp(); });


    LOG("[INFO] Starting Display thread");
    m_displayThread.start([this]() { m_display.EventLoop(); });

    LOG("[INFO] Waiting for API startup to finish....");
    m_APIStartupThread.join();

    m_displayThread.flags_set((uint32_t)m_state); // Update the thread flags of the display

    if (m_datetime.code != NSAPI_ERROR_OK){
        LOG("[WARN] Failed to get Timestamp");
        m_datetime.mutex.lock();
        LOG("[WARN] %d", m_datetime.code);
        m_datetime.mutex.unlock();
    } else {
    m_datetime.mutex.lock();
    LOG("[INFO] Unix Timestamp: %d", m_datetime.timestamp);
    LOG("[INFO] TIMEZONE OFFSET: %d", m_datetime.offset);
    m_datetime.mutex.unlock();
    }

    // Initialize the rss and weather struct right away

    m_API.GetRSS();
    m_API.GetDailyForecastByCoordinates();

    // Start inputhandler
    LOG("Starting input handler thread");
    osThreadId_t programThreadId = ThisThread::get_id();
    m_inputThread.start([this, programThreadId]() {
    m_input.Init(programThreadId);
    m_input.InputLoop();
    });



    // Create a routine that automatically updates rss and forecast
    Thread updateWeatherandRSS;
    updateWeatherandRSS.start([this] {
        while (true) {
        ThisThread::sleep_for(15min);
        m_API.GetRSS();
        m_API.GetDailyForecastByCoordinates();
        }
    });
        
    }

    
    ButtonState Program::waitForSingleButtonPress() {
      int32_t flags = ThisThread::flags_wait_any(ANYBUTTONSTATE);
        if (flags < 0) {
            LOG("[Error] osThreadFlagsWait returned error: 0x%08x\n", (uint32_t)flags);
            return ButtonState::NONE;
        }

        if (__builtin_popcount((uint32_t)flags) != 1) {
            LOG("[Error] Multiple or zero flags set: 0x%08x\n", (uint32_t)flags);
            return ButtonState::NONE;
        }
        // Cast the flags into buttonstate for easier handling in switch statements
        return static_cast<ButtonState>(flags);
    }


    void Program::handleAlarmActive() {
        // update the state of display for displaying live clock still
        m_displayThread.flags_set((uint32_t)m_state);

        // if autmute is not un while the alarm is active, start it
        if (m_alarmData.automute == false) {
        m_alarm.StartAutoMute();
            LOG("Started automute");
        }

        ButtonState buttonState = waitForSingleButtonPress();
        // Left and UP (left side of buttons) snooze
        // DOWN and RIGHT (right side of buttons) turn off the alarm (not disable, but mute it)
        switch (buttonState){
            case ButtonState::LEFT:
              m_alarmData.snoozed = true;
            m_alarm.StopAutoMute();
            m_alarm.Snooze();
            m_alarm.m_buzzer.write(0.0);
                break;
            case ButtonState::RIGHT:
            m_alarmData.active = false;
            LOG("TURNED OFF");
            m_alarm.m_buzzer.write(0.0);
            m_alarm.StopAutoMute();
                m_alarm.ScheduleNextAlarm();
                break;
            case ButtonState::UP:
                m_alarmData.snoozed = true;
                m_alarm.Snooze();
            m_alarm.StopAutoMute();

                m_alarm.m_buzzer.write(0.0);
                break;
            case ButtonState::DOWN:
            m_alarmData.active = false;
            LOG("TURNED OFF");
            m_alarm.m_buzzer.write(0.0);
            m_alarm.StopAutoMute();

                m_alarm.ScheduleNextAlarm();
                break;
            default:
            break;
        }
            
    }

    int Program::ProgramLoop() {
        ButtonState buttonState;
        m_state = State::SHOWALARM; // Initial state (after startup)
        State previousState = State::STARTUP; // Track previous state , for the display views that dont need updating
        int32_t flags = 0;

        while (true) {
            if (m_alarmData.active) {
                m_state = State::SHOWALARM;
                if (m_state != previousState) {
                    m_displayThread.flags_set((uint32_t)m_state);
                    previousState = m_state;
                }
                handleAlarmActive();
                continue;
            }

            
            // Update regardless for the given states
            if (m_state == State::SETLOC || m_state == State::EDITALARM || m_state == State::SHOWALARM || m_state == State::WEATHER || m_state == State::TEMPHUMID) {
                m_displayThread.flags_set((uint32_t)m_state);
            } else if (m_state != previousState) {
                m_displayThread.flags_set((uint32_t)m_state);
                previousState = m_state;
            }


            // Pass responsibilities off to member functions
            switch (m_state) {
                case State::STARTUP:      startup();      break;
                case State::SHOWALARM:    showalarm();    break;
                case State::EDITALARM:    editAlarm();  break;
                case State::TEMPHUMID:    temphumid();    break;
                case State::WEATHER:      weather();      break;
                case State::SETLOC:       setloc();       break;
                case State::NEWS:         news();         break;
                default:
                    LOG("[Error] Unknown state %d\n", static_cast<int>(m_state));
                    break;
                }
                ThisThread::sleep_for(100ms);
        }

        return 0;
    }

    void Program::startup() { m_state = State::SHOWALARM; }


    // The following functions handle how the input should behave based on
    // display view selected
    
void Program::showalarm(){
    
    ButtonState buttonState = waitForSingleButtonPress();
    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::NEWS;           break;
        case ButtonState::RIGHT:    m_state = State::TEMPHUMID;      break;
        case ButtonState::UP:       m_state = State::EDITALARM;      break;
        case ButtonState::DOWN:     toggleAlarm();                   break;
    }
}

// Alarm editing logic
void Program::toggleAlarm() {
  if (m_alarmData.enabled)
    m_alarmData.enabled = false;
  else
    m_alarmData.enabled = true;
}

void Program::editAlarm() {

    if (!m_editAlarm.editing)
        m_editAlarm.editing = true;

    ButtonState buttonState = waitForSingleButtonPress();

    switch(buttonState) {
        case ButtonState::LEFT:  alarmLeft();  break;
        case ButtonState::RIGHT: alarmRight(); break;
        case ButtonState::UP:    alarmUp();    break;
        case ButtonState::DOWN:  alarmDown();  break;
    }

    LOG("Alarm: %02i:%02i", m_editAlarm.hour, m_editAlarm.minute);
}

void Program::alarmLeft() {
    if (m_editAlarm.pos == 0) {
        return;
    }
    m_editAlarm.pos--;
}

void Program::alarmRight() {
    if (m_editAlarm.pos > 3) {
        return;
    }
    m_editAlarm.pos++;
}

void Program::alarmUp() {
    switch (m_editAlarm.pos) {
        case 0:
            m_editAlarm.hour += 10;
            break;
        case 1:
            m_editAlarm.hour += 1;
            break;
        case 2:
            m_editAlarm.minute += 10;
            break;
        case 3:
            m_editAlarm.minute += 1;
            break;
    }

    if (m_editAlarm.hour > 23)
        m_editAlarm.hour = 0;
    
    if (m_editAlarm.minute > 59)
        m_editAlarm.minute = 0;
}

void Program::alarmDown() {
    LOG("Saving edited alarm...");

    m_alarmData.mutex.lock();
    m_alarmData.hour   = m_editAlarm.hour;
    m_alarmData.minute = m_editAlarm.minute;
    m_alarmData.enabled = true;
    m_alarmData.mutex.unlock();

    m_editAlarm.editing = false;      // avslutt edit-modus
    m_alarm.ScheduleNextAlarm();      // planlegg ny trigger
    m_state = State::SHOWALARM;
}


void Program::temphumid(){
        m_sensor.GetTempAndHum();
        ButtonState buttonState = waitForSingleButtonPress();

        switch(buttonState){
            case ButtonState::LEFT:     m_state = State::SHOWALARM;     break;
            case ButtonState::RIGHT:    m_state = State::WEATHER;       break;
            case ButtonState::UP:       LOG("No action\n");          break;
            case ButtonState::DOWN:     LOG("No action\n");          break;
        }
}


    void Program::weather(){

        ButtonState buttonState = waitForSingleButtonPress();
        m_location.mutex.lock();
        switch(buttonState){
            case ButtonState::LEFT:     m_state = State::TEMPHUMID;     break;
            case ButtonState::RIGHT:    m_state = State::NEWS;          break;
            case ButtonState::UP:       m_state = State::SETLOC;        break;
            case ButtonState::DOWN:     LOG("No action\n");             break;
        }
        m_location.mutex.unlock();
    }

    // Adjust coordinates manually
    void Program::setloc(){

        if (m_tlc.locationChanging == false) {
            m_tlc.locationChanging = true;
            LOG("STARTING TO CHANGE LOCATION");
            m_location.mutex.lock();

           m_tlc.latitude = (m_location.latitude >= 0 ? " " : "") + std::to_string(m_location.latitude);
            m_tlc.longitude = (m_location.longitude >= 0 ? " " : "") + std::to_string(m_location.longitude);

            m_location.mutex.unlock();
        }


        ButtonState buttonState = waitForSingleButtonPress();

        switch(buttonState){
            case ButtonState::LEFT:     locleft();      break;
            case ButtonState::RIGHT:    locright();     break;
            case ButtonState::UP:       locup();        break;
            case ButtonState::DOWN:     locdown();      break;
        }
        LOG("Latitude %s, Longitude %s", m_tlc.latitude.c_str(), m_tlc.longitude.c_str());
    }

    void Program::locleft() {
        if (m_tlc.pos == 0) {
          m_tlc.latitudeChanging = !m_tlc.latitudeChanging;
          return;
    

        m_tlc.pos--; // move back one character
        }
    }
    void Program::locright() {
        LOG("%s", m_tlc.latitudeChanging ? m_tlc.latitude.c_str() : m_tlc.longitude.c_str());

        // Switch if at end
        if ((m_tlc.latitudeChanging && m_tlc.pos == m_tlc.latitude.size()-1) || (!m_tlc.latitudeChanging && m_tlc.pos == m_tlc.longitude.size()-1)) {
        m_tlc.latitudeChanging = !m_tlc.latitudeChanging;
        m_tlc.pos = 0;
        return;
        }

        m_tlc.pos++; // move forward one character
    }

    void Program::locup() {
        std::string& currentStr = m_tlc.latitudeChanging ? m_tlc.latitude : m_tlc.longitude;

        char& ch = currentStr[m_tlc.pos];

        if (m_tlc.pos == 0) {
          ch = ch == '-' ? ' ' : '-'; // if its a minus then remove, if not then put a -

        }

        else {
        
            if (ch >= '0' && ch <= '8') {
                ch = ch + 1;  // increment digit
            } else if (ch == '9') {
                ch = '.';     // switch from 9 to comma
            } else if (ch == '.') {
                ch = '0';     // cycle back to 0
            } else {
                ch = '0';
            }
        }
    }


    void Program::locdown() {
        LOG("SAVING");
        LOG("UPDATING INFO");
        

        m_tlc.locationChanging = false;

        m_location.mutex.lock();
        m_location.latitude = std::stod(m_tlc.latitude); // Converts to double
        m_location.longitude = std::stod(m_tlc.longitude); // Converts to double

        m_location.latitude = std::max(-90.0, std::min(90.0, m_location.latitude)); // Clamps latitude between -90 and 90
        m_location.longitude = std::max(-180.0, std::min(180.0, m_location.longitude)); // Clamps longitude between -180 and 180
        
        m_location.mutex.unlock();

        // Update datetime and forecast based on new coordinate
        m_API.GetDateTimeByCoordinates();
        m_API.GetDailyForecastByCoordinates();
        
        m_state = State::WEATHER; // go back to weather views
    }

    void Program::news(){

        ButtonState buttonState = waitForSingleButtonPress();

        switch(buttonState){
            case ButtonState::LEFT:     m_state = State::WEATHER;    break;
            case ButtonState::RIGHT:    m_state = State::SHOWALARM;  break;
            case ButtonState::UP:       LOG("No action\n");          break;
            case ButtonState::DOWN:     LOG("No action\n");          break;
        }
    }