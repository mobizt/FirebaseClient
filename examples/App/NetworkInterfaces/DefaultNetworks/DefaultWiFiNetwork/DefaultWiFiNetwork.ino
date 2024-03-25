
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
 * DefaultWiFiNetwork network(<FirebaseWiFi>, <reconnect>);
 *
 * The DefaultWiFiNetwork is the Network class that provides the WiFiMulti network configuarion to work in this library for the WiFiMulti supported devices.
 *
 * The DefaultWiFiNetwork class constructor arguments.
 *
 * <FirebaseWiFi> - The FirebaseWiFi class object that used for keeping the network credentials (WiFi APs and WiFi passwords).
 *
 * <reconnect> - The bool option for network reconnection.
 *
 * For normal WiFi, see examples/NetworkInterfaces/DefaultNetwork/DefaultNetwork.ino
 *
 *
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
#endif

#include <FirebaseClient.h>

#define WIFI_SSID1 "WIFI_AP1"
#define WIFI_PASSWORD1 "WIFI_PASSWORD1"

#define WIFI_SSID2 "WIFI_AP2"
#define WIFI_PASSWORD2 "WIFI_PASSWORD2"

#define WIFI_SSID3 "WIFI_AP3"
#define WIFI_PASSWORD3 "WIFI_PASSWORD3"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

void asyncCB(AsyncResult &aResult);

FirebaseWiFi wifimulti;

DefaultWiFiNetwork default_network(wifimulti, true /* reconnect network */);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(default_network));

void setup()
{

    Serial.begin(115200);

    wifimulti.addAP(WIFI_SSID1, WIFI_PASSWORD1);
    wifimulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
    wifimulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
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
        Firebase.printf("Event msg: %s, code: %d\n", aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug msg: %s\n", aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error msg: %s, code: %d\n", aResult.error().message().c_str(), aResult.error().code());
    }
}