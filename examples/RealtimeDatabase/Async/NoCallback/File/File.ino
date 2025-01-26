/**
 * ABOUT:
 *
 * The non-blocking (async) example to store your file data to the database and read it back.
 *
 * With the FileConfig object provides with the set function, the file data will be converted to base64
 * encoded string on-the-fly when storing to your database.
 *
 * And when the FileConfig object provides with the get function, the data in your database
 * (assumed that it is base64 encoded string) will be converted to binary data
 * on-the-fly and stores as file when reading from your database.
 *
 * See the FileConfig class constructor syntax below.
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
 * FileConfig::FileConfig(<file_name>, <file_callback>);
 *
 * <file_name> - The filename included path of file that will be used.
 * <file_callback> - The callback function that provides file operation.
 *
 * The file_callback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
 * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
 *
 * The file name can be a name of source (input) and target (output) file that used in upload and download.
 *
 * 2.------------------------
 *
 * RealtimeDatabase::set(<AsyncClient>, <path>, <file_config_data>, <AsyncResult>);
 *
 * RealtimeDatabase::get(<AsyncClient>, <path>, <file_config_data>, <AsyncResult>);
 *
 * <AsyncClient> - The async client.
 * <path> - The node path to set/get the file data.
 * <file_config_data> - The file config data which in case of filesystem data, it will be obtained from FileConfig via getFile.
 * <AsyncResult> - The async result (AsyncResult).
 */

#include <Arduino.h>
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

// In ESP32 Core SDK v3.x.x, to use filesystem in this library,
// the File object should be defined globally
// and the library's internal defined FS object should be set with
// this global FS object in fileCallback function.
#include <FS.h>
File myFile;

#if defined(ESP32)
#include <SPIFFS.h>
#endif
#define MY_FS SPIFFS

#include <FirebaseClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void printResult(AsyncResult &aResult);

#if defined(ENABLE_FS)
void fileCallback(File &file, const char *filename, file_operating_mode mode);

void printFile();
#endif

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

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

RealtimeDatabase Database;

AsyncResult aResult_no_callback;

#if defined(ENABLE_FS)
FileConfig upload_data("/upload.bin", fileCallback); // Can be set later with upload_data.setFile("/upload.bin", fileCallback);

FileConfig download_data("/download.bin", fileCallback); // Can be set later with download_data.setFile("/download.bin", fileCallback);
#endif

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

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    // if the file size to download is large (> 16k), set the rx buffer size to 16k.
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    Serial.println("Initializing the app...");
    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);

    // Binding the FirebaseApp for authentication handler.
    // To unbind, use Database.resetApp();
    app.getApp<RealtimeDatabase>(Database);

    // Set your database URL (requires only for Realtime Database)
    Database.url(DATABASE_URL);

    // Prepare file data
#if defined(ENABLE_FS)

    // FILE_OPEN_MODE_READ, FILE_OPEN_MODE_WRITE and FILE_OPEN_MODE_APPEND are defined in this library
    // MY_FS is defined in this example

    MY_FS.begin();

    File file = MY_FS.open("/upload.bin", FILE_OPEN_MODE_WRITE);
    for (size_t i = 0; i < 2048; i++)
        file.write(i);
    file.close();

#endif
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    app.loop();

    Database.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;
#if defined(ENABLE_FS)

        Serial.println("Setting the file data... ");

        // Binary data from file -> Base64 encoding on-the-fly -> Base64 encoded string stores in your database
        Database.set(aClient, "/test/file", getFile(upload_data), aResult_no_callback);

        Serial.println("Getting the file data... ");

        // Base64 encoded string stores in your database -> Base64 decoding on-the-fly -> Binary data stores as a file
        Database.get(aClient, "/test/file", getFile(download_data), aResult_no_callback);
#endif
    }

    printResult(aResult_no_callback);
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

    if (aResult.downloadProgress())
    {
        Firebase.printf("Download task: %s, downloaded %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.downloadInfo().progress, "%", aResult.downloadInfo().downloaded, aResult.downloadInfo().total);
        if (aResult.downloadInfo().total == aResult.downloadInfo().downloaded)
        {
            Firebase.printf("Download task: %s, completed!\n", aResult.uid().c_str());
#if defined(ENABLE_FS)
            printFile();
#endif
        }
    }

    if (aResult.uploadProgress())
    {
        Firebase.printf("Upload task: %s, uploaded %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.uploadInfo().progress, "%", aResult.uploadInfo().uploaded, aResult.uploadInfo().total);
        if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded)
            Firebase.printf("Upload task: %s, completed!\n", aResult.uid().c_str());
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
        myFile = MY_FS.open(filename, FILE_OPEN_MODE_READ);
        break;
    case file_mode_open_write:
        myFile = MY_FS.open(filename, FILE_OPEN_MODE_WRITE);
        break;
    case file_mode_open_append:
        myFile = MY_FS.open(filename, FILE_OPEN_MODE_APPEND);
        break;
    case file_mode_remove:
        MY_FS.remove(filename);
        break;
    default:
        break;
    }
    // Set the internal FS object with global File object.
    file = myFile;
}

void printFile()
{
    File file = MY_FS.open("/download.bin", FILE_OPEN_MODE_READ);
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

#endif