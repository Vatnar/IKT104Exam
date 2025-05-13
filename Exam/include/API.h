#pragma once
#include <mbed.h>
#include <stdio.h>
#include <NetworkInterface.h>
#include "json.hpp"
using json = nlohmann::json;


struct startupStruct {
  time_t timestamp;
  int offset;
  nsapi_error_t code;
};


//Provides access to networking features for synchronizing time and fetching position
class API {

public:
// Fetches Unix timestamp and location + timezone and returns them in apiargs
  void StartUp();
  API(startupStruct &apiargs) : apiargs(apiargs){};


private:
  NetworkInterface *net = nullptr;
  SocketAddress *address = nullptr;
  startupStruct &apiargs;

  void connectWiFi();
  void connectToHost(TCPSocket &socket, const char *hostname);
  char* getTimezoneData(Socket &socket);

  void parseJSON(json &j, char *http_response);
  bool sanitizeJSON(const std::string& input, std::string& out);
};