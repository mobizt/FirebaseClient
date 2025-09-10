/**
 * The example to append the map value timestamp in the document using commit.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 * 
 * For Google REST API reference documentation, please visit
 * https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents/commit
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_FIRESTORE

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define FIREBASE_PROJECT_ID "PROJECT_ID"

void processData(AsyncResult &aResult);
void append_map_value_timestamp_async(Writes &writes);
void append_map_value_timestamp_async2(Writes &writes);
void append_map_value_timestamp_await(Writes &writes);

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

    if (app.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
        dataMillis = millis();
        counter++;

        // test_collection is the collection id, test_document is the document id.
        String documentPath = "test_collection/test_document_map_value_timestamp";

        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

        // Map value to append
        Values::MapValue mapV("name", Values::StringValue("value" + String(counter)));
        mapV.add("count", Values::StringValue(String(counter)));

        Values::MapValue mapV2("key" + String(counter), mapV);

        Document<Values::Value> updateDoc;
        updateDoc.setName(documentPath);
        updateDoc.add("myMap", Values::Value(mapV2));

        Writes writes(Write(DocumentMask("myMap.key" + String(counter)) /* updateMask */, updateDoc, Precondition() /* currentDocument precondition */));

        String fieldPath = "myMap.key" + String(counter) + ".timestamp";
        // See https://firebase.google.com/docs/firestore/reference/rest/v1/Write#servervalue
        FieldTransform::SetToServerValue setValue(FieldTransform::REQUEST_TIME);
        FieldTransform::FieldTransform fieldTransforms(fieldPath, setValue);
        DocumentTransform transform(documentPath, fieldTransforms);

        writes.add(Write(transform, Precondition() /* currentDocument precondition */));

        // All Writes, DocumentTransform and Values::xxxx objects can be printed on Serial port

        // You can set the content of write and writes objects directly with write.setContent("your content") and writes.setContent("your content")

        append_map_value_timestamp_async(writes);

        // append_map_value_timestamp_async2(writes);

        // append_map_value_timestamp_await(writes);
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

void append_map_value_timestamp_async(Writes &writes)
{
    Serial.println("Appending the map value timestamp in the document... ");

    // Async call with callback function.
    Docs.commit(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), writes, processData, "commitTask");
}

void append_map_value_timestamp_async2(Writes &writes)
{
    Serial.println("Appending the map value timestamp in the document... ");

    // Async call with AsyncResult for returning result.
    Docs.commit(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), writes, firestoreResult);
}

void append_map_value_timestamp_await(Writes &writes)
{
    Serial.println("Appending the map value timestamp in the document... ");

    // Sync call which waits until the payload was received.
    String payload = Docs.commit(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), writes);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}