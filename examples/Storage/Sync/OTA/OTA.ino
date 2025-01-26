/**
 * ABOUT:
 *
 * The blocking (sync) example to perform OTA firmware update using object (bin file) stores in Storage bucket.
 *
 * This example uses the UserAuth class for authentication, and the DefaultNetwork class for network interface configuration.
 * See examples/App/AppInitialization and examples/App/NetworkInterfaces for more authentication and network examples.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * bool Storage::ota(<AsyncClient>, <FirebaseStorage::Parent>);
 *
 * <AsyncClient> - The async client.
 * <FirebaseStorage::Parent> - The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
 *
 * The bucketid is the Storage bucket Id of object to download.
 * The object is the object in Storage bucket to download.
 * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
 *
 * This function returns bool status when task is complete.
 */

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>) || defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#elif __has_include(<WiFiC3.h>) || defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif __has_include(<WiFi.h>)
#include <WiFi.h>
#endif

#include <FirebaseClient.h>

// For Arduino SAMD21 OTA supports.
// See https://github.com/mobizt/FirebaseClient#ota-update.
#if defined(ARDUINO_ARCH_SAMD)
#include <Internal_Storage_OTA.h>
#define OTA_STORAGE InternalStorage
#endif

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

// Define the Firebase storage bucket ID e.g bucket-name.appspot.com */
#define STORAGE_BUCKET_ID "BUCKET-NAME.appspot.com"

void authHandler();

void printResult(AsyncResult &aResult);

void printError(int code, const String &msg);

void restart();

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA) || defined(ARDUINO_PORTENTA_C33) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

Storage storage;

AsyncResult aResult_no_callback;

bool taskCompleted = false;

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

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    // The large RX buffer is required for large data.
    ssl_client.setBufferSizes(16384, 1024);
#endif
#endif

    Serial.println("Initializing the app...");
    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);

    authHandler();

    // Binding the FirebaseApp for authentication handler.
    // To unbind, use storage.resetApp();
    app.getApp<Storage>(storage);

    // In case setting the external async result to the sync task (optional)
    // To unset, use unsetAsyncResult().
    aClient.setAsyncResult(aResult_no_callback);
}

void loop()
{
    authHandler();

    storage.loop();

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

#if defined(OTA_STORAGE)
        storage.setOTAStorage(OTA_STORAGE);
#endif

        // There is no OTA download progress available for sync OTA download.
        // To get the OTA download progress, use async OTA download instead.

        Serial.println("Updating your firmware (OTA)... ");
        bool result = storage.ota(aClient, FirebaseStorage::Parent(STORAGE_BUCKET_ID, "firmware.bin"));
        // You can provide the access token in case non-authentication mode (NoAuth) for priviledge access file download.
        // bool result = storage.ota(aClient, FirebaseStorage::Parent(STORAGE_BUCKET_ID, "firmware.bin", "access token"));

        if (result)
            restart();
        else
            printError(aClient.lastError().code(), aClient.lastError().message());
    }
}

void authHandler()
{
    // Blocking authentication handler with timeout
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // The JWT token processor required for ServiceAuth and CustomAuth authentications.
        // JWT is a static object of JWTClass and it's not thread safe.
        // In multi-threaded operations (multi-FirebaseApp), you have to define JWTClass for each FirebaseApp,
        // and set it to the FirebaseApp via FirebaseApp::setJWTProcessor(<JWTClass>), before calling initializeApp.
        JWT.loop(app.getAuth());
        printResult(aResult_no_callback);
    }
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
}

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void restart()
{
    Serial.println("Update firmware completed.");
    Serial.println();
#if defined(OTA_STORAGE)
    Serial.println("Applying update...");
    OTA_STORAGE.apply();
#elif defined(ESP32) || defined(ESP8266)
    Serial.println("Restarting...\n\n");
    ESP.restart();
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
    Serial.println("Restarting...\n\n");
    rp2040.restart();
#endif
}