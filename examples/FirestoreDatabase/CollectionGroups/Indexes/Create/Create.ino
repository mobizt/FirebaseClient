/**
 * The example to create a Firestore composite index.
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The OAuth2.0 authentication or access token authorization is required for indexing operations.
 * 
 * For Google REST API reference documentation, please visit
 * https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/create
 * https://firebase.google.com/docs/firestore/reference/rest/Shared.Types/FieldOperationMetadata#Index
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
void create_index(const String &fieldPath1, const String &fieldPath2, CollectionGroupsIndex::Index &index);
void create_async(const String &collectionId, CollectionGroupsIndex::Index &index);
void create_async2(const String &collectionId, CollectionGroupsIndex::Index &index);
void create_await(const String &collectionId, CollectionGroupsIndex::Index &index);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

Firestore::CollectionGroups::Indexes indexes;

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

    app.getApp<Firestore::CollectionGroups::Indexes>(indexes);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        String collectionId = "a0", fieldPath1 = "myInt", fieldPath2 = "myDouble";
        CollectionGroupsIndex::Index index;

        create_index(fieldPath1, fieldPath2, index);

        create_async(collectionId, index);

        // create_async2(collectionId, index);

        // create_await(collectionId, index);
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

void create_index(const String &fieldPath1, const String &fieldPath2, CollectionGroupsIndex::Index &index)
{
    CollectionGroupsIndex::VectorConfig vectorConfig;
    vectorConfig.dimension(1);
    vectorConfig.flat();

    CollectionGroupsIndex::IndexField indexField;
    indexField.fieldPath(fieldPath1);

    // Union field value_mode can be only one of the following:
    indexField.order(CollectionGroupsIndex::Order::ASCENDING);
    // indexField.arrayConfig(CollectionGroupsIndex::ArrayConfig::CONTAINS);
    // indexField.vectorConfig(vectorConfig);

    CollectionGroupsIndex::IndexField indexField2;
    indexField2.fieldPath(fieldPath2);

    // Union field value_mode can be only one of the following:
    // indexField2.order(CollectionGroupsIndex::Order::DESCENDING);
    indexField2.arrayConfig(CollectionGroupsIndex::ArrayConfig::CONTAINS);
    // indexField2.vectorConfig(vectorConfig);

    index.apiScope(CollectionGroupsIndex::ApiScope::ANY_API);
    index.queryScope(CollectionGroupsIndex::QueryScope::COLLECTION);
    // This change in v1.0.7 from addField to fields to correspond to the Firestore documentation.
    index.fields(indexField);
    index.fields(indexField2);

    // You can set the content of index object directly with index.setContent("your content")
}

void create_async(const String &collectionId, CollectionGroupsIndex::Index &index)
{
    Serial.println("Creating a composite index... ");

    // Async call with callback function.
    indexes.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, index, processData, "createTask");
}

void create_async2(const String &collectionId, CollectionGroupsIndex::Index &index)
{
    Serial.println("Creating a composite index... ");

    // Async call with AsyncResult for returning result.
    indexes.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, index, firestoreResult);
}

void create_await(const String &collectionId, CollectionGroupsIndex::Index &index)
{
    Serial.println("Creating a composite index... ");

    // Sync call which waits until the payload was received.
    String payload = indexes.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, index);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}