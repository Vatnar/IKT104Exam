#include "mbed.h"
#include "DFRobot_RGBLCD1602.h"
#include "HTS221Sensor.h"

class TempHum {
    public:
    TempHum();
    static float temperature;
    static float humidity;

    void SensorLoop();
    void SensorInit();

    private:
    float m_getTemperature();
    float m_getHumidity();

    DevI2C i2c;
    HTS221Sensor hts221;
};