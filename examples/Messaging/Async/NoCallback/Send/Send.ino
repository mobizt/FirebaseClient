/**
 * SYNTAX:
 *
 * Messaging::send(<AsyncClient>, <Messages::Parent>, <Messages::Message>, <AsyncResult>);
 *
 * <AsyncClient> - The async client.
 * The <Messages::Parent> - The Messages::Parent object included project Id in its constructor.
 * The <Messages::Message> - The Messages::Message object that holds the information of message to send.
 * <AsyncResult> - The async result (AsyncResult).
 *
 * The Firebase project Id should be only the name without the firebaseio.com.
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>) || defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#elif __has_include(<WiFiC3.h>) || defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif __has_include(<WiFi.h>)
#include <WiFi.h>
#endif

#include <FirebaseClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

/**
 * This information can be taken from the service account JSON file.
 *
 * To download service account file, from the Firebase console, goto project settings,
 * select "Service accounts" tab and click at "Generate new private key" button
 */
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

void getMsg(Messages::Message &msg);

void timeStatusCB(uint32_t &ts);

void printResult(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

// ServiceAuth is required for Cloud Messaging.
ServiceAuth sa_auth(timeStatusCB, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA) || defined(ARDUINO_PORTENTA_C33) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

Messaging messaging;

AsyncResult aResult_no_callback;

bool taskCompleted = false;

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

    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    initializeApp(aClient, app, getAuth(sa_auth), aResult_no_callback);

    // Binding the FirebaseApp for authentication handler.
    // To unbind, use messaging.resetApp();
    app.getApp<Messaging>(messaging);
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    // The JWT token processor required for ServiceAuth and CustomAuth authentications.
    // JWT is a static object of JWTClass and it's not thread safe.
    // In multi-threaded operations (multi-FirebaseApp), you have to define JWTClass for each FirebaseApp,
    // and set it to the FirebaseApp via FirebaseApp::setJWTProcessor(<JWTClass>), before calling initializeApp.
    JWT.loop(app.getAuth());

    app.loop();

    messaging.loop();

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        Serial.println("Sending message...");

        Messages::Message msg;
        getMsg(msg);

        // You can set the content of msg object directly with msg.setContent("your content")

        messaging.send(aClient, Messages::Parent(FIREBASE_PROJECT_ID), msg, aResult_no_callback);
    }

    printResult(aResult_no_callback);
}

void getMsg(Messages::Message &msg)
{
    msg.topic("test");
    // msg.token("DEVICE_TOKEN"); // Registration token to send a message to
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

    // Priority of a message to send to Android devices.
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

void timeStatusCB(uint32_t &ts)
{
#if defined(ESP8266) || defined(ESP32) || defined(CORE_ARDUINO_PICO)
    if (time(nullptr) < FIREBASE_DEFAULT_TS)
    {

        configTime(3 * 3600, 0, "pool.ntp.org");
        while (time(nullptr) < FIREBASE_DEFAULT_TS)
        {
            delay(100);
        }
    }
    ts = time(nullptr);
#elif __has_include(<WiFiNINA.h>) || __has_include(<WiFi101.h>)
    ts = WiFi.getTime();
#endif
}

void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
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
