/**
 * The example for creating a ruleset.
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The OAuth2.0 authentication or access token authorization is required for security rules operations.
 * 
 * For Google REST API reference documentation, please visit
 * https://firebase.google.com/docs/reference/rules/rest/v1/projects.rulesets/create
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
void create_ruleset_async(Ruleset &ruleset);
void create_ruleset_async2(Ruleset &ruleset);
void create_ruleset_await(Ruleset &ruleset);

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

        Ruleset ruleset;
        Rules::Source src;
        Rules::File file;

        String rules = "rules_version = '2';\n\nservice cloud.firestore {\n  match /databases/{database}/documents {\n    match /{document=**} {\n";
        rules += "      allow read, write: if request.auth.uid != null;\n    }\n  }\n}";
        file.content(rules);
        file.name("firestore.rules");
        file.fingerprint(ruleSets.getSha1(rules));
        src.files(file);
        ruleset.source(src);

        create_ruleset_async(ruleset);

        // create_ruleset_async2(ruleset);

        // create_ruleset_await(ruleset);
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

void create_ruleset_async(Ruleset &ruleset)
{
    Serial.println("Creating a ruleset...");

    // Async call with callback function.
    ruleSets.create(aClient, FIREBASE_PROJECT_ID, ruleset, processData, "ruleSetsTask");
}

void create_ruleset_async2(Ruleset &ruleset)
{
    Serial.println("Creating a ruleset...");

    // Async call with AsyncResult for returning result.
    ruleSets.create(aClient, FIREBASE_PROJECT_ID, ruleset, rulesResult);
}

void create_ruleset_await(Ruleset &ruleset)
{
    Serial.println("Creating a ruleset...");

    // Sync call which waits until the payload was received.
    String payload = ruleSets.create(aClient, FIREBASE_PROJECT_ID, ruleset);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}