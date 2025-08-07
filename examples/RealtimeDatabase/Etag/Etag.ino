/**
 * The example to work with Etag to prevent writing to the out dated data.
 *
 * This example uses the UserAuth class for authentication, and the DefaultNetwork class for network interface configuration.
 * See examples/App/AppInitialization and examples/App/NetworkInterfaces for more authentication and network examples.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void processData(AsyncResult &aResult);
void set_loop(uint16_t timeout);
void update_loop(uint16_t timeout);

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult databaseResult;

unsigned long set_loop_ms = 0, update_loop_ms = 0;
int set_count = 0;
String current_etag;

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
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready())
    {
        set_loop(10000);
        update_loop(40000);
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
        if (aResult.uid() == "setJsonEtagTask")
        {
            if (aResult.error().code() == 412)
            {
                // The sever's etag value has changed due to data changed by updateJsonTask.
                // We cannot use current erag for setJsonEtagTask anymore.
                current_etag = "";
            }
        }

        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
        if (aResult.uid() == "setJsonEtagTask")
        {
            current_etag = aResult.etag();
            Firebase.printf("Etag: %s\n", current_etag.c_str());
        }
    }
}

void set_loop(uint16_t timeout)
{
    if (app.ready() && (millis() - set_loop_ms > timeout || set_loop_ms == 0))
    {
        set_loop_ms = millis();
        set_count++;

        object_t json, obj1, obj2, obj3;
        JsonWriter writer;

        writer.create(obj1, "count/value", set_count);
        writer.create(obj2, "string/value", string_t("hello"));
        writer.join(obj3, 2 /* no. of object_t (s) to join */, obj1, obj2);
        writer.create(json, "node/set", obj3);
        // The etag can be obtained when using set, push and gat unless update.
        Database.set<object_t>(aClient, "/examples/Etag/JSON", json, processData, "setJsonEtagTask", current_etag);
    }
}

void update_loop(uint16_t timeout)
{
    if (app.ready() && millis() - update_loop_ms > timeout)
    {
        update_loop_ms = millis();
        Database.set<String>(aClient, "/examples/Etag/JSON/node/set/string/value", String(random(1000, 2000)), processData, "updateJsonTask");
    }
}