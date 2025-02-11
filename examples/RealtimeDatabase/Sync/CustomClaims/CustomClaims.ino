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
 * https://firebase.google.com/docs/database/security/rules-conditions
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

bool mofifyRules(const String &parentPath, const String &node, const String &readCondition, const String &writeCondition, const String &databaseSecret);

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

    // The following process will modify the Realtime Database security rules to allow the read/write accesses only at UsersData/$userId
    // if the auth.token variables are match the custom claims we set.
    //
    // Here is the final security rules we want to set in this example.
    //
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
    //              }
    //       }
    // }
    //
    // We use $ variable in the rules e.g. #userId to capture the path segment. If we access database at /UserData/xyz, the xyz well be our $userId variable in the rules.
    // When the $userId was used in the condition e.g. "$userId === auth.uid", the result will be false for this xyz UID case.
    // This will allow us to access the database only at /UserData/Node1.
    // Then the user with provided UID e.g. xyz can only access to database at /UserData/xyz because the UID obtains from auth.uid variable
    // matches the path segment variable $userId.
    //
    // In our rules above at the path UsersData/$userId, other auth variables are also checked
    // i.e. auth.token which the admin's, premium_account's and foo's values are checked for maching.
    // If one of these condition is false, the access to UsersData/$userId will be denied.
    // The auth.token.??? is variable that taken from the ID token's claims.
    //
    // For more information, visit https://firebase.google.com/docs/database/security/rules-conditions#using_variables_to_capture_path_segments
    //
    // =========
    // IMPORTANT
    // =========
    // To allow read/write access only the conditions we set above,
    // you have to remove the following rules from your security rules (if it exists).
    // ".read": "auth != null"
    // ".write": "auth != null"
    // ".read": "true"
    // ".write": "true"
    // ".read": "some other conditions that allow access by date"
    // ".write": "some other conditions that allow access by date"

    String parentPath = "/UsersData";
    String node = "$userId";
    String readConditions = "($userId === auth.uid && auth.token.premium_account === true && auth.token.admin === true && auth.token.foo === 'bar')";
    String writeConditions = readConditions;

    mofifyRules(parentPath, node, readConditions, writeConditions, DATABASE_SECRET);

    // We will set the claims to the token we used here (ID token using CustomAuth).
    // We set the values of the claims to math the auth.token varibles values in the security rules condition checking.
    // The claims string must be JSON serialized string when passing to CustomAuth::setClaims or CustomAuth class constructor.

    // For more details about custom claims, please see https://firebase.google.com/docs/auth/admin/custom-claims.
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

        // If you try to change the claim value e.g. from {"foo":"bar"} to {"foo":"bear"} the write and read accesses will be denied.
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
 * @param parent The parent path of child's node that the which we want to control access.
 * @param child The child node path which we want to control access.
 * @param readCondition The read access condition.
 * @param writeCondition The write access condition.
 * @param databaseSecret The database secret.
 */
bool mofifyRules(const String &parentPath, const String &child, const String &readCondition, const String &writeCondition, const String &databaseSecret)
{
    // Use database secret for to allow security rules access.
    // The ServiceAuth (OAuth2.0 access token authorization) can also be used
    // but database secret is more simple for this case.

    LegacyToken legacy_token(databaseSecret);
    initializeApp(aClient, app, getAuth(legacy_token));
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);

    Serial.print("Getting the security rules to check the existing conditions... ");

    String jsonStr = Database.get<String>(aClient, ".settings/rules");

    // If security rules are ready get.
    if (aClient.lastError().code() == 0)
    {
        Serial.println("ok");

        bool ret = true;
        FirebaseJsonData parseResult;
        FirebaseJson currentRules(jsonStr);
        bool readConditionExists = false, writeConditionExists = false;

        String rulePath = "rules";

        if (parentPath.length() && parentPath[0] != '/')
            rulePath += '/';

        rulePath += parentPath;

        if (rulePath[rulePath.length() - 1] == '/')
            rulePath[rulePath.length() - 1] = 0;

        if (child.length() && child[0] != '/' && rulePath[rulePath.length() - 1] != '/')
            rulePath += '/';

        rulePath += child;

        // Check the read condition exists or matches
        if (readCondition.length() > 0)
        {
            String readPath = rulePath;
            readPath += "/.read";
            if (currentRules.get(parseResult, readPath.c_str()) && strcmp(parseResult.to<const char *>(), readCondition.c_str()) == 0)
                readConditionExists = true;
        }

        // Check the write condition exists or matches
        if (writeCondition.length() > 0)
        {
            String writePath = rulePath;
            writePath += "/.write";
            if (currentRules.get(parseResult, writePath.c_str()) && strcmp(parseResult.to<const char *>(), writeCondition.c_str()) == 0)
                writeConditionExists = true;
        }

        // Add conditions if they do not exist.
        if (!readConditionExists || !writeConditionExists)
        {
            FirebaseJson addedRules;
            if (!readConditionExists)
                addedRules.add(".read", readCondition);
            if (!writeConditionExists)
                addedRules.add(".write", writeCondition);

            currentRules.set(rulePath, addedRules);

            String modifiedRules;
            currentRules.toString(modifiedRules, true);

            Serial.print("Setting the security rules to add the modified conditions... ");
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