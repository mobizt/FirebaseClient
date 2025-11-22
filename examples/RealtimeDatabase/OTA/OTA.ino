/**
 * The example to perform OTA firmware/filesystem update using data stores in your database.
 *
 * Assume that your firmware/filesystem bin file e.g., firmware.bin or spiffs.bin was converted to base64 encoded string and stores in your database before running
 * this example.
 *
 * See examples/RealtimeDatabase/File/File.ino for how to upload your bin file.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE
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

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

#define OTA_UPDATE_PARTITION U_FLASH // For the Firmware Partition
// #define OTA_UPDATE_PARTITION U_FS // For the ESP8266 or Raspberry Pi Pico/RP2040 Filesystem Partition
// #define OTA_UPDATE_PARTITION U_FLASHFS // For the ESP32 Filesystem Partition
// #define OTA_UPDATE_PARTITION U_SPIFFS // For the ESP32 Filesystem Partition
// #define OTA_UPDATE_PARTITION U_FATFS // For the ESP32 Filesystem Partition
// #define OTA_UPDATE_PARTITION U_LITTLEFS // For the ESP32 Filesystem Partition

void processData(AsyncResult &aResult);
void ota_async();
void ota_async2();
void ota_await();
void restart();

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult databaseResult;

bool taskComplete = false;

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

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;

#if defined(OTA_STORAGE)
        Database.setOTAStorage(OTA_STORAGE);
#endif

        // Assume that your firmware bin file was converted to base64 encoded string
        // and stores at "/examples/OTA/firmware"

        Serial.println("Updating your firmware/filesystem (OTA)... ");
        ota_async();
        // ota_async2();
        // ota_await();
    }

    // For async call with AsyncResult.
    processData(databaseResult);
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
            Firebase.printf("Upload task: %s, complete!✅️\n", aResult.uid().c_str());
    }
}

void ota_async()
{
    // Async call with callback function.
    Database.ota(aClient, "/examples/OTA/firmware", processData, "otaTask", OTA_UPDATE_PARTITION);
}

void ota_async2()
{
    // Async call with AsyncResult for returning result.
    Database.ota(aClient, "/examples/OTA/firmware", databaseResult, OTA_UPDATE_PARTITION);
}

void ota_await()
{
    // Sync call which waits until the operation complete.
    bool status = Database.ota(aClient, "/examples/OTA/firmware", OTA_UPDATE_PARTITION);
    if (status)
        Serial.println("⚡OTA dowload task (await), complete!✅️");
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
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