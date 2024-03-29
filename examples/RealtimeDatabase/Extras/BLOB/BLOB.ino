
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
 * BLOB CONFIG SYNTAX
 * ==================
 *
 * BlobConfig blob_config(<data>, <size>);
 *
 * <data> - The BLOB data (uint8_t array).
 * <size> - The size of data.
 *
 * The data can be a source (input) and target (output) data that used in upload and download.
 */

/** THE STATIC ASYNC RESULT REQUIRED FOR ASYNC OPERATION
 * =====================================================
 *
 * Library provided the class object called AsyncResult that keeps the server response data, debug and error information.
 *
 * There are two sources of async result in this library:
 *
 * 1. From user provided with async application (function).
 *
 * For example:
 *
 * `Database.get(aClient, "/test/int", options, aResult);`
 *
 * 2. From dynamic allocation in the async client.
 *
 * For example:
 *
 * `Database.get(aClient, "/test/int", options, asyncCB);`
 *
 * From source 1, the async result (`aResult`) shall be defined globally to use in async application because of the static data is needed for use while running the sync task.
 *
 * From source 2, the async client (`aClient`) shall be defined globally to use in async application too to make sure the instance of async result was existed or valid while running the sync task.
 *
 * If async result was destroyed (destructed or not existed) before it was used by async task handler, the danglig pointer problem will be occurred.
 *
 * Note that, the async client object used in authentication task shoul be defined globally as it is async task.
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

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void asyncCB(AsyncResult &aResult);
void printBlob(uint8_t *blob, size_t size);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

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

RealtimeDatabase Database;

uint8_t source[2048];
uint8_t dest[2048];

BlobConfig upload_data(source, 2048);
BlobConfig download_data(dest, 2048);

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

  app.getApp<RealtimeDatabase>(Database);

  Database.url(DATABASE_URL);

  // Prepare BLOB data
  for (size_t i = 0; i < 2048; i++)
    source[i] = i;

  Serial.println("Set blob... ");
  Database.set(aClient, "/test/blob", getBlob(upload_data), asyncCB);

  // To assign UID for async result
  // Database.set(aClient, "/test/blob", getBlob(upload_data), asyncCB, "uploadTask");

  Serial.println("Get blob... ");
  Database.get(aClient, "/test/blob", getBlob(download_data), asyncCB);

  // To assign UID for async result
  // Database.get(aClient, "/test/blob", getBlob(download_data), asyncCB, "downloadTask");
}

void loop()
{
  // This function is required for handling async operations and maintaining the authentication tasks.
  app.loop();

  // This required when different AsyncClients than used in FirebaseApp assigned to the Realtime database functions.
  Database.loop();
}

void asyncCB(AsyncResult &aResult)
{

  // To get the UID (string) from async result
  // aResult.uid();

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

  if (aResult.downloadProgress())
  {

    Firebase.printf("Downloaded: %d%s (%d of %d)\n", aResult.downloadInfo().progress, "%", aResult.downloadInfo().downloaded, aResult.downloadInfo().total);
    if (aResult.downloadInfo().total == aResult.downloadInfo().downloaded)
    {
      Serial.println("Download completed!");
      printBlob(dest, 2048);
    }
  }

  if (aResult.uploadProgress())
  {
    Firebase.printf("Uploaded: %d%s (%d of %d)\n", aResult.uploadInfo().progress, "%", aResult.uploadInfo().uploaded, aResult.uploadInfo().total);
    if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded)
      Serial.println("Upload completed!");
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