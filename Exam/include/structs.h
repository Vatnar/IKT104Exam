#pragma once
#include <chrono>
#include <mbed.h>
#include <string>

// Contains definitions of structs

struct TempHumid {
    Mutex mutex;
    float temp;
    float humid;
} __attribute__((aligned(16)));

struct Datetime {
    Mutex mutex;
    time_t timestamp;
    int offset;
    nsapi_error_t code;
} __attribute__((aligned(16)));

struct Coordinate {
    Mutex mutex;
    double latitude;
    double longitude;
} __attribute__((aligned(16)));

struct Weather {
    Mutex mutex;
    std::string description;
    float temp;
} __attribute__((aligned(16)));

struct AlarmData {
    Mutex mutex;  
    int hour      = 7;
    int minute    = 30;
    bool enabled  = false;
    bool active   = false;
    bool snoozed  = false;
} __attribute__((aligned(16)));