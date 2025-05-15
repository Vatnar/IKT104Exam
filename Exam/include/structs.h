#pragma once
#include <mbed.h>

struct temphumidstruct{
Mutex mutex;
float temp;
float humid;
};