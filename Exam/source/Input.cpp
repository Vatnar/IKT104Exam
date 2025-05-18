#include "Input.h"
#include "States.h"
#include <mbed.h>
#include "Logger.h"



 constexpr bool LOG_ENABLED = false;

    #define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
    #define LINE() LINE_IF(LOG_ENABLED)


#define FLAG 0xFFFFFFFF
Input::Input(): m_left(D0, PullUp), m_up(D2, PullUp), m_down(D3, PullUp), m_right(D4, PullUp) {
}
void Input::Init(osThreadId_t programThreadId){
    m_programThreadId = programThreadId;
}
void Input::InputLoop() {
    while (true) {
        uint32_t flag = uint32_t(ButtonState::NONE);
        int pressedCount = 0;

        if (!m_left.read()) {
            flag = uint32_t(ButtonState::LEFT);
            pressedCount++;
        }
        if (!m_up.read()) {
            flag = uint32_t(ButtonState::UP);
            pressedCount++;
        }
        if (!m_down.read()) {
            flag = uint32_t(ButtonState::DOWN);
            pressedCount++;
        }
        if (!m_right.read()) {
            flag = uint32_t(ButtonState::RIGHT);
            pressedCount++;
        }
          osThreadFlagsClear(ANYBUTTONSTATE);

        if (pressedCount == 1) {
            osThreadFlagsSet(m_programThreadId, flag);
        } else {
            osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::NONE));
        }

        ThisThread::sleep_for(200ms);
    }
}

