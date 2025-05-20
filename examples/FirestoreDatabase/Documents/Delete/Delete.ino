/**
 * ABOUT:
 *
 * The example to delete the Firestore document.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * Firestore::Documents::deleteDoc(<AsyncClient>, <Firestore::Parent>, <documentPath>, <Precondition>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <Firestore::Parent> - The Firestore::Parent object included project Id and database Id in its constructor.
 * <documentPath> - The relative path of document to delete in the collection.
 * <Precondition> - The Precondition object for an optional precondition on the document and provides the functions to set the exists and updateTime.
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
 *
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 *
 * The following are the Precondition member functions.
 *
 * Precondition::exists - When set to true, the target document must exist. When set to false, the target document must not exist.
 * Precondition::updateTime - When set, the target document must exist and have been last updated at that time.
 *
 * A timestamp is in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
 * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
 *
 * The request will fail if Precondition is set and not met by the target document.
 */

// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
#define ENABLE_USER_CONFIG
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
void create_document_await(const String &documentPath, Document<Values::Value> &doc);
void delete_document_async(const String &documentPath);
void delete_document_async2(const String &documentPath);
void delete_document_aswait(const String &documentPath);

SSL_CLIENT ssl_client;

// This uses built-in core WiFi/Ethernet for network connection.
// See examples/App/NetworkInterfaces for more network examples.
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;

Firestore::Documents Docs;

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

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        // collection id > document id.
        String documentPath = "test_doc_deletion/my_doc";

        Document<Values::Value> doc("myDouble", Values::Value(Values::DoubleValue(123.456)));

        create_document_await(documentPath, doc);

        delete_document_async(documentPath);

        // delete_document_async2(documentPath);

        // delete_document_await(documentPath);
    }

    // For async call with AsyncResult.
    processData(firestoreResult);
}

void processData(AsyncResult &aResult)
{
    // Exits when no result available when calling from the loop.
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

void create_document_await(const String &documentPath, Document<Values::Value> &doc)
{
    Serial.println("Creating a document... ");

    // Sync call which waits until the payload was received.
    String payload = Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc);

    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}

void delete_document_async(const String &documentPath)
{
    Serial.println("Deleting a document... ");

    // Async call with callback function.
    Docs.deleteDoc(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, Precondition() /* Precondition (currentocument) */, processData, "deleteDocTask");
}

void delete_document_async2(const String &documentPath)
{
    Serial.println("Deleting a document... ");

    // Async call with AsyncResult for returning result.
    Docs.deleteDoc(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, Precondition() /* Precondition (currentocument) */, firestoreResult);
}

void delete_document_await(const String &documentPath)
{
    Serial.println("Deleting a document... ");

    // Sync call which waits until the payload was received.
    String payload = Docs.deleteDoc(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, Precondition() /* Precondition (currentocument) */);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}
