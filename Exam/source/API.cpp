#include "API.h"
#include "Logger.h"
#include "json.hpp"
constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
#define LINE() LINE_IF(LOG_ENABLED)

void API::StartUp(startupStruct &apiargs) {
    // Connect to default network interface
    LINE();
    net = NetworkInterface::get_default_instance();
    if (!net) {
        apiargs.code = NSAPI_ERROR_NO_CONNECTION;
        return;
    }
    LOG("[INFO] Successfully got network interface");


    LINE();
    nsapi_size_or_error_t status  = 0;
    do {
        status = net->connect();

        if (status != NSAPI_ERROR_OK) {
            LOG("[WARN] Couldn't connect to network %d", status);
        }
    } while (status != NSAPI_ERROR_OK);
    LOG("[INFO] Connected to network");

    address = new SocketAddress;


    LINE();    
    do {
        LOG("[INFO] Getting local IP...");
        status = net->get_ip_address(address);

        if (status != NSAPI_ERROR_OK) {
            LOG("[WARN] Failed to get local I %d", status);
        }
    }
    while (status != NSAPI_ERROR_OK);

    LOG("[INFO] Connected to WLAN and received IP address: %s",
        address->get_ip_address());
    LINE();






    TCPSocket *socket = new TCPSocket;


    status = socket->open(net);
     // Uten set_timout blir det kastet en exception før den får koblet seg til.
    socket->set_timeout(500);


        if (status != NSAPI_ERROR_OK) {
        LOG("[WARN] Failed to open TCPSocket: %d", status);
        }
        // Setter opp TCP sertifikatene som er definert i cert.h
    
    LINE();
    // const char *hostname = "http://api.ipgeolocation.io/v2/timezone?apiKey=dd8232f6deda47b7a1bf3eeb2512129d";
     const char *hostname = "api.ipgeolocation.io";
    

    status = net->gethostbyname(hostname, address);
    if (status != 0) {
        LOG("[WARN] gethostbyname(%s) returned: %d", hostname, status);
    }

    LOG("[INFO] IP address of server %s is %s", hostname, address->get_ip_address());

     if (status != NSAPI_ERROR_OK) {
     LOG("[WARN] DNS resolution failed for %s: %d", hostname, status);
     }
    address->set_port(80);


    LINE();
    status = socket->connect(*address);

    if (status != NSAPI_ERROR_OK) {
    LOG("[WARN] Failed to connect to %s on port 80: %d", hostname, status);
    socket->close();
    apiargs.code = status;
    return;
    }

    LOG("Successfully connected to %s", hostname);



    
    // TODO Perhaps put in env for code style
    const char *api_key = "dd8232f6deda47b7a1bf3eeb2512129d";

    char request[512];
    snprintf(request, sizeof(request),
             "GET /v2/timezone?apiKey=%s HTTP/1.1\r\n"
             "Host: api.ipgeolocation.io\r\n"
             "Connection: close\r\n\r\n",
             api_key);
    int bytes_sent = socket->send(request, strlen(request));
    if (bytes_sent < 0) {
        LOG("[ERROR] Failed to send request: %d", bytes_sent);
        return;
    }

    char buffer[2048];
    int total_bytes_received = 0;
    int bytes_received = 0;
    int retry_count = 0;
    const int max_retries = 3; // You can adjust the number of retries

    while (total_bytes_received < sizeof(buffer) - 1 && (bytes_received = socket->recv(buffer + total_bytes_received, sizeof(buffer) - 1 - total_bytes_received)) > 0) {
        total_bytes_received += bytes_received;
        retry_count = 0; // Reset retry count on successful reception
    }

    if (bytes_received < 0) {
        LOG("[ERROR] Error receiving data: %d", bytes_received);
        socket->close();
        return;
    }

    buffer[total_bytes_received] = '\0';
    LOG("[INFO] Full response (%d bytes): %s", total_bytes_received, buffer);

    if (total_bytes_received == 0) {
        while (retry_count < max_retries) {
            retry_count++;
            LOG("[WARN] No data received, retrying (%d/%d)...", retry_count, max_retries);
            // Consider adding a delay here to avoid overwhelming the server
            ThisThread::sleep_for(std::chrono::milliseconds(1000)); // Example delay of 1 second

            bytes_sent = socket->send(request, strlen(request));
            if (bytes_sent < 0) {
                LOG("[ERROR] Failed to send retry request: %d", bytes_sent);
                break; // Exit retry loop if send fails
            }

            total_bytes_received = 0;
            bytes_received = 0;
            while (total_bytes_received < sizeof(buffer) - 1 && (bytes_received = socket->recv(buffer + total_bytes_received, sizeof(buffer) - 1 - total_bytes_received)) > 0) {
                total_bytes_received += bytes_received;
            }
            buffer[total_bytes_received] = '\0';
            LOG("[INFO] Retry response (%d bytes): %s", total_bytes_received, buffer);

            if (total_bytes_received > 0) {
                break; // Exit retry loop if data is received
            }
        }
        if (total_bytes_received == 0) {
            LOG("[ERROR] Failed to receive data after %d retries.", max_retries);
            socket->close();
            apiargs.code = NSAPI_ERROR_BUSY; // Or a more appropriate error code
            return;
        }
    }

    // Find the start of the JSON body (look for the first '{' after the headers)
    const char *json_start = strstr(buffer, "\r\n\r\n");
    if (json_start != nullptr) {
        json_start += 4; // Move past the "\r\n\r\n"
        const char *first_brace = strchr(json_start, '{');
        if (first_brace != nullptr) {
            LOG("[INFO] JSON Body: %s", first_brace);
            ParseJSON(const_cast<char*>(first_brace)); // Pass only the JSON body
        } else {
            LOG("[ERROR] Could not find the start of the JSON object");
        }
    } else {
        LOG("[ERROR] Could not find JSON body in the response");
    }





    // Get unix timestamp
    // Get location based on IP
    // Get timezone based on location


    // Set system time
    // set_time(timestamp);

    // apiargs.timestamp = timestamp;
    // apiargs.code = NSAPI_ERROR_OK;
}


bool findFirstBalancedBlock(const std::string& input, std::string& out) {
    size_t start = input.find('{');
    if (start == std::string::npos) return false;

    // 
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

void API::ParseJSON(char *buffer) {
    using json = nlohmann::json;

    LOG("[DEBUG] ParseJSON received buffer starting with: %.*s", 20, buffer); // Log the first 20 characters
    std::string j = buffer;
    std::string s;

    if (!findFirstBalancedBlock(j, s)){
      LOG("[WARN] coulndt get");
      return;
    }
    json parsed_json = json::parse(s);


        LOG("[INFO] City: %s", parsed_json["location"]["city"].get<std::string>().c_str());
        LOG("[INFO] Country: %s", parsed_json["location"]["country_name"].get<std::string>().c_str());
        LOG("[INFO] Time Zone: %s", parsed_json["time_zone"]["name"].get<std::string>().c_str());
        LOG("[INFO] DST Start: %s", parsed_json["time_zone"]["dst_start"]["utc_time"].get<std::string>().c_str());
}