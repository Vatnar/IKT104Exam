#pragma once
#include <mbed.h>
#include <stdio.h>
#include <NetworkInterface.h>
#include "json.hpp"
using json = nlohmann::json;


struct Datetime {
  Mutex mutex;
  time_t timestamp;
  int offset;
  nsapi_error_t code;
} __attribute__((aligned(16)));

struct Coordinate {
    Mutex mutex;
    double latitude;
    double longitude;
} __attribute__((aligned(16)));

struct Weather {
    Mutex mutex;
    std::string description;
    float temp;
} __attribute__((aligned(16)));


//Provides access to networking features for synchronizing time and fetching position
class API {

public:
// Fetches Unix timestamp and location + timezone and returns them in apiargs
  void StartUp();

  API(Datetime &datetime, Weather &weather, Coordinate &coordinate)
      : m_datetime(datetime), m_weather(weather),
        m_coordinate(coordinate){};
  ~API();
  void GetDateTimeByCoordinates();
  void GetDailyForecastByCoordinates();

private:
  NetworkInterface *m_net = nullptr;
  SocketAddress *m_address = nullptr;
  Datetime &m_datetime;
  Weather &m_weather;
  Coordinate &m_coordinate;

  void connectWiFi();
  void connectToHost(TCPSocket &socket, const char *hostname);
  std::string getTimezoneData(Socket &socket);

  void parseJSON(json &j, const char *buffer);
  bool sanitizeJSON(const std::string& input, std::string& out);
};