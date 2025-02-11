/**
 * ABOUT:
 *
 * The example to show how to use custom claims and UID with security rules to control Firebase Realtime database access.
 *
 * We use the sync functions in this example because it is easy to describe the processes stype by step.
 *
 * For more details of using claims with security rules to control Firebase services accesses,
 * visit https://firebase.google.com/docs/auth/admin/custom-claims
 * 
 * For security rules information.
 * https://firebase.google.com/docs/rules 
 * https://firebase.google.com/docs/rules/rules-language
 *
 * This example uses the CustomAuth class for authentication for setting our custom UID and additional claims.
 *
 * The LegacyToken class (database secret) was used only for security rules read and modification process.
 *
 * The DefaultNetwork class was used for network interface configuration.
 *
 * See examples/App/AppInitialization and examples/App/NetworkInterfaces for more authentication and network examples.
 *
 * The FirebaseJson library used in this example is available to install in IDE's Library Manager or
 * can be downloaded from https://github.com/mobizt/FirebaseJson.
 *
 * The syntaxes used in this example are described in example/App/AppInitialization/Sync/CustomAuth/CustomAuth.ino
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
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

CustomAuth custom_auth(timeStatusCB, API_KEY, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, "Node1" /* UID */, "" /* claims can be set later */, 3600 /* expire period in seconds (<3600) */);

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
    // =========
    // IMPORTANT
    // =========
    // To allow read/write access only the path we want, 
    // you have to remove the following rules from your security rules (if it existed).
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
    // Once, the conditions are set in the .read/.write rules are "auth.token.premium_account === true && auth.token.admin === true && auth.token.foo === 'bar'",
    // if our claims set to CustomAuth object here e.g. premium_account is not true, admin is not true and foo is not "bar", the access to the database will fail.
    
    // For more details about claims, please visit https://firebase.google.com/docs/auth/admin/custom-claims.
    String claims = "{\"premium_account\": true,\"admin\": true, \"foo\":\"bar\"}";
    custom_auth.setClaims(claims);
    
    // Now we authenticate (sign in) with CustomAuth (ID token with custom UID and claims).
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
        path += app.getUid(); // Node1 is the UID that previously defind in CustomAuth constructor.
        path += "/test/int";

        Serial.print("Setting the int value to the granted path... ");
        // This should be ok. Because we write to the path /UsersData/Node1/... which is allowed in the security rules.
        bool status = Database.set<int>(aClient, path, 12345); 
        if (status)
            Serial.println("ok");
        else
            printError(aClient.lastError().code(), aClient.lastError().message());

        Serial.print("Setting the int value to outside of granted path... ");
        // This should be failed because we write to the path that is not allowed.
        // Only /UsersData/Node1/... is allowed in the security rules.
        status = Database.set<int>(aClient, "/test/int", 12345);
        if (status)
            Serial.println("ok");
        else
            printError(aClient.lastError().code(), aClient.lastError().message());

        // If you change auth.token.foo === 'bar' in the security rules to something like auth.token.foo === 'bear'
        // The write access to "/UsersData/Node1/..." with current claims will be denied too because our claim foo i.e. {"foo":"bar"}
        // does not match the auth.token.foo variable.
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
 * @param basePath The parent path of child's node that the .read and .write rules are being set.
 * @param node The child node key in security rules to set the .read and .write rules.
 * @param readVal The child node key .read value to set.
 * @param writeVal The child node key .write value to set.
 * @param databaseSecret The database secret.
 */
bool mofifyRules(const String &basePath, const String &node, const String &readVal, const String &writeVal, const String &databaseSecret)
{
    // Use database secret for to allow for security rules access.
    // The ServiceAuth (OAuth2.0 access token authorization) also can be used for authorization
    // but for simple demonstration and testing the database secret will be used.

    // The final result is the following rules are added, which you also can add it manually.
    //     "UsersData": {
    //         "$userId": {
    //             ".read": "($userId === auth.uid && auth.token.premium_account === true && auth.token.admin === true && auth.token.foo === 'bar')",
    //             ".write": "($userId === auth.uid && auth.token.premium_account === true && auth.token.admin === true && auth.token.foo === 'bar')"
    //                     }
    //                 }

    // If our claims set to CustomAuth object e.g. premium_account is not true, admin is not true and foo is not bar, the access to the database will fail.

    LegacyToken legacy_token(databaseSecret);
    initializeApp(aClient, app, getAuth(legacy_token));
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);

    Serial.print("Getting the security rules to check the existing keys... ");

    String jsonStr = Database.get<String>(aClient, ".settings/rules");

    // The Security Rules are ready to check and modify.
    if (aClient.lastError().code() == 0)
    {
        Serial.println("ok");

        bool ret = true;
        FirebaseJsonData parseResult;
        FirebaseJson currentRules(jsonStr);
        bool readKeyExists = false, writeKeyExists = false;

        String rulePath = basePath.length() && basePath[0] != '/' ? "rules" : "rules/";
        rulePath += basePath;
        rulePath += "/";
        rulePath += node;

        // Check the .read key exists in rules/<path>/<var>
        if (readVal.length() > 0)
        {
            String readPath = rulePath;
            readPath += "/.read";
            if (currentRules.get(parseResult, readPath.c_str()) && strcmp(parseResult.to<const char *>(), readVal.c_str()) == 0)
                readKeyExists = true;
        }

        // Check the .write key exists in rules/<path>/<var>
        if (writeVal.length() > 0)
        {
            String writePath = rulePath;
            writePath += "/.write";
            if (currentRules.get(parseResult, writePath.c_str()) && strcmp(parseResult.to<const char *>(), writeVal.c_str()) == 0)
                writeKeyExists = true;
        }

        // Add our read/write keys if they do not exist.
        if (!readKeyExists || !writeKeyExists)
        {
            FirebaseJson addedRules;
            if (!readKeyExists)
                addedRules.add(".read", readVal);
            if (!writeKeyExists)
                addedRules.add(".write", writeVal);

            currentRules.set(rulePath, addedRules);

            String modifiedRules;
            currentRules.toString(modifiedRules, true);

            Serial.print("Setting the security rules to add the modified rules... ");
            bool status = Database.set<object_t>(aClient, ".settings/rules", object_t(modifiedRules));
            if (status)
                Serial.println("ok");
            else
                printError(aClient.lastError().code(), aClient.lastError().message());
        }

        currentRules.clear();
        return aClient.lastError().code() == 0;
    }
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    return false;
}