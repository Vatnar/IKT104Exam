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
    if (!m_left.read()) {
            osThreadFlagsClear(ANYBUTTONSTATE);
            auto result = osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::LEFT));
            ThisThread::sleep_for(200ms);
            continue;

        }
        if (!m_up.read()) {
            osThreadFlagsClear(ANYBUTTONSTATE);
            auto result = osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::UP));
            ThisThread::sleep_for(200ms);
            continue;

        }
        if (!m_down.read()) {
            osThreadFlagsClear(ANYBUTTONSTATE);
          
            auto result = osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::DOWN));
            ThisThread::sleep_for(200ms);
            continue;

        }
        if (!m_right.read()) {
            osThreadFlagsClear(ANYBUTTONSTATE);
          
            auto result = osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::RIGHT));
            ThisThread::sleep_for(200ms);
            continue;
        }
            osThreadFlagsClear(ANYBUTTONSTATE);

        osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::NONE));
        ThisThread::sleep_for(200ms);
    }
}
