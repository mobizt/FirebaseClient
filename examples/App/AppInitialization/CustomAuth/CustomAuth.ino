
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
 * SYNTAX:
 *
 * CustomAuth custom_auth(<TimeStatusCallback>, <JWTCallback>, <api_key>, <client_email>, <project_id>, <private_key>, <user_id>, <scope>, <claims>, <expire>);
 *
 * <TimeStatusCallback> - The time status callback that provide the UNIX timestamp value used for JWT token signing.
 * <JWTCallback> - The JWT token process callback. This callback was required for JWT token processing inside or outside the callback.
 * <api_key> - The web API key of project.
 * <client_email> - The service account client Email.
 * <project_id> - The service account project ID.
 * <private_key> - The service account private key.
 * <user_id> - The user ID.
 * <scope> - The OAuth scopes.
 * <claims> - The OAuth claims.
 * <expire> - The expiry period in seconds (less than or equal to 3600).
 *
 * The default network (built-in WiFi) configuration was used by default when
 * it was not assign to the function.
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
 * The auth data can be set to Services App e.g. Database to initialize via function getApp.
 *
 * SYNTAX:
 *
 * app.getApp<T>(<services_app>);
 *
 * Deallocation of FirebaseApp causes these services apps uninitialized and cannot be used.
 *
 * ASYNC QUEUE
 * ===========
 *
 * Each sync and async requests data consume memory up to 1k. When many async operations are added to queue (FIFO), the memory usage was increased.
 *
 * Each AsyncClient handles this queue separately. Then in order to limit the memory used for each AsyncClient,
 * this library allows 3-5 async operations (called slots) can be stored in the queue at the same time.
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
 * (Database, Firestore, Messaging, Functions, Storage and CloudStorage) objects was run in the loop via app.loop() or database.loop().
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

#if defined(ESP32)
#include <SPIFFS.h>
#endif

#include <FirebaseClient.h>

#if defined(ESP8266) || defined(ESP32)
#include <WiFiClientSecure.h>
#endif

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

/**
 * This information can be taken from the service account JSON file.
 *
 * To download service account file, from the Firebase console, goto project settings,
 * select "Service accounts" tab and click at "Generate new private key" button
 */
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

void timeStatusCB(uint32_t &ts);

void jwtCB(bool able_to_run);

void processJWT(const char *str = __builtin_FUNCTION());

void asyncCB(AsyncResult &aResult);

void fileCallback(File &file, const char *filename, file_operating_mode mode);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

CustomAuth custom_auth(timeStatusCB, jwtCB, API_KEY, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, "myId" /* UID */, "" /* scope */, "" /* claims */, 3600 /* expire period in seconds (<= 3600) */);

// FileConfig sa_file("/sa_file.json", fileCallback);

// CustomAuth sa_file_auth(timeStatusCB, getFile(sa_file), "myId");

FirebaseApp app;

WiFiClientSecure ssl_client;

bool jwt_loop_process = false;

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

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

    SPIFFS.begin();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(custom_auth));
    // Or initializeApp(aClient, app, getAuth(sa_file_auth));

    // To re-authenticate manually at any time, just call initializeApp again.

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;
}

void loop()
{
    if (jwt_loop_process)
    {
        processJWT();
    }
    // This function is required for handling and maintaining the authentication tasks.
    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();
}

void timeStatusCB(uint32_t &ts)
{
    if (time(nullptr) < FIREBASE_DEFAULT_TS)
    {
        configTime(3 * 3600, 0, "pool.ntp.org");
        while (time(nullptr) < FIREBASE_DEFAULT_TS)
        {
            delay(100);
        }
    }

    ts = time(nullptr);
}

void jwtCB(bool able_to_run)
{
    jwt_loop_process = !able_to_run;
    processJWT();
}

void processJWT(const char *str)
{
    // This prevents the stack overflow in ESP8266
    // Exit the function when it was called from jwt callback while it was unable to run from there.
    if (jwt_loop_process && strcmp(str, "jwtCB") == 0)
        return;
    jwt_loop_process = false;
    JWT.begin(app.getAuth());
    JWT.create();
}

void asyncCB(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
    {
        Serial.println("**************");
        Serial.printf("Event msg: %s, code: %d\n", aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isError())
    {
        Serial.println("**************");
        Serial.printf("Error msg: %s, code: %d\n", aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.isDebug())
    {
        Serial.println("**************");
        Serial.printf("Debug msg: %s\n", aResult.debug().c_str());
    }
}

void fileCallback(File &file, const char *filename, file_operating_mode mode)
{
    switch (mode)
    {
    case file_mode_open_read:
        file = SPIFFS.open(filename, "r");
        break;
    case file_mode_open_write:
        file = SPIFFS.open(filename, "w");
        break;
    case file_mode_open_append:
        file = SPIFFS.open(filename, "a");
        break;
    case file_mode_remove:
        SPIFFS.remove(filename);
        break;
    default:
        break;
    }
}
