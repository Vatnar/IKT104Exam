#pragma once
#include <mbed.h>
#include <stdio.h>
#include <NetworkInterface.h>
#include "json.hpp"
using json = nlohmann::json;


struct Datetime {
  time_t timestamp;
  int offset;
  nsapi_error_t code;
} __attribute__((aligned(16)));

struct Coordinate {
    double latitude;
    double longitude;
} __attribute__((aligned(16)));


//Provides access to networking features for synchronizing time and fetching position
class API {

public:
// Fetches Unix timestamp and location + timezone and returns them in apiargs
  void StartUp();

  API(Datetime &apiDatetime) : datetime(apiDatetime){};

  void GetDateTimeByCoordinates(Coordinate coordinate);

private:
  NetworkInterface *net = nullptr;
  SocketAddress *address = nullptr;
  Datetime &datetime;

  void connectWiFi();
  void connectToHost(TCPSocket &socket, const char *hostname);
  char* getTimezoneData(Socket &socket);

  void parseJSON(json &j, char *http_response);
  bool sanitizeJSON(const std::string& input, std::string& out);
};