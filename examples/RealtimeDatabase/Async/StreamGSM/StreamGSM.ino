
// Created by K. Suwatchai (Mobizt)
// Email: k_suwatchai@hotmail.com
// Github: https://github.com/mobizt/FirebaseClient
// Copyright (c) 2024 mobizt

/**
 * APP INITIALIZATION
 * ==================
 *
 * This library provides the APIs covers the different types of authentication and services.
 *
 * The FirebaseApp class was introduced as user authentication provider which provides the credentials data
 * used for accessing the Firebase services.
 *
 * The Database, Firestore, Functions, Messaging, Storage and Cloud Storage apps provided the functions based on
 * the specific Firebase services which take the auth credentials data from FirebaseApp.
 *
 * Multiples instances of FirebaseApp (auth provider) and services apps (Database, Firestore...) can be available
 * which depends on the device memory.
 *
 * The FirebaseApp should be initialized first before use via static function initializeApp.
 *
 * SYNTAXES:
 *
 * initializeApp(<AsyncClient>, <FirebaseApp>, <user_auth_data>);
 *
 * The AsyncClient stores the user, server and network data which can be initialized as.
 *
 * WiFiClientSecure ssl_client;
 * DefaultNetwork network;
 * AsyncClient aClient(ssl_client, getNetwork(network));
 *
 * The FirebaseApp manage and stores the authentication data;
 *
 * The user_auth_data store the user credentials which can be obtained from one of following Auth classes.
 * UserAuth, ServiceAuth, CustomAuth, IDToken, AccessToken, CustomToken, LegacyToken and NoAuth via static function getAuth.
 *
 * Please see examples/App/AppInitialization for usage guidelines.
 *
 * To use other network interfaces, network data from one of the following Network classes
 * can be assigned.
 *
 * DefaultNetwork, GenericNetwork, EthernetNetwork and GSMNetwork.
 *
 * The network data (network_config_data) can be obtained from these Network classes via static function getNetwork.
 *
 * Please see examples/App/NetworkInterfaces for the usage guidelines.
 *
 * The auth credential data can be set to Services App e.g. Database to initialize via function getApp.
 *
 * SYNTAX:
 *
 * app.getApp<T>(<services_app>);
 *
 * Deallocation of FirebaseApp causes these services apps uninitialized and cannot be used.
 *
 */

/**
 * ASYNC GET SSE (STREAM) FUNCTIONS
 * ================================
 *
 * SYNTAXES:
 *
 * Database.get(<AsyncClient>, <path>, <AsyncResult>, <SSE>);
 * Database.get(<AsyncClient>, <path>, <AsyncResultCallback>, <SSE>, <uid>);
 *
 * The async functions required AsyncResult or AsyncResultCallback function that keeping the result.
 *
 * The uid is user specified UID of async result (optional) which used as async task identifier.
 *
 * The uid can later get from AsyncResult object of AsyncResultCallback function via aResult.uid().
 *
 * To stop one (first) async operation,
 *
 * aClient.stopAsync(); or
 *
 * To stop all async operations
 *
 * aClient.stopAsync(true);
 */

