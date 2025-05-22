
/**
 * ABOUT:
 *
 * The example to get/set blob data from/to database.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
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
 * RealtimeDatabase::set(<AsyncClient>, <path>, <file_config_data>, <AsyncResultCallback>, <uid>);
 *
 * RealtimeDatabase::get(<AsyncClient>, <path>, <file_config_data>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <path> - The node path to set/get the BLOB data.
 * <file_config_data> - The file config data which in case of BLOB, it will be obtained from BlobConfig via getBlob.
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
 */

// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
#define ENABLE_USER_CONFIG
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

// For network independent usage (disable all network features).
// #define DISABLE_NERWORKS

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void processData(AsyncResult &aResult);
void set_get_blob_async();
void set_get_blob_asyn2();
void set_get_blob_await();
void printBlob(uint8_t *blob, size_t size);

SSL_CLIENT ssl_client;

// This uses built-in core WiFi/Ethernet for network connection.
// See examples/App/NetworkInterfaces for more network examples.
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult databaseResult;

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

  set_ssl_client_insecure_and_buffer(ssl_client);

  Serial.println("Initializing app...");
  initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

  // Or intialize the app and wait.
  // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);

  app.getApp<RealtimeDatabase>(Database);

  Database.url(DATABASE_URL);

  // Prepare BLOB data
  for (size_t i = 0; i < 2048; i++)
    source[i] = i;
}

void loop()
{
  // To maintain the authentication and async tasks
  app.loop();

  if (app.ready() && !taskComplete)
  {
    taskComplete = true;

    set_get_blob_async();

    // set_get_blob_async2();
    
    // set_get_blob_await();
  }

  // For async call with AsyncResult.
  processData(databaseResult);
}

void processData(AsyncResult &aResult)
{
  // Exits when no result available when calling from the loop.
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
    Firebase.printf("Downloaded, task: %s, %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.downloadInfo().progress, "%", aResult.downloadInfo().downloaded, aResult.downloadInfo().total);
    if (aResult.downloadInfo().total == aResult.downloadInfo().downloaded)
    {
      Firebase.printf("Download task: %s, complete!✅️\n", aResult.uid().c_str());
      printBlob(dest, 2048);
    }
  }

  if (aResult.uploadProgress())
  {
    Firebase.printf("Uploaded, task: %s, %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.uploadInfo().progress, "%", aResult.uploadInfo().uploaded, aResult.uploadInfo().total);
    if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded)
      Firebase.printf("Upload task: %s, complete!✅️\n", aResult.uid().c_str());
  }
}

void set_get_blob_async()
{
  Serial.println("Setting the blob data... ");

  // Async call with callback function.
  Database.set(aClient, "/examples/BLOB/data1", getBlob(upload_data), processData, "⬆️  uploadTask");

  Serial.println("Getting the blob data... ");

  // Async call with callback function.
  Database.get(aClient, "/examples/BLOB/data1", getBlob(download_data), processData, "⬇️  downloadTask");
}

void set_get_blob_async2()
{
  Serial.println("Setting the blob data... ");

  // Async call with AsyncResult for returning result.
  Database.set(aClient, "/examples/BLOB/data2", getBlob(upload_data), databaseResult);

  Serial.println("Getting the blob data... ");

  // Async call with AsyncResult for returning result.
  Database.get(aClient, "/examples/BLOB/data2", getBlob(download_data), databaseResult);
}

void set_get_blob_await()
{
  Serial.println("Setting the blob data... ");

  // Sync call which waits until the payload was received.
  bool status = Database.set(aClient, "/examples/BLOB/data3", getBlob(upload_data));
  if (status)
    Serial.println("🔼 Upload task(await), complete!✅️");
  else
    Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

  Serial.println("Getting the blob data... ");

  // Sync call which waits until the payload was received.
  bool status = Database.get(aClient, "/examples/BLOB/data3", getBlob(download_data));
  if (status)
  {
    Serial.println("🔽 Dowload task (await), complete!✅️");
    printBlob(dest, 2048);
    for (size_t i = 0; i < 2048; i++)
      dest[i] = 0;
  }
  else
    Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
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