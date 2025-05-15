#pragma once
#include <mbed.h>
#include <stdio.h>
#include <NetworkInterface.h>
#include "json.hpp"
#include "structs.h"
using json = nlohmann::json;





//Provides access to networking features for synchronizing time and fetching position
class API {

public:
// Fetches Unix timestamp and location + timezone and returns them in apiargs
  void StartUp();

  API(Datetime &datetime, Weather &weather, Coordinate &coordinate, RSSStream &rssstream)
      : m_datetime(datetime), m_weather(weather),
        m_coordinate(coordinate), m_rssstream(rssstream){};
  ~API();
  void GetDateTimeByCoordinates();
  void GetDailyForecastByCoordinates();
  void GetRSS();

private:
  NetworkInterface *m_net = nullptr;
  SocketAddress *m_address = nullptr;

  Datetime &m_datetime;
  Weather &m_weather;
  Coordinate &m_coordinate;
  RSSStream &m_rssstream;

  void connectWiFi();
  void connectToHost(TCPSocket &socket, const char *hostname);
  std::string getTimezoneData(Socket &socket);

  
  void parseJSON(json &j, const char *buffer);
  bool sanitizeJSON(const std::string& input, std::string& out);
};