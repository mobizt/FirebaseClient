/**
 * The example to set the value's priority and do some filtering.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
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
    initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    Serial.println("Setting the data (JSON object) with priority... ");

    object_t json, obj1, obj2;
    JsonWriter writer;

    for (int i = 0; i < 15; i++)
    {
        // Set priority to object or node that contains children.
        float priority = 15 - i;

        writer.create(obj1, "item_" + String(i + 1), string_t("value_" + String(i + 1))); // -> {"item_x":"value_x"}
        writer.create(obj2, ".priority", priority);                             // -> {".priority":x}
        writer.join(json, 2, obj1, obj2);

        String path = "/examples/Priority/items/priority_" + String(15 - i);

        bool status = Database.set<object_t>(aClient, path, json);
        if (status)
            Serial.println(String("Success"));
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    }

    // Change priority
    Serial.println();
    Serial.println("Setting the primitive value with priority... ");

    writer.create(obj1, ".value", "item_15");
    // Set priority to primitive value.
    writer.create(obj2, ".priority", 6.0);
    writer.join(json, 2, obj1, obj2);

    bool status = Database.set<object_t>(aClient, "/examples/Priority/items/priority_1", json);
    if (status)
        Serial.println(String("Success"));
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

    Serial.println();
    Serial.println("Getting the priority... ");
    double v = Database.get<double>(aClient, "/examples/Priority/items/priority_1/.priority");

    if (aClient.lastError().code() == 0)
        Serial.println(String(v));
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

    Serial.println();
    Serial.println("Filtering the data based on its priority... ");
    DatabaseOptions options;
    // Now the node priority_1 which its priority changed to 6.0 should include in the filtering result.
    options.filter.orderBy("$priority").startAt(3.0).endAt(8.0);

    String payload = Database.get<String>(aClient, "/examples/Priority/items", options);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();
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