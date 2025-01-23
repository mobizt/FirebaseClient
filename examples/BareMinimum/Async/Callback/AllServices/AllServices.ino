/**
 * ABOUT:
 *
 * The beare minimum code example for all Firebase services in async mode with callback function.
 *
 * The steps which are generally required and explained below.
 *
 * Step 1. Include the network, SSL client and Firebase libraries.
 * ===============================================================
 *
 * Step 2. Define the user functions that requred for the library usage.
 * =====================================================================
 *
 * Step 3. Define the network config (identifier) class.
 * =====================================================
 * Why this is existed and Is it used for?
 *
 * This library supports many types of network interfaces.
 * Then we have to know which type of network is currently inused and how to check the connection status
 * and how to re-connect the network.
 *
 * This reduces the user code to maintain the network connection and management and also provides
 * the networks switching or bridge usage.
 *
 * Step 4. Define the authentication config (identifier) class.
 * ============================================================
 * In the Firebase/Google Cloud services REST APIs, the auth tokens are used for authentication/authorization.
 *
 * The auth token is a short-lived token that will be expired in 60 minutes and need to be refreshed or re-created when it expired.
 *
 * There can be some special use case that some services provided the non-authentication usages e.g. using database secret
 * in Realtime Database, setting the security rules in Realtime Database, Firestore and Firebase Storage to allow public read/write access.
 *
 * The UserAuth (user authentication with email/password) is the basic authentication for Realtime Database,
 * Firebase Storage and Firestore services except for some Firestore services that involved with the Google Cloud services.
 *
 * It stores the email, password and API keys for authentication process.
 *
 * In Google Cloud services e.g. Cloud Storage and Cloud Functions, the higest authentication level is required and
 * the ServiceAuth class (OAuth2.0 authen) and AccessToken class will be use for this case.
 *
 * While the CustomAuth provides the same authentication level as user authentication unless it allows the custom UID, scopes and claims.
 *
 * The internal process of some authentication types e.g. ServiceAuth and CustomAuth, requires the signed JWT token generation process.
 * Because of it uses large memory and high cpu usage while signing the JWT token, this process requires another class to work
 * called JWT processor which defined as a static object for global usage to use Heap instead of stack memory.
 *
 * As the valid timestamp is required in JWT token signing process, the time status callback function is required
 * and assigned to the ServiceAuth and CustomAuth classes constructors.
 *
 * Step 5. Define the authentication handler class.
 * ================================================
 * The FirebaseApp actually works as authentication handler.
 * It also maintains the authentication or re-authentication when you place the FirebaseApp::loop() inside the main loop.
 *
 * Step 6. Define the SSL client.
 * ==============================
 * It handles server connection and data transfer.
 *
 * In this beare minimum examples we use only one SSL client for all process.
 * In some use cases e.g. Realtime Database Stream connection, you may have to define the SSL client for it separately.
 *
 * Step 7. Define the Async Client.
 * ================================
 * This is the class that is used with the functions where the server data transfer is involved.
 * It stores all sync/async taks in its queue.
 *
 * It requires the SSL client and network config (identifier) data for its class constructor for
 * server connection and data transfer.
 *
 * Step 8. Define the class that provides the Firebase/Google Cloud services.
 * ==========================================================================
 * The Firebase/Google Cloud services classes provide the member functions that works with AsyncClient.
 *
 * Step 9. Define the AsyncResult class
 * ====================================
 * This keeps the processing result when it assigned to any sync/async w/o callback functions.
 *
 * Step 10. Start the authenticate process.
 * ========================================
 * In this step, it actually adds the authentication task to the AsyncClient queue which will be processed later.
 * The result/status will send to the callback function in case of async with callback usage or stores in the AsyncResult object
 * in case asynce without callback and sync usages.
 *
 * This allows us to use different authentications for each Firebase/Google Cloud services with different
 * FirebaseApp (authentication handler).
 *
 * Please avoid placing your code that uses large memory and cpu time inside the callback function to prevent the stack overflow and
 * nested callback calls.
 *
 * Step 11. Bind the FirebaseApp (authentication handler) with your Firebase/Google Cloud services classes.
 * ========================================================================================================
 * This allows us to use different authentications for each Firebase/Google Cloud services.
 *
 * It is easy to bind/unbind/chanhe the authentication for authentication for different Firebase/Google Cloud services APIs.
 *
 * Step 12. Set the Realtime Database URL (for Realtime Database only)
 * ===================================================================
 *
 * Step 13. Maintain the authentication (async) task in the loop.
 * ==============================================================
 * This is required for authentication/re-authentication process.
 *
 * When the UserAuth and CustomAuth classes are used, the JWT.loop() will be required to run in the loop().
 *
 * Step 14. Maintain the Firebase/Google Cloud services async tasks in the loop.
 * =============================================================================
 * This depends on how the AsyncClient class was used.
 *
 * If only one AsyncClient object was used in all tasks i.e.authentication and
 * Firbase/Google Cloud services tasks, then these tasks stored in
 * only one AsyncClient's queue then the Step 13 is enough and Step 14 is not necessary.
 *
 * Step 15. Checking the authentication status before use.
 * =======================================================
 * Before calling the Firebase/Google Cloud services functions, the FirebaseApp::ready() of authentication handler that bined to it
 * should return true.
 *
 * Step 16. Process the sync/async w/o callback results in the end of the loop.
 * ============================================================================
 * This requires only when using library in sync/async w/o callback modes to process the information in its AsyncResult.
 */

