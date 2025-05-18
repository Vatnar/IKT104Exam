#include "API.h"
#include "Logger.h"
#include "json.hpp"
#include <ios>
constexpr bool LOG_ENABLED = false;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
#define LINE() LINE_IF(LOG_ENABLED)

using json = nlohmann::json;

void API::connectWiFi() {

  // Connect to default network interface
    LINE();
    m_net = NetworkInterface::get_default_instance();
    if (!m_net) {
        m_datetime.code = NSAPI_ERROR_NO_CONNECTION;
        return;
    }
    LOG("[INFO] Successfully got network interface");


    LINE();
    nsapi_size_or_error_t status  = 0;
    do {
        status = m_net->connect();

        if (status != NSAPI_ERROR_OK) {
            LOG("[WARN] Couldn't connect to network %d", status);
        }
    } while (status != NSAPI_ERROR_OK);
    LOG("[INFO] Connected to network");

    m_address = new SocketAddress;


    LINE();    
    do {
        LOG("[INFO] Getting local IP...");
        status = m_net->get_ip_address(m_address);

        if (status != NSAPI_ERROR_OK) {
            LOG("[WARN] Failed to get local I %d", status);
        }
    }
    while (status != NSAPI_ERROR_OK);

    LOG("[INFO] Connected to WLAN and received IP address: %s",
        m_address->get_ip_address());
    LINE();
    if (m_address->get_ip_address() == "0.0.0.0") {
      LOG("[ERROR] DIDNT GEt API, WAIT A FEW MINUTES\n\n\n\n\n\n\n\n");
    }
}

// Retries if failed
void API::connectToHost(TCPSocket &socket, const char *hostname) {
    const int MAX_DNS_RETRIES = 3;
    const int MAX_CONNECT_RETRIES = 3;
    const int RETRY_DELAY_MS = 1000; // 1 second

    nsapi_size_or_error_t status = socket.open(m_net);
    socket.set_timeout(500);

    if (status != NSAPI_ERROR_OK) {
        LOG("[WARN] Failed to open TCPSocket: %d", status);
        return;
    }

    LINE();

    // Retry DNS resolution
    int dns_attempts = 0;
    do {
        status = m_net->gethostbyname(hostname, m_address);
        if (status == NSAPI_ERROR_OK) break;

        LOG("[WARN] gethostbyname(%s) attempt %d failed: %d", hostname, dns_attempts + 1, status);
        ThisThread::sleep_for(RETRY_DELAY_MS);
    } while (++dns_attempts < MAX_DNS_RETRIES);

    if (status != NSAPI_ERROR_OK) {
        LOG("[ERROR] DNS resolution failed after %d attempts for %s: %d", dns_attempts, hostname, status);
        m_datetime.code = status;
        return;
    }

    LOG("[INFO] IP address of server %s is %s", hostname, m_address->get_ip_address());
    m_address->set_port(80);

    LINE();

    // Retry connection
    int connect_attempts = 0;
    do {
        status = socket.connect(*m_address);
        if (status == NSAPI_ERROR_OK) break;

        LOG("[WARN] Connection attempt %d to %s failed: %d", connect_attempts + 1, hostname, status);
        ThisThread::sleep_for(RETRY_DELAY_MS);
    } while (++connect_attempts < MAX_CONNECT_RETRIES);

    if (status != NSAPI_ERROR_OK) {
        LOG("[ERROR] Failed to connect to %s on port 80 after %d attempts: %d", hostname, connect_attempts, status);
        socket.close();
        m_datetime.code = status;
        return;
    }

    LOG("Successfully connected to %s", hostname);
}


