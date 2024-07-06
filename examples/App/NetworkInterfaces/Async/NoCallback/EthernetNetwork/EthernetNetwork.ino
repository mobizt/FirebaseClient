/**
 * SYNTAX:
 *
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
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 */

#include <Arduino.h>
#include <Ethernet.h>
#include <FirebaseClient.h>

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

#define WIZNET_RESET_PIN 26 // Connect W5500 Reset pin to GPIO 26 of ESP32 (-1 for no reset pin assigned)
#define WIZNET_CS_PIN 5     // Connect W5500 CS pin to GPIO 5 of ESP32
#define WIZNET_MISO_PIN 19  // Connect W5500 MISO pin to GPIO 19 of ESP32
#define WIZNET_MOSI_PIN 23  // Connect W5500 MOSI pin to GPIO 23 of ESP32
#define WIZNET_SCLK_PIN 18  // Connect W5500 SCLK pin to GPIO 18 of ESP32

uint8_t Eth_MAC[] = {0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01};

void printResult(AsyncResult &aResult);

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

AsyncResult aResult_no_callback;

void setup()
{

    Serial.begin(115200);

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    ssl_client.setClient(&eth);

    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif

    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();

    printResult(aResult_no_callback);
}

void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}