/**
 * SYNTAX:
 *
 * String RealtimeDatabase::push<T>(<AsyncClient>, <path>, <value>);
 *
 * T - The type of value to push.
 * <AsyncClient> - The async client.
 * <path> - The node path to push the value.
 * <value> - The value to push.
 *
 * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
 *
 * The string_t is for string placeholder e.g. string_t("hello there").
 * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
 * The boolean_t is for boolean placeholder e.g. boolean_t(true).
 * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
 *
 * The sync Push function returns the node name that will be created.
 *
 * In case of error, the operation error information can be obtain from AsyncClient via aClient.lastError().message() and
 * aClient.lastError().code().
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

void authHandler();

void printResult(AsyncResult &aResult);

void printError(int code, const String &msg);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

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

AsyncResult aResult_no_callback;

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

    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);

    authHandler();

    // Binding the FirebaseApp for authentication handler.
    // To unbind, use Database.resetApp();
    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    // In case setting the external async result to the sync task (optional)
    // To unset, use unsetAsyncResult().
    aClient.setAsyncResult(aResult_no_callback);

    // Push int
    Serial.print("Push int... ");
    String name = Database.push<int>(aClient, "/test/int", 12345);
    if (aClient.lastError().code() == 0)
        Firebase.printf("ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push bool
    Serial.print("Push bool... ");
    name = Database.push<bool>(aClient, "/test/bool", true);
    if (aClient.lastError().code() == 0)
        Firebase.printf("ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push string
    Serial.print("Push String... ");
    name = Database.push<String>(aClient, "/test/string", "hello");
    if (aClient.lastError().code() == 0)
        Firebase.printf("ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push json

    // Library does not provide JSON parser library, the following JSON writer class will be used with
    // object_t for simple demonstration.

    object_t json;
    JsonWriter writer;
    writer.create(json, "test/data", 123); // -> {"test":{"data":123}}
    // Or set the seialized JSON string to object_t as object_t("{\"test\":{\"data\":123}}")

    Serial.print("Push JSON... ");
    name = Database.push<object_t>(aClient, "/test/json", json);
    if (aClient.lastError().code() == 0)
        Firebase.printf("ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push array

    object_t arr;
    arr.initArray();                                                                                      // To use as Array placeholder
    writer.join(arr, 4, object_t(1), object_t(2), object_t(string_t("test")), object_t(boolean_t(true))); // -> [1,2,"test",true]
    // Or set the seialized JSON Array string to the object_t as object_t("[1,2,\"test\",true]")

    Serial.print("Push Array... ");
    name = Database.push<object_t>(aClient, "/test/arr", arr);
    if (aClient.lastError().code() == 0)
        Firebase.printf("ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push float
    Serial.print("Push float... ");
    name = Database.push<number_t>(aClient, "/test/float", number_t(123.456, 2));
    if (aClient.lastError().code() == 0)
        Firebase.printf("ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Push double
    Serial.print("Push double... ");
    name = Database.push<number_t>(aClient, "/test/double", number_t(1234.56789, 4));
    if (aClient.lastError().code() == 0)
        Firebase.printf("ok, name: %s\n", name.c_str());
    else
        printError(aClient.lastError().code(), aClient.lastError().message());
}

void loop()
{
    authHandler();

    Database.loop();
}

void authHandler()
{
    // Blocking authentication handler with timeout
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // The JWT token processor required for ServiceAuth and CustomAuth authentications.
        // JWT is a static object of JWTClass and it's not thread safe.
        // In multi-threaded operations (multi-FirebaseApp), you have to define JWTClass for each FirebaseApp,
        // and set it to the FirebaseApp via FirebaseApp::setJWTProcessor(<JWTClass>), before calling initializeApp.
        JWT.loop(app.getAuth());
        printResult(aResult_no_callback);
    }
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
}

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}
