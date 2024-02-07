
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
 * SYNC PUSH FUNCTIONS
 * ===================
 *
 * SYNTAXES:
 *
 * String name = database.push<T>(<AsyncClient>, <path>, <value>);
 *
 * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
 *
 * The string_t is for string placeholder e.g. string_t("hello there").
 * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
 * The boolean_t is for boolean placeholder e.g. boolean_t(true).
 * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
 *
 * The sync Push function returns the node name that will be created.
 *
 * In case of error, the operation error information can be obtain from AsyncClient via aClient.lastError().message() and
 * aClient.lastError().code().
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

void printError(int code, const String &msg);

void asyncCB(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

WiFiClientSecure ssl_client;

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

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<Database>(database);

    database.url(DATABASE_URL);

    // Push int
    String name = database.push<int>(aClient, "/test/int", 12345);
    if (aClient.lastError().code() == 0)
        Serial.printf("Push int is ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push bool
    name = database.push<bool>(aClient, "/test/bool", true);
    if (aClient.lastError().code() == 0)
        Serial.printf("Push bool is ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push string
    name = database.push<String>(aClient, "/test/string", "hello");
    if (aClient.lastError().code() == 0)
        Serial.printf("Push string is ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push json

    // Library does not provide JSON parser library, the following JSON writer class will be used with
    // object_t for simple demonstration.

    object_t json;
    JsonWriter writer;
    writer.create(json, "test/data", 123); // -> {"test":{"data":123}}
    // Or set the seialized JSON string to object_t as object_t("{\"test\":{\"data\":123}}")

    name = database.push<object_t>(aClient, "/test/json", json);
    if (aClient.lastError().code() == 0)
        Serial.printf("Push json is ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push array

    object_t arr;
    arr.initArray();// To use as Array placeholder
    writer.join(arr, 4, object_t(1), object_t(2), object_t(string_t("test")), object_t(boolean_t(true)));// -> [1,2,"test",true]
    // Or set the seialized JSON Array string to the object_t as object_t("[1,2,\"test\",true]")
    
    name = database.push<object_t>(aClient, "/test/arr", arr);
    if (aClient.lastError().code() == 0)
        Serial.printf("Push array is ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push float
    name = database.push<number_t>(aClient, "/test/float", number_t(123.456, 2));
    if (aClient.lastError().code() == 0)
        Serial.printf("Push float is ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push double
    name = database.push<number_t>(aClient, "/test/double", number_t(1234.56789, 4));
    if (aClient.lastError().code() == 0)
        Serial.printf("Push double is ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());
}

void loop()
{
    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // (For Async Only) This required when different AsyncClients than used in FirebaseApp assigned to the database functions.
    database.loop();
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
}

void printError(int code, const String &msg)
{
    Serial.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}
