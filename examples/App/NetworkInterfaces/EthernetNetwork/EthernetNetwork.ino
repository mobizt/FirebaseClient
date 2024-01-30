
// Created by K. Suwatchai (Mobizt)
// Email: k_suwatchai@hotmail.com
// Github: https://github.com/mobizt/FirebaseClient
// Copyright (c) 2024 mobizt

/**
 * ETHERNET NETWORK CLASS INITIALIZATION
 * =====================================
 *
 *
 * SYNTAXES:
 *
 * EthernetNetwork eth_network(<mac>, <cs_pin>, <reset_pin>);
 * EthernetNetwork eth_network(<mac>, <cs_pin>, <reset_pin>, <static_ip>);
 *
 * Firebase_StaticIP staIP(<local_ip>, <subnet>, <gateway>, <dns_server>, <optional>);
 *
 * The EthernetNetwork is the Network class that provides the Ethernet network configuarion to work in this library.
 *
 * The EthernetNetwork class constructor arguments.
 *
 * <mac> - The six bytes mac address.
 * <cs_pin> - The Ethernet module chip select/enable pin.
 * <reset_pin> - The Ethernet module reset pin. Assign -1 if not used.
 * <static_ip> - The pointer to Firebase_StaticIP object that holds the static ip configuration.
 *
 * The Firebase_StaticIP class constructor arguments.
 *
 * <local_ip>, <subnet>, <gateway> and <dns_server> - The static IP, subnet, default gateway and dns server IPs.
 * <optional> - The boolean option to force use static ip only (not use DHCP)
 *
 */

#include <Arduino.h>
#include <FirebaseClient.h>

#include <Ethernet.h>

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

/* Define the static IP (Optional)
IPAddress localIP(192, 168, 1, 104);
IPAddress subnet(255, 255, 0, 0);
IPAddress gateway(192, 168, 1, 1);
IPAddress dnsServer(8, 8, 8, 8);
bool optional = false; // Use this static IP only no DHCP
Firebase_StaticIP staIP(localIP, subnet, gateway, dnsServer, optional);
*/

void asyncCB(AsyncResult &aResult);

EthernetClient eth;

EthernetNetwork eth_network(Eth_MAC, WIZNET_CS_PIN, WIZNET_RESET_PIN);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

WiFiClientSecure ssl_client;

AsyncClient aClient(ssl_client, getNetwork(eth_network));

void setup()
{

    Serial.begin(115200);

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;
}

void loop()
{
    // This function is required for handling and maintaining the authentication tasks.
    app.loop();
}

void asyncCB(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
    {
        Serial.println("**************");
        Serial.printf("Event msg: %s, code: %d\n", aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isError())
    {
        Serial.println("**************");
        Serial.printf("Error msg: %s, code: %d\n", aResult.error().message().c_str(), aResult.error().code());
    }
}