/**
 * ABOUT:
 * The beare minimum code example for all Firebase services in async mode with callback function.
 */

// 1. Include the network, SSL client and Firebase libraries.
// ==========================================================
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// 2. Define the function prototypes of functions we will use.
// ===========================================================
void timeStatusCB(uint32_t &ts);
void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);
void getMsg(Messages::Message &msg);

// 3. Define the network config (identifier) class.
// ================================================
// The DefaultNetwork class object will provide the WiFi network identifier for ESP32/ESP8266 and
// any WiFi capable devices.
// Why we have to define this? It is because this library supports many types of network interfaces.
// The library have to know which type of network you are using and used for connecting status checking and
// performs network re-connection if neccessary.
DefaultNetwork network;

// 4. Define the authentication config (identifier) class.
// =======================================================
// In this case the ServiceAuth class (OAuth2.0 authen) can be used for all services 
// especially Cloud Storage and Cloud Functions required this authentication type.
// The timeStatusCB is the callback function to set the library timestamp for internal process.
ServiceAuth sa_auth(timeStatusCB, "CLIENT_EMAIL", "PROJECT_ID", "PRIVATE_KEY", 3000);

// 5. Define the authentication handler class.
// ===========================================
// It handles and maintains (re-authenticate in case auth token was expired in 60 min) 
// the Firebase authentication process for you.
// Actually you can define different FirebaseApps for each Firebase/Google Cloud services. 
FirebaseApp app;

// 6. Define the SSL client.
// =========================
// In this case we use only one SSL client for all process.
WiFiClientSecure ssl_client;

// 7. Define the Async Client.
// ===========================
// We have to use it for all processes.
// The getNetwork(network) in Step 3 and SSL client in Step 6 are required for Async Client
// for server connection and data transfer.
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));

// 8. Define the class that provides the Firebase service API.
// ===========================================================
RealtimeDatabase Database;
Messaging messaging;
Firestore::Documents Docs;
Storage storage;
CloudStorage cstorage;
CloudFunctions cfunctions;

bool onetimeTest = false;

void setup()
{
    Serial.begin(115200);

    // 9. Setup network and connect
    // ============================
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

    // 10. Start authenticate process.
    // ===============================
    // It actually adds the authentication task to the AsyncClient queue which will be processed later.
    // The result/status will send to the callback function "asyncCB".
    initializeApp(aClient, app, getAuth(sa_auth), asyncCB, "authTask");

    // 11. Transfer or bind the authentication credentials
    // ==================================================== 
    // The auth credentials from FirebaseApp will be applied to the Firebase/Google Cloud services classes
    // that defined in Step 8.
    app.getApp<RealtimeDatabase>(Database);
    app.getApp<Messaging>(messaging);
    app.getApp<Firestore::Documents>(Docs);
    app.getApp<Storage>(storage);
    app.getApp<CloudStorage>(cstorage);
    app.getApp<CloudFunctions>(cfunctions);

    // 12. Set your database URL (requires only for Realtime Database)
    // ===============================================================
    Database.url("DATABASE_URL");
}

void loop()
{
    // 13. Calling the JWT token processor in the loop
    // ===============================================
    // This handles the internal processes (JWT token generation) for ServiceAuth 
    // and CustomAuth authentications.
    JWT.loop(app.getAuth());

    // 14. Maintain the authentication (async) task in the loop
    // ========================================================
    // This required for authentication/re-authentication.
    app.loop();

    // 15. Maintain the Firebase service async tasks in the loop.
    // ==========================================================
    // This is not neccessary if the same AsyncClient or aClient provided for 
    // all authentication processes and Firebase services functions, calling app.loop()
    // is enough.
    Database.loop();
    messaging.loop();
    Docs.loop();
    storage.loop();
    cstorage.loop();
    cfunctions.loop();

    // 16. Checking the authentication status before calling Firebase services API.
    // ============================================================================
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
}

// 17. Defined the auxiliary function required to get the timestamp for internal JWT token signing process.
// ========================================================================================================.
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

// 18. The callback function that will be called when the information/error/data 
// from the auth and Firebase processes are available.
// =============================================================================
void asyncCB(AsyncResult &aResult) { printResult(aResult); }

// 19. Optional for debugging.
// ===========================
// The auxiliary function that we will print all information that obtained from the processes.
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

// 20 The auxiliary function to create the message to send.
// ========================================================
void getMsg(Messages::Message &msg)
{
    msg.topic("test");

    Messages::Notification notification;
    notification.body("Notification body").title("Notification title");

    msg.notification(notification);
}
