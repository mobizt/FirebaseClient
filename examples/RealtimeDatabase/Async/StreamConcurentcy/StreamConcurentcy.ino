/**
 * SYNTAX:
 *
 * RealtimeDatabase::get(<AsyncClient>, <path>, <AsyncResult>, <SSE>);
 * RealtimeDatabase::get(<AsyncClient>, <path>, <AsyncResultCallback>, <SSE>, <uid>);
 *
 * RealtimeDatabase::get(<AsyncClient>, <path>, <DatabaseOption>, <AsyncResult>);
 * RealtimeDatabase::get(<AsyncClient>, <path>, <DatabaseOption>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <path> - The node path to get/watch the value.
 * <DatabaseOption> - The database options (DatabaseOptions).
 * <AsyncResult> - The async result (AsyncResult).
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
 * <SSE> - The Server-sent events (HTTP Streaming) mode.
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
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
#define DATABASE_URL "URL"

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

WiFiClient basic_client1, basic_client2, basic_client3;

// The ESP_SSLClient uses PSRAM by default (if it is available), for PSRAM usage, see https://github.com/mobizt/FirebaseClient#memory-options
// For ESP_SSLClient documentation, see https://github.com/mobizt/ESP_SSLClient
ESP_SSLClient ssl_client1, ssl_client2, ssl_client3;

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient1(ssl_client1, getNetwork(network));

AsyncClient aClient2(ssl_client2, getNetwork(network));

AsyncClient aClient3(ssl_client3, getNetwork(network));

RealtimeDatabase Database;

AsyncResult aResult_no_callback;

unsigned long ms = 0;

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

    ssl_client1.setClient(&basic_client1);
    ssl_client2.setClient(&basic_client2);
    ssl_client3.setClient(&basic_client3);

    ssl_client1.setInsecure();
    ssl_client2.setInsecure();
    ssl_client3.setInsecure();

    ssl_client1.setBufferSizes(2048, 1024);
    ssl_client2.setBufferSizes(2048, 1024);
    ssl_client3.setBufferSizes(2048, 1024);

    // In case using ESP8266 without PSRAM and you want to reduce the memory usage,
    // you can use WiFiClientSecure or ESP_SSLClient with minimum receive and transmit buffer size setting as following.
    // ssl_client1.setBufferSizes(1024, 512);
    // ssl_client2.setBufferSizes(1024, 512);
    // ssl_client3.setBufferSizes(1024, 512);
    // Note that, because the receive buffer size was set to minimum save value, 1024, the large server response may not be able to handle.

    ssl_client1.setDebugLevel(1);
    ssl_client2.setDebugLevel(1);
    ssl_client3.setDebugLevel(1);

    app.setCallback(asyncCB);

    // The async client used for auth task should not use for stream task
    initializeApp(aClient3, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    Database.get(aClient1, "/test/stream/path1", asyncCB, true /* SSE mode (HTTP Streaming) */, "streamTask1");

    Database.get(aClient2, "/test/stream/path2", asyncCB, true /* SSE mode (HTTP Streaming) */, "streamTask2");
}

void loop()
{
    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // This required when different AsyncClients than used in FirebaseApp assigned to the Realtime database functions.
    Database.loop();

    if (millis() - ms > 20000 && app.ready())
    {
        ms = millis();

        JsonWriter writer;

        object_t json, obj1, obj2;

        writer.create(obj1, "ms", ms);
        writer.create(obj2, "rand", random(10000, 30000));
        writer.join(json, 2, obj1, obj2);

        Database.set<object_t>(aClient3, "/test/stream/path1/number", json, asyncCB, "setTask");

        Database.set<object_t>(aClient3, "/test/stream/path2/number", random(100000, 200000), asyncCB, "setTask");
    }
}

void asyncCB(AsyncResult &aResult)
{
    printResult(aResult);
}

void printResult(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
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
        RealtimeDatabaseResult &RTDB = aResult.to<RealtimeDatabaseResult>();
        if (RTDB.isStream())
        {
            Serial.println("----------------------------");
            Firebase.printf("task: %s\n", aResult.uid().c_str());
            Firebase.printf("event: %s\n", RTDB.event().c_str());
            Firebase.printf("path: %s\n", RTDB.dataPath().c_str());
            Firebase.printf("data: %s\n", RTDB.to<const char *>());
            Firebase.printf("type: %d\n", RTDB.type());

            // The stream event from RealtimeDatabaseResult can be converted to the values as following.
            bool v1 = RTDB.to<bool>();
            int v2 = RTDB.to<int>();
            float v3 = RTDB.to<float>();
            double v4 = RTDB.to<double>();
            String v5 = RTDB.to<String>();
        }
        else
        {
            Serial.println("----------------------------");
            Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
        }
        Firebase.printf("Free Heap: %d\n", ESP.getFreeHeap());
    }
}