std::string API::getTimezoneData(Socket &socket ) {
  // TODO Perhaps put in env for code style
    const char *api_key = "";

    // Sends a request to the server
    char request[512];
    snprintf(request, sizeof(request),
             "GET /v2/timezone?apiKey=%s HTTP/1.1\r\n"
             "Host: api.ipgeolocation.io\r\n"
             "Connection: close\r\n\r\n",
             api_key);
    int bytes_sent = socket.send(request, strlen(request));
    if (bytes_sent < 0) {
        LOG("[ERROR] Failed to send request: %d", bytes_sent);
        return nullptr;
    }

    char buffer[2048];
    int total_bytes_received = 0;
    int bytes_received = 0;
    int retry_count = 0;
    const int max_retries = 3; 

    while (total_bytes_received < sizeof(buffer) - 1 && (bytes_received = socket.recv(buffer + total_bytes_received, sizeof(buffer) - 1 - total_bytes_received)) > 0) {
        total_bytes_received += bytes_received;
        retry_count = 0; 
    }

    if (bytes_received < 0) {
        LOG("[ERROR] Error receiving data: %d", bytes_received);
        socket.close();
        return "";
    }

    buffer[total_bytes_received] = '\0';

    if (total_bytes_received == 0) {
        while (retry_count < max_retries) {
            retry_count++;
            LOG("[WARN] No data received, retrying (%d/%d)...", retry_count, max_retries);
        
            ThisThread::sleep_for(std::chrono::milliseconds(1000)); // to not overwhelm server

            bytes_sent = socket.send(request, strlen(request));
            if (bytes_sent < 0) {
                LOG("[ERROR] Failed to send retry request: %d", bytes_sent);
                break; 
            }

            total_bytes_received = 0;
            bytes_received = 0;
            while (total_bytes_received < sizeof(buffer) - 1 && (bytes_received = socket.recv(buffer + total_bytes_received, sizeof(buffer) - 1 - total_bytes_received)) > 0) {
                total_bytes_received += bytes_received;
            }
            buffer[total_bytes_received] = '\0';
            LOG("[INFO] Retry response (%d bytes): %s", total_bytes_received, buffer);

            if (total_bytes_received > 0) {
                break; 
            }
        }
        if (total_bytes_received == 0) {
            LOG("[ERROR] Failed to receive data after %d retries.", max_retries);
            socket.close();
            m_datetime.code = NSAPI_ERROR_BUSY;
            return "";
        }
    }
    return std::string(buffer, total_bytes_received);
}


void API::StartUp() {
    connectWiFi();

    // New socket for use with host
    std::unique_ptr<TCPSocket> socket = std::make_unique<TCPSocket>();

    connectToHost(*socket, "api.ipgeolocation.io");
    std::string  buffer = getTimezoneData(*socket);

    
    json j;
    parseJSON(j, buffer.c_str());


    // Json is validated that it actually contains the information requested

    LOG("%d", (int)j["time_zone"]["date_time_unix"]);

    m_datetime.mutex.lock();
    if (j.contains("time_zone") &&
        j["time_zone"].contains("date_time_unix")) {
        m_datetime.timestamp = (int)j["time_zone"]["date_time_unix"];
        set_time(m_datetime.timestamp);
        m_datetime.code = NSAPI_ERROR_OK;
    } else {
        LOG("[ERROR] Missing timezone/unix in JSON");
        m_datetime.code = NSAPI_ERROR_PARAMETER;
    }

    if (j.contains("time_zone") &&
        j["time_zone"].contains("offset")) {
        m_datetime.offset = j["time_zone"]["offset"];
    } else {
        LOG("[ERROR] MIssing timezone/offset in JSON");
        m_datetime.code = NSAPI_ERROR_PARAMETER;
    }
    m_datetime.mutex.unlock();


    m_location.mutex.lock();

    if (j.contains("location") && j["location"].contains("longitude")) {
        auto lon_str = j["location"]["longitude"].get<std::string>();
        const char *cstr = lon_str.c_str();
        char* end;
        double lon = std::strtod(cstr, &end);

        if (end != lon_str.c_str()) {
          m_location.longitude = lon;
        } else {
            LOG("[ERROR] Failed to convert longitude string to double");
        }
    }
    if (j.contains("location") && j["location"].contains("latitude")) {
        LOG("DOES CONTAIN LOCATION");
        auto lon_str = j["location"]["latitude"].get<std::string>();
        const char* cstr = lon_str.c_str();  
        char* end;
        double lon = std::strtod(cstr, &end);

        if (end != lon_str.c_str()) {
          m_location.latitude = lon;
        } else {
            LOG("[ERROR] Failed to convert latitude string to double");
        }
    }
    m_location.city = j["location"]["city"].get<std::string>();

    LOG("long: %f, lat %f, city: %s", m_location.longitude, m_location.latitude, m_location.city.c_str());

    m_location.mutex.unlock();

    socket->close();
}

