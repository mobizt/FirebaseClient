
// Created by K. Suwatchai (Mobizt)
// Email: k_suwatchai@hotmail.com
// Github: https://github.com/mobizt/FirebaseClient
// Copyright (c) 2024 mobizt

/**
 * GENERIC NETWORK CLASS INITIALIZATION
 * ====================================
 *
 *
 * SYNTAXES:
 *
 * GenericNetwork generic_network(<net_connect_callback>, <network_status_callback>);
 *
 * The GenericNetwork is the Network class that provides the generic network configuarion to work in this library.
 *
 * The GenericNetwork class constructor arguments.
 *
 * <net_connect_callback> - The network connection callback function.
 * <network_status_callback> - The network status callback function.
 *
 *
 * THE STATIC ASYNC RESULT REQUIRED FOR ASYNC OPERATION
 * ====================================================
 *
 * Library provided the class object called AsyncResult that keeps the server response data, debug and error information.
 *
 * There are two sources of async result in this library:
 *
 * 1. From user provided with async application (function).
 *
 * For example:
 *
 * `Database.get(aClient, "/test/int", options, aResult);`
 *
 * 2. From dynamic allocation in the async client.
 *
 * For example:
 *
 * `Database.get(aClient, "/test/int", options, asyncCB);`
 *
 * From source 1, the async result (`aResult`) shall be defined globally to use in async application because of the static data is needed for use while running the sync task.
 *
 * From source 2, the async client (`aClient`) shall be defined globally to use in async application too to make sure the instance of async result was existed or valid while running the sync task.
 *
 * If async result was destroyed (destructed or not existed) before it was used by async task handler, the danglig pointer problem will be occurred.
 *
 * Note that, the async client object used in authentication task shoul be defined globally as it is async task.
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

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

void netconnect();
void netStatus(bool &status);
void asyncCB(AsyncResult &aResult);

GenericNetwork generic_network(netconnect, netStatus);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

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

AsyncClient aClient(ssl_client, getNetwork(generic_network));

void setup()
{

  Serial.begin(115200);

  netconnect();

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

void netconnect()
{
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
}

void netStatus(bool &status)
{
  status = WiFi.status() == WL_CONNECTED;
}