
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
 * FILE CONFIG SYNTAX
 * ==================
 * SYNTAX:
 *
 * FileConfig file_config(<file_name>, <file_callback>);
 *
 * <file_name> - The filename included path of file that will be used.
 * <file_callback> - The callback function that provides file operation.
 *
 * The file name can be a name of source (input) and target (output) file that used in upload and download.
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

#if defined(ESP32)
#include <SPIFFS.h>
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

void fileCallback(File &file, const char *filename, file_operating_mode mode);

void printFile();

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

WiFiClientSecure ssl_client;

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

Database database;

FileConfig upload_data("/upload.bin", fileCallback);

FileConfig download_data("/download.bin", fileCallback);

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

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<Database>(database);

    database.url(DATABASE_URL);

    // Prepare file data

    SPIFFS.begin();

    File file = SPIFFS.open("/upload.bin", "w");
    for (size_t i = 0; i < 2048; i++)
        file.write(i);
    file.close();

    Serial.println("[+] Set file... ");
    database.set(aClient, "/test/file", getFile(upload_data), asyncCB);

    // To assign UID for async result
    // database.set(aClient, "/test/file", getFile(upload_data), asyncCB, "uploadTask");

    Serial.println("[+] Get file... ");
    database.get(aClient, "/test/file", getFile(download_data), asyncCB);

    // To assign UID for async result
    // database.get(aClient, "/test/file", getFile(download_data), asyncCB, "downloadTask");
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

    if (aResult.downloadProgress())
    {
        Serial.println("**************");
        Serial.printf("Downloaded: %d%s (%d of %d)\n", aResult.downloadInfo().progress, "%", aResult.downloadInfo().downloaded, aResult.downloadInfo().total);
        if (aResult.downloadInfo().total == aResult.downloadInfo().downloaded)
        {
            Serial.println("Download completed!");
            printFile();
        }
    }

    if (aResult.uploadProgress())
    {
        Serial.println("**************");
        Serial.printf("Uploaded: %d%s (%d of %d)\n", aResult.uploadInfo().progress, "%", aResult.uploadInfo().uploaded, aResult.uploadInfo().total);
        if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded)
            Serial.println("Upload completed!");
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

void printFile()
{
    File file = SPIFFS.open("/download.bin", "r");
    int i = 0;
    if (file)
    {
        while (file.available())
        {
            if (i > 0 && i % 16 == 0)
                Serial.println();

            int v = file.read();

            if (v < 16)
                Serial.print((const char *)FPSTR("0"));

            Serial.print(v, HEX);
            Serial.print((const char *)FPSTR(" "));
            i++;
        }
        Serial.println();
        file.close();
    }
}