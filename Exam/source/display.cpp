#include "mbed-os/mbed.h"
#include "DFRobot_RGBLCD1602/DFRobot_RGBLCD1602.h"

I2C lcdI2C(D14, D15); 
DFRobot_RGBLCD1602 lcd(&lcdI2C); 

thread_sleep_for(80); // Trenger sleep for å initialisere LCD-displayet
lcd.clear();
lcd.display();

lcd.printf("innhold");

void updateDisplay()
{
    switch (state) {
    case STARTUP: displayStartup();             break;
    case DATETIME: displayDate();               break;
    case TEMPHUMID: displaySensor();            break;
    case WEATHER: displayWeather();             break;
    case NEWS: displayNews();                   break;
    case ALARM: displayDate(); displayAlarm();  break;
    case EDITHOUR;
    case EDITMINUTE;
    case SETLOCATION: setLocation();            break;
    }
}
void displayAlarm(){
    switch(alarmState){
        
    }
}