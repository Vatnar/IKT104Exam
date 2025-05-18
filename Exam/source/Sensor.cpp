#include "mbed.h"
#include "DFRobot_RGBLCD1602.h"
#include "HTS221Sensor.h"
#include "Sensor.h"
#include "Logger.h"
constexpr bool LOG_ENABLED = false;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
#define LINE() LINE_IF(LOG_ENABLED)

// References to shared struct
Sensor::Sensor(TempHumid &temphum) : m_i2c(PB_11, PB_10), m_hts221(&m_i2c), m_tempHumid(temphum) {
    m_hts221.enable();
    m_hts221.init(NULL);
    thread_sleep_for(20); 
    m_hts221.reset();
}


void Sensor::GetTempAndHum() {
    float temperature, humidity;
    m_hts221.get_temperature(&temperature);
    m_hts221.get_humidity(&humidity);

    m_tempHumid.mutex.lock();
    m_tempHumid.temp = temperature;
    m_tempHumid.humid = humidity;
    m_tempHumid.mutex.unlock();

    LOG("Temp: %f",temperature);
    LOG("Hum: %f", humidity);
}