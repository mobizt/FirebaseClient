/**
 * The example for deleting a ruleset.
 *
 * Note that, the ruleset that has been deployed or used for release creation, will not be deleted.
 *
 * You can delete this ruleset only when the release for this ruleset was update with another rulset or the release for that ruleset was deleted.
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The OAuth2.0 authentication or access token authorization is required for security rules operations.
 * 
 * For Google REST API reference documentation, please visit
 * https://firebase.google.com/docs/reference/rules/rest/v1/projects.rulesets/delete
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_SERVICE_AUTH
#define ENABLE_RULESETS

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

void processData(AsyncResult &aResult);
void delete_ruleset_async(const String &ruleSetId);
void delete_ruleset_async2(const String &ruleSetId);
void delete_ruleset_await(const String &ruleSetId);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

RuleSets ruleSets;

AsyncResult rulesResult;

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

    app.getApp<RuleSets>(ruleSets);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;

        // The ruleset id can be obtained from rulesets.name of the list function JSON response.
        // e.g. projects/<projectId>/rulesets/<rulesetId>
        String ruleSetId = "<rulesetId>";

        delete_ruleset_async(ruleSetId);

        // delete_ruleset_async2(ruleSetId);

        // delete_ruleset_await(ruleSetId);
    }

    // For async call with AsyncResult.
    processData(rulesResult);
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

void delete_ruleset_async(const String &ruleSetId)
{
    Serial.println("Deleting a ruleset...");

    // Async call with callback function.
    ruleSets.remove(aClient, FIREBASE_PROJECT_ID, ruleSetId, processData, "ruleSetsTask");
}

void delete_ruleset_async2(const String &ruleSetId)
{
    Serial.println("Deleting a ruleset...");

    // Async call with AsyncResult for returning result.
    ruleSets.remove(aClient, FIREBASE_PROJECT_ID, ruleSetId, rulesResult);
}

void delete_ruleset_await(const String &ruleSetId)
{
    Serial.println("Deleting a ruleset...");

    // Sync call which waits until the payload was received.
    String payload = ruleSets.remove(aClient, FIREBASE_PROJECT_ID, ruleSetId);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}
