/**
 * The beare minimum code example for Realtime Database in sync mode.
 */

// 1. Include the network, SSL client and Firebase libraries.
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// 2. Define the function prototypes of functions we will use.
void authHandler();
void printResult(AsyncResult &aResult);

// 3. Define the network config (identifier) class for your network type.
// The DefaultNetwork class object will provide the WiFi network identifier for ESP32/ESP8266 and
// any WiFi capable devices.
DefaultNetwork network;

// 4. Define the authentication config class for your Firebase authentication.
UserAuth user_auth("Web_API_KEY", "USER_EMAIL", "USER_PASSWORD");

// 5. Define the authentication handler class which handles the Firebase authentication process for you.
FirebaseApp app;

// 6. Define the SSL client.
WiFiClientSecure ssl_client;

// 7. Define the Async Client that we have to use it for all processes.
// The getNetwork(network) from step 3 will provide the network configuration for Async Client.
// The network config is for network status determination and connection/reconnection in some cases.
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));

// 8. Here the class that provides the Firebase service API.
// The RealtimeDatabase class provides the Realtime Database functions.
RealtimeDatabase Database;

// 9. Define the AsyncResult class that keeps all information/debug/error/data from all processes.
AsyncResult authResult;

void setup()
{

    Serial.begin(115200);

    // 10. Setup network and connect
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

    // 11. Start authenticate process.
    // It actually sets up the authentication task in the queue to process later.
    // The result/status will store in AsyncResult(authResult).
    initializeApp(aClient, app, getAuth(user_auth), authResult);

    // 12. Then we need to wait until
    // the authentication processes were complete.
    // The authHandler function in this example provides the loop for authentication processing.
    authHandler();

    // 13. Now we can transfer or bind the authentication credentials called tokens to the class in Step 8.
    app.getApp<RealtimeDatabase>(Database);

    // 14. Set your database URL (requires only for Realtime Database)
    Database.url("DATABASE_URL");

    // 15. Checking the authentication status before calling Firebase services API.
    if (app.ready())
    {
        // Testing with set and get.
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
    // 16. Call authentication processing in loop (always) to
    // handle the authentication process that is required e.g. in case the auth token expires.
    authHandler();

    // 17. Place the Firebase service class in the loop.
    // Note that if the Async Client or aClient that provided with the function initializeApp
    // and the Async Client that provided with Database member functions, is the same,
    // this Database.loop() is not necessary.
    Database.loop();

}

// 18. Define the function that provides the loop for authentication processing..
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

// 19. Optional for debugging.
// The auxiliary function that we will print all information that obtained from the authentication processes.
void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());

    if (aResult.isDebug())
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

    if (aResult.isError())
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
}