/**
 * ABOUT:
 *
 * The Realtime Database Stream performance test example.
 *
 * This example will show how fast your device gets the Stream event.
 *
 * You will get all continouse data changes events from your Realtime Database without data lost.
 *
 * Open the index.html file with web browser and follow the instructions on that page to test.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 */

#include <Arduino.h>
#include <WiFi.h>

#include <FirebaseClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"
#define DATABASE_URL "URL"

void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);

DefaultNetwork network;
NoAuth no_auth;
FirebaseApp app;

#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));
RealtimeDatabase Database;

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

    ssl_client.setInsecure();
    initializeApp(aClient, app, getAuth(no_auth));
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
    Database.get(aClient, "/test/performance", printResult, true, "streamTask");
}

void loop()
{
    app.loop();
    Database.loop();
}

int counter = 0;

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
        if (RTDB.isStream() && RTDB.type() == 0)
            counter = 0;

        if (RTDB.isStream() && RTDB.type() == 1)
        {
            Firebase.printf("counter: %d\n", counter);
            Firebase.printf("data: %d\n", RTDB.to<int>());
            counter++;
        }
    }
}