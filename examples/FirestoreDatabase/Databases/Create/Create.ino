/**
 * The example to create a Firestore database.
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The OAuth2.0 authentication or access token authorization is required for Firestore database operations.
 * 
 * For Google REST API reference documentation, please visit
 * https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases/create
 * https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases#Database
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_SERVICE_AUTH
#define ENABLE_FIRESTORE

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

void processData(AsyncResult &aResult);
void create_async(const String &databaseId, Firestore::Database &db);
void create_async2(const String &databaseId, Firestore::Database &db);
void create_await(const String &databaseId, Firestore::Database &db);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

Firestore::Databases Databases;

AsyncResult firestoreResult;

bool taskCompleted = false;

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

    app.getApp<Firestore::Databases>(Databases);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        Firestore::Database db;
        String databaseId = "myDb";
        db.concurrencyMode(Firestore::ConcurrencyMode::PESSIMISTIC);
        db.databaseType(Firestore::DatabaseType::FIRESTORE_NATIVE);
        db.name(databaseId);
        // See https://cloud.google.com/firestore/docs/locations
        db.locationId("us-east1");

        // You can set the content of db object directly with db.setContent("your content")

        create_async(databaseId, db);

        // create_async2(databaseId, db);

        // create_await(databaseId, db);
    }

    // For async call with AsyncResult.
    processData(firestoreResult);
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

void create_async(const String &databaseId, Firestore::Database &db)
{
    Serial.println("Creating a database... ");

    // Async call with callback function.
    Databases.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID, databaseId /* database Id */), db, processData, "createTask");
}

void create_async2(const String &databaseId, Firestore::Database &db)
{
    Serial.println("Creating a database... ");

    // Async call with AsyncResult for returning result.
    Databases.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID, databaseId /* database Id */), db, firestoreResult);
}

void create_await(const String &databaseId, Firestore::Database &db)
{
    Serial.println("Creating a database... ");

    // Sync call which waits until the payload was received.
    String payload = Databases.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID, databaseId /* database Id */), db);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}
