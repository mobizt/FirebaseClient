
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
 * BATCH WRITE DOCUMENTS FUNCTIONS
 * ===============================
 *
 * SYNTAXES:
 *
 * firestore.batchWriteDocuments(<AsyncClient>, <ParentResource>, <Writes>);
 * firestore.batchWriteDocuments(<AsyncClient>, <ParentResource>, <Writes>, <AsyncResult>);
 * firestore.batchWriteDocuments(<AsyncClient>, <ParentResource>, <Writes>, <AsyncResultCallback>, <uid>);
 *
 * The <ParentResource> is the ParentResource object included project Id and database Id in its constructor.
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 *
 * The <Writes> is the writes to apply.
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

/**
 * This information can be taken from the service account JSON file.
 *
 * To download service account file, from the Firebase console, goto project settings,
 * select "Service accounts" tab and click at "Generate new private key" button
 */
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

void timeStatusCB(uint32_t &ts);

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

void fileCallback(File &file, const char *filename, file_operating_mode mode);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

// ServiceAuth is required for batch write.
ServiceAuth sa_auth(timeStatusCB, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

// FileConfig sa_file("/sa_file.json", fileCallback);

// ServiceAuth sa_file_auth(timeStatusCB, getFile(sa_file));

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

    initializeApp(aClient, app, getAuth(sa_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // This JWT token process required for ServiceAuth and CustomAuth authentications
        if (app.isJWT())
            JWT.process(app.getAuth());
    }

    app.getApp<Firestore>(firestore);
}

void loop()
{
    // This JWT token process required for ServiceAuth and CustomAuth authentications
    if (app.isJWT())
        JWT.process(app.getAuth());

    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // This required when different AsyncClients than used in FirebaseApp assigned to the firestore functions.
    firestore.loop();

    // To get anyc result without callback
    // printResult(aResult_no_callback);

    if (app.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
        dataMillis = millis();
        counter++;

        Serial.println("[+] Batch write documents... ");

        String documentPath = "test_collection/test_document_map_value";

        Values::MapValue mapV("name", Values::StringValue("value" + String(counter)));
        mapV.add("count", Values::StringValue(String(counter)));

        Values::MapValue mapV2("key" + String(counter), mapV);

        Document<Values::Value> updateDoc;
        updateDoc.setName(documentPath);
        updateDoc.add("myMap", Values::Value(mapV2));

        Values::MapValue labels;

        Writes writes(Write(DocumentMask("myMap.key" + String(counter)) /* updateMask */, updateDoc, Precondition() /* currentDocument precondition */), Values::MapValue() /* Labels */);

        String documentPath2 = "test_collection/test_document_timestamp";
        String fieldPath = "myTime";
        FieldTransform::SetToServerValue setValue(FieldTransform::REQUEST_TIME);
        FieldTransform::FieldTransform fieldTransforms(fieldPath, setValue);
        DocumentTransform transform(documentPath2, fieldTransforms);

        writes.add(Write(transform, Precondition() /* currentDocument precondition */));

        // Serial.println(writes);

        // All Writes, DocumentTransform and Values::xxxx objects can be printed on Serial port

        firestore.batchWriteDocuments(aClient, ParentResource(FIREBASE_PROJECT_ID), writes, asyncCB);

        // To assign UID for async result
        // firestore.batchWriteDocuments(aClient, ParentResource(FIREBASE_PROJECT_ID), writes, asyncCB, "myUID");

        // To get anyc result without callback
        // firestore.batchWriteDocuments(aClient, ParentResource(FIREBASE_PROJECT_ID), writes, aResult_no_callback);
    }
}

void timeStatusCB(uint32_t &ts)
{
    if (time(nullptr) < FIREBASE_DEFAULT_TS)
    {
        configTime(3 * 3600, 0, "pool.ntp.org");
        while (time(nullptr) < FIREBASE_DEFAULT_TS)
        {
            delay(100);
        }
    }

    ts = time(nullptr);
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
        Serial.printf("payload: %s\n", aResult.c_str());
    }
}
