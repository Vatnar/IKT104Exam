#pragma once
#include <mbed.h>
#include <stdio.h>
#include <NetworkInterface.h>

struct startupStruct {
  time_t timestamp;
  int offset;
  nsapi_error_t code;
};


//Provides access to networking features for synchronizing time and fetching position
class API {

public:
// Fetches Unix timestamp and location + timezone and returns them in apiargs
void StartUp(startupStruct &apiargs);


private:
  NetworkInterface *net = nullptr;
  SocketAddress *address = nullptr;

  void ParseJSON(char *http_response);
};