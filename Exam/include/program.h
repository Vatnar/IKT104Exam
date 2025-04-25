#ifndef PROGRAM_H
#define PROGRAM_H
#include <mbed.h>
#include "Input.h"
#include "States.h"
#define DEMO 1



#ifdef DEMO
class Display {
    public:
    void Init();
    void EventLoop();
};
class API{
    public:
    void DoStuff(void *ptr);
};
#endif

class Program {
    public:
    Program();
    int ProgramLoop();
    private:
    void startup(ButtonState &buttonState);
    void showalarm(ButtonState &buttonState);
    void editenabled(ButtonState &buttonState);
    void edithour(ButtonState &buttonState);
    void editminute(ButtonState &buttonState);
    void temphumid(ButtonState &buttonState);
    void weather(ButtonState &buttonState);
    void setloc(ButtonState &buttonState);
    void news(ButtonState &buttonState);


    private:
    State m_state;

    Display m_display;
    Thread m_displayThread;

    API m_api;
    Thread m_apiThread;

    Input m_input;
    Thread m_inputThread;

};
#endif