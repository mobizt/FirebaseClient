
/**
 * The example to create a function (Cloud Functions).
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The OAuth2.0 authentication or access token authorization is required for Cloud Functions operations.
 * 
 * For Google REST API reference documentation, please visit
 * https://cloud.google.com/functions/docs/reference/rest/v2/projects.locations.functions/create
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_SERVICE_AUTH
#define ENABLE_FUNCTIONS

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

// Define the project location e.g. us-central1 or asia-northeast1 */
// https://firebase.google.com/docs/projects/locations
#define PROJECT_LOCATION "PROJECT_LOCATION"

// Define the Firebase storage bucket ID e.g bucket-name.appspot.com */
#define STORAGE_BUCKET_ID "BUCKET-NAME.appspot.com"

void processData(AsyncResult &aResult);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

CloudFunctions cfunctions;

bool taskCompleted = false;

AsyncResult cloudFunctionsResult;

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

    app.getApp<CloudFunctions>(cfunctions);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        GoogleCloudFunctions::Function function;

        GoogleCloudFunctions::BuildConfig buildConfig;
        buildConfig.entryPoint("helloHttp");
        buildConfig.runtime("nodejs20");

        GoogleCloudFunctions::Source source;
        GoogleCloudFunctions::StorageSource storageSource;
        storageSource.bucket(STORAGE_BUCKET_ID);
        storageSource.object("helloWorld.zip"); // The functions source files (zip) in the Storage bucket.
        source.storageSource(storageSource);
        buildConfig.source(source);
        function.buildConfig(buildConfig);

        GoogleCloudFunctions::ServiceConfig serviceConfig;
        serviceConfig.availableMemory("256M");
        serviceConfig.ingressSettings(GoogleCloudFunctions::ALLOW_ALL);
        serviceConfig.allTrafficOnLatestRevision(true);
        serviceConfig.maxInstanceRequestConcurrency(1);

        function.serviceConfig(serviceConfig);

        function.environment(GoogleCloudFunctions::Environment::GEN_2);

        // You can set the content of function object directly with function.setContent("your content")

        Serial.println("Createing a new function...");

        // Async call with callback function.
        cfunctions.create(aClient, GoogleCloudFunctions::Parent(FIREBASE_PROJECT_ID, PROJECT_LOCATION), "helloHttp" /* function name */, function, processData, "createTask");

        // Async call with AsyncResult for returning result.
        cfunctions.create(aClient, GoogleCloudFunctions::Parent(FIREBASE_PROJECT_ID, PROJECT_LOCATION), "helloHttp" /* function name */, function, cloudFunctionsResult);

        // Sync call which waits until the payload was received.
        String payload = cfunctions.create(aClient, GoogleCloudFunctions::Parent(FIREBASE_PROJECT_ID, PROJECT_LOCATION), "test" /* function name */, function);
        if (aClient.lastError().code() == 0)
            Serial.println(payload);
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    }

    // For async call with AsyncResult.
    processData(cloudFunctionsResult);
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

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}