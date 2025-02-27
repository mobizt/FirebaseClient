/**
 * ABOUT:
 *
 * The bare minimum example for working with the Ethernet network
 * using Ethernet.h.
 *
 * The divices used in this example are WIZnet W5500 SPI Ethernet module and ESP32.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 * 
 * EthernetNetwork::EthernetNetwork;
 * EthernetNetwork::EthernetNetwork(<mac>, <cs_pin>, <reset_pin>);
 * EthernetNetwork::EthernetNetwork(<mac>, <cs_pin>, <reset_pin>, <static_ip>);
 *
 * <mac> - The six bytes mac address.
 * <cs_pin> - The Ethernet module chip select/enable pin.
 * <reset_pin> - The Ethernet module reset pin. Assign -1 if not used.
 * <static_ip> - The pointer to Firebase_StaticIP object that holds the static ip configuration.
 *
 * Firebase_StaticIP staIP(<local_ip>, <subnet>, <gateway>, <dns_server>, <optional>);
 *
 * <local_ip>, <subnet>, <gateway> and <dns_server> - The static IP, subnet, default gateway and dns server IPs.
 * <optional> - The boolean option to force use static ip only (not use DHCP)
 * 
 * If no parameters assigned to the constructor, the Ethernet client initialization will be ignored.
 */

#include <Arduino.h>
#include <Ethernet.h>
#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

#define WIZNET_RESET_PIN 26 // Connect W5500 Reset pin to GPIO 26 of ESP32 (-1 for no reset pin assigned)
#define WIZNET_CS_PIN 5     // Connect W5500 CS pin to GPIO 5 of ESP32
#define WIZNET_MISO_PIN 19  // Connect W5500 MISO pin to GPIO 19 of ESP32
#define WIZNET_MOSI_PIN 23  // Connect W5500 MOSI pin to GPIO 23 of ESP32
#define WIZNET_SCLK_PIN 18  // Connect W5500 SCLK pin to GPIO 18 of ESP32

uint8_t Eth_MAC[] = {0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01};

EthernetClient eth;

EthernetNetwork eth_network(Eth_MAC, WIZNET_CS_PIN, WIZNET_RESET_PIN);

/* Or configure the static IP with EthernetNetwork.

IPAddress localIP(192, 168, 1, 104);
IPAddress subnet(255, 255, 0, 0);
IPAddress gateway(192, 168, 1, 1);
IPAddress dnsServer(8, 8, 8, 8);
bool optional = false; // True = use this static IP only no DHCP,  false = switch to DHCP if static failed.
Firebase_StaticIP staIP(localIP, subnet, gateway, dnsServer, optional);
EthernetNetwork eth_network(Eth_MAC, WIZNET_CS_PIN, WIZNET_RESET_PIN, staIP);

*/

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

// This is a library internal SSL client.
// You can use any SSL Client that works with Ethernet library.
// The ESP_SSLClient uses PSRAM by default (if it is available), for PSRAM usage, see https://github.com/mobizt/FirebaseClient#memory-options
// For ESP_SSLClient documentation, see https://github.com/mobizt/ESP_SSLClient
ESP_SSLClient ssl_client;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(eth_network));

void setup()
{
    Serial.begin(115200);

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl_client.setClient(&eth);

    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);
}

void loop()
{
    // To maintain the authentication and async tasks.
    app.loop();
}