void API::GetDateTimeByCoordinates() {
  std::unique_ptr<TCPSocket> socket = std::make_unique<TCPSocket>();
  connectToHost(*socket, "api.ipgeolocation.io");


  
    const char *api_key = "";

    char request[512];


    m_location.mutex.lock();
    snprintf(request, sizeof(request),
             "GET /v2/timezone?apiKey=%s&lat=%f&long=%f HTTP/1.1\r\n"
             "Host: api.ipgeolocation.io\r\n"
             "Connection: close\r\n\r\n",
             api_key, m_location.latitude, m_location.longitude);
    m_location.mutex.unlock();
    
    int bytes_sent = socket->send(request, strlen(request));
    if (bytes_sent < 0) {
        LOG("[ERROR] Failed to send request: %d", bytes_sent);
        return;
    }

    char buffer[2048];
    int total_bytes_received = 0;
    int bytes_received = 0;
    int retry_count = 0;
    const int max_retries = 3; 

    while (total_bytes_received < sizeof(buffer) - 1 && (bytes_received = socket->recv(buffer + total_bytes_received, sizeof(buffer) - 1 - total_bytes_received)) > 0) {
        total_bytes_received += bytes_received;
        retry_count = 0; 
    }

    if (bytes_received < 0) {
        LOG("[ERROR] Error receiving data: %d", bytes_received);
        socket->close();
        return;
    }

    buffer[total_bytes_received] = '\0';

    if (total_bytes_received == 0) {
        while (retry_count < max_retries) {
            retry_count++;
            LOG("[WARN] No data received, retrying (%d/%d)...", retry_count, max_retries);
        
            ThisThread::sleep_for(std::chrono::milliseconds(1000)); // to not overwhelm server

            bytes_sent = socket->send(request, strlen(request));
            if (bytes_sent < 0) {
                LOG("[ERROR] Failed to send retry request: %d", bytes_sent);
                break; 
            }

            total_bytes_received = 0;
            bytes_received = 0;
            while (total_bytes_received < sizeof(buffer) - 1 && (bytes_received = socket->recv(buffer + total_bytes_received, sizeof(buffer) - 1 - total_bytes_received)) > 0) {
                total_bytes_received += bytes_received;
            }
            buffer[total_bytes_received] = '\0';
            LOG("[INFO] Retry response (%d bytes): %s", total_bytes_received, buffer);

            if (total_bytes_received > 0) {
                break; 
            }
        }
        if (total_bytes_received == 0) {
            LOG("[ERROR] Failed to receive data after %d retries.", max_retries);
            socket->close();
            m_datetime.code = NSAPI_ERROR_BUSY;
            return;
        }
    }
    socket->close();

    json j;
    parseJSON(j, buffer);

    int timestamp = (int)j["time_zone"]["date_time_unix"];
    int offset = j["time_zone"]["offset"];

    set_time(timestamp);
    m_datetime.mutex.lock();
    m_datetime.timestamp = timestamp;
    m_datetime.offset = offset;
    m_datetime.code = NSAPI_ERROR_OK;
    m_datetime.mutex.unlock();
}

