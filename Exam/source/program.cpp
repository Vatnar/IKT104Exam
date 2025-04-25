
#include "Program.h"
#include <mbed.h>
#include <stdio.h>

Program::Program(){
    printf("Initializing program");
    printf("."); printf(".\n");
    
    struct startupStruct {
        time_t timestamp; 
        nsapi_error_t code;
    };
    startupStruct m_apiargs;

    // Henter UNIX timestamp og slikt
    m_apiThread.start([this, &m_apiargs]() {
        m_api.doStartupStuff(&m_apiargs);
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

void Display::EventLoop(){
    State state;
    while (true){
        #define ANY 0xFFFFFFFF
        state = (State)ThisThread::flags_wait_any(ANY, false);
        // Dette vil vente til en state er sendt, og behandle den
        switch (state){
            case State::STARTUP:    /*.....*/ break;
            case State::SHOWALARM: /*.....*/ break;
        }
    }
}