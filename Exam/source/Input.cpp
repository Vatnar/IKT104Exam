#include "Input.h"
#include <mbed.h>
#define FLAG 0xFFFFFFFF
Input::Input(): m_left(D0, PullUp), m_up(D2, PullUp), m_down(D3, PullUp), m_right(D4, PullUp) {
    printf("INPUT: Assigned port and pullup for buttons\n");
}
void Input::Init(osThreadId_t programThreadId){
    m_programThreadId = programThreadId;
}
void Input::InputLoop(){
    osThreadFlagsSet(m_programThreadId,FLAG);
}