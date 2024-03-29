
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
 * RUN QUERY FUNCTIONS
 * ===================
 *
 * SYNTAXES:
 *
 * Docs.runQuery(<AsyncClient>, <Firestore::Parent>, <documentPath>, <QueryOptions>);
 * Docs.runQuery(<AsyncClient>, <Firestore::Parent>, <documentPath>, <QueryOptions>, <AsyncResult>);
 * Docs.runQuery(<AsyncClient>, <Firestore::Parent>, <documentPath>, <QueryOptions>, <AsyncResultCallback>, <uid>);
 *
 * The <Firestore::Parent> is the Firestore::Parent object included project Id and database Id in its constructor.
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 *
 * The <QueryOptions> is the QueryOptions object that provides the function to create the query (StructuredQuery) and consistency mode which included
 * structuredQuery, transaction, newTransaction and readTime functions.
 *
 * The following function used for creating the union field consistency_selector and can be only one of the following field e.g.
 * transaction, newTransaction and readTime  functions.
 *
 * Then the following functions can't be mixed used.
 * - transaction used for running the query within an already active transaction. A base64-encoded string.
 * - newTransaction used for starting a new transaction and reads the documents. Defaults to a read-only transaction.
 * The new transaction ID will be returned as the first response in the stream.
 * - readTime used for reading the documents as they were at the given time.
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
 *
 * THE STATIC ASYNC RESULT REQUIRED FOR ASYNC OPERATION
 * ====================================================
 *
 * Library provided the class object called AsyncResult that keeps the server response data, debug and error information.
 *
 * There are two sources of async result in this library:
 *
 * 1. From user provided with async application (function).
 *
 * For example:
 *
 * `Database.get(aClient, "/test/int", options, aResult);`
 *
 * 2. From dynamic allocation in the async client.
 *
 * For example:
 *
 * `Database.get(aClient, "/test/int", options, asyncCB);`
 *
 * From source 1, the async result (`aResult`) shall be defined globally to use in async application because of the static data is needed for use while running the sync task.
 *
 * From source 2, the async client (`aClient`) shall be defined globally to use in async application too to make sure the instance of async result was existed or valid while running the sync task.
 *
 * If async result was destroyed (destructed or not existed) before it was used by async task handler, the danglig pointer problem will be occurred.
 *
 * Note that, the async client object used in authentication task shoul be defined globally as it is async task.
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

        Serial.println("Query a Firestore database... ");

        StructuredQuery query;

        Projection projection(FieldReference("myDouble"));
        projection.add(FieldReference("myInteger"));
        projection.add(FieldReference("myTimestamp"));
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

        Docs.runQuery(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, queryOptions, asyncCB);
        queryOptions.clear();

        // Note that in this case the document path is the root ("/") because the collection a0 is at the root.

        // To assign UID for async result
        // Docs.runQuery(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, queryOptions, asyncCB, "myUID");

        // To get anyc result without callback
        // Docs.runQuery(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, queryOptions, aResult_no_callback);
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