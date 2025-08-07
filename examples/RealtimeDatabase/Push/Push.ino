/**
 * The example to push data to the database.
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
void push_async();
void push_async2();
void show_status(const String &name);
void push_await();

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

        Serial.println("------------------------------");
        Serial.println("🕒 Await push values");
        Serial.println("------------------------------");
        push_await();

        Serial.println("------------------------------");
        Serial.println("🎈 Async push values");
        Serial.println("------------------------------");
        push_async();

        push_async2();
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
        if (aResult.to<RealtimeDatabaseResult>().name().length())
            Firebase.printf("task: %s, name: %s\n", aResult.uid().c_str(), aResult.to<RealtimeDatabaseResult>().name().c_str());
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

void push_async()
{
    // Push the specific value (no waits)
    // Using Database.push with the callback function or AsyncResult object

    // Push int
    Serial.println("Pushing the int value... ");
    Database.push<int>(aClient, "/examples/Push/Async1/int", 12345, processData, "pushIntTask");

    // Push bool
    Serial.println("Pushting the bool value... ");
    Database.push<bool>(aClient, "/examples/Push/Async1/bool", true, processData, "pushBoolTask");

    // Push string
    Serial.println("Pushting the String value... ");
    Database.push<String>(aClient, "/examples/Push/Async1/String", "hello", processData, "pushStringTask");

    // Push json
    Serial.println("Pushting the JSON value... ");
    Database.push<object_t>(aClient, "/examples/Push/Async1/JSON", object_t("{\"data\":123}"), processData, "pushJsonTask");

    object_t json, obj1, obj2, obj3, obj4;
    JsonWriter writer;

    writer.create(obj1, "int/value", 9999);
    writer.create(obj2, "string/value", string_t("hello"));
    writer.create(obj3, "float/value", number_t(123.456, 2));
    writer.join(obj4, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);
    writer.create(json, "node/list", obj4);

    // To print object_t
    // Serial.println(json);
    Serial.println("Pushting the JSON value... ");
    Database.push<object_t>(aClient, "/examples/Push/Async1/JSON", json, processData, "pushJsonTask");
    Database.push<object_t>(aClient, "/examples/Push/Async2/JSON", json, databaseResult);

    object_t arr;
    arr.initArray(); // initialize to be used as array
    writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

    // Note that value that sets to object_t other than JSON ({}) and Array ([]) can be valid only if it
    // used as array member value as above i.e. object_t(string_t("steve")) and object_t(888).

    // Push array
    Serial.println("Pushting the Array value... ");
    Database.push<object_t>(aClient, "/examples/Push/Async1/Array", arr, processData, "pushArrayTask");

    // Push float
    Serial.println("Pushting the float value... ");
    Database.push<number_t>(aClient, "/examples/Push/Async1/float", number_t(123.456, 2), processData, "pushFloatTask");

    // Push double
    Serial.println("Pushting the double value... ");
    Database.push<number_t>(aClient, "/examples/Push/Async1/double", number_t(1234.56789, 4), processData, "pushDoubleTask");
}

void push_async2()
{
    // Push the specific value (no waits)
    // Using Database.push with the callback function or AsyncResult object

    // Push int
    Serial.println("Pushing the int value... ");
    Database.push<int>(aClient, "/examples/Push/Async2/int", 12345, databaseResult);

    // Push bool
    Serial.println("Pushting the bool value... ");
    Database.push<bool>(aClient, "/examples/Push/Async2/bool", true, databaseResult);

    // Push string
    Serial.println("Pushting the String value... ");
    Database.push<String>(aClient, "/examples/Push/Async2/String", "hello", databaseResult);

    // Push json
    Serial.println("Pushting the JSON value... ");
    Database.push<object_t>(aClient, "/examples/Push/Async2/JSON", object_t("{\"data\":123}"), databaseResult);

    object_t json, obj1, obj2, obj3, obj4;
    JsonWriter writer;

    writer.create(obj1, "int/value", 9999);
    writer.create(obj2, "string/value", string_t("hello"));
    writer.create(obj3, "float/value", number_t(123.456, 2));
    writer.join(obj4, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);
    writer.create(json, "node/list", obj4);

    // To print object_t
    // Serial.println(json);
    Serial.println("Pushting the JSON value... ");
    Database.push<object_t>(aClient, "/examples/Push/Async2/JSON", json, databaseResult);

    object_t arr;
    arr.initArray(); // initialize to be used as array
    writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

    // Note that value that sets to object_t other than JSON ({}) and Array ([]) can be valid only if it
    // used as array member value as above i.e. object_t(string_t("steve")) and object_t(888).

    // Push array
    Serial.println("Pushting the Array value... ");
    Database.push<object_t>(aClient, "/examples/Push/Async2/Array", arr, databaseResult);

    // Push float
    Serial.println("Pushting the float value... ");
    Database.push<number_t>(aClient, "/examples/Push/Async2/float", number_t(123.456, 2), databaseResult);

    // Push double
    Serial.println("Pushting the double value... ");
    Database.push<number_t>(aClient, "/examples/Push/Async2/double", number_t(1234.56789, 4), databaseResult);
}

void show_status(const String &name)
{
    if (aClient.lastError().code() == 0)
        Firebase.printf("Success, Name: : %s\n", name.c_str());
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}

void push_await()
{
    // Push the specific value (waits until the value was sucessfully push)
    // Using Database.push<T>

    // Push int
    Serial.println("Pushting the int value... ");
    String name = Database.push<int>(aClient, "/examples/Push/Await/int", 12345);
    show_status(name);

    // Push bool
    Serial.println("Pushting the bool value... ");
    name = Database.push<bool>(aClient, "/examples/Push/Await/bool", true);
    show_status(name);

    // Push string
    Serial.println("Pushting the String value... ");
    name = Database.push<String>(aClient, "/examples/Push/Await/String", "hello");
    show_status(name);

    // Push json
    Serial.println("Pushting the JSON value... ");
    name = Database.push<object_t>(aClient, "/examples/Push/Await/JSON", object_t("{\"data\":123}"));
    show_status(name);

    // Library does not provide JSON parser library, the following JSON writer class will be used with
    // object_t for simple demonstration.

    object_t json, obj1, obj2, obj3, obj4;
    JsonWriter writer;

    writer.create(obj1, "int/value", 9999);
    writer.create(obj2, "string/value", string_t("hello"));
    writer.create(obj3, "float/value", number_t(123.456, 2));
    writer.join(obj4, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);
    writer.create(json, "node/list", obj4);

    // To print object_t
    // Serial.println(json);

    Serial.println("Pushting the JSON value... ");
    name = Database.push<object_t>(aClient, "/examples/Push/Await/JSON", json);
    show_status(name);

    object_t arr;
    arr.initArray(); // initialize to be used as array
    writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

    // Note that value that sets to object_t other than JSON ({}) and Array ([]) can be valid only if it
    // used as array member value as above i.e. object_t(string_t("steve")) and object_t(888).

    // Push array
    Serial.println("Pushting the Array value... ");
    name = Database.push<object_t>(aClient, "/examples/Push/Await/Array", arr);
    show_status(name);

    // Push float
    Serial.println("Pushting the float value... ");
    name = Database.push<number_t>(aClient, "/examples/Push/Await/float", number_t(123.456, 2));
    show_status(name);

    // Push double
    Serial.println("Pushting the double value... ");
    name = Database.push<number_t>(aClient, "/examples/Push/Await/double", number_t(1234.56789, 4));
    show_status(name);
}