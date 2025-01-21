/**
 * The beare minimum code example for Realtime Database and Cloud Messaging in async mode.
 */

// 1. Include the network, SSL client and Firebase libraries.
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// 2. Define the function prototypes of functions we will use.
void timeStatusCB(uint32_t &ts);
void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);
void getMsg(Messages::Message &msg);

// 3. Define the network config (identifier) class for your network type.
// The DefaultNetwork class object will provide the WiFi network identifier for ESP32/ESP8266 and
// any WiFi capable devices.
DefaultNetwork network;

// 4. Define the authentication config class for your Firebase authentication.
// The ServiceAuth class is required for Cloud Messaging authentication which can be used with Realtime Database authentication too.
ServiceAuth sa_auth(timeStatusCB, "CLIENT_EMAIL", "PROJECT_ID", "PRIVATE_KEY", 3000);

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
// The Messaging class provides the Cloud Messaging functions.
RealtimeDatabase Database;
Messaging messaging;

bool onetimeTest = false;

void setup()
{

    Serial.begin(115200);

    // 9. Setup network and connect
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
    // It actually sets up the authentication task in the queue to process later.
    // The result/status will send to the callback function "asyncCB".
    initializeApp(aClient, app, getAuth(sa_auth), asyncCB, "authTask");

    // 11. After authentication (initialization) started, no matter it is finished or not
    // we can transfer or bind the authentication credentials called tokens to the class in Step 8.
    app.getApp<RealtimeDatabase>(Database);
    app.getApp<Messaging>(messaging);

    // 12. Set your database URL (requires only for Realtime Database)
    Database.url("DATABASE_URL");
}

void loop()
{
    // 13 Place the JWT token processor required for ServiceAuth and CustomAuth authentications in the loop (always).
    JWT.loop(app.getAuth());

    // 14. Place the FirebaseApp class object in Step 5 in the loop (always)
    app.loop();

    // 15. Place the Firebase service class in the loop.
    // Note that if the Async Client or aClient that provided with the function initializeApp
    // and the Async Client that provided with Database member functions, is the same,
    // this Database.loop() and messaging.loop() are not necessary.
    Database.loop();
    messaging.loop();

    // 16. Checking the authentication status before calling Firebase services API.
    if (app.ready() && !onetimeTest)
    {
        onetimeTest = true;

        // Testing with set and send message.
        Database.set<int>(aClient, "/data", 9999, asyncCB, "setTask");

        Messages::Message msg;
        getMsg(msg);

        messaging.send(aClient, Messages::Parent("PROJECT_ID"), msg, asyncCB, "sendTask");
    }
}

// 17. The auxiliary function required to get the timestamp for internal JWT token signing process.
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

// 18. The callback function that will be called when the information/error/data from the auth and Firebase processes are available.
void asyncCB(AsyncResult &aResult) { printResult(aResult); }

// 19. Optional for debugging.
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
void getMsg(Messages::Message &msg)
{
    msg.topic("test");

    Messages::Notification notification;
    notification.body("Notification body").title("Notification title");

    msg.notification(notification);
}
