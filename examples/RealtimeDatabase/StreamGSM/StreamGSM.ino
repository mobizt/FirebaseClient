/**
 * The example to stream changes to a single location in Realtime Database.
 *
 * This example uses TinyGsmClient as the network client.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE
#define ENABLE_GSM_NETWORK
#define ENABLE_ESP_SSLCLIENT

#define TINY_GSM_MODEM_SIM7600 // SIMA7670 Compatible with SIM7600 AT instructions

// For network independent usage (disable all network features).
// #define DISABLE_NERWORKS

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "YourAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

#define UART_BAUD 115200

// LilyGO TTGO T-A7670 development board (ESP32 with SIMCom A7670)
#define SIM_MODEM_RST 5
#define SIM_MODEM_RST_LOW true // active LOW
#define SIM_MODEM_RST_DELAY 200
#define SIM_MODEM_TX 26
#define SIM_MODEM_RX 27

#include <Arduino.h>
// https://github.com/vshymanskyy/TinyGSM
#include <TinyGsmClient.h>

// Note that the library https://github.com/lewisxhe/TinyGSM-fork, may not incompatible with 
// vshymanskyy's TinyGSM code using in this example.

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void processData(AsyncResult &aResult);

TinyGsm modem(SerialAT);

TinyGsmClient gsm_client(modem, 0), stream_gsm_client(modem, 1);

// This is the library's internal SSL clients.
// You can use any SSL Client that works with GSM library.
// The ESP_SSLClient uses PSRAM by default (if it is available), for PSRAM usage, see https://github.com/mobizt/FirebaseClient#memory-options
// For ESP_SSLClient documentation, see https://github.com/mobizt/ESP_SSLClient
ESP_SSLClient ssl_client, stream_ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client), streamClient(stream_ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult streamResult;

unsigned long ms = 0;

bool initModem()
{
    SerialMon.begin(115200);
    delay(10);

    // Resetting the modem
#if defined(SIM_MODEM_RST)
    pinMode(SIM_MODEM_RST, SIM_MODEM_RST_LOW ? OUTPUT_OPEN_DRAIN : OUTPUT);
    digitalWrite(SIM_MODEM_RST, SIM_MODEM_RST_LOW);
    delay(100);
    digitalWrite(SIM_MODEM_RST, !SIM_MODEM_RST_LOW);
    delay(3000);
    digitalWrite(SIM_MODEM_RST, SIM_MODEM_RST_LOW);
#endif

    DBG("Wait...");
    delay(3000);

    SerialAT.begin(UART_BAUD, SERIAL_8N1, SIM_MODEM_RX, SIM_MODEM_TX);

    DBG("Initializing modem...");
    if (!modem.init())
    {
        DBG("Failed to restart modem, delaying 10s and retrying");
        return false;
    }

    /**
     * 2 Automatic
     * 13 GSM Only
     * 14 WCDMA Only
     * 38 LTE Only
     */
    modem.setNetworkMode(38);
    if (modem.waitResponse(10000L) != 1)
    {
        DBG(" setNetworkMode faill");
        return false;
    }

    String name = modem.getModemName();
    DBG("Modem Name:", name);

    String modemInfo = modem.getModemInfo();
    DBG("Modem Info:", modemInfo);

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork())
    {
        SerialMon.println(" fail");
        delay(10000);
        return false;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected())
        SerialMon.println("Network connected");

    return true;
}

void setup()
{
    Serial.begin(115200);

    if (!initModem())
        return;

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl_client.setInsecure();
    ssl_client.setDebugLevel(1);
    ssl_client.setBufferSizes(2048 /* rx */, 1024 /* tx */);
    ssl_client.setClient(&gsm_client);

    stream_ssl_client.setInsecure();
    stream_ssl_client.setDebugLevel(1);
    stream_ssl_client.setBufferSizes(2048 /* rx */, 1024 /* tx */);
    stream_ssl_client.setClient(&stream_gsm_client);

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

    if (millis() - ms > 20000 && app.ready())
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