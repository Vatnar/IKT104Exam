#ifndef INPUT_H
#define INPUT_H
#include <mbed.h>   

// InterruptIn kan ikke være medlemsvariabler så de må bli deklarert her.

class Input{

    public: 
    Input();
    void Init(osThreadId_t programThreadId);
    void InputLoop();

    private:


    osThreadId_t m_programThreadId;
    InterruptIn m_left;   
    InterruptIn m_up;
    InterruptIn m_down;
    InterruptIn m_right;

};
#endif