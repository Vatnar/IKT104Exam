#include "Input.h"
#include "States.h"
#include <mbed.h>
#include "Logger.h"

constexpr bool LOG_ENABLED = false;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)

#define FLAG 0xFFFFFFFF
Input::Input(): m_left(D0, PullUp), m_up(D2, PullUp), m_down(D3, PullUp), m_right(D4, PullUp) {
    LOG("INPUT: Assigned port and pullup for buttons\n");
}
void Input::Init(osThreadId_t programThreadId){
    m_programThreadId = programThreadId;
}
void Input::InputLoop() {
    while (true) {
        if (!m_left.read()) {
            LOG("LEFT\n");
            LOG("thread ID: %p\n", (void*)ThisThread::get_id());
            LOG("m_programThreadID: %p\n", m_programThreadId);
            auto result = osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::LEFT));
            if (int32_t(result) < 0) {
                LOG("Error setting LEFT flag: 0x%08X\n", result);
            }
            ThisThread::sleep_for(200ms);
        }
        if (!m_up.read()) {
            LOG("UP\n");
            auto result = osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::UP));
            if (int32_t(result) < 0) {
                LOG("Error setting UP flag: 0x%08X\n", result);
            }
            ThisThread::sleep_for(200ms);
        }
        if (!m_down.read()) {
            LOG("DOWN\n");
            auto result = osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::DOWN));
            if (int32_t(result) < 0) {
                LOG("Error setting DOWN flag: 0x%08X\n", result);
            }
            ThisThread::sleep_for(200ms);
        }
        if (!m_right.read()) {
            LOG("RIGHT\n");
            auto result = osThreadFlagsSet(m_programThreadId, uint32_t(ButtonState::RIGHT));
            if (int32_t(result) < 0) {
                LOG("Error setting RIGHT flag: 0x%08X\n", result);
            }
            ThisThread::sleep_for(200ms);
        }
    }
}
