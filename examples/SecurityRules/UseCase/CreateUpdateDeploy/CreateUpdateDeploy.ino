/**
 * The example for creating a ruleset for Firestore, creating a release (deploying a ruleset) and updating a release.
 *
 * The Firestore database access will be tested before and after updating release.
 * 
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_SERVICE_AUTH
#define ENABLE_RULESETS

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.
#include <FirebaseJson.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

String create_ruleset(Ruleset &ruleset);
bool delete_release(const String &releaseId);
bool create_release(const String &releaseId, const String &rulesetId);
void update_release(const String &releaseId, const String &rulesetId);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

RuleSets ruleSets;

Releases releases;

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
    initializeApp(aClient, app, getAuth(sa_auth), 120 * 1000, auth_debug_print);

    app.getApp<RuleSets>(ruleSets);
    app.getApp<Releases>(releases);

    if (app.ready())
    {
        // Create a ruleset that disable user access;
        Ruleset ruleset;
        Rules::Source src;
        Rules::File file;

        String rules = "rules_version = '2';\n\nservice cloud.firestore {\n  match /databases/{database}/documents {\n    match /{document=**} {\n";
        rules += "      allow read, write: if false;\n    }\n  }\n}";
        file.content(rules);
        file.name("firestore.rules");
        file.fingerprint(ruleSets.getSha1(rules));
        src.files(file);
        ruleset.source(src);

        // Create a ruleset;
        String rulesetId = create_ruleset(ruleset);

        if (rulesetId.length())
        {
            // Delete a release.
            delete_release("cloud.firestore/(default)");

            // Create (deploy) a release with previously created ruleset.
            bool res = create_release("cloud.firestore/(default)", rulesetId);

            if (res)
            {
                // Now we want to change the rules by allow user to access.
                Ruleset newRleset;
                Rules::Source newSrc;
                Rules::File newFile;
                String newRules = "rules_version = '2';\n\nservice cloud.firestore {\n  match /databases/{database}/documents {\n    match /{document=**} {\n";
                newRules += "      allow read, write: if request.auth.uid != null;\n    }\n  }\n}";
                newFile.content(newRules);
                newFile.name("firestore.rules");
                newFile.fingerprint(ruleSets.getSha1(newRules));
                newSrc.files(newFile);
                newRleset.source(newSrc);

                // Create new ruleset.
                String newRulesetId = create_ruleset(newRleset);

                // Update a release with new ruleset.
                update_release("cloud.firestore/(default)", newRulesetId);
            }
        }
    }
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();
}

String create_ruleset(Ruleset &ruleset)
{
    String rulesetId;
    Serial.println("Creating a ruleset...");
    String payload = ruleSets.create(aClient, FIREBASE_PROJECT_ID, ruleset);

    if (aClient.lastError().code() == 0)
    {
        Serial.println(payload);

        FirebaseJson json(payload);
        FirebaseJsonData result;

        json.get(result, "name");

        if (result.success)
        {
            rulesetId = result.to<String>();
            rulesetId = rulesetId.substring(rulesetId.lastIndexOf("/") + 1);
        }
    }
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

    return rulesetId;
}

bool delete_release(const String &releaseId)
{
    Serial.println("Deleting a release...");

    // Sync call which waits until the payload was received.
    String payload = releases.remove(aClient, FIREBASE_PROJECT_ID, releaseId);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

    return aClient.lastError().code() == 0;
}

bool create_release(const String &releaseId, const String &rulesetId)
{
    Release release;

    // The release name (Id) should reflect the developer's deployment practices.
    // For example, the release name may include the environment name, application name, application version, or any other name meaningful to the developer.
    // Once a Release refers to a Ruleset, the rules can be enforced by Firebase Rules-enabled services.
    release.releaseId(releaseId);

    // The ruleset Id return from the JSON payload from RuleSets::list, RuleSets::create,
    // e.g. projects/<projectId>/rulesets/<rulesetId>
    release.rulesetId(rulesetId);

    Serial.println("Creating a release...");

    // Sync call which waits until the payload was received.
    String payload = releases.create(aClient, FIREBASE_PROJECT_ID, release);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

    return aClient.lastError().code() == 0;
}

void update_release(const String &releaseId, const String &rulesetId)
{
    Release release;

    release.releaseId(releaseId);
    release.rulesetId(rulesetId);

    Serial.println("Updating the release...");

    // Sync call which waits until the payload was received.
    String payload = releases.patch(aClient, FIREBASE_PROJECT_ID, release, "" /* update mask */);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}
