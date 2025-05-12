#include "API.h"
#include "Logger.h"
constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)

void API::StartUp(startupStruct &apiargs) {
    // Connect to default network interface

    net = NetworkInterface::get_default_instance();
    if (!net) {
        apiargs.code = NSAPI_ERROR_NO_CONNECTION;
        return;
    }
    LOG("Successfully got network interface");


    //Temporarily disabled
    // nsapi_error_t status = net->connect();
    // if (status != NSAPI_ERROR_OK) {
    //     apiargs.code = status;
    //     LOG("[WARN] Did not connect to WiFI");
    //     return;
    // }
    // LOG("Successfully connected to WiFi");

    // Set system time
    // set_time(timestamp);

    // apiargs.timestamp = timestamp;
    // apiargs.code = NSAPI_ERROR_OK;
}
