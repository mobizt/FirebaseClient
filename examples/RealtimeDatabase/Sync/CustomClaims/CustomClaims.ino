/**
 * ABOUT:
 *
 * The example to show how to control the user access with security rules, UID and custom claims.
 * 
 * User is allowed to access only the certain Realtime database resource under its user name if the auth.tokens variables
 * match the custom claims.  
 *
 * This example also shows how to simulate the push operation with the custom UUID.
 *
 * This example uses the CustomAuth class for authentication, and the DefaultNetwork class for network interface configuration.
 * See examples/App/AppInitialization and examples/App/NetworkInterfaces for more authentication and network examples.
 * 
 * The LegacyToken class (database secret) was used only for security rules read and modification.
 * 
 * The FirebaseJson library used in this example is available in Library Manager or 
 * can download from https://github.com/mobizt/FirebaseJson.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * bool RealtimeDatabase::set<T>(<AsyncClient>, <path>, <value>);
 *
 * T - The type of value to set.
 * <AsyncClient> - The async client.
 * <path> - The node path to set the value.
 * <value> - The value to set.
 * <AsyncResult> - The async result (AsyncResult).
 *
 * 2.------------------------
 *
 * String RealtimeDatabase::push<T>(<AsyncClient>, <path>, <value>, <AsyncResult>);
 *
 * T - The type of value to push.
 * <AsyncClient> - The async client.
 * <path> - The node path to push the value.
 * <value> - The value to push.
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
#include <FirebaseJson.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

/**
 * This information can be taken from the service account JSON file.
 *
 * To download service account file, from the Firebase console, goto project settings,
 * select "Service accounts" tab and click at "Generate new private key" button
 */
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";
#define DATABASE_URL "URL"

#define DATABASE_SECRET "DATABASE_SECRET"

#include <FirebaseJson.h>

void authHandler();

void printResult(AsyncResult &aResult);

bool mofifyRules(const String &path, const String &var, const String &readVal, const String &writeVal, const String &databaseSecret);

void printError(int code, const String &msg);

void timeStatusCB(uint32_t &ts);

String genUUID();

DefaultNetwork network;

CustomAuth custom_auth(timeStatusCB, API_KEY, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, "Node1" /* UID */, "" /* claims */, 3600 /* expire period in seconds (<3600) */);

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

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    aClient.setAsyncResult(aResult_no_callback);

    // This will modify the security rules to allow the read/write accesses at UsersData/$userId and the auth.token
    // variables match the custom claims.
    // {
    // "rules": {
    //     ...
    //
    //     ...
    //     "UsersData": {
    //         "$userId": {
    //             ".read": "($userId === auth.uid && auth.token.premium_account === true && auth.token.admin === true && auth.token.foo === 'bar')",
    //             ".write": "($userId === auth.uid && auth.token.premium_account === true && auth.token.admin === true && auth.token.foo === 'bar')"
    //                     }
    //                 }
    //         }
    //    }
    // }
    //
    // To acheive what we want, you have to remove the following rules from your security rules.
    // ".read": "auth != null"
    // ".write": "auth != null"
    // ".read": "true"
    // ".write": "true"
    // Because it allows all authenticated user to access any resources without restriction.

    String base_path = "/UsersData";
    String var = "$userId";
    String val = "($userId === auth.uid && auth.token.premium_account === true && auth.token.admin === true && auth.token.foo === 'bar')";
    mofifyRules(base_path, var, val, val, DATABASE_SECRET);

    // The custom claims.
    // For more details about claims, please visit https://firebase.google.com/docs/auth/admin/custom-claims.
    String claims = "{\"premium_account\": true,\"admin\": true, \"foo\":\"bar\"}";
    custom_auth.setClaims(claims);

    Serial.println("Initializing the app...");
    initializeApp(aClient, app, getAuth(custom_auth), aResult_no_callback);

    authHandler();

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);
}

void loop()
{
    authHandler();

    Database.loop();

    if (app.ready() && (ms == 0 || millis() - ms > 10000))
    {
        ms = millis();

        String path = "/UsersData/";
        path += app.getUid(); //<- "Node1"
        path += "/test/int";

        Serial.print("Setting the int value to the granted path... ");
        bool status = Database.set<int>(aClient, path, 12345);
        if (status)
            Serial.println("ok");
        else
            printError(aClient.lastError().code(), aClient.lastError().message());

        Serial.print("Setting the int value to outside of granted path... ");
        status = Database.set<int>(aClient, "/test/int", 12345);
        if (status)
            Serial.println("ok");
        else
            printError(aClient.lastError().code(), aClient.lastError().message());
    }
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

    if (aResult.available())
    {
        if (aResult.to<RealtimeDatabaseResult>().name().length())
            Firebase.printf("task: %s, name: %s\n", aResult.uid().c_str(), aResult.to<RealtimeDatabaseResult>().name().c_str());
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
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

/**
 * @param path The parent path of child's node that the .read and .write rules are being set.
 * @param var The child node key that the .read and .write rules are being set.
 * @param readVal The child node key .read value.
 * @param writeVal The child node key .write value.
 * @param databaseSecret The database secret.
 */
bool mofifyRules(const String &path, const String &var, const String &readVal, const String &writeVal, const String &databaseSecret)
{
    LegacyToken legacy_token(databaseSecret);

    initializeApp(aClient, app, getAuth(legacy_token));

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    Serial.print("Getting the security rules to check the existing keys... ");

    String jsonStr = Database.get<String>(aClient, ".settings/rules");

    if (aClient.lastError().code() == 0)
    {
        Serial.println("ok");

        bool ret = true;
        FirebaseJsonData data;
        FirebaseJson json(jsonStr);
        bool rd = false, wr = false;

        String s = "rules";
        s += path;
        s += "/";
        s += var;

        if (readVal.length() > 0)
        {
            rd = true;
            String r = s;
            r += '/';
            r += ".read";
            if (json.get(data, r.c_str()) && strcmp(data.to<const char *>(), readVal.c_str()) == 0)
                rd = false;
        }

        if (writeVal.length() > 0)
        {
            wr = true;
            MB_String w = s;
            w += "/";
            w += ".write";
            if (json.get(data, w.c_str()) && strcmp(data.to<const char *>(), writeVal.c_str()) == 0)
                wr = false;
        }

        // modify if the rules changed or does not exist.
        if (wr || rd)
        {
            FirebaseJson js;
            if (rd)
                js.add(".read", readVal);
            if (wr)
                js.add(".write", writeVal);

            json.set(s.c_str(), js);

            String str;
            json.toString(str, true);

            Serial.print("Setting the security rules to add the modified rules... ");
            bool status = Database.set<object_t>(aClient, ".settings/rules", object_t(str));
            if (status)
                Serial.println("ok");
            else
                printError(aClient.lastError().code(), aClient.lastError().message());
        }

        json.clear();
        return aClient.lastError().code() == 0;
    }
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    return false;
}