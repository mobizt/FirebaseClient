/**
 * The example to get data from database.
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
void get_async();
void get_async2();
void get_await();
void create_dummy_data();

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult databaseResult;

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

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;
        create_dummy_data();

        Serial.println("------------------------------");
        Serial.println("🕒 Await get values");
        Serial.println("------------------------------");
        get_await();

        Serial.println("------------------------------");
        Serial.println("🎈 Async get values");
        Serial.println("------------------------------");
        get_async();
        get_async2();
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
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

void get_async()
{
    // Get the generic value (no waits)
    // Using Database.get with the callback function or AsyncResult object

    Serial.println("Getting the value... ");

    // Async call with callback function.
    Database.get(aClient, "/examples/Get/Async/data1", processData, false /* only for Stream */, "getTask");

    // Apply the filter
    DatabaseOptions options;
    options.filter.orderBy("Data").startAt(105).endAt(120).limitToLast(8);

    Serial.println("Getting the value with filter... ");

    // Async call with callback function.
    Database.get(aClient, "/examples/Get/Async/data3", options, processData, "queryTask");
}


void get_async2()
{
    // Get the generic value (no waits)
    // Using Database.get with the callback function or AsyncResult object

    Serial.println("Getting the value... ");

    // Async call with AsyncResult for returning result.
    Database.get(aClient, "/examples/Get/Async/data2", databaseResult);

    // Apply the filter
    DatabaseOptions options;
    options.filter.orderBy("Data").startAt(105).endAt(120).limitToLast(8);

    Serial.println("Getting the value with filter... ");

    // Async call with AsyncResult for returning result.
    Database.get(aClient, "/examples/Get/Async/data4", options, databaseResult);
}

template <typename T>
void check_and_print_value(T value)
{
    // To make sure that we actually get the result or error.
    if (aClient.lastError().code() == 0)
    {
        Serial.print("Success, Value: ");
        Serial.println(value);
    }
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}

void get_await()
{
    // Get the specific value (waits until the value was received)
    // Using Database.get<T>

    Serial.println("Getting the int value... ");
    int value1 = Database.get<int>(aClient, "/examples/Get/Await/data");
    check_and_print_value(value1);

    Serial.println("Getting the bool value... ");
    bool value2 = Database.get<bool>(aClient, "/examples/Get/Await/data");
    check_and_print_value(value2);

    Serial.println("Getting the float value... ");
    float value3 = Database.get<float>(aClient, "/examples/Get/Await/data");
    check_and_print_value(value3);

    Serial.println("Getting the double value... ");
    double value4 = Database.get<double>(aClient, "/examples/Get/Await/data");
    check_and_print_value(value4);

    Serial.println("Getting the String value... ");
    // The filter can be applied.
    String value5 = Database.get<String>(aClient, "/examples/Get/Await/data");
    check_and_print_value(value5);
}

void create_dummy_data()
{
    Database.set<number_t>(aClient, "/examples/Get/Async/data1", number_t(123.456789, 6));
    Database.set<number_t>(aClient, "/examples/Get/Async/data2", number_t(987.654321, 6));
    Database.set<number_t>(aClient, "/examples/Get/Async/data3", number_t(321.234567, 6));
    Database.set<number_t>(aClient, "/examples/Get/Async/data4", number_t(456.789012, 6));
    Database.set<number_t>(aClient, "/examples/Get/Await/data", number_t(63.475869, 6));
}