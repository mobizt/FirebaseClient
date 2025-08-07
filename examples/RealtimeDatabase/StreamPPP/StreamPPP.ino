/**
 * The example to stream changes to a single location in Realtime Database.
 *
 * This example requires PPP.h from ESP32 Core v3.x.x.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <PPP.h>

#include <FirebaseClient.h>
#include <NetworkClientSecure.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define PPP_MODEM_APN "YourAPN"
#define PPP_MODEM_PIN "0000" // or NULL

// WaveShare SIM7600 HW Flow Control
#define PPP_MODEM_RST 25
#define PPP_MODEM_RST_LOW false // active HIGH
#define PPP_MODEM_RST_DELAY 200
#define PPP_MODEM_TX 21
#define PPP_MODEM_RX 22
#define PPP_MODEM_RTS 26
#define PPP_MODEM_CTS 27
#define PPP_MODEM_FC ESP_MODEM_FLOW_CONTROL_HW
#define PPP_MODEM_MODEL PPP_MODEM_SIM7600

// LilyGO TTGO T-A7670 development board (ESP32 with SIMCom A7670)
// #define PPP_MODEM_RST 5
// #define PPP_MODEM_RST_LOW true // active LOW
// #define PPP_MODEM_RST_DELAY 200
// #define PPP_MODEM_TX 26
// #define PPP_MODEM_RX 27
// #define PPP_MODEM_RTS -1
// #define PPP_MODEM_CTS -1
// #define PPP_MODEM_FC ESP_MODEM_FLOW_CONTROL_NONE
// #define PPP_MODEM_MODEL PPP_MODEM_SIM7600

// SIM800 basic module with just TX,RX and RST
// #define PPP_MODEM_RST     0
// #define PPP_MODEM_RST_LOW true //active LOW
// #define PPP_MODEM_TX      2
// #define PPP_MODEM_RX      19
// #define PPP_MODEM_RTS     -1
// #define PPP_MODEM_CTS     -1
// #define PPP_MODEM_FC      ESP_MODEM_FLOW_CONTROL_NONE
// #define PPP_MODEM_MODEL   PPP_MODEM_SIM800

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void processData(AsyncResult &aResult);

NetworkClientSecure ssl_client, stream_ssl_client;
// Or use NetworkClient and ESP_SSLClient for lower memory usage
// NetworkClient net_client, stream_net_client;
// ESP_SSLClient ssl_client, stream_ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client), streamClient(stream_ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult streamResult;

unsigned long ms = 0;

void onEvent(arduino_event_id_t event, arduino_event_info_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_PPP_START:
        Serial.println("PPP Started");
        break;
    case ARDUINO_EVENT_PPP_CONNECTED:
        Serial.println("PPP Connected");
        break;
    case ARDUINO_EVENT_PPP_GOT_IP:
        Serial.println("PPP Got IP");
        break;
    case ARDUINO_EVENT_PPP_LOST_IP:
        Serial.println("PPP Lost IP");
        break;
    case ARDUINO_EVENT_PPP_DISCONNECTED:
        Serial.println("PPP Disconnected");
        break;
    case ARDUINO_EVENT_PPP_STOP:
        Serial.println("PPP Stopped");
        break;
    default:
        break;
    }
}

void setup()
{
    Serial.begin(115200);

    // Resetting the modem
#if defined(PPP_MODEM_RST)
    pinMode(PPP_MODEM_RST, PPP_MODEM_RST_LOW ? OUTPUT_OPEN_DRAIN : OUTPUT);
    digitalWrite(PPP_MODEM_RST, PPP_MODEM_RST_LOW);
    delay(100);
    digitalWrite(PPP_MODEM_RST, !PPP_MODEM_RST_LOW);
    delay(3000);
    digitalWrite(PPP_MODEM_RST, PPP_MODEM_RST_LOW);
#endif

    // Listen for modem events
    Network.onEvent(onEvent);

    // Configure the modem
    PPP.setApn(PPP_MODEM_APN);
    PPP.setPin(PPP_MODEM_PIN);
    PPP.setResetPin(PPP_MODEM_RST, PPP_MODEM_RST_LOW, PPP_MODEM_RST_DELAY);
    PPP.setPins(PPP_MODEM_TX, PPP_MODEM_RX, PPP_MODEM_RTS, PPP_MODEM_CTS, PPP_MODEM_FC);

    Serial.println("Starting the modem. It might take a while!");
    PPP.begin(PPP_MODEM_MODEL);

    Serial.print("Manufacturer: ");
    Serial.println(PPP.cmd("AT+CGMI", 10000));
    Serial.print("Model: ");
    Serial.println(PPP.moduleName());
    Serial.print("IMEI: ");
    Serial.println(PPP.IMEI());

    bool attached = PPP.attached();
    if (!attached)
    {
        int i = 0;
        unsigned int s = millis();
        Serial.print("Waiting to connect to network");
        while (!attached && ((++i) < 600))
        {
            Serial.print(".");
            delay(100);
            attached = PPP.attached();
        }
        Serial.print((millis() - s) / 1000.0, 1);
        Serial.println("s");
        attached = PPP.attached();
    }

    Serial.print("Attached: ");
    Serial.println(attached);
    Serial.print("State: ");
    Serial.println(PPP.radioState());
    if (attached)
    {
        Serial.print("Operator: ");
        Serial.println(PPP.operatorName());
        Serial.print("IMSI: ");
        Serial.println(PPP.IMSI());
        Serial.print("RSSI: ");
        Serial.println(PPP.RSSI());
        int ber = PPP.BER();
        if (ber > 0)
        {
            Serial.print("BER: ");
            Serial.println(ber);
            Serial.print("NetMode: ");
            Serial.println(PPP.networkMode());
        }

        Serial.println("Switching to data mode...");
        PPP.mode(ESP_MODEM_MODE_CMUX); // Data and Command mixed mode
        if (!PPP.waitStatusBits(ESP_NETIF_CONNECTED_BIT, 1000))
        {
            Serial.println("Failed to connect to internet!");
        }
        else
        {
            Serial.println("Connected to internet!");
        }
    }
    else
    {
        Serial.println("Failed to connect to network!");
    }

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl_client.setInsecure();

    // In case ESP_SSLClient
    // ssl_client.setDebugLevel(1);
    // ssl_client.setBufferSizes(2048 /* rx */, 1024 /* tx */);
    // ssl_client.setClient(&net_client1);

    stream_ssl_client.setInsecure();

    // In case ESP_SSLClient
    // stream_ssl_client.setDebugLevel(1);
    // stream_ssl_client.setBufferSizes(2048 /* rx */, 1024 /* tx */);
    // stream_ssl_client.setClient(&net_client2);

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
    streamClient.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");

    // The "unauthenticate" error can be occurred in this case because we don't wait
    // the app to be authenticated before connecting the stream.
    // This is ok as stream task will be reconnected automatically when the app is authenticated.

    Database.get(streamClient, "/examples/Stream/data", processData, true /* SSE mode */, "streamTask");

    // Async call with AsyncResult for returning result.
    // Database.get(streamClient, "/examples/Stream/data", streamResult, true /* SSE mode */);
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

        Database.set<object_t>(aClient, "/examples/Stream/data", json, processData, "setTask");
    }

    // For async call with AsyncResult.
    // processData(streamResult);
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
        Firebase.printf("Free Heap: %d\n", ESP.getFreeHeap());
    }
}