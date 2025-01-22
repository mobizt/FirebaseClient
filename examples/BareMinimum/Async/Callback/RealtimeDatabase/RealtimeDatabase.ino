/**
 * ABOUT:
 * The beare minimum code example for Realtime Database in async mode with callback function.
 */

// 1. Include the network, SSL client and Firebase libraries.
// ==========================================================
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// 2. Define the function prototypes of functions we will use.
// ===========================================================
void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);

// 3. Define the network config (identifier) class for your network type.
// ======================================================================
// The DefaultNetwork class object will provide the WiFi network identifier for ESP32/ESP8266 and
// any WiFi capable devices.
// Why we have to define this? It is because this library supports many types of network interfaces.
// The library have to know which type of network you are using and used for connecting status checking and
// performs network re-connection if neccessary.
DefaultNetwork network;

// 4. Define the authentication config (identifier) class.
// =======================================================
// The UserAuth (user authentication) is the basic for Realtime Database authentication.
UserAuth user_auth("Web_API_KEY", "USER_EMAIL", "USER_PASSWORD");

// 5. Define the authentication handler class.
// ===========================================
// It handles and maintains (re-authenticate in case auth token was expired in 60 min) 
// the Firebase authentication process for you.
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
    // It actually add the authentication task in the AsyncClient queue which will be processed later.
    // The result/status will send to the callback function "asyncCB".
    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "authTask");

    // 11. Transfer or bind the authentication credentials
    // ==================================================== 
    // The auth credentials from will FirebaseApp will be applied to the Firebase/Google Cloud services classes
    // that defined in Step 8.
    app.getApp<RealtimeDatabase>(Database);

    // 12. Set your database URL (requires only for Realtime Database)
    // ===============================================================
    Database.url("DATABASE_URL");
}

void loop()
{
    // 13. Maintain the authentication (async) task in the loop
    // =================================================
    // This required for authentication/re-authentication.
    app.loop();

    // 14. Maintain the Firebase service async tasks in the loop.
    // ==========================================================
    // This is not neccessary if the same AsyncClient or aClient provided for 
    // all authentication processes and Firebase services functions, calling app.loop()
    // is enough.
    Database.loop();

    // 15. Checking the authentication status before calling Firebase services API.
    // ============================================================================
    if (app.ready() && !onetimeTest)
    {
        onetimeTest = true;

        // Testing with Realtime Database set and get.
        Database.set<int>(aClient, "/data", 9999, asyncCB, "setTask");

        Database.get(aClient, "/data", asyncCB, false, "getTask");
    }
}

// 16. The callback function that will be called when the information/error/data 
// from the auth and Firebase processes are available.
// =============================================================================
void asyncCB(AsyncResult &aResult) { printResult(aResult); }

// 17. Optional for debugging.
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