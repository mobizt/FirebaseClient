/**
 * SYNTAX:
 *
 * DefaultEthernetNetwork::DefaultEthernetNetwork(<Firebase_SPI_ETH_Module>);
 *
 * <Firebase_SPI_ETH_Module> - The Firebase_SPI_ETH_Module class object that keeping the pointer to lwip driver for each
 * SPI Ethernet module (e.g. ENC28J60lwIP, W5100lwIP and W5500lwIP).
 *
 *
 * The ENC28J60 Ethernet module and ESP8266 board, SPI port wiring connection.
 *
 * ESP8266 (Wemos D1 Mini or NodeMCU)        ENC28J60
 *
 * GPIO12 (D6) - MISO                        SO
 * GPIO13 (D7) - MOSI                        SI
 * GPIO14 (D5) - SCK                         SCK
 * GPIO16 (D0) - CS                          CS
 * GND                                       GND
 * 3V3                                       VCC
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <FirebaseClient.h>

#include <WiFiClientSecure.h>

#include <ENC28J60lwIP.h>
// #include <W5100lwIP.h>
// #include <W5500lwIP.h>

#include <LwipEthernet.h>

/** Don't gorget to define this in src/Config.h or src/UserConfig.h
  #define ENABLE_ESP8266_ENC28J60_ETH
*/

/** For PlatformIO IDE, please set the lib_ldf_mode in platformio.ini as the following.
  lib_ldf_mode = chain+
*/

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

#define ETH_CS_PIN 16 // D0

ENC28J60lwIP eth(ETH_CS_PIN);
// Wiznet5100lwIP eth(ETH_CS_PIN);
// Wiznet5500lwIP eth(ETH_CS_PIN);

Firebase_SPI_ETH_Module spi_eth;

DefaultEthernetNetwork default_network(spi_eth);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

WiFiClientSecure ssl_client;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(default_network));

void setup()
{

    Serial.begin(115200);

    // https://github.com/esp8266/Arduino/blob/master/libraries/lwIP_Ethernet/examples/EthClient/EthClient.ino

    Serial.print("Connecting to Ethernet... ");

    if (!ethInitDHCP(eth))
    {
        Serial.println("no hardware found!");
        while (1)
        {
            delay(1000);
        }
    }

    while (!eth.connected())
    {
        Serial.printf(".");
        delay(500);
    }

    Firebase.printf("\nEthernet: IP Address: %s\n", eth.localIP().toString().c_str());

    /* Assign the pointer to Ethernet module lwip interface */
#if defined(ENABLE_ESP8266_ENC28J60_ETH)
    spi_eth.enc28j60 = &eth;
#endif
#if defined(ENABLE_ESP8266_W5100_ETH)
    spi_eth.w5100 = &eth;
#endif
#if defined(ENABLE_ESP8266_W5500_ETH)
    spi_eth.w5500 = &eth;
#endif

    Serial.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    ssl_client.setInsecure();
    ssl_client.setBufferSizes(4096, 1024);

    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "authTask");
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();
}

void asyncCB(AsyncResult &aResult)
{
    // WARNING!
    // Do not put your codes inside the callback and printResult.

    printResult(aResult);
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