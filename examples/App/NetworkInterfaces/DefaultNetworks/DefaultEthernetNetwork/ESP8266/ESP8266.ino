
// Created by K. Suwatchai (Mobizt)
// Email: k_suwatchai@hotmail.com
// Github: https://github.com/mobizt/FirebaseClient
// Copyright (c) 2024 mobizt

/**
 * DEFAULT NETWORK CLASS INITIALIZATION
 * ====================================
 *
 *
 * SYNTAXES:
 *
 * DefaultEthernetNetwork network(<SPI_ETH_Module>);
 *
 * The DefaultEthernetNetwork is the Network class that provides the built-in WiFi network configuarion to work in this library.
 *
 * The DefaultEthernetNetwork class constructor arguments.
 *
 * <SPI_ETH_Module> - The SPI_ETH_Module class object that keeping the pointer to lwip driver for each
 *                    SPI Ethernet module (e.g. ENC28J60lwIP, W5100lwIP and W5500lwIP).
 *
 *
 */

#include <Arduino.h>

/**
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
 */

#include <ESP8266WiFi.h>

#include <FirebaseClient.h>

#if defined(ESP8266) || defined(ESP32)
#include <WiFiClientSecure.h>
#endif

#include <ENC28J60lwIP.h>
// #include <W5100lwIP.h>
// #include <W5500lwIP.h>

/** Don't gorget to define this in src/Config.h or src/UserConfig.h
  #define ENABLE_ESP8266_ENC28J60_ETH
*/

/** For W5100 ethernet module and PlatformIO IDE, please set the lib_ldf_mode in platformio.ini as this
  lib_ldf_mode = chain+
*/

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

void asyncCB(AsyncResult &aResult);

#define ETH_CS_PIN 16 // D0

ENC28J60lwIP eth(ETH_CS_PIN);
// Wiznet5100lwIP eth(ETH_CS_PIN);
// Wiznet5500lwIP eth(ETH_CS_PIN);

SPI_ETH_Module spi_eth;

DefaultEthernetNetwork default_network(spi_eth);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

WiFiClientSecure ssl_client;

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(default_network));

void setup()
{

    Serial.begin(115200);

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

    // To get the authentication time to live in seconds before expired.
    // app.ttl();
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

    if (aResult.isDebug())
    {
        Serial.println("**************");
        Serial.printf("Debug msg: %s\n", aResult.debug().c_str());
    }
}