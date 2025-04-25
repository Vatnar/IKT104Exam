
#include "Program.h"
#include <mbed.h>
#include <stdio.h>
#define ANYINPUT 0xFFFFFFFF

Program::Program(){
    printf("Initializing program\n");
    
    struct startupStruct {
        time_t timestamp; 
        nsapi_error_t code;
    };
    startupStruct m_apiargs;

    // Henter UNIX timestamp og slikt
    m_apiThread.start([this, &m_apiargs]() {
        m_api.DoStuff(&m_apiargs);
    });

    // init DISPLAY
    m_displayThread.start([this](){
        m_display.Init();
    });


    // Init INPUT
    m_inputThread.start([this]() {
    m_input.Init(ThisThread::get_id());
    }); 
    
    // Venter til threadsene er ferdige
    m_displayThread.join();
    m_apiThread.join();
    m_inputThread.join();
    
    m_displayThread.start([this](){
        m_display.EventLoop();
    });

    m_state = State::STARTUP;
    m_displayThread.flags_set((uint32_t)m_state);

    m_inputThread.start([this](){
        m_input.InputLoop();
    });



    printf("Program constructed\n");
}

int Program::ProgramLoop(){
    ButtonState buttonState;
    while (true){
        // Wait for button input, or interrupt from alarm.
        // Then handle the input based on what the current state is.
        
        buttonState = (ButtonState)ThisThread::flags_wait_any(ANYINPUT, true);
    
        switch (m_state){
            case State::STARTUP:        startup(buttonState);       break;
            case State::SHOWALARM:      showalarm(buttonState);     break;
            case State::EDITHOUR:       edithour(buttonState);      break;
            case State::EDITMINUTE:     editminute(buttonState);    break;
            case State::DATETIME:       datetime(buttonState);      break;
            case State::TEMPHUMID:      temphumid(buttonState);     break;
            case State::WEATHER:        weather(buttonState);       break;
            case State::SETLOC:         setloc(buttonState);        break;
            case State::NEWS:           news(buttonState);          break;
        }
    

    }
    return 0;
}
void Program::startup(ButtonState &buttonState){
    switch (buttonState){
        // Should not be able to do any actions while starting up
        default: break;
    }
}
// TODO refactor it to be better on switching instead of setting explicit
void Program::showalarm(ButtonState &buttonState){
    switch(buttonState){
        case ButtonState::LEFT:  m_state = State::NEWS;
        case ButtonState::RIGHT: m_state = State::TEMPHUMID;
        case ButtonState::UP:    m_state = State::EDITENABLED;
        case ButtonState::
    }

}
void Program::editenabled(ButtonState &buttonState){
    switch(buttonState){
        case ButtonState::DOWN: m_state = State::EDITHOUR;
    }
}
void Program::edithour(ButtonState &buttonState){
    switch(buttonState){

        case ButtonState::DOWN: m_state = State::EDITMINUTE;

    }

}
void Program::editminute(ButtonState &buttonState){
    switch(buttonState){
        case ButtonState::DOWN: m_state = State::EDITENABLED;
    }
}
void Program::datetime(ButtonState &buttonState){

}
void Program::temphumid(ButtonState &buttonState){
}
void Program::weather(ButtonState &buttonState){

}
void Program::setloc(ButtonState &buttonState){

}
void Program::news(ButtonState &buttonState){

}