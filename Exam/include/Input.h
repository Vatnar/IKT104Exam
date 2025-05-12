#pragma once
#include <mbed.h>   

// InterruptIn kan ikke være medlemsvariabler så de må bli deklarert her.

class Input{

    public: 
    Input();
    void Init(osThreadId_t programThreadId);
    void InputLoop();

    private:


    osThreadId_t m_programThreadId;
    DigitalIn m_left;   
    DigitalIn m_up;
    DigitalIn m_down;
    DigitalIn m_right;

};