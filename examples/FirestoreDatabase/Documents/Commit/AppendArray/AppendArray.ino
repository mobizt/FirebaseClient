/**
 * The example to append an array in Firestore documents using commit.
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
void append_array_async();
void append_array_async2();
void append_array_await();

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
        String documentPath = "test_collection/test_document";
        String fieldPath = "appended_data";

        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

        // array value to append
        Values::ArrayValue arrV(Values::IntegerValue((int)rand()));
        arrV.add(Values::StringValue("word don't come easy " + String(counter)));

        FieldTransform::AppendMissingElements<Values::ArrayValue> appendValue(arrV);
        FieldTransform::FieldTransform fieldTransforms(fieldPath, appendValue);
        DocumentTransform transform(documentPath, fieldTransforms);

        Writes writes(Write(transform, Precondition() /* currentDocument precondition */));

        // Another array value to append
        Values::ArrayValue arrV2(Values::DoubleValue((int)rand() * 1.234));
        arrV2.add(Values::StringValue("never gonna give you up " + String(counter)));
        // Another append array object
        FieldTransform::AppendMissingElements<Values::ArrayValue> appendValue2(arrV2);
        // Another field transform
        FieldTransform::FieldTransform fieldTransforms2(fieldPath, appendValue2);
        // Another doc transform
        DocumentTransform transform2(documentPath, fieldTransforms2);
        // Add another Write object of another transform to the Writes object
        writes.add(Write(transform2, Precondition()));

        // All Writes, Write, DocumentTransform FieldTransform::xxx, and Values::xxxx objects can be printed on Serial port

        // You can set the content of write and writes objects directly with write.setContent("your content") and writes.setContent("your content")

        append_array_async(writes);

        // append_array_async2(writes);

        // append_array_await(writes);
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

void append_array_async(Writes &writes)
{
    Serial.println("Appending an array... ");

    // Async call with callback function.
    Docs.commit(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), writes, processData, "commitTask");
}

void append_array_async2(Writes &writes)
{
    Serial.println("Appending an array... ");

    // Async call with AsyncResult for returning result.
    Docs.commit(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), writes, firestoreResult);
}

void append_array_await(Writes &writes)
{
    Serial.println("Appending an array... ");

    // Sync call which waits until the payload was received.
    String payload = Docs.commit(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), writes);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}