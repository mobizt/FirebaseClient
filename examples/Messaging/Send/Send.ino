/**
 * The example to send the Cloud Messaging message.
 *
 * This example uses the ServiceAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * You can test this example with Firebase Cloud Messaging Web Client App in /examples/Messaging/WebClient.
 * 
 * For Google REST API reference documentation, please visit
 * https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages/send
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_SERVICE_AUTH
#define ENABLE_MESSAGING

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

void processData(AsyncResult &aResult);
void getMsg(Messages::Message &msg);
void send_message_async(Messages::Message &msg);
void send_message_async2(Messages::Message &msg);
void send_message_await(Messages::Message &msg);

// ServiceAuth is required for Google Cloud Functions functions.
ServiceAuth sa_auth(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

Messaging messaging;
AsyncResult fcmResult;

bool taskComplete = false;

void setup()
{
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    set_ssl_client_insecure_and_buffer(ssl_client);

    // Assign the valid time only required for authentication process with ServiceAuth and CustomAuth.
    app.setTime(get_ntp_time());

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(sa_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(sa_auth), 120 * 1000, auth_debug_print);

    app.getApp<Messaging>(messaging);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;

        Messages::Message msg;
        getMsg(msg);
        // You can set the content of msg object directly with msg.setContent("your content")

        send_message_async(msg);

        // send_message_async2(msg);

        // send_message_await(msg);
    }

    // For async call with AsyncResult.
    processData(fcmResult);
}

void processData(AsyncResult &aResult)
{
    // Exits when no result is available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

void getMsg(Messages::Message &msg)
{
    // To subscribe the App Instance Token to the topic,
    // see examples/Messaging/Instances/AddRemove/AddRemove.ino
    
    msg.topic("test_topic");
    // msg.token("APP_INSTANCE_TOKEN"); // Registration token to send a message to
    // msg.condition("'foo' in topics && 'bar' in topics");

    // Basic notification
    Messages::Notification notification;
    notification.body("Notification body").title("Notification title");

    // Library does not provide JSON parser library, the following JSON writer class will be used with
    // object_t for simple demonstration.

    object_t data, obj1, obj2, obj3, obj4;
    JsonWriter writer;

    writer.create(obj1, "name", string_t("wrench"));
    writer.create(obj2, "mass", string_t("1.3kg"));
    writer.create(obj3, "count", string_t("3"));
    writer.join(data, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);

    // object_t data2("{\"name\":\"wrench\",\"mass\":\"1.3kg\",\"count\":\"3\"}");

    msg.data(data);

    data.clear();

    Messages::AndroidConfig androidConfig;

    // Set the priority of a message to send to Android devices.
    // https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages#androidmessagepriority
    androidConfig.priority(Messages::AndroidMessagePriority::_HIGH);

    Messages::AndroidNotification androidNotification;

    // Set the relative priority for this notification.
    // Priority is an indication of how much of the user's attention should be consumed by this notification.
    // Low-priority notifications may be hidden from the user in certain situations,
    // while the user might be interrupted for a higher-priority notification.
    // https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages#NotificationPriority
    androidNotification.notification_priority(Messages::NotificationPriority::PRIORITY_HIGH);

    androidConfig.notification(androidNotification);

    msg.android(androidConfig);

    msg.notification(notification);
}

void send_message_async(Messages::Message &msg)
{
    Serial.println("Sending a message...");

    // Async call with callback function.
    messaging.send(aClient, Messages::Parent(FIREBASE_PROJECT_ID), msg, processData, "fcmSendTask");
}

void send_message_async2(Messages::Message &msg)
{
    Serial.println("Sending a message...");

    // Async call with AsyncResult for returning result.
    messaging.send(aClient, Messages::Parent(FIREBASE_PROJECT_ID), msg, fcmResult);
}

void send_message_await(Messages::Message &msg)
{
    Serial.println("Sending a message...");

    // Sync call which waits until the payload was received.
    String payload = messaging.send(aClient, Messages::Parent(FIREBASE_PROJECT_ID), msg);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}
