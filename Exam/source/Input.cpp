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
            printf("\n INPUT: LEFTon thread \n");
            printf("Input thread ID: %p\n", (void*)ThisThread::get_id());
            printf("m_programThreadID: %p\n", m_programThreadId);
            osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::LEFT));
            ThisThread::sleep_for(200ms); 
        }
        if (!m_up.read()) {
            printf("\n INPUT: UP\n");
            osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::UP));
            ThisThread::sleep_for(200ms); 
        }
        if (!m_down.read()) {
            printf("\n INPUT: DOWN\n");
            osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::DOWN));
            ThisThread::sleep_for(200ms); 
        }
        if (!m_right.read()) {
            printf("\n INPUT: RIGHT\n");
            osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::RIGHT));
            ThisThread::sleep_for(200ms); 
        }
    }
}