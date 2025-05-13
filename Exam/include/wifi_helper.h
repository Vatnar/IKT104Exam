// #pragma once
// #include "mbed.h"
// #include "cert.h"
// #include "mbed.h"
// #include <stdexcept>
// #include <string>
// #include <json.hpp>

// #include "HTS221Sensor.h"
// #define WAIT_TIME_MS 500ms
// #define DISCO_BLUE_BUTTON PC_13

// DevI2C i2c (PB_11, PB_10);
// HTS221Sensor sensor(&i2c);
// UnbufferedSerial serial_port(USBTX, USBRX, 115200);

// static bool make_a_http_request = false;

// static void blue_button_interrupt_cb() { make_a_http_request = true; }

// // Setter pointer til NetworkInterfacet
// // Har en dobbel pointer siden NetworkInterface i seg selv er en pointer
// void get_network_interface(NetworkInterface **network) {
// do {
// printf("Skaffer pointer to network interface...\n");
// (*network) = NetworkInterface::get_default_instance();

// if (!(*network)) {
//     printf("Fikk ikke tak i default network interface\n");
// }
// // Vil prøve på nytt så lenge networkinterfacet ikke har blit "skaffet"
// } while ((*network) == nullptr);
// }

// // Kobler til Wifi StudentFOU
// void connect_to_network(NetworkInterface **network) {
// // Trenger ikke å sjekke om network eksisterer siden denne funksjonen ikke blir kalt hvis forrige ikke fullfører.
// nsapi_size_or_error_t result;
// do {
//     printf("Kobler til nettverket...\n");
//     result = (*network)->connect();

//     if (result != NSAPI_ERROR_OK) {
//         printf("Greidde ikke å koble til nettverket: %d\n", result);
//     }
// } while (result != NSAPI_ERROR_OK);

// printf("Koblet til nettverket :)\n");
// }

// // Får tak i local IP til mikrokontroller
// void get_local_ip(NetworkInterface **network, SocketAddress *address) {
// nsapi_size_or_error_t result;

// do {
// printf("Skaffer local IP...\n");
// result = (*network)->get_ip_address(address);

// if (result != NSAPI_ERROR_OK) {
//     printf("Feilet på å få local IP: %d\n", result);
// }
// } while (result != NSAPI_ERROR_OK);

// printf("Koblet tilWLAN og har IP address %s\n",
//         address->get_ip_address());
// }
// // Åpner en socket og kobler den til nettverksinterfacet
// void configure_socket(NetworkInterface**network, TCPSocket *socket) {
// nsapi_size_or_error_t result;

// result = socket->open(*network);
//     // Uten set_timout blir det kastet en exception før den får koblet seg til.
//     socket->set_timeout(500);

//     if (result != NSAPI_ERROR_OK) {
//     printf("Failed to open TCPSocket: %d\n", result);
//     }
//     // Setter opp TCP sertifikatene som er definert i cert.h
// }
// // Gjør catfact.ninja om til en IPv4 addresse, og feilsjekker om feil med nameservere.
// void resolve_hostname(NetworkInterface **network, TCPSocket *socket, SocketAddress *address, const char host[]) {

// nsapi_size_or_error_t result = (*network)->gethostbyname(host, address);
// if (result != 0) {
// printf("Error! gethostbyname(%s) returned: %d\r\n", host, result);
// }

// printf("IP address of server %s is %s\n", host, address->get_ip_address());

// if (result != NSAPI_ERROR_OK) {
// printf("DNS resolution failed for %s: %d\n", host, result);
// }
// // Setter HTTPS porten til catfact
// address->set_port(9090); 
// }

// void connect_to_host(TCPSocket *socket, SocketAddress *address, const char host[]) {
// // Kobler til catfact.ninja serveren
// nsapi_size_or_error_t result = socket->connect(*address);

// // Sjekker om mikrokontrolleren greier å koble seg til serveren, hvis ikke lukker vi den
// if (result != NSAPI_ERROR_OK) {
// printf("Failed to connect to %s on port 9090: %d\n", host, result);
// socket->close();
// }

