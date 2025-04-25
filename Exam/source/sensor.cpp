#include "mbed.h"
#include "DFRobot_RGBLCD1602.h"
#include "HTS221Sensor.h"
#include "sensor.h"

float TempHum::temperature;
float TempHum::humidity;
TempHum::TempHum()
    : i2c(PB_11, PB_10), hts221(&i2c) {
}

void TempHum::SensorInit(){
hts221.enable();
hts221.init(NULL);
thread_sleep_for(20); 
};

float TempHum::m_getTemperature(){
hts221.get_temperature(&temperature);
return temperature;  
};

float TempHum::m_getHumidity(){
hts221.get_humidity(&humidity);
return humidity;
};

void TempHum::SensorLoop(){
    while (true){
        m_getTemperature();
        m_getHumidity();
        thread_sleep_for(5000);  
    }
};