// Step 1
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// Step 2
void timeStatusCB(uint32_t &ts);
void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);
void getMsg(Messages::Message &msg);

// Step 3
DefaultNetwork network;

// Step 4
ServiceAuth sa_auth(timeStatusCB, "CLIENT_EMAIL", "PROJECT_ID", "PRIVATE_KEY", 3000);

// Step 5
FirebaseApp app;

// Step 6
WiFiClientSecure ssl_client;

// Step 7
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));

// Step 8
RealtimeDatabase Database;
Messaging messaging;
Firestore::Documents Docs;
Storage storage;
CloudStorage cstorage;
CloudFunctions cfunctions;

// Step 9 (not used in this async with callback usage)
// AsyncResult myResult;

bool onetimeTest = false;

void setup()
{
    Serial.begin(115200);

    WiFi.begin("WIFI_AP", "WIFI_PASSWORD");

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

    // Skip certificate verification
    ssl_client.setInsecure();

    // Step 10
    initializeApp(aClient, app, getAuth(sa_auth), asyncCB, "authTask");

    // Step 11
    app.getApp<RealtimeDatabase>(Database);
    app.getApp<Messaging>(messaging);
    app.getApp<Firestore::Documents>(Docs);
    app.getApp<Storage>(storage);
    app.getApp<CloudStorage>(cstorage);
    app.getApp<CloudFunctions>(cfunctions);

    // Step 12
    Database.url("DATABASE_URL");
}

void loop()
{
    // Step 13
    JWT.loop(app.getAuth());
    app.loop();

    // Step 14
    Database.loop();
    messaging.loop();
    Docs.loop();
    storage.loop();
    cstorage.loop();
    cfunctions.loop();

    // Step 15
    if (app.ready() && !onetimeTest)
    {
        onetimeTest = true;

        // Realtime Database set value.
        Database.set<int>(aClient, "/data", 9999, asyncCB, "RealtimeDatabase_SetTask");

        Messages::Message msg;
        getMsg(msg);

        // Cloud Messaging send message.
        messaging.send(aClient, Messages::Parent("PROJECT_ID"), msg, asyncCB, "Messaging_SendMessageTask");

        // Firestore get document.
        Docs.get(aClient, Firestore::Parent("PROJECT_ID"), "info/countries", GetDocumentOptions(DocumentMask("Singapore")), asyncCB, "Firestore_GetDocumentTask");

        // Firebase Storage get object metadata
        storage.getMetadata(aClient, FirebaseStorage::Parent("STORAGE_BUCKET_ID", "media.mp4"), asyncCB, "Storage_GetMetadataTask");

        // Cloud Storage list objects.
        GoogleCloudStorage::ListOptions options;
        options.maxResults(3);
        options.prefix("media");
        cstorage.list(aClient, GoogleCloudStorage::Parent("STORAGE_BUCKET_ID"), options, asyncCB, "CloudStorage_ListObjectTask");

        // Cloud Functions call function.
        cfunctions.call(aClient, GoogleCloudFunctions::Parent("PROJECT_ID", "PROJECT_LOCATION"), "helloWorld", "test", asyncCB, "Functions_CallTask");
    }

    // Step 16 (not used in this async with callback usage)
    // printResult(myResult);
}

void timeStatusCB(uint32_t &ts)
{
    // The valid timestamp is needed for signed JWT token process when ServiceAuth and CustomAuth classes are used.
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

void asyncCB(AsyncResult &aResult) { printResult(aResult); }

void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());

    if (aResult.isDebug())
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

    if (aResult.isError())
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

    if (aResult.available())
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}

void getMsg(Messages::Message &msg)
{
    msg.topic("test");

    Messages::Notification notification;
    notification.body("Notification body").title("Notification title");

    msg.notification(notification);
}
