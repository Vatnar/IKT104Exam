#pragma once
#include <chrono>
#include <mbed.h>
#include <string>

// Contains definitions of structs
struct RSSStream {
  Mutex mutex;
  std::string rss;
}__attribute__((aligned(16)));

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

struct Location {
    Mutex mutex;
    double latitude;
    double longitude;
    std::string city;
} __attribute__((aligned(16)));

struct Weather {
    Mutex mutex;
    std::string description;
    float temp;
} __attribute__((aligned(16)));

struct AlarmData {
    Mutex mutex;  
    int hour;
    int minute;
    bool enabled;
    bool automute;
    bool active;
    bool snoozed;
} __attribute__((aligned(16)));


struct TempLocationChange {
    bool locationChanging = false;
    bool latitudeChanging = true;
    int pos = 0;
    std::string latitude;
    std::string longitude;
}__attribute__((aligned(16)));

struct ClockData {
    const char* day;
    int date;
    const char* month;
    int hour;
    int minute;
};

struct EditAlarm {
    bool editing;
    int hour;
    int minute;
    int pos;             // posisjon for redigering (valgfritt om du vil støtte tegnredigering)
};