// printf("Successfully connected to %s\n", host);
// }

// void send_sensor_data(TCPSocket *socket, float temp, float humidity){
// // HTTP POST request for å sende data
//     char *request = (char*)malloc(sizeof(char)*400);
//     memset(request, 0, sizeof(*request)*400);
//     char *content = (char*)malloc(sizeof(char)*70);
//     memset(content, 0,sizeof(*content)*100);

//     // sprintf(content, "{\r\n\"temperature\": %f\r\n\"humidity\": %f\r\n}\r\n", temp, humidity);
//     sprintf(content, "{\"temperature\": %f, \"humidity\": %f}\r\n", temp, humidity);
//     auto host = "10.245.30.49";

//     //sprintf(request,"POST /api/v1/sh4t7m5rbtx8sskjov19/telemetry HTTP/1.1\r\nHost: 10.245.30.49\r\nContent-Type: application/json\r\nContent-Length: %d\r\n{\r\n\"temperature\": %f\r\n\"humidity\": %f\r\n}\r\n", strlen(content), temp, humidity);
//     snprintf(request, 400,
//             "POST /api/v1/sh4t7m5rbtx8sskjov19/telemetry HTTP/1.1\r\n"
//             "HOST: %s\r\n"
//             "Content-Type: application/json\r\n"
//             "Content-Length: %d\r\n"
//             "Connection: Keep-Alive\r\n\r\n"
//             "%s",
//             host, strlen(content), content);

//     // kan ikke nødvendigvis sende hele på en gang, så må holde styr på hvor mye vi har sent
//     nsapi_size_t bytes_sent = 0;
//     // Enn så lenge vi har mer å sende
//     while (bytes_sent < strlen(request)) {
        
//     // request + bytes_sent gir en pointer til der hvor vi skal starte å sende fra.
//     nsapi_size_or_error_t result =
//         socket->send(request + bytes_sent, strlen(request) - bytes_sent);
//     if (result < 0) {
//         printf("Error sending request: %d\n", result);
//         break;
//     }
//     // result av send gir oss hvor mange vi sendte
//     bytes_sent += result;
//     }
//     printf("Sendte %d bytes:\n", bytes_sent);
//     free(request); free(content);
// }
// int main() {
// InterruptIn button(DISCO_BLUE_BUTTON, PullNone); // Blå knappen som allerde er på brettet

// button.fall(blue_button_interrupt_cb); 			// Kobler opp falling edge til callback funksjonen

// // Vi må allokere minnet til NetworkInterface** siden vi bruker network til å passe inn til funksjoner som endrer verdiene deres
// NetworkInterface **network = new(NetworkInterface*);
// // Setter "selve" network interfaced til nullptr siden vi ikka har "skaffet den" enda
// *network = nullptr;

// get_network_interface(network);
// connect_to_network(network);

// SocketAddress *address = new SocketAddress;
// get_local_ip(network, address);

// while (true) {
//     ThisThread::sleep_for(WAIT_TIME_MS);
//     // fortsetter bare forbi dette punktet hvis den blå knappen har blitt trykket
//     if (!make_a_http_request) {
//     continue;
//     }

//     make_a_http_request = false;

//     TCPSocket *socket = new TCPSocket;
//     configure_socket(network, socket);

//     const char *hostname = "10.245.30.49";
//     resolve_hostname(network, socket, address, hostname);
//     connect_to_host(socket, address, hostname);
//     sensor.init(NULL);
//     sensor.enable();

//     float temperature = 0.0f, humidity = 0.0f;
//     sensor.reset();
    
//     while (true){
//     ThisThread::sleep_for(1s);
//     sensor.get_humidity(&humidity);
//     sensor.get_temperature(&temperature);

//     send_sensor_data(socket, temperature, humidity);
//     }

//     socket->close();
//     delete socket;
//     printf("\n Socket lukket.\n");
// }

// delete network;
// delete address;
// return 0;
// }