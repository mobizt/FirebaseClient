

/**
 * ABOUT:
 *
 * The non-blocking (async) example to store your binary data to the database and read it back.
 *
 * With the BlobConfig object provides with the set function, the binary data will be converted to base64
 * encoded string on-the-fly when storing to your database.
 *
 * And when the BlobConfig object provides with the get function, the data in your database
 * (assumed that it is base64 encoded string) will be converted to binary data
 * on-the-fly when reading from your database.
 *
 * See the BlobConfig class constructor syntax below.
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
 * BlobConfig::BlobConfig(<data>, <size>);
 *
 * <data> - The BLOB data (byte array).
 * <size> - The size of data.
 *
 * The data can be a source (input) and target (output) data that used in upload and download.
 *
 * 2.------------------------
 *
 * RealtimeDatabase::set(<AsyncClient>, <path>, <file_config_data>, <AsyncResult>);
 *
 * RealtimeDatabase::get(<AsyncClient>, <path>, <file_config_data>, <AsyncResult>);
 *
 * <AsyncClient> - The async client.
 * <path> - The node path to set/get the BLOB data.
 * <file_config_data> - The file config data which in case of BLOB, it will be obtained from BlobConfig via getBlob.
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
void printBlob(uint8_t *blob, size_t size);

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

uint8_t source[2048];
uint8_t dest[2048];

BlobConfig upload_data(source, 2048); // Can be set later with upload_data.setBlob(source, 2048);
BlobConfig download_data(dest, 2048); // Can be set later with download_data.setBlob(dest, 2048);

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

  // Prepare BLOB data
  for (size_t i = 0; i < 2048; i++)
    source[i] = i;
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

    Serial.println("Setting the blob data... ");

    // Binary data -> Base64 encoding on-the-fly -> Base64 encoded string stores in your database
    Database.set(aClient, "/test/blob", getBlob(upload_data), aResult_no_callback);

    Serial.println("Getting the blob data... ");

    // Base64 encoded string stores in your database -> Base64 decoding on-the-fly -> Binary data
    Database.get(aClient, "/test/blob", getBlob(download_data), aResult_no_callback);
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

  if (aResult.downloadProgress())
  {
    Firebase.printf("Downloaded, task: %s, %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.downloadInfo().progress, "%", aResult.downloadInfo().downloaded, aResult.downloadInfo().total);
    if (aResult.downloadInfo().total == aResult.downloadInfo().downloaded)
    {
      Firebase.printf("Download task: %s, completed!", aResult.uid().c_str());
      printBlob(dest, 2048);
    }
  }

  if (aResult.uploadProgress())
  {
    Firebase.printf("Uploaded, task: %s, %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.uploadInfo().progress, "%", aResult.uploadInfo().uploaded, aResult.uploadInfo().total);
    if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded)
      Firebase.printf("Upload task: %s, completed!", aResult.uid().c_str());
  }
}

void printBlob(uint8_t *blob, size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    if (i > 0 && i % 16 == 0)
      Serial.println();
    if (blob[i] < 16)
      Serial.print((const char *)FPSTR("0"));
    Serial.print(blob[i], HEX);
    Serial.print((const char *)FPSTR(" "));
  }
  Serial.println();
}