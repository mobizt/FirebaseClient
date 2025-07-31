/**
 * The bare minimum code example for using the Realtime Database service.
 *
 * The generally required steps are explained below.
 *
 * Step 1. Include the network, SSL client, and Firebase libraries.
 * =================================================================
 *
 * Step 2. Define the user functions that are required for library usage.
 * =======================================================================
 *
 * Step 3. Define the authentication configuration (identifier) object.
 * ======================================================================
 * In the Firebase and Google Cloud REST APIs, auth tokens are used for authentication and authorization.
 *
 * The auth token is a short-lived token that expires in 60 minutes and needs to be refreshed or re-created.
 *
 * For some special use cases, services may provide non-authenticated access, e.g., using a database secret
 * in the Realtime Database, or setting security rules in the Realtime Database, Firestore, and Firebase Storage to allow public read/write access.
 *
 * UserAuth (user authentication with email/password) is the basic authentication method for the Realtime Database,
 * Firebase Storage, and Firestore services, except for some Firestore services that are involved with Google Cloud.
 *
 * The UserAuth object stores the email, password, and API key for the authentication process.
 *
 * For Google Cloud services, e.g., Cloud Storage and Cloud Functions, the highest authentication level is required.
 * For these cases, the ServiceAuth (OAuth2.0) and AccessToken classes are used.
 *
 * CustomAuth provides the same authentication level as UserAuth, but allows for a custom UID and claims.
 *
 * Step 4. Define the authentication handler object.
 * ==================================================
 * The FirebaseApp object works as the authentication handler.
 * It also maintains authentication and re-authentication when you place `FirebaseApp::loop()` inside your main loop.
 *
 * Step 5. Define the SSL client.
 * ================================
 * The SSL client handles the server connection and data transfer.
 *
 * In this bare minimum example, we use only one SSL client for all processes.
 * For some use cases, e.g., a Realtime Database Stream connection, you may have to define a separate SSL client for that stream.
 *
 * Step 6. Define the Async Client.
 * ================================
 * This class is used for any function where server data transfer is involved.
 * It stores all synchronous and asynchronous tasks in its queue.
 *
 * It requires the SSL client and network configuration data in its constructor for handling network re-connections
 * (e.g., Wi-Fi and GSM), checking network status, and managing the server connection and data transfer processes.
 *
 * This design makes the library reliable and allows it to operate precisely under various server and network conditions.
 *
 * Step 7. Define the class that provides the Firebase/Google Cloud service.
 * ============================================================================
 * The Firebase/Google Cloud service classes provide the member functions that work with the AsyncClient.
 *
 * Step 8. Start the authentication process.
 * ==========================================
 * At this step, the authentication credentials will be used to generate auth tokens by
 * calling `initializeApp`.
 *
 * This allows us to use different authentication methods for each Firebase/Google Cloud service by using different
 * FirebaseApp (authentication handler) objects.
 *
 * The workflow of the authentication process is as follows:
 *
 * -----------------------------------------------------------------------------------------------------------------
 *  Setup   |    FirebaseApp [account credentials/tokens] ---> InitializeApp (w/ or w/o timeout) ---> FirebaseApp::getApp
 * -----------------------------------------------------------------------------------------------------------------
 *  Loop    |    FirebaseApp::loop  ---> FirebaseApp::ready ---> Firebase Service API [auth token]
 * -----------------------------------------------------------------------------------------------------------------
 *
 * Step 9. Bind the FirebaseApp (authentication handler) with your Firebase/Google Cloud service object.
 * =========================================================================================================
 * This allows us to use different authentication methods for each Firebase/Google Cloud service.
 *
 * It is easy to bind, unbind, or change the authentication method for different Firebase/Google Cloud service APIs.
 *
 * Step 10. Set the Realtime Database URL (for the Realtime Database only).
 * ========================================================================
 *
 * Step 11. Maintain authentication and async tasks in the loop.
 * ===============================================================
 * This is required for the authentication/re-authentication process and to keep async tasks running.
 *
 * Step 12. Check the authentication status before use.
 * ====================================================
 * Before calling a Firebase/Google Cloud service function, the `FirebaseApp::ready()` function of the associated authentication handler
 * should return true.
 *
 * Step 13. Process the results of async tasks at the end of the loop.
 * =====================================================================
 * This is only required when an `AsyncResult` object was assigned to a Firebase/Google Cloud service function.
 */

// Step 1

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// Step 2
void asyncCB(AsyncResult &aResult);
void processData(AsyncResult &aResult);

// Step 3
UserAuth user_auth("Web_API_KEY", "USER_EMAIL", "USER_PASSWORD");

// Step 4
FirebaseApp app;

// Step 5
// SSL client async tasks.
WiFiClientSecure ssl_client;

// Step 6
// Use AsyncClient async tasks.
using AsyncClient = AsyncClientClass;
AsyncClient async_client(ssl_client);

// Step 7
RealtimeDatabase Database;

bool onetimeTest = false;

// The Optional proxy object that provides the data/information
//  when used in async mode without callback.
AsyncResult dbResult;

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

    // The SSL client options depend on the SSL client used.

    // Skip certificate verification
    ssl_client.setInsecure();

    // ESP8266 Set buffer size
    // ssl_client.setBufferSizes(4096, 1024);

    // Step 8
    initializeApp(async_client, app, getAuth(user_auth), processData, "🔐 authTask");

    // Step 9
    app.getApp<RealtimeDatabase>(Database);

    // Step 10
    Database.url("DATABASE_URL");
}

void loop()
{
    // Step 11
    app.loop();

    // Step 12
    if (app.ready() && !onetimeTest)
    {
        onetimeTest = true;

        // Realtime Database set value.
        // ============================

        // Async call with callback function
        Database.set<String>(async_client, "/examples/BareMinimum/data/set1", "abc", processData, "RealtimeDatabase_SetTask");

        // Async call with AsyncResult for returning result.
        Database.set<bool>(async_client, "/examples/BareMinimum/data/set2", true, dbResult);

        // Realtime Database get value.
        // ============================

        // Async call with callback function
        Database.get(async_client, "/examples/BareMinimum/data/set1", processData, false, "RealtimeDatabase_GetTask");

        // Async call with AsyncResult for returning result.
        Database.get(async_client, "/examples/BareMinimum/data/set2", dbResult, false);
    }

    // Step 13
    processData(dbResult);
}

void processData(AsyncResult &aResult)
{
    // Exits when no result available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

    if (aResult.isDebug())
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

    if (aResult.isError())
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

    if (aResult.available())
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}
