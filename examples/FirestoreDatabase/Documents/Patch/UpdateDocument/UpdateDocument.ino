/**
 * The example to update a Firestore document using patch.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 * 
 * For Google REST API reference documentation, please visit
 * https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents/patch
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
void create_document_await(const String &documentPath, Document<Values::Value> &doc);
void append_map_value_async(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc);
void append_map_value_async2(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc);
void append_map_value_await(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc);

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;

Firestore::Documents Docs;

AsyncResult firestoreResult;

int cnt = 0;

unsigned long dataMillis = 0;

bool taskcomplete = false;

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

        // aa is the collection id, bb is the document id.
        String documentPath = "aa/bb";

        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

        if (!taskcomplete)
        {
            taskcomplete = true;

            Values::IntegerValue intV(cnt);
            Values::BooleanValue boolV(cnt % 2 == 0);

            Document<Values::Value> doc("count", Values::Value(intV));
            doc.add("status", Values::Value(boolV));

            // The value of Values::xxxValue, Values::Value and Document can be printed on Serial.

            create_document_await(documentPath, doc);
        }

        cnt++;
        Values::IntegerValue intV(cnt);
        Values::BooleanValue boolV(cnt % 2 == 0);

        Document<Values::Value> doc("count", Values::Value(intV));
        doc.add("status", Values::Value(boolV));

        // The value of Values::xxxValue, Values::Value and Document can be printed on Serial.

        /** if updateMask contains the field name that exists in the remote document and
         * this field name does not exist in the document (content), that field will be deleted from remote document
         */

        PatchDocumentOptions patchOptions(DocumentMask("count,status") /* updateMask */, DocumentMask() /* mask */, Precondition() /* precondition */);

        // You can set the content of doc object directly with doc.setContent("your content")

        update_document_async(documentPath, patchOptions, doc);

        // update_document_async2(documentPath, patchOptions, doc);

        // update_document_await(documentPath, patchOptions, doc);
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

void update_document_async(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc)
{
    Serial.println("Updating a document... ");

    // Async call with callback function.
    Docs.patch(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, patchOptions, doc, processData, "patchTask");
}

void update_document_async2(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc)
{
    Serial.println("Updating a document... ");

    // Async call with AsyncResult for returning result.
    Docs.patch(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, patchOptions, doc, firestoreResult);
}

void update_document_await(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc)
{
    Serial.println("Updating a document... ");

    // Sync call which waits until the payload was received.
    String payload = Docs.patch(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, patchOptions, doc);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}
