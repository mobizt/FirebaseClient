/**
 * SYNTAXES:
 *
 * cstorage.upload(<AsyncClient>, <FirebaseStorage::Parent>, <file_config_data>, <GoogleCloudStorage::uploadOptions>);
 * cstorage.upload(<AsyncClient>, <FirebaseStorage::Parent>, <file_config_data>, <GoogleCloudStorage::uploadOptions>, <AsyncResult>);
 * cstorage.upload(<AsyncClient>, <FirebaseStorage::Parent>, <file_config_data>, <GoogleCloudStorage::uploadOptions>, <AsyncResultCallback>, <uid>);
 *
 * The <GoogleCloudStorage::Parent> is the GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
 * The bucketid is the Storage bucket Id of object to upload.
 * The object is the object to be stored in the Storage bucket.
 *
 * The <file_config_data> is the filesystem data (file_config_data) obtained from FileConfig class object.
 *
 * The <GoogleCloudStorage::uploadOptions> is the <GoogleCloudStorage::uploadOptions that holds the information for insert options, properties and types of upload.
 * For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
 * For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties
 *
 * The cstorage is Google Cloud Storage service app.
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

/**
 * This information can be taken from the service account JSON file.
 *
 * To download service account file, from the Firebase console, goto project settings,
 * select "Service accounts" tab and click at "Generate new private key" button
 */
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

// Define the Firebase storage bucket ID e.g bucket-name.appspot.com */
#define STORAGE_BUCKET_ID "BUCKET-NAME.appspot.com"

void timeStatusCB(uint32_t &ts);

void asyncCB(AsyncResult &aResult);

#if defined(ENABLE_FS)

void fileCallback(File &file, const char *filename, file_operating_mode mode);

FileConfig media_file("/media.mp4", fileCallback);

#endif

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

// ServiceAuth is required for Google Cloud Storage functions.
ServiceAuth sa_auth(timeStatusCB, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

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

CloudStorage cstorage;

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

    initializeApp(aClient, app, getAuth(sa_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // This JWT token process required for ServiceAuth and CustomAuth authentications
        JWT.loop(app.getAuth());
    }

    app.getApp<CloudStorage>(cstorage);

#if defined(ENABLE_FS)
    MY_FS.begin();
#endif
}

void loop()
{
    // This JWT token process required for ServiceAuth and CustomAuth authentications
    JWT.loop(app.getAuth());

    // This function is required for handling and maintaining the authentication tasks.
    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();

    // This required when different AsyncClients than used in FirebaseApp assigned to the Google Cloud Storage functions.
    cstorage.loop();

    // To get anyc result without callback
    // printResult(aResult_no_callback);

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        Serial.println("Upload file...");

        GoogleCloudStorage::uploadOptions options;
        options.mime = "media.mp4";
        options.uploadType = GoogleCloudStorage::upload_type_resumable;
        // options.uploadType = GoogleCloudStorage::upload_type_simple;
        // options.uploadType = GoogleCloudStorage::upload_type_multipart;

#if defined(ENABLE_FS)
        cstorage.upload(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), options, asyncCB);
#endif
        // To assign UID for async result
        // cstorage.upload(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), options, asyncCB, "myUID");

        // To get anyc result without callback
        // cstorage.upload(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), options, aResult_no_callback);
    }
}

void timeStatusCB(uint32_t &ts)
{
#if defined(ESP8266) || defined(ESP32) || defined(CORE_ARDUINO_PICO)
    if (time(nullptr) < FIREBASE_DEFAULT_TS)
    {

        configTime(3 * 3600, 0, "pool.ntp.org");
        while (time(nullptr) < FIREBASE_DEFAULT_TS)
        {
            delay(100);
        }
    }
    ts = time(nullptr);
#elif __has_include(<WiFiNINA.h>) || __has_include(<WiFi101.h>)
    ts = WiFi.getTime();
#endif
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
            // Not available for simple upload.
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