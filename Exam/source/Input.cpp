#include "Input.h"
#include "States.h"
#include <mbed.h>
#define FLAG 0xFFFFFFFF
Input::Input(): m_left(D0, PullUp), m_up(D2, PullUp), m_down(D3, PullUp), m_right(D4, PullUp) {
    printf("INPUT: Assigned port and pullup for buttons\n");
}
void Input::Init(osThreadId_t programThreadId){
    m_programThreadId = programThreadId;
}
void Input::InputLoop(){
    while (true) {
        if (!m_left.read()) {
            osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::LEFT));
        }
        if (!m_up.read()) {
            osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::UP));
        }
        if (!m_down.read()) {
            osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::DOWN));
        }
        if (!m_right.read()) {
            osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::RIGHT));
        }
        ThisThread::sleep_for(100ms); 
    }
}
