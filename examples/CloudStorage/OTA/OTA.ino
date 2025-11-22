/**
 * The example to perform OTA firmware/filesystem update using object (bin file) stores in Cloud Storage bucket.
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The OAuth2.0 authentication or access token authorization is required for Cloud Storage operations.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_SERVICE_AUTH
#define ENABLE_CLOUD_STORAGE
#define ENABLE_OTA

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

// For Arduino SAMD21 OTA supports.
// See https://github.com/mobizt/FirebaseClient#ota-update.
#if defined(ARDUINO_ARCH_SAMD)
#include <Internal_Storage_OTA.h>
#define OTA_STORAGE InternalStorage
#endif

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

// Define the Firebase storage bucket ID e.g bucket-name.appspot.com */
#define STORAGE_BUCKET_ID "BUCKET-NAME.appspot.com"

#define OTA_UPDATE_PARTITION U_FLASH // For the Firmware Partition
// #define OTA_UPDATE_PARTITION U_FS // For the ESP8266 or Raspberry Pi Pico/RP2040 Filesystem Partition
// #define OTA_UPDATE_PARTITION U_FLASHFS // For the ESP32 Filesystem Partition
// #define OTA_UPDATE_PARTITION U_SPIFFS // For the ESP32 Filesystem Partition
// #define OTA_UPDATE_PARTITION U_FATFS // For the ESP32 Filesystem Partition
// #define OTA_UPDATE_PARTITION U_LITTLEFS // For the ESP32 Filesystem Partition

void processData(AsyncResult &aResult);
void restart();

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

CloudStorage cstorage;

bool taskCompleted = false;

AsyncResult cloudStorageResult;

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

    set_ssl_client_insecure_and_buffer(ssl_client);

    // Assign the valid time only required for authentication process with ServiceAuth and CustomAuth.
    app.setTime(get_ntp_time());

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(sa_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(sa_auth), 120 * 1000, auth_debug_print);

    app.getApp<CloudStorage>(cstorage);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

#if defined(OTA_STORAGE)
        cstorage.setOTAStorage(OTA_STORAGE);
#endif

        GoogleCloudStorage::GetOptions options;

        Serial.println("Updating your firmware/fielsystem (OTA)... ");

        // Async call with callback function.
        cstorage.ota(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "firmware.bin" /* or "spiffs.bin" */), options, processData, "⚡otaTask", OTA_UPDATE_PARTITION);

        // Async call with AsyncResult for returning result.
        // cstorage.ota(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "firmware.bin" /* or "spiffs.bin" */), options, cloudStorageResult, OTA_UPDATE_PARTITION);

        // Sync call which waits until the operation complete.
        // bool status = cstorage.ota(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "firmware.bin" /* or "spiffs.bin" */), options, OTA_UPDATE_PARTITION);
        // if (status)
        //     Serial.println("⚡OTA dowload task (await), complete!✅️");
        // else
        //     Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    }

    // For async call with AsyncResult.
    processData(cloudStorageResult);
}

void processData(AsyncResult &aResult)
{
    // Exits when no result is available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.downloadProgress())
    {
        Firebase.printf("Download task: %s, downloaded %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.downloadInfo().progress, "%", aResult.downloadInfo().downloaded, aResult.downloadInfo().total);
        if (aResult.downloadInfo().total == aResult.downloadInfo().downloaded)
        {
            Firebase.printf("Download task: %s, complete!✅️\n", aResult.uid().c_str());
            if (aResult.isOTA())
                restart();
        }
    }

    if (aResult.uploadProgress())
    {
        Firebase.printf("Upload task: %s, uploaded %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.uploadInfo().progress, "%", aResult.uploadInfo().uploaded, aResult.uploadInfo().total);
        if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded)
        {
            Firebase.printf("Upload task: %s, complete!✅️\n", aResult.uid().c_str());
            Serial.print("Download URL: ");
            Serial.println(aResult.uploadInfo().downloadUrl);
        }
    }
}

void restart()
{
    Serial.println("Update firmware/filesystem completed.");
    Serial.println();
#if defined(OTA_STORAGE)
    Serial.println("Applying update...");
    OTA_STORAGE.apply();
#elif defined(ESP32) || defined(ESP8266)
    Serial.println("Restarting...\n\n");
    ESP.restart();
#elif defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_NANO_RP2040_CONNECT)
    Serial.println("Restarting...\n\n");
    rp2040.restart();
#endif
}