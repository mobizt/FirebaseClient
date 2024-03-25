// Created by K. Suwatchai (Mobizt)
// Email: k_suwatchai@hotmail.com
// Github: https://github.com/mobizt/FirebaseClient
// Copyright (c) 2024 mobizt

/**
 * PRE REQUISITE
 * =============
 *
 * IAM owner permission required for service account,
 * https://github.com/mobizt/Firebase-ESP-Client#iam-permission-and-api-enable
 */

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
 * UserAuth user_auth(<api_key>, <user_email>, <user_password>, <expire>);
 *
 * <api_key> - API key can be obtained from Firebase console > Project Overview > Project settings.
 * <user_email> - The user Email that in the project.
 * <user_password> - The user password in the project.
 * <expire> - The expiry period in seconds (less than or equal to 3600).
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
 * OTA DOWNLOAD FUNCTIONS
 * ======================
 *
 * SYNTAXES:
 *
 * cstorage.ota(<AsyncClient>, <FirebaseStorage::Parent>, <GoogleCloudStorage::GetOptions>);
 * cstorage.ota(<AsyncClient>, <FirebaseStorage::Parent>, <GoogleCloudStorage::GetOptions>, <AsyncResult>);
 * cstorage.ota(<AsyncClient>, <FirebaseStorage::Parent>, <GoogleCloudStorage::GetOptions>, <AsyncResultCallback>, <uid>);
 *
 * The <GoogleCloudStorage::Parent> is the GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
 * The bucketid is the Storage bucket Id of object to download.
 * The object is the object to be downloaded in the Storage bucket.
 *
 * The <GoogleCloudStorage::GetOptions> is the GoogleCloudStorage::GetOptions that holds the get options.
 * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
 *
 * The cstorage is Google Cloud Storage service app.
 *
 * The async functions required AsyncResult or AsyncResultCallback function that keeping the result.
 *
 * The uid is user specified UID of async result (optional) which used as async task identifier.
 *
 * The uid can later get from AsyncResult object of AsyncResultCallback function via aResult.uid().
 *
 */

/**
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

///////////////////////////////////////////////////////////////////
// THIS EXAMPLE WORKS FOR ESP8266, ESP32 AND RASPBERRY PI PICO ONLY
///////////////////////////////////////////////////////////////////

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

void restart();

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

        Serial.println("OTA update download...");

        GoogleCloudStorage::GetOptions options;

        cstorage.ota(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "firmware.bin"), options, asyncCB);

        // To assign UID for async result
        // cstorage.ota(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "firmware.bin"), options, asyncCB, "myUID");

        // To get anyc result without callback
        // cstorage.ota(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "firmware.bin"), options, aResult_no_callback);
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

            if (aResult.isOTA())
                restart();
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

void restart()
{
    Serial.println("Update firmware completed.");
    Serial.println();
    Serial.println("Restarting...\n\n");
    delay(2000);
#if defined(ESP32) || defined(ESP8266)
    ESP.restart();
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
    rp2040.restart();
#endif
}