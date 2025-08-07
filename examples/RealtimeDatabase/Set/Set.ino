/**
 * The example to set/store value to the database.
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
void set_async();
void set_async2();
void set_await();

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
        Serial.println("🕒 Await set values");
        Serial.println("------------------------------");
        set_await();

        Serial.println("------------------------------");
        Serial.println("🎈 Async set values");
        Serial.println("------------------------------");
        set_async();
        set_async2();
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

void set_async()
{
    // Set the specific value (no waits)
    // Using Database.set with the callback function or AsyncResult object

    // Set int
    Serial.println("Setting the int value... ");

    Database.set<int>(aClient, "/examples/Set/Async1/int", 12345, processData, "setIntTask");
   
    // Set bool
    Serial.println("Setting the bool value... ");
    Database.set<bool>(aClient, "/examples/Set/Async1/bool", true, processData, "setBoolTask");
   
    // Set string
    Serial.println("Setting the String value... ");
    Database.set<String>(aClient, "/examples/Set/Async1/String", "hello", processData, "setStringTask");
   
    // Set json
    Serial.println("Setting the JSON value... ");
    Database.set<object_t>(aClient, "/examples/Set/Async1/JSON", object_t("{\"data\":123}"), processData, "setJsonTask");
   
   
    object_t json, obj1, obj2, obj3, obj4;
    JsonWriter writer;

    writer.create(obj1, "int/value", 9999);
    writer.create(obj2, "string/value", string_t("hello"));
    writer.create(obj3, "float/value", number_t(123.456, 2));
    writer.join(obj4, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);
    writer.create(json, "node/list", obj4);

    // To print object_t
    // Serial.println(json);
    Serial.println("Setting the JSON value... ");
    Database.set<object_t>(aClient, "/examples/Set/Async1/JSON", json, processData, "setJsonTask");
    
    object_t arr;
    arr.initArray(); // initialize to be used as array
    writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

    // Note that value that sets to object_t other than JSON ({}) and Array ([]) can be valid only if it
    // used as array member value as above i.e. object_t(string_t("steve")) and object_t(888).

    // Set array
    Serial.println("Setting the Array value... ");
    Database.set<object_t>(aClient, "/examples/Set/Async1/Array", arr, processData, "setArrayTask");
    
    // Set float
    Serial.println("Setting the float value... ");
    Database.set<number_t>(aClient, "/examples/Set/Async1/float", number_t(123.456, 2), processData, "setFloatTask");
   
    // Set double
    Serial.println("Setting the double value... ");
    Database.set<number_t>(aClient, "/examples/Set/Async1/double", number_t(1234.56789, 4), processData, "setDoubleTask");
}


void set_async2()
{
    // Set the specific value (no waits)
    // Using Database.set with the callback function or AsyncResult object

    // Set int
    Serial.println("Setting the int value... ");

    Database.set<int>(aClient, "/examples/Set/Async2/int", 12345, databaseResult);

    // Set bool
    Serial.println("Setting the bool value... ");
    Database.set<bool>(aClient, "/examples/Set/Async2/bool", true, databaseResult);

    // Set string
    Serial.println("Setting the String value... ");
    Database.set<String>(aClient, "/examples/Set/Async2/String", "hello", databaseResult);

    // Set json
    Serial.println("Setting the JSON value... ");
    Database.set<object_t>(aClient, "/examples/Set/Async2/JSON", object_t("{\"data\":123}"), databaseResult);

    object_t json, obj1, obj2, obj3, obj4;
    JsonWriter writer;

    writer.create(obj1, "int/value", 9999);
    writer.create(obj2, "string/value", string_t("hello"));
    writer.create(obj3, "float/value", number_t(123.456, 2));
    writer.join(obj4, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);
    writer.create(json, "node/list", obj4);

    // To print object_t
    // Serial.println(json);
    Serial.println("Setting the JSON value... ");
    Database.set<object_t>(aClient, "/examples/Set/Async2/JSON", json, databaseResult);

    object_t arr;
    arr.initArray(); // initialize to be used as array
    writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

    // Note that value that sets to object_t other than JSON ({}) and Array ([]) can be valid only if it
    // used as array member value as above i.e. object_t(string_t("steve")) and object_t(888).

    // Set array
    Serial.println("Setting the Array value... ");
    Database.set<object_t>(aClient, "/examples/Set/Async2/Array", arr, databaseResult);

    // Set float
    Serial.println("Setting the float value... ");
    Database.set<number_t>(aClient, "/examples/Set/Async2/float", number_t(123.456, 2), databaseResult);

    // Set double
    Serial.println("Setting the double value... ");
    Database.set<number_t>(aClient, "/examples/Set/Async2/double", number_t(1234.56789, 4), databaseResult);
}

void show_status(bool status)
{
    if (status)
        Serial.println("Success");
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}

void set_await()
{
    // Set the specific value (waits until the value was sucessfully set)
    // Using Database.set<T>

    // Set int
    Serial.println("Setting the int value... ");
    bool status = Database.set<int>(aClient, "/examples/Set/Await/int", 12345);
    show_status(status);

    // Set bool
    Serial.println("Setting the bool value... ");
    status = Database.set<bool>(aClient, "/examples/Set/Await/bool", true);
    show_status(status);

    // Set string
    Serial.println("Setting the String value... ");
    status = Database.set<String>(aClient, "/examples/Set/Await/String", "hello");
    show_status(status);

    // Set json
    Serial.println("Setting the JSON value... ");
    status = Database.set<object_t>(aClient, "/examples/Set/Await/JSON", object_t("{\"data\":123}"));
    show_status(status);

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

    Serial.println("Setting the JSON value... ");
    status = Database.set<object_t>(aClient, "/examples/Set/Await/JSON", json);
    show_status(status);

    object_t arr;
    arr.initArray(); // initialize to be used as array
    writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

    // Note that value that sets to object_t other than JSON ({}) and Array ([]) can be valid only if it
    // used as array member value as above i.e. object_t(string_t("steve")) and object_t(888).

    // Set array
    Serial.println("Setting the Array value... ");
    status = Database.set<object_t>(aClient, "/examples/Set/Await/Array", arr);
    show_status(status);

    // Set float
    Serial.println("Setting the float value... ");
    status = Database.set<number_t>(aClient, "/examples/Set/Await/float", number_t(123.456, 2));
    show_status(status);

    // Set double
    Serial.println("Setting the double value... ");
    status = Database.set<number_t>(aClient, "/examples/Set/Await/double", number_t(1234.56789, 4));
    show_status(status);
}