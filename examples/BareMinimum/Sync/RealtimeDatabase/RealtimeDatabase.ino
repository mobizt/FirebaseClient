/**
 * ABOUT:
 *
 * The beare minimum code example for Realtime Database in sync mode.
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
 * It handles server connection and data transfer works.
 *
 * In this beare minimum example we use only one SSL client for all process.
 * In some use cases e.g. Realtime Database Stream connection, you may have to define the SSL client for it separately.
 *
 * Step 7. Define the Async Client.
 * ================================
 * This is the class that is used with the functions where the server data transfer is involved.
 * It stores all sync/async taks in its queue.
 *
 * It requires the SSL client and network config (identifier) data for its class constructor for its network re-connection 
 * (e.g. WiFi and GSM), network connection status checking, server connection, and data transfer processes.
 * 
 * This makes this library reliable and operates precisely under various server and network conditions. 
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
 * in case async without callback and sync usages.
 *
 * This allows us to use different authentications for each Firebase/Google Cloud services with different
 * FirebaseApps (authentication handler)s.
 * 
 * The workflow of authentication process.
 * 
 * FirebaseApp [holds unauthenticate data] ───> InitializeApp [prepares the authentication process] ───> 
 * FirebaseApp [holds authenticated data] ───> FirebaseApp::getApp ───> Firebase Service Class [uses authenticated data]
 *                                        └──> FirebaseApp::loop [maintains the (re)authentication process]
 *
 * Step 11. Waits the authentication task to be complete.
 * ======================================================
 * Because this is sync mode usage, we have to wait until it is authenticated.
 *
 * Step 12. Bind the FirebaseApp (authentication handler) with your Firebase/Google Cloud services classes.
 * ========================================================================================================
 * This allows us to use different authentications for each Firebase/Google Cloud services.
 *
 * It is easy to bind/unbind/chanhe the authentication for authentication for different Firebase/Google Cloud services APIs.
 *
 * Step 13. Set the Realtime Database URL (for Realtime Database only)
 * ===================================================================
 *
 * Step 14. Checking the authentication status before use.
 * =======================================================
 * Before calling the Firebase/Google Cloud services functions, the FirebaseApp::ready() of authentication handler that bined to it
 * should return true.
 *
 * Step 15. Maintain the authentication task in the loop.
 * ======================================================
 * The same code used in Step 11.
 *
 * Step 16. Checking the authentication status before use.
 * =======================================================
 */

// Step 1
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// Step 2
void authHandler();
void printResult(AsyncResult &aResult);

// Step 3
DefaultNetwork network;

// Step 4
UserAuth user_auth("Web_API_KEY", "USER_EMAIL", "USER_PASSWORD");

// Step 5
FirebaseApp app;

// Step 6
WiFiClientSecure ssl_client;

// Step 7
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));

// Step 8
RealtimeDatabase Database;

// Step 9
AsyncResult authResult;

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
    
    // Set timeout
    ssl_client.setConnectionTimeout(1000);
    ssl_client.setHandshakeTimeout(5);

    // Step 10
    initializeApp(aClient, app, getAuth(user_auth), authResult);

    // Step 11
    authHandler();

    // Step 12
    app.getApp<RealtimeDatabase>(Database);

    // Step 13
    Database.url("DATABASE_URL");

    // Step 14
    if (app.ready())
    {
        // Testing with Realtime Datanbase set and get.
        bool result = Database.set<String>(aClient, "/data", "Hello");

        if (!result)
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

        String value = Database.get<String>(aClient, "/data");

        if (aClient.lastError().code() == 0)
            Serial.println(value);
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    }
}

void loop()
{
    // Step 15
    authHandler();

    // Step 16
    if (app.ready())
    {
        // Your loop Firebase code here.
    }
}

void authHandler()
{
    // This function maintains the authentication process by calling app.ready() in the finite loop that exits when timed out.
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // The is require for signed JWT token generation process with ServiceAuth and CustomAuth and not use in this example.
        JWT.loop(app.getAuth());

        // To get authentication status.
        printResult(authResult);
    }
}

void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());

    if (aResult.isDebug())
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

    if (aResult.isError())
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
}