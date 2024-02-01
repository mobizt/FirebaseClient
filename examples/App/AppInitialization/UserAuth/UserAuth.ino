
// Created by K. Suwatchai (Mobizt)
// Email: k_suwatchai@hotmail.com
// Github: https://github.com/mobizt/FirebaseClient
// Copyright (c) 2024 mobizt

/**
 * APP INITIALIZATION
 * ==================
 *
 * This library provides the APIs covers the different types of authentication and services.
 *
 * The FirebaseApp class was introduced as user authentication provider which provides the credentials data
 * used for accessing the Firebase services.
 *
 * The Database, Firestore, Functions, Messaging, Storage and Cloud Storage apps provided the functions based on
 * the specific Firebase services which take the auth credentials data from FirebaseApp.
 *
 * The FirebaseApp should be initialized first before use via static function initializeApp.
 *
 * SYNTAXES:
 *
 * initializeApp(<AsyncClient>, <FirebaseApp>, <user_auth_data>);
 *
 * The AsyncClient stores the user, server and network data which can be initialized as.
 *
 * WiFiClientSecure ssl_client;
 * DefaultNetwork network;
 * AsyncClient aClient(ssl_client, getNetwork(network));
 *
 * The FirebaseApp manage and stores the authentication data;
 *
 * The user_auth_data store the user credentials which can be obtained from one of following Auth classes.
 * UserAuth, ServiceAuth, CustomAuth, IDToken, AccessToken, CustomToken, LegacyToken and NoAuth via static function getAuth.
 *
 * SYNTAX:
 *
 * UserAuth user_auth(<api_key>, <user_email>, <user_password>, <expire>);
 *
 * <api_key> - API key can be obtained from Firebase console > Project Overview > Project settings.
 * <user_email> - The user Email that in the project.
 * <user_password> - The user password in the project.
 * <expire> - The expiry period in seconds (less than or equal to 3600).
 *
 * To use other network interfaces, network data from one of the following Network classes
 * can be assigned.
 *
 * DefaultNetwork, GenericNetwork, EthernetNetwork and GSMNetwork.
 *
 * The network data (network_config_data) can be obtained from these Network classes via static function getNetwork.
 *
 * Please see examples/App/NetworkInterfaces for the usage guidelines.
 *
 * The auth credential data can be set to Services App e.g. Database to initialize via function getApp.
 *
 * SYNTAX:
 *
 * app.getApp<T>(<services_app>);
 *
 * Deallocation of FirebaseApp causes these services apps uninitialized and cannot be used.
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

#if defined(ESP8266) || defined(ESP32)
#include <WiFiClientSecure.h>
#endif

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

void asyncCB(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

WiFiClientSecure ssl_client;

AsyncClient aClient(ssl_client, getNetwork(network));

void setup()
{

    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

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
    ms = millis();
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
}