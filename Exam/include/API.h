#pragma once
#include <mbed.h>
#include <stdio.h>
#include <NetworkInterface.h>
#include "json.hpp"
#include "Structs.h"
using json = nlohmann::json;





//Provides access to networking features for synchronizing time and fetching position
class API {

public:
// Fetches Unix timestamp and location + timezone and returns them in apiargs
  void StartUp();

  // Initializes with the shared struct references
  API(Datetime &datetime, Weather &weather, Location &coordinate, RSSStream &rssstream)
      : m_datetime(datetime), m_weather(weather),
        m_location(coordinate), m_rssstream(rssstream){};
  ~API();

  
  void GetDateTimeByCoordinates(); // Uses either manually entered or fetched coordinates to get UNIX time and offset
  void GetDailyForecastByCoordinates(); // Uses either manually entered or fetched coordinates to get forecast
  void GetRSS();                        // Gets rss top news feed from cnn

private:
  NetworkInterface *m_net = nullptr;
  SocketAddress *m_address = nullptr;

  Datetime &m_datetime;
  Weather &m_weather;
  Location &m_location;
  RSSStream &m_rssstream;

  void connectWiFi(); // Connects to wifi specified in mbed_app.json
  void connectToHost(TCPSocket &socket, const char *hostname); // Connects to specified host with the specified socket
  std::string getTimezoneData(Socket &socket); // Retrieves timezone data based on IP

  
  void parseJSON(json &j, const char *buffer);  // Parses a json response into a json object
  bool sanitizeJSON(const std::string& input, std::string& out);    // Sanitizes a json (removes uneccessary information and validates)
};