/**
 * The AsyncResult object in the AsyncResultCallback function provides the result of async and non-async operations.
 *
 * APP EVENT
 * ===========
 *
 * The event information can be obtained from aResult.appEvent().code() and aResult.appEvent().message() respectively.
 *
 * The following event code (firebase_auth_event_type), auth_event_uninitialized, auth_event_initializing, auth_event_signup, auth_event_send_verify_email,
 * auth_event_delete_user, auth_event_reset_password, auth_event_token_signing, auth_event_authenticating, auth_event_auth_request_sent
 * auth_event_auth_response_received, auth_event_ready and auth_event_error are available.
 *
 *
 * The following event strings "undefined", "initializing", "sign up", "send verification email", "delete user", "reset password",
 * "token signing", "authenticating", "auth request sent", "auth response received", "ready" and "error" are available.
 *
 * RESULT DATA
 * ===========
 *
 * The result data can be obtained from AsyncResult object via aResult.payload(), aResult.available(), aResult.path(), aResult.etag()
 * RTDB.isStream(), RTDB.event(), RTDB.dataPath(), RTDB.type() and RTDB.name() where RTDB is the RealtimeDatabaseResult object
 * that cast from aResult.to<RealtimeDatabaseResult>().
 *
 * The function aResult.payload() returns server serponse payload.
 *
 * The function aResult.available() returns the size of data that is ready to read.
 *
 * The function aResult.path() returns the resource path that the request was sent.
 *
 * The function aResult.etag() returns the ETag from server response header.
 *
 * The function RTDB.name() returns the name (random UID) of node that will be creaated after calling push.
 *
 * The function RTDB.type() returns the following realtime data type enum.
 * realtime_database_data_type_undefined (-1), realtime_database_data_type_null (0), realtime_database_data_type_integer(1), realtime_database_data_type_float (2),
 * realtime_database_data_type_double (3), realtime_database_data_type_boolean (4), realtime_database_data_type_string (5), realtime_database_data_type_json (6),
 * and realtime_database_data_type_array (7).
 *
 * The RTDB.dataPath() and RTDB.event() are the Realtime database node path that data has changed and type of event in server-sent events (stream) mode.
 *
 * The server response payload in AsyncResult can be converted to the the values e.g. boolean, integer,
 * float, double and string via RTDB.to<T>().
 *
 * ASYNC QUEUE
 * ===========
 *
 * Each sync and async requests data consume memory up to 1k. When many async operations are added to queue (FIFO), the memory usage was increased.
 *
 * Each AsyncClient handles this queue separately. Then in order to limit the memory used for each AsyncClient,
 * this library allows 10 async operations (called slots) can be stored in the queue at a time.
 * 
 * The maximum queue size can be set via the build flag FIREBASE_ASYNC_QUEUE_LIMIT or macro in
 * src/Config.h or created your own config in src/UserConfig.h.
 *
 * If the authentication async operation was required, it will insert to the first slot of the queue.
 *
 * If the sync operation was called, it will insert to the first slot in the queue too but after the authentication task slot.
 *
 * When async Get operation in SSE mode (HTTP Streaming) was currently stored in queue, the new sync and async operations will be inserted before
 * the async SSE (HTTP Streaming) slot.
 *
 * When the async operation queue is full, the new sync and async operations will be cancelled.
 *
 * The finished and time out operating slot will be removed from the queue unless the async SSE and allow the vacant slot for the new async operation.
 *
 * The async SSE operation will run continuously and repeatedly as long as the FirebaseApp and the services app
 * (Database, Firestore, Messaging, Functions, Storage and CloudStorage) objects was run in the loop via app.loop() or Database.loop().
 *
 * STOP QUEUE
 * ===========
 *
 * SYNTAX:
 *
 * The async operation will be cancelled and remove from the queue by calling thes functions.
 *
 * asyncClient.stopAsync() - stop the last async operation in the queue.
 *
 * asyncClient.stopAsync(true) - stop all async operation in the queue.
 *
 * asyncClient.stopAsync("xxxx") - stop the async operation in the queue that has the async result UID xxxx.
 *
 * ASYNC CLIENT
 * ============
 *
 * The async client stores the async operating data in its queue and holds the pointer of SSL Client that assigned with it.
 *
 * The SSL Client should be existed in the AsyncClient usage scope in case of sync or should defined as global object for async usage.
 *
 * The SSL Client should not be shared among various AsyncClients because of interferences in async operations.
 *
 * Only one SSL Client should be assigned to or used with only one AsyncClient.
 *
 * SSL Client can force to stop by user or calling stop() from asyncClient.
 *
 * SYNTAX:
 *
 * asyncClient.stop() - stop the SSL Client to terminate the server connection.
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

#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 600         // Time ESP32 will go to sleep (in seconds)

#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 26
#define PIN_RX 27
#define PWR_PIN 4
#define BAT_ADC 35
#define BAT_EN 12
#define PIN_RI 33
#define PIN_DTR 25
#define RESET 5

#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13

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

// This is the library internal SSL clients.
// You can use any SSL Client that works with Ethernet library.
ESP_SSLClient ssl_client1, ssl_client2;

GSMNetwork gsm_network(&modem, GSM_PIN, apn, gprsUser, gprsPass);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient1(ssl_client1, getNetwork(gsm_network));

AsyncClient aClient2(ssl_client2, getNetwork(gsm_network));

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

RealtimeDatabase Database;

unsigned long ms = 0;

void setup()
{

    Serial.begin(115200);
    Serial.begin(115200);

    delay(10);
    pinMode(BAT_EN, OUTPUT);
    digitalWrite(BAT_EN, HIGH);

    // A7670 Reset
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, LOW);
    delay(100);
    digitalWrite(RESET, HIGH);
    delay(3000);
    digitalWrite(RESET, LOW);

    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(100);
    digitalWrite(PWR_PIN, HIGH);
    delay(1000);
    digitalWrite(PWR_PIN, LOW);

    DBG("Wait...");

    delay(3000);

    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

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

    Serial.println("Initializing app...");

    app.setCallback(asyncCB);

    initializeApp(aClient1, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    Database.get(aClient2, "/test/stream", asyncCB, true /* SSE mode */);
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

        Database.set<object_t>(aClient1, "/test/stream/number", json, asyncCB);

        // When the async operation queue was full, the operation will be cancelled for new sync and async operation.
        // When the async operation was time out, it will be removed from queue and allow the slot for the new async operation.

        // To assign UID for async result
        // Database.set<object_t>(aClient2, "/test/stream/number", json, asyncCB, "myUID");
    }

    // To get anyc result without callback
    // printResult(aResult_no_callback);
}

void asyncCB(AsyncResult &aResult)
{
    // To get the UID (string) from async result
    // aResult.uid();

    printResult(aResult);
}

void printResult(AsyncResult &aResult)
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

    if (aResult.available())
    {
        // To get the UID (string) from async result
        // aResult.uid();

        RealtimeDatabaseResult &RTDB = aResult.to<RealtimeDatabaseResult>();
        if (RTDB.isStream())
        {
            Firebase.printf("event: %s\n", RTDB.event().c_str());
            Firebase.printf("path: %s\n", RTDB.dataPath().c_str());
            Firebase.printf("data: %s\n", RTDB.to<const char *>());
            Firebase.printf("type: %d\n", RTDB.type());

            // The stream event from RealtimeDatabaseResult can be converted to the the values as following.
            bool v1 = RTDB.to<bool>();
            int v2 = RTDB.to<int>();
            float v3 = RTDB.to<float>();
            double v4 = RTDB.to<double>();
            String v5 = RTDB.to<String>();
        }
        else
        {
            Firebase.printf("payload: %s\n", aResult.c_str());
        }
    }
}