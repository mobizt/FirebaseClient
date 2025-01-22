/**
 * ABOUT:
 * The beare minimum code example for all Firebase services in sync mode.
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
void authHandler();
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

// 9. Define the AsyncResult class
// ===============================
// This keeps all information/debug/error from auth processes.
// Don't be confused with its name. It can be used for sync and
// async tasks to keep the task result when it assigned to any functions.
AsyncResult authResult;

void setup()
{

    Serial.begin(115200);

    // 10. Setup network and connect
    // =============================
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

    // 11. Start the authenticate process.
    // ===============================
    // It actually adds the authentication task to the AsyncClient queue which will be processed later.
    // The result/status will store in AsyncResult(authResult).
    initializeApp(aClient, app, getAuth(sa_auth), authResult);

    // 12. Process the authentication task
    // ===================================
    // This function required for sync mode usage.
    // The authHandler function in this example provides the loop for authentication processing.
    authHandler();

    // 13. Transfer or bind the authentication credentials
    // ====================================================
    // The auth credentials from FirebaseApp will be applied to the Firebase/Google Cloud services classes
    // that defined in Step 8.
    app.getApp<RealtimeDatabase>(Database);
    app.getApp<Messaging>(messaging);
    app.getApp<Firestore::Documents>(Docs);
    app.getApp<Storage>(storage);
    app.getApp<CloudStorage>(cstorage);
    app.getApp<CloudFunctions>(cfunctions);

    // 14. Set your database URL (requires only for Realtime Database)
    // ===============================================================
    Database.url("DATABASE_URL");

    // 15. Checking the authentication status before calling Firebase services API.
    // ============================================================================
    if (app.ready())
    {

        // Realtime Database set value.
        bool result = Database.set<String>(aClient, "/data", "Hello");

        if (!result)
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

        Messages::Message msg;
        getMsg(msg);

        // Cloud Messaging send message.
        String payload = messaging.send(aClient, Messages::Parent("PROJECT_ID"), msg);

        if (aClient.lastError().code() == 0)
            Serial.println(payload);
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

        // Firestore get document.
        payload = Docs.get(aClient, Firestore::Parent("PROJECT_ID"), "info/countries", GetDocumentOptions(DocumentMask("Singapore")));

        if (aClient.lastError().code() == 0)
            Serial.println(payload);
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

        // Firebase Storage get object metadata
        payload = storage.getMetadata(aClient, FirebaseStorage::Parent("STORAGE_BUCKET_ID", "media.mp4"));

        if (aClient.lastError().code() == 0)
            Serial.println(payload);
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

        // Cloud Storage list objects.
        GoogleCloudStorage::ListOptions options;
        options.maxResults(3);
        options.prefix("media");
        payload = cstorage.list(aClient, GoogleCloudStorage::Parent("STORAGE_BUCKET_ID"), options);

        if (aClient.lastError().code() == 0)
            Serial.println(payload);
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

        // Cloud Functions call function.
        payload = cfunctions.call(aClient, GoogleCloudFunctions::Parent("PROJECT_ID", "PROJECT_LOCATION"), "helloWorld", "test");

        if (aClient.lastError().code() == 0)
            Serial.println(payload);
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    }
}

void loop()
{
    // 16. Maintain the authentication processes in the loop.
    // ======================================================
    authHandler();

    // In only sync mode application (unless used both sync and async), the foolowing functions
    // are not necessary to run in the loop.
    // Database.loop();
    // messaging.loop();
    // Docs.loop();
    // storage.loop();
    // cstorage.loop();
    // cfunctions.loop();
}

// 17. Defined the auxiliary function required to get the timestamp for internal JWT token signing process.
// ========================================================================================================
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

// 18. Define the function that provides the loop for authentication processing.
// =============================================================================
void authHandler()
{
    // Blocking authentication handler with timeout
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        JWT.loop(app.getAuth());
        printResult(authResult);
    }
}

// 19. Optional function for debugging.
// ====================================
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
