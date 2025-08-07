/**
 * The example to read (get) the security rules and write (set) the indexing rule for data filtering.
 *
 * This example uses the LegacyToken (or ServiceAuth) class for authentication/authorization which
 * has the highest privilledge which needs to access the security rules.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_SERVICE_AUTH
#define ENABLE_DATABASE

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.
#include <FirebaseJson.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

#define DATABASE_URL "URL"

void processData(AsyncResult &aResult);
void addIndexing(const String &path, const String &key);

ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<3600) */);

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

FirebaseApp app;
RealtimeDatabase Database;

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

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    // For indexing data doc. please see https://firebase.google.com/docs/database/security/indexing-data

    // The path to index: /examples/Indexing/data
    // The node name to index: Data

    addIndexing("examples/Indexing/data/.indexOn", "json"); // -> {"examples":{"Indexing":{"data":{".indexOn":"json"}}}}
}

void loop()
{
    app.loop(); // For async call with AsyncResult.
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
}

void addIndexing(const String &path, const String &key)
{
    Serial.println("Getting the security rules to check the existing conditions... ");

    String jsonStr = Database.get<String>(aClient, ".settings/rules");

    // If security rules are ready get.
    if (aClient.lastError().code() == 0)
    {
        Serial.println("Success");

        bool ret = true;
        FirebaseJsonData parseResult;
        FirebaseJson currentRules(jsonStr);

        if (path.length() > 0)
        {
            String rulePath = "rules/";
            rulePath += path;
            if (!currentRules.get(parseResult, rulePath))
            {
                currentRules.set(rulePath, key);
                String modifiedRules;
                currentRules.toString(modifiedRules, true);

                Serial.println("Setting the security rules... ");
                bool status = Database.set<object_t>(aClient, ".settings/rules", object_t(modifiedRules));
                if (status)
                    Serial.println("Success");
                else
                    Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
            }
            else
            {
                Serial.println("The indexing rules exist, nothing to change");
            }
        }

        currentRules.clear();
    }
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}