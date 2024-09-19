
/**
 * SYNTAX:
 *
 * Firestore::Documents::runQuery(<AsyncClient>, <Firestore::Parent>, <documentPath>, <QueryOptions>, <AsyncResult>);
 *
 * <AsyncClient> - The async client.
 * <Firestore::Parent> - The Firestore::Parent object included project Id and database Id in its constructor.
 * <documentPath> - The relative path of document to patch with the input document.
 * <QueryOptions> - The QueryOptions object that provides the function to create the query (StructuredQuery) and consistency mode which included
 * structuredQuery, transaction, newTransaction and readTime functions.
 * <AsyncResult> - The async result (AsyncResult).
 *
 * The following are the QueryOptions member functions applied for uinion field consistency_selector.
 *
 * QueryOptions::transaction used for running the query within an already active transaction. A base64-encoded string.
 * QueryOptions::newTransaction used for starting a new transaction and reads the documents. Defaults to a read-only transaction.
 * The new transaction ID will be returned as the first response in the stream.
 * QueryOptions::readTime used for reading the documents as they were at the given time.
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>) || defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#elif __has_include(<WiFiC3.h>) || defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif __has_include(<WiFi.h>)
#include <WiFi.h>
#endif

#include <FirebaseClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

#define FIREBASE_PROJECT_ID "PROJECT_ID"

void printResult(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA) || defined(ARDUINO_PORTENTA_C33) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

Firestore::Documents Docs;

AsyncResult aResult_no_callback;

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

    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);

    // Binding the FirebaseApp for authentication handler.
    // To unbind, use Docs.resetApp();
    app.getApp<Firestore::Documents>(Docs);
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    app.loop();

    Docs.loop();

    if (app.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
        dataMillis = millis();

        Serial.println("Query a Firestore database... ");

        StructuredQuery query;

        Projection projection(FieldReference("myDouble"));
        // This change in v1.0.7 from add to fields to correspond to the Firestore documentation.
        projection.fields(FieldReference("myInteger"));
        projection.fields(FieldReference("myTimestamp"));
        query.select(projection);
        // Or shorter form
        // query.select(Projection(FieldReference("myDouble")).add(FieldReference("myInteger")).add(FieldReference("myTimestamp")));

        query.from(CollectionSelector("a0", false));

        Order order;
        order.field(FieldReference("myInteger"));
        order.direction(FilterSort::DESCENDING);
        query.orderBy(order);
        // Or shorter form
        // query.orderBy(Order(FieldReference("myInteger"), FilterSort::DESCENDING));

        query.limit(3);

        QueryOptions queryOptions;
        queryOptions.structuredQuery(query);

        query.clear();
        projection.clear();
        order.clear();

        String documentPath = "/"; // Query from all collections under root

        // You can set the content of queryOptions object directly with queryOptions.setContent("your content")

        Docs.runQuery(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, queryOptions, aResult_no_callback);
        queryOptions.clear();

        // Note that in this case the document path is the root ("/") because the collection a0 is at the root.
    }

    printResult(aResult_no_callback);
}

void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
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