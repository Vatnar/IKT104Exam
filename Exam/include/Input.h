#pragma once
#include <mbed.h>   


class Input{

public:
    Input();

    // Give the thread id of pogram so that input can set the thread flags.
    void Init(osThreadId_t programThreadId);

    // Loop that runs indefinietely
    void InputLoop();

private:
      osThreadId_t m_programThreadId;


    // Buttons, DigitalIn is used so that interface can be more responsive
    DigitalIn m_left;   
    DigitalIn m_up;
    DigitalIn m_down;
    DigitalIn m_right;

};