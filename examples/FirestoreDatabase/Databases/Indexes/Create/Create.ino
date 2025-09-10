/**
 * The example to create the specified Firestore index.
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The OAuth2.0 authentication or access token authorization is required for indexes operations.
 * 
 * For Google REST API reference documentation, please visit
 * https://firebase.google.com/docs/firestore/reference/rest#rest-resource:-v1beta1.projects.databases.indexes
 * https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/create
 * https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes#Index
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
#define STORAGE_BUCKET_ID "BUCKET-NAME.appspot.com"

void processData(AsyncResult &aResult);
void create_async(DatabaseIndex::Index &index);
void create_async2(DatabaseIndex::Index &index);
void create_await(DatabaseIndex::Index &index);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

Firestore::Databases::Indexes indexes;

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

    app.getApp<Firestore::Databases::Indexes>(indexes);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        String collectionId = "a0", fieldPath1 = "myInt", fieldPath2 = "myDouble";

        DatabaseIndex::Index index(collectionId);
        DatabaseIndex::IndexField indexField1, indexField2;

        indexField1.fieldPath(fieldPath1);
        indexField1.mode(DatabaseIndex::IndexMode::ASCENDING);

        indexField2.fieldPath(fieldPath2);
        indexField2.mode(DatabaseIndex::IndexMode::ASCENDING);

        index.fields(indexField1);
        index.fields(indexField2);

        create_async(index);

        // create_async2(index);

        // create_await(index);
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

void create_async(DatabaseIndex::Index &index)
{
    Serial.println("Createing the specified index... ");

    // Async call with callback function.
    indexes.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), index, processData, "createTask");
}

void create_async(DatabaseIndex::Index &index)
{
    Serial.println("Createing the specified index... ");

    // Async call with AsyncResult for returning result.
    indexes.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), index, firestoreResult);
}

void create_await(DatabaseIndex::Index &index)
{
    Serial.println("Createing the specified index... ");

    // Sync call which waits until the payload was received.
    String payload = indexes.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), index);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}