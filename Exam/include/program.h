#ifndef PROGRAM_H
#define PROGRAM_H
#include <mbed.h>
#include "Input.h"
#define DEMO 1

enum class State {
    STARTUP     = 1 << 0,
    SHOWALARM   = 1 << 1,
    EDITHOUR    = 1 << 2,
    EDITMINUTE  = 1 << 3,
    DATETIME    = 1 << 4,
    TEMPHUMID   = 1 << 5,
    WEATHER     = 1 << 6,
    SETLOC      = 1 << 7,
    NEWS        = 1 << 8
};

#ifdef DEMO
class Display {
    public:
    void Init();
    void EventLoop();
};
class API{
    public:
    void doStartupStuff(void *ptr);
};
#endif

class Program {
    public:
    Program();
    void Start();
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