#include "mbed.h"
#include "DFRobot_RGBLCD1602.h"
#include "HTS221Sensor.h"
#include "sensor.h"
#include "Logger.h"
constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
#define LINE() LINE_IF(LOG_ENABLED)


Sensor::Sensor()
    : i2c(PB_11, PB_10), hts221(&i2c) {
    hts221.enable();
    hts221.init(NULL);
    thread_sleep_for(20); 
    hts221.reset();
}



temphumidstruct Sensor::getTempAndHum(){
    temphumidstruct tempHum;
    float t,h;
    hts221.get_temperature(&t);
    tempHum.temp = t;
    LOG("Temp: %f",t);
    hts221.get_humidity(&h);
    tempHum.humid = h;
    LOG("Hum: %f", h);
    return tempHum;
}

