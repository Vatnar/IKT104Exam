#include "program.h"
#include <mbed.h>
#include <stdio.h>
#include "Logger.h"
#include "API.h"
#include "structs.h"
#include "sensor.h"
#include "Display.h"

constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
#define LINE() LINE_IF(LOG_ENABLED)


Program::Program()
    : m_API(m_datetime, m_weather, m_location, m_rssstream),
      m_sensor(m_tempHumid), m_display(m_tempHumid, m_location, m_datetime, m_weather){

  LINE();
  LINE();
  LINE();
  printf("-----------------------------------------\n");
  printf("             ALARM app                   \n\n");
  LINE();



    
    m_state = State::STARTUP;   // set initial state
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

    // Hente unix timestamp tar litt tid så derfor si velkommen eller et eller annet på displayet i mens
    



    LOG("[INFO] Starting API startup thread");
    m_APIStartupThread.start([this]() { m_API.StartUp(); });

     
    LOG("[INFO] Starting Display thread");
    m_displayThread.start([this]() { m_display.EventLoop(); });

    LOG("[INFO] Waiting for API startup to finish....");
    m_APIStartupThread.join();

    m_displayThread.flags_set((uint32_t)m_state);
    
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

    // Initialize the rss struct right away
    Thread rss;
    rss.start([this] {
      m_API.GetRSS();
    });

    

    // Start inputhandler
    LOG("Starting input handler thread");
    osThreadId_t programThreadId = ThisThread::get_id();
    m_inputThread.start([this, programThreadId]() {
    m_input.Init(programThreadId);
    m_input.InputLoop();
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

    return static_cast<ButtonState>(flags);
}

int Program::ProgramLoop(){
    ButtonState buttonState;
    m_state = State::SHOWALARM;

    int32_t flags = 0;
    while (true) {

      LOG("[Info] Sent state to display");

        m_displayThread.flags_set((uint32_t)m_state);
        switch (m_state){
            case State::STARTUP:        startup();      break;
            case State::SHOWALARM:      showalarm();    break;
            case State::EDITHOUR:       edithour();     break;
            case State::EDITMINUTE:     editminute();   break;
            case State::EDITENABLED:    editenabled();  break;
            case State::TEMPHUMID:      temphumid();    break;
            case State::WEATHER:        weather();      break;
            case State::SETLOC:         setloc();       break;
            case State::NEWS:           news();         break;
            default: LOG("[Error] Unknown state %d\n", static_cast<int>(m_state)); break;
        }
            

        
    }
    return 0;
}
void Program::startup(){
    m_state = State::SHOWALARM;
}
// TODO refactor it to be better on switching instead of setting explicit
void Program::showalarm(){
    LOG("STATE: SHOW ALARM\n");
    
    ButtonState buttonState = waitForSingleButtonPress();
    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::NEWS;          break;
        case ButtonState::RIGHT:    m_state = State::TEMPHUMID;     break;
        case ButtonState::UP:       m_state = State::EDITENABLED;   break;
        case ButtonState::DOWN:     LOG("No action\n");          break;
    }

}
void Program::editenabled(){
    LOG("STATE: EDITENABLED\n");

    ButtonState buttonState = waitForSingleButtonPress();

    switch(buttonState){
        case ButtonState::LEFT:     LOG("Toggle enabled\n");     break;
        case ButtonState::RIGHT:    LOG("Toggle enabled\n");     break;
        case ButtonState::UP:       m_state = State::SHOWALARM;     break;
        case ButtonState::DOWN:     m_state = State::EDITHOUR;      break;
    }   
}
void Program::edithour(){
    LOG("STATE: EDITHOUR\n");

    ButtonState buttonState = waitForSingleButtonPress();
    
    switch(buttonState){
        case ButtonState::LEFT:     LOG("Value down\n");         break;
        case ButtonState::RIGHT:    LOG("Value up\n");           break;
        case ButtonState::UP:       m_state = State::SHOWALARM;     break;
        case ButtonState::DOWN:     m_state = State::EDITMINUTE;    break;
    }

}
void Program::editminute(){
    LOG("STATE: EDITMINUTE\n");


    ButtonState buttonState = waitForSingleButtonPress();
    
    switch(buttonState){
        case ButtonState::LEFT:     LOG("Value down\n");         break;
        case ButtonState::RIGHT:    LOG("Value up\n");           break;
        case ButtonState::UP:       m_state = State::SHOWALARM;     break;
        case ButtonState::DOWN:     m_state = State::EDITENABLED;   break;
    }
}

void Program::temphumid(){

  LOG("STATE: TEMPHUMID\n");

  
    LOG("[DEBUG] Creating thread for sensor reading...");
    auto thread = std::make_unique<Thread>();
    thread->start([this] {
        LOG("[DEBUG] Sensor thread running");
        m_sensor.getTempAndHum();
        LOG("[DEBUG] Sensor thread done");
    });

    LOG("[DEBUG] Moving thread pointer into Display");
    m_display.SetThreadPointer(std::move(thread));

    ThisThread::sleep_for(20ms); // Avoid race

    LOG("[DEBUG] Setting display flag");


    ButtonState buttonState = waitForSingleButtonPress();
    
    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::SHOWALARM;     break;
        case ButtonState::RIGHT:    m_state = State::WEATHER;       break;
        case ButtonState::UP:       LOG("No action\n");          break;
        case ButtonState::DOWN:     LOG("No action\n");          break;
    }
}
void Program::weather(){
    LOG("STATE: WEATHER\n");

    auto thread = std::make_unique<Thread>();
    LOG("MADE THREAD");

    thread->start([this] {
        LOG("TRYING TO DO THINGS");
        m_API.GetDailyForecastByCoordinates();
        LOG("DDID THINGS");
        });

    m_display.SetThreadPointer(std::move(thread)); // move ownership
    LOG("SENT");


    ButtonState buttonState = waitForSingleButtonPress();

    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::TEMPHUMID;     break;
        case ButtonState::RIGHT:    m_state = State::NEWS;          break;
        case ButtonState::UP:       m_state = State::SETLOC;        break;
        case ButtonState::DOWN:     LOG("No action\n");             break;
    }

}

// TODO implement location thing.
void Program::setloc(){
    LOG("STATE: SETLOC\n");

    
    // TODO send to display
    // m_displayThread.flags_set(uint32_t(State::SETLOC));


    ButtonState buttonState = waitForSingleButtonPress();
    
    switch(buttonState){
        case ButtonState::LEFT:     LOG("No action\n");          break;
        case ButtonState::RIGHT:    LOG("No action\n");          break;
        case ButtonState::UP:       LOG("No action\n");          break;
        case ButtonState::DOWN:     LOG("No action\n");          break;
    }

}
void Program::news(){
    LOG("STATE: NEWS\n");

    // TODO send to display


    ButtonState buttonState = waitForSingleButtonPress();
    
    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::WEATHER;    break;
        case ButtonState::RIGHT:    m_state = State::SHOWALARM;  break;
        case ButtonState::UP:       LOG("No action\n");          break;
        case ButtonState::DOWN:     LOG("No action\n");          break;
    }

}