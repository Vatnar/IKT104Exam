#include "mbed.h"
#include "DFRobot_RGBLCD1602.h"
#include "HTS221Sensor.h"

I2C lcdI2C(D14, D15);
DFRobot_RGBLCD1602 lcd(&lcdI2C);
DevI2C i2c(PB_11, PB_10);
HTS221Sensor hts221(&i2c);

lcd.init();
lcd.display();