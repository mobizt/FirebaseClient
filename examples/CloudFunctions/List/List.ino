/**
 * ABOUT:
 *
 * The example to List the functions that belong to a project (Cloud Functions).
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The OAuth2.0 authentication or access token authorization is required for Cloud Functions operations.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * CloudFunctions::list(<AsyncClient>, <GoogleCloudFunctions::Parent>, <GoogleCloudFunctions::ListOptions>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <GoogleCloudFunctions::Parent> - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
 * <functionId> - The function name or Id to get.
 * <GoogleCloudFunctions::ListOptions> - The GoogleCloudFunctions::ListOptions object that provides the functions to set query parameters e.g. pageSize, pageToken, filter, and orderBy.
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
 *
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The location name is the project location.
 */

#include <Arduino.h>
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

void processData(AsyncResult &aResult);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

// This uses built-in core WiFi/Ethernet for network connection.
// See examples/App/NetworkInterfaces for more network examples.
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

        GoogleCloudFunctions::ListOptions options;
        String filter = "name:projects/";
        filter += FIREBASE_PROJECT_ID;
        filter += "/locations/";
        filter += PROJECT_LOCATION;
        filter += "/functions/helloWorld";

        options.filter(filter).pageSize(1);

        Serial.println("Listing the functions that belong to a project...");

        // Async call with callback function.
        cfunctions.list(aClient, GoogleCloudFunctions::Parent(FIREBASE_PROJECT_ID, PROJECT_LOCATION), options, processData, "listTask");

        // Async call with AsyncResult for returning result.
        cfunctions.list(aClient, GoogleCloudFunctions::Parent(FIREBASE_PROJECT_ID, PROJECT_LOCATION), options, cloudFunctionsResult);

        // Sync call which waits until the payload was received.
        String payload = cfunctions.list(aClient, GoogleCloudFunctions::Parent(FIREBASE_PROJECT_ID, PROJECT_LOCATION), options);
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

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}