#include "mbed.h"
#include "DFRobot_RGBLCD1602.h"
#include "HTS221Sensor.h"
#include "sensor.h"
#include "Logger.h"
constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
#define LINE() LINE_IF(LOG_ENABLED)

float TempHum::temperature;
float TempHum::humidity;
TempHum::TempHum()
    : i2c(PB_11, PB_10), hts221(&i2c) {
}

void TempHum::SensorInit(){
hts221.enable();
hts221.init(NULL);
thread_sleep_for(20); 
hts221.reset();
};
/*
float TempHum::m_getTemperature(){
hts221.get_temperature(&temperature);
return temperature;  
};

float TempHum::m_getHumidity(){
hts221.get_humidity(&humidity);
return humidity;
};
*/
std::pair<float,float> TempHum::getTempAndHum(){
    float t,h;
    hts221.get_temperature(&t);
    LOG("Temp: %f",t);
    hts221.get_humidity(&h);
    LOG("Hum: %f", h);
    return std::make_pair(t, h);
}

