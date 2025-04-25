#include "mbed-os/mbed.h"
#include "DFRobot_RGBLCD1602/DFRobot_RGBLCD1602.h"
#include "display.h"

#define STARTUP = 1
#define DATETIME = 2
#define TEMPHUMID = 3
#define WEATHER = 4
#define NEWS = 5
#define ALARM = 6
#define EDITHOUR = 7
#define EDITMINUTE = 8
#define SETLOCATION = 9

void Display::EventLoop() {
    State state;

    while (true) {
        
    }
}

void updateDisplay()
{
    switch (state) {
    case STARTUP: Display.Startup();             break;
    case DATETIME: displayDate();               break;
    case TEMPHUMID: displaySensor();            break;
    case WEATHER: displayWeather();             break;
    case NEWS: displayNews();                   break;
    case ALARM: displayDate(); displayAlarm();  break;
    case EDITHOUR;                              break;
    case EDITMINUTE;                            break;
    case SETLOCATION: setLocation();            break;
    }
}
void displayAlarm(){
    switch(alarmState){
        
    }
}