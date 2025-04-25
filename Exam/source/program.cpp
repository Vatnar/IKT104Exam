
#include "Program.h"
#include <mbed.h>
#include <stdio.h>

Program::Program(){
    printf("Initializing program\n");
    
    struct startupStruct {
        time_t timestamp; 
        nsapi_error_t code;
    };
    startupStruct m_apiargs;

    // Henter UNIX timestamp og slikt
    // m_apiThread.start([this, &m_apiargs]() {
        // m_api.DoStuff(&m_apiargs);
    // });

    // init DISPLAY
    // m_displayThread.start([this](){
        // m_display.Init();
    // });


    // Init INPUT
    m_inputThread.start([this]() {
    m_input.Init(ThisThread::get_id());
    m_input.InputLoop();
});
 
    
    // Venter til threadsene er ferdige
    // m_displayThread.join();
    // m_apiThread.join();
    
    // m_displayThread.start([this](){
        // m_display.EventLoop();
    // });

    m_state = State::STARTUP;
    // m_displayThread.flags_set((uint32_t)m_state);

 



    printf("Program constructed\n");
}

int Program::ProgramLoop(){
    ButtonState buttonState;
    while (true){
        // Wait for button input, or interrupt from alarm.
        // Then handle the input based on what the current state is.
        
        uint32_t flags;

    if ((int32_t)flags < 0) {
        printf("Error: osThreadFlagsWait returned error: 0x%08x\n", flags);
        return -1; // or handle it however you want
    }

    buttonState = static_cast<ButtonState>(flags);
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
        }
    ThisThread::sleep_for(50ms);
    }
    return 0;
}
void Program::startup(ButtonState &buttonState){
    m_displayThread.flags_set(uint32_t(State::STARTUP));
    switch (buttonState){
        // Should not be able to do any actions while starting up
        default: break;
    }
}
// TODO refactor it to be better on switching instead of setting explicit
void Program::showalarm(ButtonState &buttonState){
    printf("STATE: SHOW ALARM\n");
    m_displayThread.flags_set(uint32_t(State::SHOWALARM));

    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::NEWS;          break;;
        case ButtonState::RIGHT:    m_state = State::TEMPHUMID;     break;
        case ButtonState::UP:       m_state = State::EDITENABLED;   break;
        case ButtonState::DOWN:     printf("No action\n");          break;
    }

}
void Program::editenabled(ButtonState &buttonState){
    printf("STATE: EDITENABLED\n");

    m_displayThread.flags_set(uint32_t(State::EDITENABLED));

    switch(buttonState){
        case ButtonState::LEFT:     printf("Toggle enabled\n");     break;
        case ButtonState::RIGHT:    printf("Toggle enabled\n");     break;
        case ButtonState::UP:       m_state = State::SHOWALARM;     break;
        case ButtonState::DOWN:     m_state = State::EDITHOUR;      break;
    }   
}
void Program::edithour(ButtonState &buttonState){
    printf("STATE: EDITHOUR\n");

    m_displayThread.flags_set(uint32_t(State::EDITHOUR));
    switch(buttonState){
        case ButtonState::LEFT:     printf("Value down\n");         break;
        case ButtonState::RIGHT:    printf("Value up\n");           break;
        case ButtonState::UP:       m_state = State::SHOWALARM;     break;
        case ButtonState::DOWN:     m_state = State::EDITMINUTE;    break;
    }

}
void Program::editminute(ButtonState &buttonState){
    printf("STATE: EDITMINUTE\n");

    m_displayThread.flags_set(uint32_t(State::EDITMINUTE));

    switch(buttonState){
        case ButtonState::LEFT:     printf("Value down\n");         break;
        case ButtonState::RIGHT:    printf("Value up\n");           break;
        case ButtonState::UP:       m_state = State::SHOWALARM;     break;
        case ButtonState::DOWN:     m_state = State::EDITENABLED;   break;
    }
}

void Program::temphumid(ButtonState &buttonState){
    printf("STATE: TEMPHUMID\n");

    m_displayThread.flags_set(uint32_t(State::TEMPHUMID));

    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::SHOWALARM;     break;
        case ButtonState::RIGHT:    m_state = State::WEATHER;       break;
        case ButtonState::UP:       printf("No action\n");          break;
        case ButtonState::DOWN:     printf("No action\n");          break;
    }
}
void Program::weather(ButtonState &buttonState){
    printf("STATE: WEATHER\n");

    m_displayThread.flags_set(uint32_t(State::WEATHER));

    switch(buttonState){
        case ButtonState::LEFT:     m_state = State::TEMPHUMID;     break;
        case ButtonState::RIGHT:    m_state = State::SHOWALARM;     break;
        case ButtonState::UP:       m_state = State::SETLOC;        break;
        case ButtonState::DOWN:     printf("No action\n");          break;
    }

}

// TODO implement location thing.
void Program::setloc(ButtonState &buttonState){
    printf("STATE: SETLOC\n");

    m_displayThread.flags_set(uint32_t(State::SETLOC));

    switch(buttonState){
        case ButtonState::LEFT:     printf("No action\n");
        case ButtonState::RIGHT:    printf("No action\n");
        case ButtonState::UP:       printf("No action\n");
        case ButtonState::DOWN:     printf("No action\n");
    }

}
void Program::news(ButtonState &buttonState){
    printf("STATE: NEWS\n");

    m_displayThread.flags_set(uint32_t(State::NEWS));

    switch(buttonState){
        case ButtonState::LEFT:     printf("No action\n");
        case ButtonState::RIGHT:    printf("No action\n");
        case ButtonState::UP:       printf("No action\n");
        case ButtonState::DOWN:     printf("No action\n");
    }

}