void API::GetDailyForecastByCoordinates() {

    std::unique_ptr<TCPSocket> socket = std::make_unique<TCPSocket>();
    connectToHost(*socket, "api.openweathermap.org");

    const char *api_key = "";

    // Requests forecast
    char request[512];
    m_location.mutex.lock();
    snprintf(request, sizeof(request),
             "GET /data/2.5/weather?lat=%f&lon=%f&appid=%s HTTP/1.1\r\n"
             "Host: api.openweathermap.org\r\n"
             "Connection: close\r\n\r\n",
             m_location.latitude, m_location.longitude, api_key);
    m_location.mutex.unlock();

    int bytes_sent = socket->send(request, strlen(request));
    if (bytes_sent < 0) {
        LOG("[ERROR] Failed to send request: %d", bytes_sent);
        return;
    }

    char buffer[8192];
    int total_bytes_received = 0;
    int bytes_received = 0;

    while (total_bytes_received < sizeof(buffer) - 1 &&
           (bytes_received = socket->recv(buffer + total_bytes_received,
                                          sizeof(buffer) - 1 - total_bytes_received)) > 0) {
        total_bytes_received += bytes_received;
    }
    LOG("DID THIS");
    if (bytes_received < 0 || total_bytes_received == 0) {
        LOG("[ERROR] Failed to receive data.");
        socket->close();
        return;
    }

    buffer[total_bytes_received] = '\0';

    json j;
    parseJSON(j, buffer);


    std::string description = j["weather"][0]["description"].get<std::string>();
    float temp = float(j["main"]["temp"]) - 273.15;
    LINE();
    LOG("[info] Weather: %s, temp: %f", description.c_str(), temp);
    LOG("DID THIS");

    m_weather.mutex.lock();
    m_weather.description = description;
    m_weather.temp = temp;
    m_weather.mutex.unlock();


    socket->close();
}

void API::GetRSS() {
    std::unique_ptr<TCPSocket> socket = std::make_unique<TCPSocket>();
    connectToHost(*socket, "rss.cnn.com");

    // Request topstories from cnn
    const char *request = "GET /rss/cnn_topstories.rss HTTP/1.1\r\n"
                          "Host: rss.cnn.com\r\n"
                          "Connection: close\r\n\r\n";

    int bytes_sent = socket->send(request, strlen(request));
    if (bytes_sent < 0) {
        LOG("[ERROR] Failed to send request: %d", bytes_sent);
        return;
    }
    LOG("YES");
    std::string recv_buffer;    
    std::string result;         
    int story_count = 0;

    char temp[512];             
    int bytes_received = 0;

    // Recieve and extract 3 stories
    while ((bytes_received = socket->recv(temp, sizeof(temp))) > 0) {
        recv_buffer.append(temp, bytes_received);

        size_t pos = 0;
        while (story_count < 3) {
            size_t item_start = recv_buffer.find("<item>", pos);
            if (item_start == std::string::npos) break;

            size_t item_end = recv_buffer.find("</item>", item_start);
            if (item_end == std::string::npos) break;  // Wait for more data

            std::string item_content = recv_buffer.substr(item_start, item_end + 7 - item_start);

            // Extract title
            size_t title_start = item_content.find("<title>");
            size_t title_end = item_content.find("</title>");

            if (title_start != std::string::npos && title_end != std::string::npos && title_end > title_start) {
                title_start += 16;
                std::string title = item_content.substr(title_start, title_end - title_start);
                result += title + "\n";
                story_count++;
            }

            pos = item_end + 4;  
        }

        if (pos > 0) {
            recv_buffer.erase(0, pos);
        }

        if (story_count >= 3) break; 
    }
    LOG("YES");
    if (bytes_received < 0) {
        LOG("[ERROR] Error receiving data: %d", bytes_received);
        socket->close();
        return;
    }

    m_rssstream.mutex.lock();
    m_rssstream.rss = std::move(result);
    m_rssstream.mutex.unlock();
    LOG("YES");
    socket->close();
}


bool API::sanitizeJSON(const std::string& input, std::string& out) {
    size_t start = input.find('{');
    if (start == std::string::npos) return false;

    // defines integer balance, if balance is 0 at the end we can extract a
    // valid substring. For every '{' balance increases and likewise for every '}'
    
    int balance = 0;
    for (size_t i = start; i < input.size(); ++i) {
        if (input[i] == '{') {
            balance++;
        } else if (input[i] == '}') {
            balance--;
            if (balance == 0) {
                out = input.substr(start, i - start + 1);
                return true;
            }
        }
    }

    return false; 
}

void API::parseJSON(json &j, const char *buffer) {

    std::string b = buffer;
    std::string s;
    if(!sanitizeJSON(b, s)){
      return;
    }
    j = json::parse(s);
}

// Deletes heap allocated object
API::~API() {
  delete m_address;
}
