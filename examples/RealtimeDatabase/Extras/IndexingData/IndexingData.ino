
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

#define DATABASE_SECRET "DATABASE_SECRET"
#define DATABASE_URL "URL"

void asyncCB(AsyncResult &aResult);

void printError(int code, const String &msg);

void insertJson(String &json, const String &value);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

LegacyToken legacy_token(DATABASE_SECRET);
// or
// ServiceAuth sa_auth(timeStatusCB, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY);

FirebaseApp app;

WiFiClientSecure ssl_client;

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

RealtimeDatabase Database;

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

    Serial.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(legacy_token));

    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    /** If access token or ServiceAuth used

        unsigned long ms = millis();
        while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        {
            // This JWT token process required for ServiceAuth and CustomAuth authentications
            if (app.isJWT())
                JWT.process(app.getAuth());
        }

    */

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    Serial.print("Get security rules... ");

    String json_rules = Database.get<String>(aClient, ".settings/rules");

    if (aClient.lastError().code() == 0)
    {
        Serial.println("Ok");

        // For indexing data doc. please see https://firebase.google.com/docs/database/security/indexing-data

        // The path to index: /test/filter/json
        // The node name to index: Data

        // Library does not provide JSON parser library, the following JSON writer class will be used with
        // object_t for simple demonstration.

        object_t json;
        JsonWriter writer;
        writer.create(json, "test/filter/json/.indexOn", "Data"); // -> {"test":{"filter":{"json":{".indexOn":"Data"}}}}

        String new_indexon_rules = json.c_str();

        // Remove { and } before insert. -> "test":{"filter":{"json":{".indexOn":"Data"}}}
        new_indexon_rules.remove(0, 1);
        new_indexon_rules.remove(new_indexon_rules.length() - 1, 1);

        insertJson(json_rules, new_indexon_rules);

        Serial.print("Set security rules... ");
        bool status = Database.set<object_t>(aClient, ".settings/rules", object_t(json_rules));

        if (status)
            Serial.println("Ok");
        else
            printError(aClient.lastError().code(), aClient.lastError().message());
    }
    else
        printError(aClient.lastError().code(), aClient.lastError().message());
}

void loop()
{
    // This JWT token process required for ServiceAuth and CustomAuth authentications
    // if (app.isJWT())
    //    JWT.process(app.getAuth());

    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // This required when different AsyncClients than used in FirebaseApp assigned to the Realtime database functions.
    Database.loop();
}

void asyncCB(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
    {
        Serial.printf("Event msg: %s, code: %d\n", aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Serial.printf("Debug msg: %s\n", aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Serial.printf("Error msg: %s, code: %d\n", aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Serial.printf("payload: %s\n", aResult.c_str());
    }
}

void printError(int code, const String &msg)
{
    Serial.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void insertJson(String &json, const String &value)
{
    int p1 = json.indexOf("\".write\""); // Find pos of ".write" in the database rules

    if (p1 > -1)
    {
        int p2 = json.indexOf(",", p1); // Find pos of , after ".write"
        if (p2 == -1)
            p2 = json.indexOf("}", p1); // Or if it is the last node, find pos of } after ".write"

        String part1 = json.substring(0, p2);
        String part2 = json.substring(p2);

        json = part1 + "," + value + part2; // Insert the new rule between two sections.
    }
}
