/**
 * ABOUT:
 *
 * The non-blocking (async) example to listen to the changes of value (from a node) stores in your database
 * via SSE Streaming connection.
 *
 * This example uses TinyGsmClient as the network client.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization and examples/App/NetworkInterfaces for more authentication and network examples.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * RealtimeDatabase::get(<AsyncClient>, <path>, <AsyncResult>, <SSE>);
 *
 * RealtimeDatabase::get(<AsyncClient>, <path>, <DatabaseOption>, <AsyncResult>);
 *
 * <AsyncClient> - The async client.
 * <path> - The node path to get/watch the value.
 * <DatabaseOption> - The database options (DatabaseOptions).
 * <AsyncResult> - The async result (AsyncResult).
 * <SSE> - The Server-sent events (HTTP Streaming) mode.
 *
 * In ESP32 Core v3.x.x, PPP devices are natively supported.
 * See examples/RealtimeDatabase/Async/NoCallback/StreamPPP/StreamPPP.ino
 */

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7600 // SIMA7670 Compatible with SIM7600 AT instructions

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
#define SIM_MODEM_RST_LOW false // active HIGH
#define SIM_MODEM_RST_DELAY 200
#define SIM_MODEM_TX 26
#define SIM_MODEM_RX 27

// Include TinyGsmClient.h first and followed by FirebaseClient.h
#include <TinyGsmClient.h>
#include <FirebaseClient.h>

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

TinyGsm modem(SerialAT);

TinyGsmClient gsm_client1(modem, 0);
TinyGsmClient gsm_client2(modem, 1);

// This is the library's internal SSL clients.
// You can use any SSL Client that works with GSM library.
// The ESP_SSLClient uses PSRAM by default (if it is available), for PSRAM usage, see https://github.com/mobizt/FirebaseClient#memory-options
// For ESP_SSLClient documentation, see https://github.com/mobizt/ESP_SSLClient
ESP_SSLClient ssl_client1, ssl_client2;

GSMNetwork gsm_network(&modem, GSM_PIN, apn, gprsUser, gprsPass);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

using AsyncClient = AsyncClientClass;

AsyncClient aClient1(ssl_client1, getNetwork(gsm_network)), aClient2(ssl_client2, getNetwork(gsm_network));

void printResult(AsyncResult &aResult);

RealtimeDatabase Database;

AsyncResult aResult_no_callback1, aResult_no_callback2;

unsigned long ms = 0;

void setup()
{
    Serial.begin(115200);

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

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    DBG("Initializing modem...");
    if (!modem.init())
    {
        DBG("Failed to restart modem, delaying 10s and retrying");
        return;
    }

    /*
    2 Automatic
    13 GSM Only
    14 WCDMA Only
    38 LTE Only
    */
    modem.setNetworkMode(38);
    if (modem.waitResponse(10000L) != 1)
    {
        DBG(" setNetworkMode faill");
    }

    String name = modem.getModemName();
    DBG("Modem Name:", name);

    String modemInfo = modem.getModemInfo();
    DBG("Modem Info:", modemInfo);

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl_client1.setInsecure();
    ssl_client1.setDebugLevel(1);
    ssl_client1.setBufferSizes(2048 /* rx */, 1024 /* tx */);
    ssl_client1.setClient(&gsm_client1);

    ssl_client2.setInsecure();
    ssl_client2.setDebugLevel(1);
    ssl_client2.setBufferSizes(2048 /* rx */, 1024 /* tx */);
    ssl_client2.setClient(&gsm_client2);

    Serial.println("Initializing the app...");
    initializeApp(aClient1, app, getAuth(user_auth), aResult_no_callback1);

    // Binding the FirebaseApp for authentication handler.
    // To unbind, use Database.resetApp();
    app.getApp<RealtimeDatabase>(Database);

    // Set your database URL (requires only for Realtime Database)
    Database.url(DATABASE_URL);

    // Since v1.2.1, in SSE mode (HTTP Streaming) task, you can filter the Stream events by using RealtimeDatabase::setSSEFilters(<keywords>),
    // which the <keywords> is the comma separated events.
    // The event keywords supported are:
    // get - To allow the http get response (first put event since stream connected).
    // put - To allow the put event.
    // patch - To allow the patch event.
    // keep-alive - To allow the keep-alive event.
    // cancel - To allow the cancel event.
    // auth_revoked - To allow the auth_revoked event.
    // To clear all prevousely set filter to allow all Stream events, use RealtimeDatabase::setSSEFilters().
    Database.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");

    // The "unauthenticate" error can be occurred in this case because we don't wait
    // the app to be authenticated before connecting the stream.
    // This is ok as stream task will be reconnected automatically when the app is authenticated.

    Database.get(aClient2, "/test/stream", aResult_no_callback2, true /* SSE mode */);
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    app.loop();

    Database.loop();

    if (millis() - ms > 20000 && app.ready())
    {
        ms = millis();

        JsonWriter writer;

        object_t json, obj1, obj2;

        writer.create(obj1, "ms", ms);
        writer.create(obj2, "rand", random(10000, 30000));
        writer.join(json, 2, obj1, obj2);

        Database.set<object_t>(aClient1, "/test/stream/number", json, aResult_no_callback1);
    }

    printResult(aResult_no_callback1);
    printResult(aResult_no_callback2);
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
        RealtimeDatabaseResult &RTDB = aResult.to<RealtimeDatabaseResult>();
        if (RTDB.isStream())
        {
            Serial.println("----------------------------");
            Firebase.printf("task: %s\n", aResult.uid().c_str());

            // The Stream payload might contain many events data due to
            // the events are constantly changing.
            Firebase.printf("event count: %d\n", RTDB.eventCount());
            for (uint32_t i = 0; i < RTDB.eventCount(); i++)
            {
                Firebase.printf("event: %s\n", RTDB.event(i).c_str());
                Firebase.printf("path: %s\n", RTDB.dataPath(i).c_str());
                Firebase.printf("data: %s\n", RTDB.to<const char *>(i));
                Firebase.printf("type: %d\n", RTDB.type(i));
                Serial.println();

                // The stream event from RealtimeDatabaseResult can be converted to the values as following.
                bool v1 = RTDB.to<bool>(i);
                int v2 = RTDB.to<int>(i);
                float v3 = RTDB.to<float>(i);
                double v4 = RTDB.to<double>(i);
                String v5 = RTDB.to<String>(i);
            }
        }
        else
        {
            Serial.println("----------------------------");
            Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
        }
        Firebase.printf("Free Heap: %d\n", ESP.getFreeHeap());
    }
}