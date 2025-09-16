/**
 * The example to get information about app instances.
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * You can test this example with Firebase Cloud Messaging Web Client App in /examples/Messaging/WebClient.
 *
 * For Google Instance ID REST API reference documentation, please visit
 * https://developers.google.com/instance-id/reference/server#get_information_about_app_instances
 *
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_SERVICE_AUTH
#define ENABLE_MESSAGING

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

#define FIREBASE_FCM_DEVICE_TOKEN_1 "IID_TOKEN_1"

void processData(AsyncResult &aResult);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

MessagingInstance fcmInstance;
AsyncResult fcmResult;

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

    // Assign the valid time only required for authentication process with ServiceAuth and CustomAuth.
    app.setTime(get_ntp_time());

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(sa_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(sa_auth), 120 * 1000, auth_debug_print);

    app.getApp<MessagingInstance>(fcmInstance);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;

        Serial.println("Get information about app instances...");

        fcmInstance.get(aClient, FIREBASE_FCM_DEVICE_TOKEN_1, processData, "fcmGetInstanceInfoTask");
    }

    // For async call with AsyncResult.
    processData(fcmResult);
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
