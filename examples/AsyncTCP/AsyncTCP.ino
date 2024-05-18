/**
 * This example does not include any async TCP client library, you have to include it prior to use and
 * async TCP should support SSL.
 *
 * To try the async TCP client, define the following macro in src/Config.h
 * or user created config file in src/UserConfig.h.
 * #define ENABLE_ASYNC_TCP_CLIENT
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_GIGA)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>) || defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#elif __has_include(<WiFiC3.h>) || defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif __has_include(<WiFi.h>)
#include <WiFi.h>
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

void printResult(AsyncResult &aResult);

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

using AsyncClient = AsyncClientClass;

AsyncClient aClient(asyncTCP, getNetwork(network));

#endif

void setup()
{
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
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

#if defined(ENABLE_ASYNC_TCP_CLIENT)
    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "authTask");
#endif
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
