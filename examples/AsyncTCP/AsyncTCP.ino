
// Created by K. Suwatchai (Mobizt)
// Email: k_suwatchai@hotmail.com
// Github: https://github.com/mobizt/FirebaseClient
// Copyright (c) 2024 mobizt

/** This example does not include any async TCP client library, you have to include it prior to use and
 * async TCP should support SSL.
 *
 * To try the async TCP client, define the following macro in src/Config.h
 * or user created config file in src/UserConfig.h.

 #define ENABLE_ASYNC_TCP_CLIENT

 */

#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <FirebaseClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

void asyncCB(AsyncResult &aResult);

void AsyncTCPConnectCB(const char *host, uint16_t port);
void AsyncTCPStatusCB(bool &status);
void AsyncTCPSendCB(uint8_t *data, size_t size, uint32_t &sent);
void AsyncTCPReceiveCB(uint8_t *buff, size_t buffSize, int32_t &filledSize, uint32_t &available);
void AsyncTCPStop();

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

#if defined(ENABLE_ASYNC_TCP_CLIENT)
AsyncTCPConfig asyncTCP(AsyncTCPConnectCB, AsyncTCPStatusCB, AsyncTCPSendCB, AsyncTCPReceiveCB, AsyncTCPStop);

/**
 * In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
 * it with other name with keyword "using" or use the class name AsyncClientClass directly.
 */

using AsyncClient = AsyncClientClass;

AsyncClient aClient(asyncTCP, getNetwork(network));

#endif

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

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    app.setCallback(asyncCB);
#if defined(ENABLE_ASYNC_TCP_CLIENT)
    initializeApp(aClient, app, getAuth(user_auth));
#endif

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

/**
 * Async TCP Client Connection Request Callback.
 * @param host The host to connect.
 * @param port The port to connect.
 */
void AsyncTCPConnectCB(const char *host, uint16_t port)
{
}

/**
 * Async TCP Client Connection Status Callback.
 * @param status The server connection status. (set by TCP client)
 *
 * The TCP client should set the server connection status in status.
 */
void AsyncTCPStatusCB(bool &status)
{
    // Some asyn TCP client provides the server connected callback, you have to collect the required status from its callback
    // and set it to status variable.
}

/**
 * Async TCP Client Send Request Callback.
 * @param data The data to send.
 * @param size The size of data to send.
 * @param sent The data sent amount (0 for failure). (set by TCP client)
 *
 * The TCP client should send the data out by the size provided, and set the amount of sent data in sent.
 *
 * If connection or any sending failure, sets sent with 0.
 *
 */
void AsyncTCPSendCB(uint8_t *data, size_t size, uint32_t &sent)
{
}

/**
 * Async TCP Client Receive Request Callback.
 * @param buff The buffer to return (with copy) the received data.
 * @param buffSize The buffer size available.
 * @param filledSize The amount of data that return (0 for no data, -1 for failure) (set by TCP client).
 * @param available The remaining data that is available to read (set by TCP client).
 *
 * If data is available from TCP client, fills (copies) the data to buffer within the buffSize.
 * And set the amount of filled data in filledSize, and the remaining data amount in available.
 *
 * If no data is available, TCP client should set 0 to available.
 * If connection or any receiving failure, sets filledSize with -1.
 */
void AsyncTCPReceiveCB(uint8_t *buff, size_t buffSize, int32_t &filledSize, uint32_t &available)
{
    // Some asyn TCP client provides the data available callback, you have to collect the required data and status from its callback
    // and provide the data here in case of data is ready and available to read.

    // Please don't reallocate the buff, just copy data from async TCP client buffer to buff.
    // After buff was set, set the filledSize and available.
}

/**
 * Async TCP Client Connection Stop Request Callback.
 *
 * The TCP client should stop the server connection.
 */
void AsyncTCPStop()
{
}
