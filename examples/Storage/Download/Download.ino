/**
 * SYNTAXES:
 *
 * storage.download(<AsyncClient>, <FirebaseStorage::Parent>, <file_config_data>);
 * storage.download(<AsyncClient>, <FirebaseStorage::Parent>, <file_config_data>, <AsyncResult>);
 * storage.download(<AsyncClient>, <FirebaseStorage::Parent>, <file_config_data>, <AsyncResultCallback>, <uid>);
 *
 * The <FirebaseStorage::Parent> is the FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
 * The bucketid is the Storage bucket Id of object to download.
 * The object is the object in Storage bucket to download.
 *
 * The <file_config_data> is the filesystem data (file_config_data) obtained from FileConfig class object.
 *
 * The storage is Storage service app.
 *
 * The async functions required AsyncResult or AsyncResultCallback function that keeping the result.
 *
 * The uid is user specified UID of async result (optional) which used as async task identifier.
 *
 * The uid can later get from AsyncResult object of AsyncResultCallback function via aResult.uid().
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
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

#if defined(ENABLE_FS)      // Defined in this library
#if defined(FLASH_SUPPORTS) // Defined in this library
#if defined(ESP32)
#include <SPIFFS.h>
#endif
#define MY_FS SPIFFS
#else
#include <SPI.h>
#include <SD.h>
#define MY_FS SD
#endif
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

void asyncCB(AsyncResult &aResult);

#if defined(ENABLE_FS)

void fileCallback(File &file, const char *filename, file_operating_mode mode);

FileConfig media_file("/media.mp4", fileCallback);

#endif

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

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

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<Storage>(storage);

#if defined(ENABLE_FS)
    MY_FS.begin();
#endif
}

void loop()
{
    // This function is required for handling and maintaining the authentication tasks.
    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();

    // This required when different AsyncClients than used in FirebaseApp assigned to the Firebase Storage functions.
    storage.loop();

    // To get anyc result without callback
    // printResult(aResult_no_callback);

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        Serial.println("Download object...");

#if defined(ENABLE_FS)
        storage.download(aClient, FirebaseStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), asyncCB);
#endif
        // To assign UID for async result
        // storage.download(aClient, FirebaseStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), asyncCB, "myUID");

        // To get anyc result without callback
        // storage.download(aClient, FirebaseStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), aResult_no_callback);
    }
}

void asyncCB(AsyncResult &aResult)
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

        Firebase.printf("payload: %s\n", aResult.c_str());
    }

    if (aResult.downloadProgress())
    {
        Firebase.printf("Downloaded: %d%s (%d of %d)\n", aResult.downloadInfo().progress, "%", aResult.downloadInfo().downloaded, aResult.downloadInfo().total);
        if (aResult.downloadInfo().total == aResult.downloadInfo().downloaded)
        {
            Serial.println("Download completed!");
        }
    }

    if (aResult.uploadProgress())
    {
        Firebase.printf("Uploaded: %d%s (%d of %d)\n", aResult.uploadInfo().progress, "%", aResult.uploadInfo().uploaded, aResult.uploadInfo().total);
        if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded)
        {
            Serial.println("Upload completed!");
            Serial.print("Download URL: ");
            Serial.println(aResult.uploadInfo().downloadUrl);
        }
    }
}

#if defined(ENABLE_FS)
void fileCallback(File &file, const char *filename, file_operating_mode mode)
{
    // FILE_OPEN_MODE_READ, FILE_OPEN_MODE_WRITE and FILE_OPEN_MODE_APPEND are defined in this library
    // MY_FS is defined in this example
    switch (mode)
    {
    case file_mode_open_read:
        file = MY_FS.open(filename, FILE_OPEN_MODE_READ);
        break;
    case file_mode_open_write:
        file = MY_FS.open(filename, FILE_OPEN_MODE_WRITE);
        break;
    case file_mode_open_append:
        file = MY_FS.open(filename, FILE_OPEN_MODE_APPEND);
        break;
    case file_mode_remove:
        MY_FS.remove(filename);
        break;
    default:
        break;
    }
}
#endif