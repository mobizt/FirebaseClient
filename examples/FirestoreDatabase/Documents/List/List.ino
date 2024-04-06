

/**
 * SYNTAXES:
 *
 * Docs.list(<AsyncClient>, <Firestore::Parent>, <documentPath>, <ListDocumentsOptions>);
 * Docs.list(<AsyncClient>, <Firestore::Parent>, <documentPath>, <ListDocumentsOptions>, <AsyncResult>);
 * Docs.list(<AsyncClient>, <Firestore::Parent>, <documentPath>, <ListDocumentsOptions>, <AsyncResultCallback>, <uid>);
 *
 * The <Firestore::Parent> is the Firestore::Parent object included project Id and database Id in its constructor.
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 *
 * The <ListDocumentsOptions> is the ListDocumentsOptions object that provides the member functions pageSize, pageToken, orderBy, mask and
 * showMissing for creating the query string options pageSize, pageToken, orderBy, mask and showMissing respectively.
 * The option pageSize is for setting the maximum number of documents to return.
 * The option pageToken is the nextPageToken value returned from a previous List request, if any.
 * The option orderBy is the order to sort results by. For example: priority desc, name.
 * The option mask is for setting the fields to return. If not set, returns all fields.
 * If a document has a field that is not present in this mask, that field will not be returned in the response.
 * The option showMissing is for setting if the list should show missing documents.
 * A missing document is a document that does not exist but has sub-documents.
 *
 * The async functions required AsyncResult or AsyncResultCallback function that keeping the result.
 *
 * The uid is user specified UID of async result (optional) which used as async task identifier.
 *
 * The uid can later get from AsyncResult object of AsyncResultCallback function via aResult.uid().
 * 
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
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

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

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
    unsigned long ms = millis();
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

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<Firestore::Documents>(Docs);
}

void loop()
{
    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // This required when different AsyncClients than used in FirebaseApp assigned to the Firestore functions.
    Docs.loop();

    // To get anyc result without callback
    // printResult(aResult_no_callback);

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

            Serial.println("List the documents in a collection... ");

            ListDocumentsOptions listDocsOptions;
            listDocsOptions.pageSize(3).mask(DocumentMask("count")).showMissing(false);

            Docs.list(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, listDocsOptions, asyncCB);

            // To assign UID for async result
            // Docs.list(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, listDocsOptions, asyncCB, "myUID");

            // To get anyc result without callback
            // Docs.list(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, listDocsOptions, aResult_no_callback);
        }
    }
}

void asyncCB(AsyncResult &aResult)
{

    // To get the UID (string) from async result
    // aResult.uid();

    printResult(aResult);
}

void printResult(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
    {
        Firebase.printf("Event msg: %s, code: %d\n", aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug msg: %s\n", aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error msg: %s, code: %d\n", aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("payload: %s\n", aResult.c_str());
    }
}
