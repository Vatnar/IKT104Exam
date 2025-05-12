#include "Program.h"
#include <mbed.h>
#include <stdio.h>
#include "Logger.h"

constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)

Program::Program(){
    LOG("Initializing program\n");
    
    struct startupStruct {
        time_t timestamp; 
        nsapi_error_t code;
    };
    startupStruct m_apiargs;

    // Henter UNIX timestamp og slikt
     m_apiStartupThread.start([this, &m_apiargs]() {
         m_api.Starup(&m_apiargs);
    // });

    // init DISPLAY
    // m_displayThread.start([this](){
        // m_display.Init();
    // });


    // Init INPUT
    osThreadId_t programThreadId = ThisThread::get_id();
    m_inputThread.start([this, programThreadId]() {
    m_input.Init(programThreadId);
    m_input.InputLoop();

});
 
    
    // Venter til threadsene er ferdige
    // m_displayThread.join();
    // m_apiThread.join();
    
    // m_displayThread.start([this](){
        // m_display.EventLoop();
    // });

    // TODO change to startup, and state should change again automatically after startup
    m_state = State::SHOWALARM;
    // m_displayThread.flags_set((uint32_t)m_state);

 



    LOG("Program constructed\n");
}

int Program::ProgramLoop(){
    ButtonState buttonState;
    int32_t flags = 0;
    while (true){
        // Wait for button input, or interrupt from alarm.
        // Then handle the input based on what the current state is.
        
        LOG("\n Before flags on thread %p\n", (void*)ThisThread::get_id());

        flags = ThisThread::flags_wait_any(ANYBUTTONSTATE);
    if (flags < 0) {
        LOG("Error: osThreadFlagsWait returned error: 0x%08x\n", (uint32_t)flags);
        continue;  // or return -1 if it's fatal
    }

    LOG("Flags received: 0x%08x\n", (uint32_t)flags);

    // Sanity check: one and only one flag
    if (__builtin_popcount((uint32_t)flags) != 1) {
        LOG("Error: Multiple or zero flags set: 0x%08x\n", (uint32_t)flags);
        continue;
    }

    buttonState = static_cast<ButtonState>(flags);
    LOG("Button state %d", buttonState);
        switch (m_state){
            case State::STARTUP:        startup(buttonState);       break;
            case State::SHOWALARM:      showalarm(buttonState);     break;
            case State::EDITHOUR:       edithour(buttonState);      break;
            case State::EDITMINUTE:     editminute(buttonState);    break;
            case State::EDITENABLED:    editenabled(buttonState);   break;
            case State::TEMPHUMID:      temphumid(buttonState);     break;
            case State::WEATHER:        weather(buttonState);       break;
            case State::SETLOC:         setloc(buttonState);        break;
            case State::NEWS:           news(buttonState);          break;
            default: LOG("Error: Unknown state %d\n", static_cast<int>(m_state)); break;
        }
    ThisThread::sleep_for(50ms);
    }
    return 0;
}
void Program::startup(ButtonState &buttonState){
    // TODO send to display
    // m_displayThread.flags_set(uint32_t(State::STARTUP));
    switch (buttonState){
        // Should not be able to do any actions while starting up
        default: break;
    }
}
// TODO refactor it to be better on switching instead of setting explicit
void Program::showalarm(ButtonState &buttonState){
    LOG("STATE: SHOW ALARM\n");
    
    // TODO send to display
    //m_displayThread.flags_set(uint32_t(State::SHOWALARM));

    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::NEWS;          break;
        case ButtonState::RIGHT:    m_state = State::TEMPHUMID;     break;
        case ButtonState::UP:       m_state = State::EDITENABLED;   break;
        case ButtonState::DOWN:     LOG("No action\n");          break;
    }

}
void Program::editenabled(ButtonState &buttonState){
    LOG("STATE: EDITENABLED\n");

    // TODO send to display
    // m_displayThread.flags_set(uint32_t(State::EDITENABLED));

    switch(buttonState){
        case ButtonState::LEFT:     LOG("Toggle enabled\n");     break;
        case ButtonState::RIGHT:    LOG("Toggle enabled\n");     break;
        case ButtonState::UP:       m_state = State::SHOWALARM;     break;
        case ButtonState::DOWN:     m_state = State::EDITHOUR;      break;
    }   
}
void Program::edithour(ButtonState &buttonState){
    LOG("STATE: EDITHOUR\n");

    // TODO send to display
    // m_displayThread.flags_set(uint32_t(State::EDITHOUR));
    switch(buttonState){
        case ButtonState::LEFT:     LOG("Value down\n");         break;
        case ButtonState::RIGHT:    LOG("Value up\n");           break;
        case ButtonState::UP:       m_state = State::SHOWALARM;     break;
        case ButtonState::DOWN:     m_state = State::EDITMINUTE;    break;
    }

}
void Program::editminute(ButtonState &buttonState){
    LOG("STATE: EDITMINUTE\n");

    // TODO send to display
    // m_displayThread.flags_set(uint32_t(State::EDITMINUTE));

    switch(buttonState){
        case ButtonState::LEFT:     LOG("Value down\n");         break;
        case ButtonState::RIGHT:    LOG("Value up\n");           break;
        case ButtonState::UP:       m_state = State::SHOWALARM;     break;
        case ButtonState::DOWN:     m_state = State::EDITENABLED;   break;
    }
}

void Program::temphumid(ButtonState &buttonState){
    LOG("STATE: TEMPHUMID\n");

    // TODO send to display
    // m_displayThread.flags_set(uint32_t(State::TEMPHUMID));

    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::SHOWALARM;     break;
        case ButtonState::RIGHT:    m_state = State::WEATHER;       break;
        case ButtonState::UP:       LOG("No action\n");          break;
        case ButtonState::DOWN:     LOG("No action\n");          break;
    }
}
void Program::weather(ButtonState &buttonState){
    LOG("STATE: WEATHER\n");

    // TODO send to display
    // m_displayThread.flags_set(uint32_t(State::WEATHER));

    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::TEMPHUMID;     break;
        case ButtonState::RIGHT:    m_state = State::NEWS;          break;
        case ButtonState::UP:       m_state = State::SETLOC;        break;
        case ButtonState::DOWN:     LOG("No action\n");             break;
    }

}

// TODO implement location thing.
void Program::setloc(ButtonState &buttonState){
    LOG("STATE: SETLOC\n");

    // TODO send to display
    // m_displayThread.flags_set(uint32_t(State::SETLOC));

    switch(buttonState){
        case ButtonState::LEFT:     LOG("No action\n");          break;
        case ButtonState::RIGHT:    LOG("No action\n");          break;
        case ButtonState::UP:       LOG("No action\n");          break;
        case ButtonState::DOWN:     LOG("No action\n");          break;
    }

}
void Program::news(ButtonState &buttonState){
    LOG("STATE: NEWS\n");

    // TODO send to display
    // m_displayThread.flags_set(uint32_t(State::NEWS));

    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::WEATHER;    break;
        case ButtonState::RIGHT:    m_state = State::SHOWALARM;  break;
        case ButtonState::UP:       LOG("No action\n");          break;
        case ButtonState::DOWN:     LOG("No action\n");          break;
    }

}