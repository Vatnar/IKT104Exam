#pragma once
#include <chrono>
#include <mbed.h>
#include <string>

// Contains definitions of structs

// String containing the fetched rss stream 
struct RSSStream {
  Mutex mutex;
  std::string rss;
}__attribute__((aligned(16)));


// Struct containing fetched temperature
struct TempHumid {
    Mutex mutex;
    float temp;
    float humid;
} __attribute__((aligned(16)));


// Struct containing fetched unix timestamp with the timezone offset
struct Datetime {
    Mutex mutex;
    time_t timestamp;
    int offset;
    nsapi_error_t code;
} __attribute__((aligned(16)));

// Struct containing the fetched coordinates and city
struct Location {
    Mutex mutex;
    double latitude;
    double longitude;
    std::string city;
} __attribute__((aligned(16)));

// Struct containing the fetched weather description and temperature
struct Weather {
    Mutex mutex;
    std::string description;
    float temp;
} __attribute__((aligned(16)));

// Struct containing information about the alarm such as when it is set and booleans regarding its state
struct AlarmData {
    Mutex mutex;  
    int hour;
    int minute;
    bool enabled;
    bool automute;
    bool active;
    bool snoozed;
} __attribute__((aligned(16)));


// Struct used for managing temporary variables used to change coordinates
struct TempLocationChange {
    bool locationChanging = false;
    bool latitudeChanging = true;
    int pos = 0;
    std::string latitude;
    std::string longitude;
}__attribute__((aligned(16)));


// Used to store state while editing alarm
struct EditAlarm {
    bool editing;
    int hour;
    int minute;
    int pos;             
} __attribute__((aligned(16)));
