#pragma once
#include <mbed.h>


// Using unisgned long with bitshifts to store the different flags

// For internal state in program
enum class State: uint32_t {
    STARTUP     = 1UL << 0,
    SHOWALARM   = 1UL << 1,
    EDITALARM    = 1UL << 2,
    EDITMINUTE  = 1UL << 3,
    EDITENABLED = 1UL << 4,
    TEMPHUMID   = 1UL << 5,
    WEATHER     = 1UL << 6,
    SETLOC      = 1UL << 7,
    NEWS        = 1UL << 8,
};


// States buttons return from input
enum class ButtonState : uint32_t {
  LEFT = 1UL << 0,
  UP = 1UL << 1,
  DOWN = 1UL << 2,
  RIGHT = 1UL << 3,
  NONE = 1UL << 4
};

// Used to be able to wait for any state
constexpr uint32_t ANYSTATE =
    static_cast<uint32_t>(State::STARTUP) |
    static_cast<uint32_t>(State::SHOWALARM) |
    static_cast<uint32_t>(State::EDITALARM) |
    static_cast<uint32_t>(State::EDITMINUTE) |
    static_cast<uint32_t>(State::EDITENABLED) |
    static_cast<uint32_t>(State::TEMPHUMID) |
    static_cast<uint32_t>(State::WEATHER) |
    static_cast<uint32_t>(State::SETLOC) |
    static_cast<uint32_t>(State::NEWS);

// used to be able to wait for any input
constexpr uint32_t ANYBUTTONSTATE = static_cast<uint32_t>(ButtonState::LEFT) |
                                    static_cast<uint32_t>(ButtonState::UP) |
                                    static_cast<uint32_t>(ButtonState::DOWN) |
                                    static_cast<uint32_t>(ButtonState::RIGHT) |
                                    static_cast<uint32_t>(ButtonState::NONE);