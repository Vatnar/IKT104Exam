#pragma once
#include "mbed.h"
#include "DFRobot_RGBLCD1602.h"
#include "HTS221Sensor.h"
#include "structs.h"


class Sensor {
    public:
    Sensor();

    temphumidstruct getTempAndHum();



    private:
    DevI2C i2c;
    HTS221Sensor hts221;

};