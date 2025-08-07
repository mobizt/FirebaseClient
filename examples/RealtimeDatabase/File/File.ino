/**
 * The example to store your file data to the database and read it back.
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
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE
#define ENABLE_FS

// You may have to include the storage library you used e.g. SPIFFS.h or SD.h here.
#include <FS.h>
// #include <SPIFFS.h>
// #include <SD.h>

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void processData(AsyncResult &aResult);
void set_get_file_async();
void set_get_file_async2();
void set_get_file_await();
void push_file_async();
void push_file_async2();
void push_file_await();

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult databaseResult;

#if defined(ENABLE_FS)
FileConfig upload_data("/upload.bin", file_operation_callback);     // Can be set later with upload_data.setFile("/upload.bin", fileCallback);
FileConfig download_data("/download.bin", file_operation_callback); // Can be set later with download_data.setFile("/download.bin", fileCallback);
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

    set_ssl_client_insecure_and_buffer(ssl_client);

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);

    app.getApp<RealtimeDatabase>(Database);

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
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;
#if defined(ENABLE_FS)
        set_get_file_async();

        // set_get_file_async2();

        // set_get_file_await();

        // push_file_async();

        // push_file_async2();

        // push_file_await();
#endif
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
            print_file_content("/download.bin");
        }
    }

    if (aResult.uploadProgress())
    {
        Firebase.printf("Upload task: %s, uploaded %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.uploadInfo().progress, "%", aResult.uploadInfo().uploaded, aResult.uploadInfo().total);
        if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded)
            Firebase.printf("Upload task: %s, complete!✅️\n", aResult.uid().c_str());

        if (aResult.name().length())
            Firebase.printf("Name: %s\n", aResult.name().c_str());
    }
}

void set_get_file_async()
{
    Serial.println("Setting the file data... ");

    // Async call with callback function.
    Database.set(aClient, "/examples/File/data1", getFile(upload_data), processData, "⬆️  uploadTask");

    Serial.println("Getting the file data... ");

    // Async call with callback function.
    Database.get(aClient, "/examples/File/data1", getFile(download_data), processData, "⬇️  downloadTask");
}

void set_get_file_async2()
{
    Serial.println("Setting the file data... ");

    // Async call with AsyncResult for returning result.
    Database.set(aClient, "/examples/File/data2", getFile(upload_data), databaseResult);

    Serial.println("Getting the file data... ");

    // Async call with AsyncResult for returning result.
    Database.get(aClient, "/examples/File/data2", getFile(download_data), databaseResult);
}

void set_get_file_await()
{
    Serial.println("Setting the file data... ");

    // Sync call which waits until the payload was received.
    bool status = Database.set(aClient, "/examples/File/data3", getFile(upload_data));
    if (status)
        Serial.println("🔼 Upload task (await), complete!✅️");
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

    Serial.println("Getting the file data... ");

    // Sync call which waits until the payload was received.
    status = Database.get(aClient, "/examples/File/data3", getFile(download_data));
    if (status)
    {
        Serial.println("🔽 Dowload task (await), complete!✅️");
        print_file_content("/download.bin");
    }
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}

void push_file_async()
{
    Serial.println("Pushing the file data... ");

    // Async call with callback function.
    Database.push(aClient, "/examples/File/data4", getFile(upload_data), processData, "⬆️  uploadTask");
}

void push_file_async2()
{
    Serial.println("Pushing the file data... ");

    // Async call with AsyncResult for returning result.
    Database.push(aClient, "/examples/File/data5", getFile(upload_data), databaseResult);
}

void push_file_await()
{
    Serial.println("Pushing the file data... ");

    // Sync call which waits until the payload was received.
    String name = Database.push(aClient, "/examples/File/data6", getFile(upload_data));
    if (aClient.lastError().code() == 0)
        Firebase.printf("🔼 Upload task (await), complete!✅️, Name: : %s\n", name.c_str());
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}