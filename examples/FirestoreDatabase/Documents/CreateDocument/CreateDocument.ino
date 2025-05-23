
/**
 * ABOUT:
 *
 * The example to create the Firestore document.
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
 * Firestore::Documents::createDocument(<AsyncClient>, <Firestore::Parent>, <documentPath>, <DocumentMask>, <Document>, <AsyncResultCallback>, <uid>);
 *
 * Firestore::Documents::createDocument(<AsyncClient>, <Firestore::Parent>, <collectionId>, <documentId>, <DocumentMask>, <Document>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <Firestore::Parent> - The Firestore::Parent object included project Id and database Id in its constructor.
 * <documentPath> - The relative path of document to create in the collection.
 * <DocumentMask> - The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
 * <collectionId> - The relative path of document collection id to create the document.
 * <documentId> - The document id of document to be created.
 * <Document> - The Firestore document.
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
 *
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 */

// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
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
void create_document_async(Document<Values::Value> &doc, const String &documentPath);
void create_document_async2(Document<Values::Value> &doc, const String &documentPath);
void create_document_await(Document<Values::Value> &doc, const String &documentPath);
String getTimestampString(uint64_t sec, uint32_t nano);

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;

Firestore::Documents Docs;

AsyncResult firestoreResult;

int data_count = 0;

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

        // We will create the documents in this parent path "test_doc_creation/doc_1/col_1/data_?"
        // (collection > document > collection > documents that contains fields).

        // Note: If new document created under non-existent ancestor documents as in this example
        // which the document "test_doc_creation/doc_1" does not exist, that document (doc_1) will not appear in queries and snapshot
        // https://cloud.google.com/firestore/docs/using-console#non-existent_ancestor_documents.

        // In the console, you can create the ancestor document "test_doc_creation/doc_1" before running this example
        // to avoid non-existent ancestor documents case.

        String documentPath = "test_doc_creation/doc_1/col_1/data_";

        documentPath += data_count;
        data_count++;

        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

        // double (obsoleted)
        // Values::DoubleValue dblV(1234.567891);

        // double value with precision.
        Values::DoubleValue dblV(number_t(1234.567891, 6));

        // boolean
        Values::BooleanValue bolV(true);

        // integer
        Values::IntegerValue intV(data_count);

        // null
        Values::NullValue nullV;

        String doc_path = "projects/";
        doc_path += FIREBASE_PROJECT_ID;
        doc_path += "/databases/(default)/documents/coll_id/doc_id"; // coll_id and doc_id are your collection id and document id

        // reference
        Values::ReferenceValue refV(doc_path);

        // timestamp
        Values::TimestampValue tsV(getTimestampString(1712674441, 999999999));

        // bytes
        Values::BytesValue bytesV("aGVsbG8=");

        // string
        Values::StringValue strV("hello");

        // array
        Values::ArrayValue arrV(Values::StringValue("test"));
        arrV.add(Values::IntegerValue(20)).add(Values::BooleanValue(true));

        // map
        Values::MapValue mapV("name", Values::StringValue("wrench"));
        mapV.add("mass", Values::StringValue("1.3kg")).add("count", Values::IntegerValue(3));

        // lat long (Obsoleated)
        // Values::GeoPointValue geoV(1.486284, 23.678198);

        // lat long with precision
        Values::GeoPointValue geoV(number_t(1.486284, 6), number_t(23.678198, 6));

        Document<Values::Value> doc("myDouble", Values::Value(dblV));
        doc.add("myBool", Values::Value(bolV)).add("myInt", Values::Value(intV)).add("myNull", Values::Value(nullV));
        doc.add("myRef", Values::Value(refV)).add("myTimestamp", Values::Value(tsV)).add("myBytes", Values::Value(bytesV));
        doc.add("myString", Values::Value(strV)).add("myArr", Values::Value(arrV)).add("myMap", Values::Value(mapV));
        doc.add("myGeo", Values::Value(geoV));

        // The value of Values::xxxValue, Values::Value and Document can be printed on Serial.

        create_document_async(doc, documentPath);

        // create_document_async2(doc, documentPath);

        // create_document_await(doc, documentPath);
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

void create_document_async(Document<Values::Value> &doc, const String &documentPath)
{
    Serial.println("Creating a document... ");

    // Async call with callback function.
    Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc, processData, "createDocumentTask");
}

void create_document_async2(Document<Values::Value> &doc, const String &documentPath)
{
    Serial.println("Creating a document... ");

    // Async call with AsyncResult for returning result.
    Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc, firestoreResult);
}

void create_document_await(Document<Values::Value> &doc, const String &documentPath)
{
    Serial.println("Creating a document... ");

    // Sync call which waits until the payload was received.
    String payload = Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}

String getTimestampString(uint64_t sec, uint32_t nano)
{
    if (sec > 0x3afff4417f)
        sec = 0x3afff4417f;

    if (nano > 0x3b9ac9ff)
        nano = 0x3b9ac9ff;

    time_t now;
    struct tm ts;
    char buf[80];
    now = sec;
    ts = *localtime(&now);

    String format = "%Y-%m-%dT%H:%M:%S";

    if (nano > 0)
    {
        String fraction = String(double(nano) / 1000000000.0f, 9);
        fraction.remove(0, 1);
        format += fraction;
    }
    format += "Z";

    strftime(buf, sizeof(buf), format.c_str(), &ts);
    return buf;
}