/**
 * This example is for new users which are familiar with other legacy Firebase libraries.
 *
 * The example shows how to listen the data changes in your Firebase Realtime database
 * while the database was set periodically.
 * 
 * All functions used in this example are non-blocking (async) functions.
 *
 * This example will use the database secret for priviledge Realtime database access which does not need
 * to change the security rules or it can access Realtime database no matter what the security rules are set.
 *
 * This example is for ESP32, ESP8266 and Raspberry Pi Pico W.
 *
 * You can adapt the WiFi and SSL client library that are available for your devices.
 *
 * For the ethernet and GSM network which are not covered by this example,
 * you have to try another elaborate examples and read the library documentation thoroughly.
 *
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define DATABASE_SECRET "DATABASE_SECRET"
#define DATABASE_URL "URL"

// The SSL client used for secure server connection.
WiFiClientSecure ssl1, ssl2;

// The default network config object that used in this library.
DefaultNetwork network;

// The client, aka async client, is the client that handles many tasks required for each operation.
AsyncClientClass client1(ssl1, getNetwork(network)), client2(ssl2, getNetwork(network));

// The authentication task handler, aka FirebaseApp.
FirebaseApp app;

// The Realtime database class object that provides the functions.
RealtimeDatabase Database;

// The class that stores the operating result, aka AsyncResult.
AsyncResult result1, result2;

// The legacy token provider class used for authentication initialization.
LegacyToken dbSecret(DATABASE_SECRET);

unsigned long ms = 0;

void printResult(AsyncResult &aResult)
{

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
    }
}

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

    ssl1.setInsecure();
    ssl2.setInsecure();
#if defined(ESP8266)
    ssl1.setBufferSizes(1024, 1024);
    ssl2.setBufferSizes(1024, 1024);
#endif

    // Initialize the authentication handler.
    initializeApp(client1, app, getAuth(dbSecret));

    // Binding the authentication handler with your Database class object.
    app.getApp<RealtimeDatabase>(Database);

    // Set your database URL
    Database.url(DATABASE_URL);

    // Initiate the Stream connection to listen the data changes.
    // This function can be called once.
    // The Stream was connected using async get function (non-blocking) which the result will assign to the function in this case.
    Database.get(client1, "/test/stream", result1, true /* this option is for Stream connection */);
}

void loop()
{
    // Polling for internal task operation
    // This required for Stream in this case.
    Database.loop();

    // We don't have to poll authentication handler task using app.loop() as seen in other examples
    // because the database secret is the priviledge access key that never expired.

    // Set the random int value to "/test/stream/int" every 20 seconds.
    if (millis() - ms > 20000 || ms == 0)
    {
        ms = millis();

        // We set the data with this non-blocking set function (async) which the result was assign to the function.
        Database.set<int>(client2, "/test/stream/int", random(100, 999), result2);
    }

    // Polling print the result if it is available.
    printResult(result1);
    printResult(result2);
}