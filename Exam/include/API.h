#pragma once
#include <mbed.h>
#include <stdio.h>
#include <NetworkInterface.h>

struct startupStruct {
        time_t timestamp; 
        nsapi_error_t code;
    };

class API {

public:
void StartUp(startupStruct &apiargs);


private:
    NetworkInterface *net = nullptr;
};