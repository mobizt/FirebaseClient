
// Created by K. Suwatchai (Mobizt)
// Email: k_suwatchai@hotmail.com
// Github: https://github.com/mobizt/FirebaseClient
// Copyright (c) 2024 mobizt

/**
 * APP INITIALIZATION
 * ==================
 *
 * This library provides the APIs covers the different types of authentication and services.
 *
 * The FirebaseApp class was introduced as user authentication provider which provides the credentials data
 * used for accessing the Firebase services.
 *
 * The Database, Firestore, Functions, Messaging, Storage and Cloud Storage apps provided the functions based on
 * the specific Firebase services which take the auth credentials data from FirebaseApp.
 *
 * Multiples instances of FirebaseApp (auth provider) and services apps (Database, Firestore...) can be available
 * which depends on the device memory.
 *
 * The FirebaseApp should be initialized first before use via static function initializeApp.
 *
 * SYNTAXES:
 *
 * initializeApp(<AsyncClient>, <FirebaseApp>, <user_auth_data>);
 *
 * The AsyncClient stores the user, server and network data which can be initialized as.
 *
 * WiFiClientSecure ssl_client;
 * DefaultNetwork network;
 * AsyncClient aClient(ssl_client, getNetwork(network));
 *
 * The FirebaseApp manage and stores the authentication data;
 *
 * The user_auth_data store the user credentials which can be obtained from one of following Auth classes.
 * UserAuth, ServiceAuth, CustomAuth, IDToken, AccessToken, CustomToken, LegacyToken and NoAuth via static function getAuth.
 *
 * Please see examples/App/AppInitialization for usage guidelines.
 *
 * To use other network interfaces, network data from one of the following Network classes
 * can be assigned.
 *
 * DefaultNetwork, GenericNetwork, EthernetNetwork and GSMNetwork.
 *
 * The network data (network_config_data) can be obtained from these Network classes via static function getNetwork.
 *
 * Please see examples/App/NetworkInterfaces for the usage guidelines.
 *
 * The auth credential data can be set to Services App e.g. Database to initialize via function getApp.
 *
 * SYNTAX:
 *
 * app.getApp<T>(<services_app>);
 *
 * Deallocation of FirebaseApp causes these services apps uninitialized and cannot be used.
 *
 */

/**
 * DELETE DOCUMENT FUNCTIONS
 * =========================
 *
 * SYNTAXES:
 *
 * firestore.deleteDocument(<AsyncClient>, <ParentResource>, <documentPath>, <Precondition>);
 * firestore.deleteDocument(<AsyncClient>, <ParentResource>, <documentPath>, <Precondition>, <AsyncResult>);
 * firestore.deleteDocument(<AsyncClient>, <ParentResource>, <documentPath>, <Precondition>, <AsyncResultCallback>, <uid>);
 *
 * The <ParentResource> is the ParentResource object included project Id and database Id in its constructor.
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 *
 * The <Precondition> is the Precondition object for an optional precondition on the document.
 * The request will fail if this is set and not met by the target document.
 *
 * There are the member functions exists and updateTime for creating the union field exists and updateTime respectively.
 *
 * The exists option, when set to true, the target document must exist. When set to false, the target document must not exist.
 * The updateTime (timestamp) option, when set, the target document must exist and have been last updated at that time.
 * A timestamp is in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
 * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
 *
 * The async functions required AsyncResult or AsyncResultCallback function that keeping the result.
 *
 * The uid is user specified UID of async result (optional) which used as async task identifier.
 *
 * The uid can later get from AsyncResult object of AsyncResultCallback function via aResult.uid().
 */

