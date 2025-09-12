/**
 * The example to upload object (file) to Cloud Storage bucket.
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The OAuth2.0 authentication or access token authorization is required for Cloud Storage operations.
 *
 * For Google JSON API reference documentation, please visit
 * https://cloud.google.com/storage/docs/json_api/v1/objects/insert
 * https://cloud.google.com/storage/docs/resumable-uploads
 * https://cloud.google.com/storage/docs/multipart-uploads
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_SERVICE_AUTH
#define ENABLE_CLOUD_STORAGE
#define ENABLE_FS

// You may have to include the storage library you used e.g. SPIFFS.h or SD.h here.
#include <FS.h>
// #include <SPIFFS.h>
// #include <SD.h>

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

// Define the Firebase storage bucket ID e.g bucket-name.appspot.com */
#define STORAGE_BUCKET_ID "BUCKET-NAME.appspot.com"

void processData(AsyncResult &aResult);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FileConfig media_file("/media.mp4", file_operation_callback); // Can be set later with media_file.setFile("/media.mp4", file_operation_callback);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

CloudStorage cstorage;

bool taskComplete = false;

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

#if defined(ENABLE_FS)
    MY_FS.begin();
#endif
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;

        GoogleCloudStorage::UploadOptions options;
        options.mime = "video/mp4";
        options.uploadType = GoogleCloudStorage::upload_type_resumable;
        // options.uploadType = GoogleCloudStorage::upload_type_simple;

#if defined(ENABLE_FS)
        Serial.println("Uploading file...");

        // Async call with callback function.
        cstorage.upload(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), options, processData, "⬆️  uploadTask");

        // Async call with AsyncResult for returning result.
        // cstorage.upload(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), options, cloudStorageResult);

        // Sync call which waits until the operation complete.
        // bool status = cstorage.upload(aClient, GoogleCloudStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), options);
        // if (status)
        //     Serial.println("🔼 Upload task(await), complete!✅️");
        // else
        //     Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
#endif
    }

    // For async call with AsyncResult.
    processData(cloudStorageResult);
}

void processData(AsyncResult &aResult)
{
    // Exits when no result available when call from the loop.
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
        }
    }

    if (aResult.uploadProgress())
    {
        Firebase.printf("Uploaded, task: %s, %d%s (%d of %d)\n", aResult.uid().c_str(), aResult.uploadInfo().progress, "%", aResult.uploadInfo().uploaded, aResult.uploadInfo().total);
        if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded)
        {
            Firebase.printf("Upload task: %s, complete!✅️\n", aResult.uid().c_str());
            Serial.print("Download URL: ");
            Serial.println(aResult.uploadInfo().downloadUrl);
        }
    }
}