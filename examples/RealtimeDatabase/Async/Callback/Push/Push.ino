/**
 * SYNTAX:
 *
 * RealtimeDatabase::push<T>(<AsyncClient>, <path>, <value>, <AsyncResultCallback>, <uid>);
 *
 * T - The type of value to push.
 * <AsyncClient> - The async client.
 * <path> - The node path to push the value.
 * <value> - The value to push.
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
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

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

DefaultNetwork network;

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

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

RealtimeDatabase Database;

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

    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "authTask");

    // Binding the FirebaseApp for authentication handler.
    // To unbind, use Database.resetApp();
    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    app.loop();

    Database.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;

        Serial.println("Asynchronous Push... ");

        // Push int
        Database.push<int>(aClient, "/test/int", 12345, asyncCB, "pushIntTask");

        // Push bool
        Database.push<bool>(aClient, "/test/bool", true, asyncCB, "pushBoolTask");

        // Push string
        Database.push<String>(aClient, "/test/string", "hello", asyncCB, "pushStringTask");

        // Push json
        Database.push<object_t>(aClient, "/test/json", object_t("{\"data\":123}"), asyncCB, "pushJsonTask1");

        // Library does not provide JSON parser library, the following JSON writer class will be used with
        // object_t for simple demonstration.

        object_t json, obj1, obj2, obj3, obj4;
        JsonWriter writer;

        writer.create(obj1, "int/value", 9999);
        writer.create(obj2, "string/value", string_t("hello"));
        writer.create(obj3, "float/value", number_t(123.456, 2));
        writer.join(obj4, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);
        writer.create(json, "node/list", obj4);

        // To print object_t
        // Serial.println(json);

        Database.push<object_t>(aClient, "/test/json", json, asyncCB, "pushJsonTask2");

        object_t arr;
        arr.initArray(); // initialize to be used as array
        writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

        // Note that value that sets to object_t other than JSON ({}) and Array ([]) can be valid only if it
        // used as array member value as above i.e. object_t(string_t("steve")) and object_t(888).

        // Push array
        Database.push<object_t>(aClient, "/test/arr", arr, asyncCB, "pushArrayTask");

        // Push float
        Database.push<number_t>(aClient, "/test/float", number_t(123.456, 2), asyncCB, "pushFloatTask");

        // Push double
        Database.push<number_t>(aClient, "/test/double", number_t(1234.56789, 4), asyncCB, "pushDoubleTask");
    }
}

void asyncCB(AsyncResult &aResult)
{
    // WARNING!
    // Do not put your codes inside the callback and printResult.

    printResult(aResult);
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
        if (aResult.to<RealtimeDatabaseResult>().name().length())
            Firebase.printf("task: %s, name: %s\n", aResult.uid().c_str(), aResult.to<RealtimeDatabaseResult>().name().c_str());
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}