/**
 * The AsyncResult object in the AsyncResultCallback function provides the result of async and non-async operations.
 *
 * APP EVENT
 * =========
 *
 * The event information can be obtained from aResult.appEvent().code() and aResult.appEvent().message() respectively.
 *
 * The following event code (firebase_auth_event_type), auth_event_uninitialized, auth_event_initializing, auth_event_signup, auth_event_send_verify_email,
 * auth_event_delete_user, auth_event_reset_password, auth_event_token_signing, auth_event_authenticating, auth_event_auth_request_sent
 * auth_event_auth_response_received, auth_event_ready and auth_event_error are available.
 *
 *
 * The following event strings "undefined", "initializing", "sign up", "send verification email", "delete user", "reset password",
 * "token signing", "authenticating", "auth request sent", "auth response received", "ready" and "error" are available.
 *
 * RESULT DATA
 * ===========
 *
 * The result data can be obtained from AsyncResult object via aResult.payload(), aResult.available(), aResult.path(), aResult.etag()
 * aResult.database.isStream(), aResult.database.event(), aResult.database.dataPath(), aResult.database.type() and result.database.name().
 *
 * The function aResult.payload() returns server serponse payload.
 *
 * The function aResult.available() returns the size of data that is ready to read.
 *
 * The function aResult.path() returns the resource path that the request was sent.
 *
 * The function aResult.etag() returns the ETag from server response header.
 *
 * The function aResult.database.name() returns the name (random UID) of node that will be creaated after calling push.
 *
 * The function aResult.database.type() returns the following database data type enum.
 * database_data_type_undefined (-1), database_data_type_null (0), database_data_type_integer(1), database_data_type_float (2),
 * database_data_type_double (3), database_data_type_boolean (4), database_data_type_string (5), database_data_type_json (6),
 * and database_data_type_array (7).
 *
 * The aResult.database.dataPath() and aResult.database.event() are the database node path that data has changed and type of event in server-sent events (stream) mode.
 *
 * The server response payload in AsyncResult can be converted to the the values e.g. boolean, integer,
 * float, double and string via aResult.database.to<T>() or result.database.to<T>().
 *
 * ASYNC QUEUE
 * ===========
 *
 * Each sync and async requests data consume memory up to 1k. When many async operations are added to queue (FIFO), the memory usage was increased.
 *
 * Each AsyncClient handles this queue separately. Then in order to limit the memory used for each AsyncClient,
 * this library allows 3-5 async operations (called slots) can be stored in the queue at the same time.
 *
 * If the authentication async operation was required, it will insert to the first slot of the queue.
 *
 * If the sync operation was called, it will insert to the first slot in the queue too but after the authentication task slot.
 *
 * When async Get operation in SSE mode (HTTP Streaming) was currently stored in queue, the new sync and async operations will be inserted before
 * the async SSE (HTTP Streaming) slot.
 *
 * When the async operation queue is full, the new sync and async operations will be cancelled.
 *
 * The finished and time out operating slot will be removed from the queue unless the async SSE and allow the vacant slot for the new async operation.
 *
 * The async SSE operation will run continuously and repeatedly as long as the FirebaseApp and the services app
 * (Database, Firestore, Messaging, Functions, Storage and CloudStorage) objects was run in the loop via app.loop() or database.loop().
 *
 * STOP QUEUE
 * ===========
 *
 * SYNTAX:
 *
 * The async operation will be cancelled and remove from the queue by calling thes functions.
 *
 * asyncClient.stopAsync() - stop the last async operation in the queue.
 *
 * asyncClient.stopAsync(true) - stop all async operation in the queue.
 *
 * asyncClient.stopAsync("xxxx") - stop the async operation in the queue that has the async result UID xxxx.
 *
 * ASYNC CLIENT
 * ============
 *
 * The async client stores the async operating data in its queue and holds the pointer of SSL Client that assigned with it.
 *
 * The SSL Client should be existed in the AsyncClient usage scope in case of sync or should defined as global object for async usage.
 *
 * The SSL Client should not be shared among various AsyncClients because of interferences in async operations.
 *
 * Only one SSL Client should be assigned to or used with only one AsyncClient.
 *
 * SSL Client can force to stop by user or calling stop() from asyncClient.
 *
 * SYNTAX:
 *
 * asyncClient.stop() - stop the SSL Client to terminate the server connection.
 *
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

#if defined(ESP8266) || defined(ESP32)
#include <WiFiClientSecure.h>
#endif

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

WiFiClientSecure ssl_client;

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

Firestore firestore;

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

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<Firestore>(firestore);
}

void loop()
{
    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // This required when different AsyncClients than used in FirebaseApp assigned to the firestore functions.
    firestore.loop();

    // To get anyc result without callback
    // printResult(aResult_no_callback);

    if (app.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
        dataMillis = millis();

        if (!taskCompleted)
        {
            taskCompleted = true;

            // aa is the collection id, bb is the document id in collection aa.
            String documentPath = "aa/bb";

            Serial.println("[+] Create a document... ");

            Document doc("myDouble", Values::Value(Values::DoubleValue(123.456)));

            firestore.createDocument(aClient, ParentResource(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc, asyncCB);

            Serial.println("[+] Delete a document... ");

            firestore.deleteDocument(aClient, ParentResource(FIREBASE_PROJECT_ID), documentPath, Precondition() /* Precondition (currentocument) */, asyncCB);

            // To assign UID for async result
            // firestore.deleteDocument(aClient, ParentResource(FIREBASE_PROJECT_ID), documentPath, Precondition() /* Precondition (currentocument) */, asyncCB, "myUID");

            // To get anyc result without callback
            // firestore.deleteDocument(aClient, ParentResource(FIREBASE_PROJECT_ID), documentPath, Precondition() /* Precondition (currentocument) */, aResult_no_callback);
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
        Serial.println("**************");
        Serial.printf("Event msg: %s, code: %d\n", aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isError())
    {
        Serial.println("**************");
        Serial.printf("Error msg: %s, code: %d\n", aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.isDebug())
    {
        Serial.println("**************");
        Serial.printf("Debug msg: %s\n", aResult.debug().c_str());
    }

    if (aResult.available())
    {
        Serial.println("**************");
        Serial.printf("payload: %s\n", aResult.payload().c_str());
    }
}
