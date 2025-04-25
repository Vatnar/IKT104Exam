#include "mbed.h"
#include "DFRobot_RGBLCD1602.h"
#include "HTS221Sensor.h"

I2C lcdI2C(D14, D15);
DFRobot_RGBLCD1602 lcd(&lcdI2C);
DevI2C i2c(PB_11, PB_10);
HTS221Sensor hts221(&i2c);

float humidity, temperature;
void print_sensor(float temperature, float humidity){
    lcd.printf("Temp: %.1f C\nHumidity: %.1f\%", temperature, humidity);
};

int main()
{

lcd.init();
thread_sleep_for(20); 
lcd.display();

hts221.enable();
hts221.init(NULL);
thread_sleep_for(20); 

hts221.reset();                         // Resets in case of sensor failure

while (true) {                          // Loop to run the program indefinetly
    hts221.get_humidity(&humidity);
    hts221.get_temperature(&temperature);

     thread_sleep_for(1000);                 // Sleeping for 1 second (1000ms)
}


return 0;
}
