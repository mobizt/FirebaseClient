/**
 * SYNTAX:
 *
 * bool status = RealtimeDatabase::set<T>(<AsyncClient>, <path>, <value>);
 *
 * <AsyncClient> - The async client.
 * <path> - The node path to set the value.
 * <value> - The value to set.
 *
 * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
 *
 * The string_t is for string placeholder e.g. string_t("hello there").
 * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
 * The boolean_t is for boolean placeholder e.g. boolean_t(true).
 * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
 *
 * The sync Set function returns the boolean indicated the operating status.
 *
 * In case of error, the operation error information can be obtain from AsyncClient via aClient.lastError().message() and
 * aClient.lastError().code().
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
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

void printError(int code, const String &msg);

void asyncCB(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

RealtimeDatabase Database;

void setup()
{

    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
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

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    Serial.println("Synchronous Set... ");

    // Set int
    bool status = Database.set<int>(aClient, "/test/int", 12345);
    if (status)
        Serial.println("Set int is ok");
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Set bool
    status = Database.set<bool>(aClient, "/test/bool", true);
    if (status)
        Serial.println("Set bool is ok");
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Set string
    status = Database.set<String>(aClient, "/test/string", "hello");
    if (status)
        Serial.println("Set string is ok");
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Set json

    // Library does not provide JSON parser library, the following JSON writer class will be used with
    // object_t for simple demonstration.

    object_t json;
    JsonWriter writer;
    writer.create(json, "test/data", 123); // -> {"test":{"data":123}}
    // Or set the seialized JSON string to object_t as object_t("{\"test\":{\"data\":123}}")

    status = Database.set<object_t>(aClient, "/test/json", json);
    if (status)
        Serial.println("Set json is ok");
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Set array

    object_t arr;
    arr.initArray();                                                                                      // To use as Array placeholder
    writer.join(arr, 4, object_t(1), object_t(2), object_t(string_t("test")), object_t(boolean_t(true))); // -> [1,2,"test",true]
    // Or set the seialized JSON Array string to the object_t as object_t("[1,2,\"test\",true]")

    status = Database.set<object_t>(aClient, "/test/arr", arr);
    if (status)
        Serial.println("Set array is ok");
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Set float
    status = Database.set<number_t>(aClient, "/test/float", number_t(123.456, 2));
    if (status)
        Serial.println("Set float is ok");
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    // Set double
    status = Database.set<number_t>(aClient, "/test/double", number_t(1234.56789, 4));
    if (status)
        Serial.println("Set double is ok");
    else
        printError(aClient.lastError().code(), aClient.lastError().message());
}

void loop()
{
    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // (For Async Only) This required when different AsyncClients than used in FirebaseApp assigned to the Realtime database functions.
    Database.loop();
}

void asyncCB(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
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
