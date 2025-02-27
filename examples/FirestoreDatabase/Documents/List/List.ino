

/**
 * ABOUT:
 *
 * The example to list the Firestore document.
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
 * Firestore::Documents::list(<AsyncClient>, <Firestore::Parent>, <collectionId>, <ListDocumentsOptions>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <Firestore::Parent> - The Firestore::Parent object included project Id and database Id in its constructor.
 * <collectionId> - The relative path of document collection.
 * <ListDocumentsOptions> - The ListDocumentsOptions object that provides the functions to set the pageSize, pageToken, orderBy, mask and showMissing options.
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
 *
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 *
 * The following are the ListDocumentsOptions member functions.
 *
 * ListDocumentsOptions::pageSize - Setting the maximum number of documents to return.
 * ListDocumentsOptions::pageToken - The nextPageToken value returned from a previous List request, if any.
 * ListDocumentsOptions::orderBy - The order to sort results by. For example: priority desc, name.
 * ListDocumentsOptions::mask - Setting the fields to return. If not set, returns all fields.
 * If a document has a field that is not present in this mask, that field will not be returned in the response.
 * ListDocumentsOptions::showMissing - Setting if the list should show missing documents.
 * A missing document is a document that does not exist but has sub-documents.
 */

#include <Arduino.h>
#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define FIREBASE_PROJECT_ID "PROJECT_ID"

void processData(AsyncResult &aResult);
void list_document_async(const String &collectionId, ListDocumentsOptions &listDocsOptions);
void list_document_async2(const String &collectionId, ListDocumentsOptions &listDocsOptions);
void list_document_await(const String &collectionId, ListDocumentsOptions &listDocsOptions);

SSL_CLIENT ssl_client;

// This uses built-in core WiFi/Ethernet for network connection.
// See examples/App/NetworkInterfaces for more network examples.
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;

Firestore::Documents Docs;

AsyncResult firestoreResult;

int counter = 0;

unsigned long dataMillis = 0;

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

    if (app.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
        dataMillis = millis();

        if (!taskCompleted)
        {
            taskCompleted = true;
            // Should run the Create_Documents.ino prior to test this example to create the documents in the collection Id at a0/b0/c0

            // a0 is the collection id, b0 is the document id in collection a0 and c0 is the collection id id in the document b0.
            String collectionId = "a0/b0/c0";

            // If the collection Id path contains space e.g. "a b/c d/e f"
            // It should encode the space as %20 then the collection Id will be "a%20b/c%20d/e%20f"

            ListDocumentsOptions listDocsOptions;
            listDocsOptions.pageSize(3).mask(DocumentMask("count")).showMissing(false);

            list_document_async(collectionId, listDocsOptions);

            // list_document_async2(collectionId, listDocsOptions);

            // list_document_await(collectionId, listDocsOptions);
        }
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

void list_document_async(const String &collectionId, ListDocumentsOptions &listDocsOptions)
{
    Serial.println("Listing the documents in a collection... ");

    // Async call with callback function.
    Docs.list(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, listDocsOptions, processData, "listTask");
}

void list_document_async2(const String &collectionId, ListDocumentsOptions &listDocsOptions)
{
    Serial.println("Listing the documents in a collection... ");

    // Async call with AsyncResult for returning result.
    Docs.list(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, listDocsOptions, processData, "listTask");
}

void list_document_await(const String &collectionId, ListDocumentsOptions &listDocsOptions)
{
    Serial.println("Listing the documents in a collection... ");

    // Sync call which waits until the payload was received.
    String payload = Docs.list(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, listDocsOptions);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}
