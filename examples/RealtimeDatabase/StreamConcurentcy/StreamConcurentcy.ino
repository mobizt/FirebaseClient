/**
 * The example to stream changes to multiple locations in Realtime Database.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE
#define ENABLE_ESP_SSLCLIENT

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void processData(AsyncResult &aResult);

WiFiClient basic_client1, basic_client2, basic_client3;

// The ESP_SSLClient uses PSRAM by default (if it is available), for PSRAM usage, see https://github.com/mobizt/FirebaseClient#memory-options
// For ESP_SSLClient documentation, see https://github.com/mobizt/ESP_SSLClient
ESP_SSLClient ssl_client, stream_ssl_client1, stream_ssl_client2;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client), streamClient1(stream_ssl_client1), streamClient2(stream_ssl_client2);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult streamResult1, streamResult2;

unsigned long ms = 0;

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

    ssl_client.setClient(&basic_client1);
    stream_ssl_client1.setClient(&basic_client2);
    stream_ssl_client2.setClient(&basic_client3);

    ssl_client.setInsecure();
    stream_ssl_client1.setInsecure();
    stream_ssl_client2.setInsecure();

    ssl_client.setBufferSizes(2048, 1024);
    stream_ssl_client1.setBufferSizes(2048, 1024);
    stream_ssl_client2.setBufferSizes(2048, 1024);

    // In case using ESP8266 without PSRAM and you want to reduce the memory usage,
    // you can use WiFiClientSecure instead of ESP_SSLClient with minimum receive and transmit buffer size setting as following.
    // ssl_client1.setBufferSizes(1024, 512);
    // ssl_client2.setBufferSizes(1024, 512);
    // ssl_client3.setBufferSizes(1024, 512);
    // Note that, because the receive buffer size was set to minimum safe value, 1024, the large server response may not be able to handle.
    // The WiFiClientSecure uses 1k less memory than ESP_SSLClient.

    ssl_client.setDebugLevel(1);
    stream_ssl_client1.setDebugLevel(1);
    stream_ssl_client2.setDebugLevel(1);

    // In ESP32, when using WiFiClient with ESP_SSLClient, the WiFiClient was unable to detect
    // the server disconnection in case server session timed out and the TCP session was kept alive for reusage.
    // The TCP session timeout in seconds (>= 60 seconds) can be set via `ESP_SSLClient::setSessionTimeout`.
    ssl_client.setSessionTimeout(150);
    stream_ssl_client1.setSessionTimeout(150);
    stream_ssl_client2.setSessionTimeout(150);

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    // In SSE mode (HTTP Streaming) task, you can filter the Stream events by using AsyncClientClass::setSSEFilters(<keywords>),
    // which the <keywords> is the comma separated events.
    // The event keywords supported are:
    // get - To allow the http get response (first put event since stream connected).
    // put - To allow the put event.
    // patch - To allow the patch event.
    // keep-alive - To allow the keep-alive event.
    // cancel - To allow the cancel event.
    // auth_revoked - To allow the auth_revoked event.
    // To clear all prevousely set filter to allow all Stream events, use AsyncClientClass::setSSEFilters().
    streamClient1.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");
    
    streamClient2.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");

    // The "unauthenticate" error can be occurred in this case because we don't wait
    // the app to be authenticated before connecting the stream.
    // This is ok as stream task will be reconnected automatically when the app is authenticated.

    Database.get(streamClient1, "/examples/Stream/data1", processData, true /* SSE mode (HTTP Streaming) */, "streamTask1");

    Database.get(streamClient2, "/examples/Stream/data2", processData, true /* SSE mode (HTTP Streaming) */, "streamTask2");

    // Async call with AsyncResult for returning result.
    // Database.get(streamClient1, "/examples/Stream/data1", streamResult1, true /* SSE mode (HTTP Streaming) */);
    // Database.get(streamClient2, "/examples/Stream/data2", streamResult2, true /* SSE mode (HTTP Streaming) */);
}

void loop()
{

    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && millis() - ms > 20000)
    {
        ms = millis();

        JsonWriter writer;

        object_t json, obj1, obj2;

        writer.create(obj1, "ms", ms);
        writer.create(obj2, "rand", random(10000, 30000));
        writer.join(json, 2, obj1, obj2);

        Database.set<object_t>(aClient, "/examples/Stream/data1", json, processData, "setTask1");

        Database.set<int>(aClient, "/examples/Stream/data2", random(100000, 200000), processData, "setTask2");
    }

    // For async call with AsyncResult.
    // processData(streamResult1);
    // processData(streamResult2);
}

void processData(AsyncResult &aResult)
{
    // Exits when no result is available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
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
        RealtimeDatabaseResult &stream = aResult.to<RealtimeDatabaseResult>();
        if (stream.isStream())
        {
            Serial.println("----------------------------");
            Firebase.printf("task: %s\n", aResult.uid().c_str());
            Firebase.printf("event: %s\n", stream.event().c_str());
            Firebase.printf("path: %s\n", stream.dataPath().c_str());
            Firebase.printf("data: %s\n", stream.to<const char *>());
            Firebase.printf("type: %d\n", stream.type());

            // The stream event from RealtimeDatabaseResult can be converted to the values as following.
            bool v1 = stream.to<bool>();
            int v2 = stream.to<int>();
            float v3 = stream.to<float>();
            double v4 = stream.to<double>();
            String v5 = stream.to<String>();
        }
        else
        {
            Serial.println("----------------------------");
            Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
        }
#if defined(ESP32) || defined(ESP8266)
        Firebase.printf("Free Heap: %d\n", ESP.getFreeHeap());
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
        Firebase.printf("Free Heap: %d\n", rp2040.getFreeHeap());
#endif
    }
}
