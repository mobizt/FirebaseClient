
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
 * ASYNC PUSH FUNCTIONS
 * ====================
 *
 * SYNTAXES:
 *
 * database.push<T>(<AsyncClient>, <path>, <value>, <AsyncResult>);
 * database.push<T>(<AsyncClient>, <path>, <value>, <AsyncResultCallback>, <uid>);
 *
 * The async functions required AsyncResult or AsyncResultCallback function that keeping the result.
 *
 * The uid is user specified UID of async result (optional) which used as async task identifier.
 *
 * The uid can later get from AsyncResult object of AsyncResultCallback function via aResult.uid().
 *
 * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
 *
 * The string_t is for string placeholder e.g. string_t("hello there").
 * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
 * The boolean_t is for boolean placeholder e.g. boolean_t(true).
 * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
 *
 * Storing (upload) and restoring (download) the binary data in form of File from filesystems or BLOB (memory) are supported.
 *
 * The binary data will be convert to base64 encoded string when upload and base64 string stored at the node will be converted to data
 * and save to file or keep in memory when download.
 *
 * No additional data added to base64 encoded string which you can manage it freely.
 *
 * To work with BLOB and File upload and download, the file config data can be assigned to the function.
 * The file config data (also contains blob data) can be obtained from static functions getBlob and getFile.
 *
 * Please see examples/Database/extras/BLOB and examples/Database/extras/File for the usagw guidelines.
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

#include <FirebaseClient.h>

#if defined(ESP8266) || defined(ESP32)
#include <WiFiClientSecure.h>
#endif

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void asyncCB(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

WiFiClientSecure ssl_client;

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

Database database;

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

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<Database>(database);

    database.url(DATABASE_URL);

    Serial.println("[+] Asynchronous Push... ");

    // Push int
    database.push<int>(aClient, "/test/int", 12345, asyncCB);

    // Push bool
    database.push<bool>(aClient, "/test/bool", true, asyncCB);

    // Push string
    database.push<String>(aClient, "/test/string", "hello", asyncCB);

    // Push json
    database.push<object_t>(aClient, "/test/json", object_t("{\"data\":123}"), asyncCB);

    // Library does not provide JSON parser library, the following JSON writer class will be used with
    // object_t for simple demonstration.

    object_t json, obj1, obj2, obj3, obj4;
    JsonWriter writer;

    writer.create(obj1, "int/value", 9999);
    writer.create(obj2, "string/value", string_t("hello"));
    writer.create(obj3, "float/value", number_t(123.456, 2));
    writer.join(obj4, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);
    writer.create(json, "node/list", obj4);

    // To print object_t
    // Serial.println(json);

    database.push<object_t>(aClient, "/test/json", json, asyncCB);

    object_t arr;
    arr.initArray(); // initialize to be used as array
    writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

    // Note that value that sets to object_t other than JSON ({}) and Array ([]) can be valid only if it
    // used as array member value as above i.e. object_t(string_t("steve")) and object_t(888).

    // Push array
    database.push<object_t>(aClient, "/test/arr", arr, asyncCB);

    // Push float
    database.push<number_t>(aClient, "/test/float", number_t(123.456, 2), asyncCB);

    // Push double
    database.push<number_t>(aClient, "/test/double", number_t(1234.56789, 4), asyncCB);

    // To assign UID for async result
    // database.push<number_t>(aClient, "/test/double", number_t(1234.56789, 4), asyncCB, "myUID");
}

void loop()
{
    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // This required when different AsyncClients than used in FirebaseApp assigned to the database functions.
    database.loop();
}

void asyncCB(AsyncResult &aResult)
{
    // To get the UID (string) from async result
    // aResult.uid();

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

    if (aResult.available())
    {
        Serial.println("**************");
        if (aResult.database.name().length())
            Serial.printf("name: %s\n", aResult.database.name().c_str());
        Serial.printf("payload: %s\n", aResult.c_str());
    }
}