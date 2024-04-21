
/**
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

#define DATABASE_SECRET "DATABASE_SECRET"
#define DATABASE_URL "URL"

void asyncCB(AsyncResult &aResult);

void printError(int code, const String &msg);

void insertJson(String &json, const String &value);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

LegacyToken legacy_token(DATABASE_SECRET);
// or
// ServiceAuth sa_auth(timeStatusCB, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY);

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

    initializeApp(aClient, app, getAuth(legacy_token));

    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    /** If access token or ServiceAuth used

        unsigned long ms = millis();
        while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        {
            // This JWT token process required for ServiceAuth and CustomAuth authentications
            if (app.isJWT())
                JWT.process(app.getAuth());
        }

    */

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    Serial.print("Get security rules... ");

    String json_rules = Database.get<String>(aClient, ".settings/rules");

    if (aClient.lastError().code() == 0)
    {
        Serial.println("Ok");

        // For indexing data doc. please see https://firebase.google.com/docs/database/security/indexing-data

        // The path to index: /test/filter/json
        // The node name to index: Data

        // Library does not provide JSON parser library, the following JSON writer class will be used with
        // object_t for simple demonstration.

        object_t json;
        JsonWriter writer;
        writer.create(json, "test/filter/json/.indexOn", "Data"); // -> {"test":{"filter":{"json":{".indexOn":"Data"}}}}

        String new_indexon_rules = json.c_str();

        // Remove { and } before insert. -> "test":{"filter":{"json":{".indexOn":"Data"}}}
        new_indexon_rules.remove(0, 1);
        new_indexon_rules.remove(new_indexon_rules.length() - 1, 1);

        insertJson(json_rules, new_indexon_rules);

        Serial.print("Set security rules... ");
        bool status = Database.set<object_t>(aClient, ".settings/rules", object_t(json_rules));

        if (status)
            Serial.println("Ok");
        else
            printError(aClient.lastError().code(), aClient.lastError().message());
    }
    else
        printError(aClient.lastError().code(), aClient.lastError().message());
}

void loop()
{
    // This JWT token process required for ServiceAuth and CustomAuth authentications
    // if (app.isJWT())
    //    JWT.process(app.getAuth());

    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // This required when different AsyncClients than used in FirebaseApp assigned to the Realtime database functions.
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

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void insertJson(String &json, const String &value)
{
    int p1 = json.indexOf("\".write\""); // Find pos of ".write" in the database rules

    if (p1 > -1)
    {
        int p2 = json.indexOf(",", p1); // Find pos of , after ".write"
        if (p2 == -1)
            p2 = json.indexOf("}", p1); // Or if it is the last node, find pos of } after ".write"

        String part1 = json.substring(0, p2);
        String part2 = json.substring(p2);

        json = part1 + "," + value + part2; // Insert the new rule between two sections.
    }
}
