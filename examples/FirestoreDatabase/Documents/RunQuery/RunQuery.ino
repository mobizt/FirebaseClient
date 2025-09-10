
/**
 * The example to query a Firestore database using run query.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 * 
 * For Google REST API reference documentation, please visit
 * https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents/runQuery
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_FIRESTORE
#define ENABLE_FIRESTORE_QUERY

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define FIREBASE_PROJECT_ID "PROJECT_ID"

void processData(AsyncResult &aResult);
void query_async(const String &documentPath, QueryOptions &queryOptions);
void query_async2(const String &documentPath, QueryOptions &queryOptions);
void query_await(const String &documentPath, QueryOptions &queryOptions);

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;

Firestore::Documents Docs;

AsyncResult firestoreResult;

int counter = 0;

unsigned long dataMillis = 0;

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

    app.getApp<Firestore::Documents>(Docs);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && (millis() - dataMillis > 20000 || dataMillis == 0))
    {
        dataMillis = millis();

        // You can run the CreateDocument.ino example to create the data before querying.

        StructuredQuery query;

        Projection projection(FieldReference("myDouble"));
        projection.fields(FieldReference("myBool"));
        projection.fields(FieldReference("myInt"));
        projection.fields(FieldReference("myTimestamp"));
        projection.fields(FieldReference("myBytes"));
        projection.fields(FieldReference("myString"));
        projection.fields(FieldReference("myArr"));
        projection.fields(FieldReference("myMap"));
        projection.fields(FieldReference("myGeo"));
        query.select(projection);

        // Or shorter form
        // query.select(Projection(FieldReference("myDouble")).add(FieldReference("myBool")).add(FieldReference("myInt")));

        query.from(CollectionSelector("col_1", false));

        // This ordering requires index
        // Order order;
        // order.field(FieldReference("myInt"));
        // order.direction(FilterSort::DESCENDING);
        // query.orderBy(order);
        // Or shorter form
        // query.orderBy(Order(FieldReference("myInt"), FilterSort::DESCENDING));

        FieldFilter fieldFilter;
        Values::StringValue stringValue("hello");
        Values::Value val(stringValue);
        fieldFilter.field(FieldReference("myString")).op(FieldFilterOperator::EQUAL).value(val);
        Filter filter(fieldFilter);
        query.where(filter);

        query.limit(4);

        QueryOptions queryOptions;
        queryOptions.structuredQuery(query);

        query.clear();

        String documentPath = "test_doc_creation/doc_1";

        // You can set the content of queryOptions object directly with queryOptions.setContent("your content")

        query_async(documentPath, queryOptions);

        // query_async2(documentPath, queryOptions);

        // query_await(documentPath, queryOptions);

        queryOptions.clear();
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
        // The information printed from Firebase.printf may be truncated because of limited buffer memory to reduce the stack usage,
        // use Serial.println(aResult.c_str()) to print entire content.
    }
}

void query_async(const String &documentPath, QueryOptions &queryOptions)
{
    Serial.println("Querying a Firestore database... ");

    // Async call with callback function.
    Docs.runQuery(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, queryOptions, processData, "runQueryTask");
}

void query_async2(const String &documentPath, QueryOptions &queryOptions)
{
    Serial.println("Querying a Firestore database... ");

    // Async call with AsyncResult for returning result.
    Docs.runQuery(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, queryOptions, firestoreResult);
}

void query_await(const String &documentPath, QueryOptions &queryOptions)
{
    Serial.println("Querying a Firestore database... ");

    // Sync call which waits until the payload was received.
    String payload = Docs.runQuery(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, queryOptions);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}