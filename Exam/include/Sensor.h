#pragma once
#include "mbed.h"
#include "DFRobot_RGBLCD1602.h"
#include "HTS221Sensor.h"
#include "Structs.h"


class Sensor {
public:
    // Takes a reference to a temphumid struct that program owns
    Sensor(TempHumid &temphum);

    // Caller can fetch temperature and humidity
    void GetTempAndHum();

private:
    TempHumid &m_tempHumid;

    DevI2C m_i2c;
    HTS221Sensor m_hts221;

};