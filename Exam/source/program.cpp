
#include "Program.h"
#include <mbed.h>
#include <stdio.h>
#include "States.h"
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
            case State::STARTUP: break;
            case State::SHOWALARM: break;
            case State::EDITHOUR: break;
            case State::EDITMINUTE: break;
            case State::DATETIME: break;
            case State::TEMPHUMID: break;
            case State::WEATHER: break;
            case State::SETLOC: break;
            case State::NEWS: break;
        }
        switch (buttonState){
        
        }

    }
    return 0;
}