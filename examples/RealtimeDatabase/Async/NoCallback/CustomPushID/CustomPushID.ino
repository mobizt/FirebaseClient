/**
 * SYNTAX:
 *
 * RealtimeDatabase::set<T>(<AsyncClient>, <path>, <value>, <AsyncResult>);
 *
 * T - The type of value to set.
 * <AsyncClient> - The async client.
 * <path> - The node path to set the value.
 * <value> - The value to set.
 * <AsyncResult> - The async result (AsyncResult).
 *
 * RealtimeDatabase::push<T>(<AsyncClient>, <path>, <value>, <AsyncResult>);
 *
 * T - The type of value to push.
 * <AsyncClient> - The async client.
 * <path> - The node path to push the value.
 * <value> - The value to push.
 * <AsyncResult>  - The async result (AsyncResult).
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

void printResult(AsyncResult &aResult);

void timeStatusCB(uint32_t &ts);

String genUUID();

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

AsyncResult aResult_no_callback;

unsigned long ms = 0;

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

    if (app.ready() && (ms == 0 || millis() - ms > 60000))
    {
        ms = millis();

        Serial.println("Asynchronous Push vs Set with custom UUID... ");

        String path = "/test/int2/" + genUUID();

        // Push int
        Database.push<int>(aClient, "/test/int", 12345, aResult_no_callback);

        // Set int with custom UUID
        Database.set<int>(aClient, path, 12345, aResult_no_callback);
    }

    printResult(aResult_no_callback);
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

String genUUID()
{
    // Push UUID generator, https://gist.github.com/mikelehen/3596a30bd69384624c11

    /**
     * Fancy ID generator that creates 20-character string identifiers with the following properties:
     *
     * 1. They're based on timestamp so that they sort *after* any existing ids.
     * 2. They contain 72-bits of random data after the timestamp so that IDs won't collide with other clients' IDs.
     * 3. They sort *lexicographically* (so the timestamp is converted to characters that will sort properly).
     * 4. They're monotonically increasing.  Even if you generate more than one in the same timestamp, the
     *    latter ones will sort after the former ones.  We do this by using the previous random bits
     *    but "incrementing" them by 1 (only in the case of a timestamp collision).
     */

    // Modeled after base64 web-safe chars, but ordered by ASCII.
    static char PUSH_CHARS[] = "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";

    // Timestamp of last push, used to prevent local collisions if you push twice in one ms.
    static long long lastPushTime = 0;

    // We generate 72-bits of randomness which get turned into 12 characters and appended to the
    // timestamp to prevent collisions with other clients.  We store the last characters we
    // generated because in the event of a collision, we'll use those same characters except
    // "incremented" by one.
    char lastRandChars[72] = "";
    char timeStampChars[9] = "";

    uint32_t ts = 0;
    timeStatusCB(ts);

    long long now = ts * 1000LL;

    srand(now);

    bool duplicateTime = (now == lastPushTime);
    lastPushTime = now;

    for (int i = 7; i >= 0; i--)
    {
        timeStampChars[i] = PUSH_CHARS[(int)(now % 64)];
        now = now / 64;
    }

    // We should have converted the entire timestamp.
    if (now != 0)
        return String();

    timeStampChars[8] = '\0';

    String id = timeStampChars;

    if (!duplicateTime)
    {
        for (int i = 0; i < 12; i++)
        {
            double fl = ((double)rand() / (double)(RAND_MAX + 1.0)) * 64;
            lastRandChars[i] = (char)floor(fl);
        }
    }
    else
    {
        // If the timestamp hasn't changed since last push, use the same random number, except incremented by 1.
        int val = 0;
        for (int i = 11; i >= 0 && lastRandChars[i] == 63; i--)
        {
            val = i;
            lastRandChars[i] = 0;
        }

        if (val >= 0)
            lastRandChars[val]++;
    }

    for (int i = 0; i < 12; i++)
        id += PUSH_CHARS[(int)lastRandChars[i]];

    // Length should be 20.
    if (id.length() != 20)
        return String();

    return id;
}