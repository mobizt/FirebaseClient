/**
 * This example shows how to switch the network interfaces between WiFi and Ethernet.
 * The ESP32 and W5500 Ethernet module are used in this example.
 */

#include <Arduino.h>
#include <WiFi.h>

#include <Ethernet.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

#define WIZNET_RESET_PIN 26 // Connect W5500 Reset pin to GPIO 26 of ESP32 (-1 for no reset pin assigned)
#define WIZNET_CS_PIN 5     // Connect W5500 CS pin to GPIO 5 of ESP32
#define WIZNET_MISO_PIN 19  // Connect W5500 MISO pin to GPIO 19 of ESP32
#define WIZNET_MOSI_PIN 23  // Connect W5500 MOSI pin to GPIO 23 of ESP32
#define WIZNET_SCLK_PIN 18  // Connect W5500 SCLK pin to GPIO 18 of ESP32

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

uint8_t Eth_MAC[] = {0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01};

void printResult(AsyncResult &aResult);

void authHandler();

void connectWiFi();

void checkConnection(AsyncClientClass &a_client);

DefaultNetwork wifi_network(true);

EthernetNetwork ethernet_network(Eth_MAC, WIZNET_CS_PIN, WIZNET_RESET_PIN);

WiFiClientSecure wifi_ssl_client;

ESP_SSLClient ethernet_ssl_client;
EthernetClient ethernet_client;

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000);

FirebaseApp app;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(wifi_ssl_client, getNetwork(wifi_network));

AsyncResult aResult_no_callback;

unsigned long last_change_ms = 0;
bool wifi_once_connected = false;

void setup()
{

    Serial.begin(115200);

    connectWiFi();

    Serial.println();

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    wifi_ssl_client.setInsecure();

    ethernet_ssl_client.setInsecure();
    ethernet_ssl_client.setBufferSizes(1024, 512);
    ethernet_ssl_client.setClient(&ethernet_client);

    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);

    authHandler();
}

void loop()
{
    authHandler();

    app.loop();

    checkConnection(aClient);

    printResult(aResult_no_callback);
}

void authHandler()
{
    // Blocking authentication handler with timeout
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // The JWT token processor required for ServiceAuth and CustomAuth authentications.
        // JWT is a static object of JWTClass and it's not thread safe.
        // In multi-threaded operations (multi-FirebaseApp), you have to define JWTClass for each FirebaseApp,
        // and set it to the FirebaseApp via FirebaseApp::setJWTProcessor(<JWTClass>), before calling initializeApp.
        JWT.loop(app.getAuth());
        printResult(aResult_no_callback);
    }
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

void connectWiFi()
{
    if (!wifi_once_connected)
    {
        unsigned long ms = millis();
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        Serial.print("Connecting to Wi-Fi");
        while (WiFi.status() != WL_CONNECTED && millis() - ms < 10000)
        {
            Serial.print(".");
            delay(300);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            wifi_once_connected = true;
            Serial.println();
            Serial.print("Connected with IP: ");
            Serial.println(WiFi.localIP());
        }
        else
        {
            Serial.println(" failed!");
        }
    }
}

void checkConnection(AsyncClientClass &a_client)
{
    // If network was disconnected over 5 min, change the network interfaces every 1 min if it is still disconnected.

    unsigned long lastseen_timeout = 5 * 60 * 1000; // The timeout in 5 min since last seen.
    unsigned long change_timeout = 60 * 1000;       // Change network every 1 min.

    // We will check the last seen network connection with function networkLastSeen().
    // The function networkStatus() returns the current network connection status.

    if (!a_client.networkStatus() && millis() - last_change_ms > change_timeout && millis() - a_client.networkLastSeen() > lastseen_timeout)
    {
        last_change_ms = millis();
        if (a_client.getNetworkType() == firebase_network_data_default_network)
        {
            Serial.println("Switch to Ethernet network...");
            a_client.setNetwork(ethernet_ssl_client, getNetwork(ethernet_network));
        }
        else if (a_client.getNetworkType() == firebase_network_data_ethernet_network)
        {
            Serial.println("Switch to WiFi network...");
            connectWiFi();
            a_client.setNetwork(wifi_ssl_client, getNetwork(wifi_network));
        }
    }
}