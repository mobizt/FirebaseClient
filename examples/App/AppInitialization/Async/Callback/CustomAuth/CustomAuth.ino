/**
 * ABOUT:
 *
 * The bare minimum non-blocking (async) example for Firebase custom user (ID token) authentication with custom UID and claims.
 *
 * This example requires the service account credentials that obtains from the JSON key file.
 *
 * This example uses the DefaultNetwork class for network interface configuration.
 * See examples/App/NetworkInterfaces for more network examples.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * initializeApp(<AsyncClient>, <FirebaseApp>, <user_auth_data>);
 *
 * <AsyncClient> - The async client.
 * <FirebaseApp> - The authentication and access token handler.
 * <user_auth_data> - The user auth data (user_auth_data struct) that holds the user input sign-in credentials and token.
 *
 * The <user_auth_data> can be obtained from the following sign-in credentials, access key, auth token providers classs via getAuth function i.e.
 * CustomAuth, ServiceAuth, UserAuth, NoAuth, CustomToken, AccessToken, IDToken, LegacyToken.
 *
 * 2.------------------------
 *
 * CustomAuth::CustomAuth(<TimeStatusCallback>, <api_key>, <client_email>, <project_id>, <private_key>, <user_id>, <scope>, <claims>, <expire>);
 *
 * <TimeStatusCallback> - The time status callback that provide the UNIX timestamp value used for JWT token signing.
 * <api_key> - The web API key of project.
 * <client_email> - The service account client Email.
 * <project_id> - The service account project ID.
 * <private_key> - The service account private key.
 * <user_id> - The user ID.
 * <scope> - The OAuth scopes.
 * <claims> - The OAuth claims. For more details about claims, please visit https://firebase.google.com/docs/auth/admin/custom-claims.
 * <expire> - The expiry period in seconds (less than 3600), 3300 is the default value.
 *
 * See examples/RealtimeDatabase/Sync/CustomClaims/CustomClaims.ino for how it can work with security rules for database access control.
 *
 *  3.------------------------
 *
 * CustomAuth::CustomAuth(<timestamp>, <api_key>, <client_email>, <project_id>, <private_key>, <user_id>, <scope>, <claims>, <expire>);
 *
 * <timestamp> - The current UNiX timestamp (seonds since Jan 1, 1970 midnight).
 * <api_key> - The web API key of project.
 * <client_email> - The service account client Email.
 * <project_id> - The service account project ID.
 * <private_key> - The service account private key.
 * <user_id> - The unique identifier of the signed-in user must be a string, between 1-128 characters long, inclusive. Shorter uids offer better performance.
 * <claims> - Optional custom claims to include in the Security Rules auth / request.auth variables.
 * <expire> - The expiry period in seconds (less than 3600), 3300 is the default value.
 *
 * NOTE:
 *
 * To reduce the stack usage of BearSSL engine crpto function, the JWT token creation process
 * will be performed outside the FirebaseApp.
 *
 * The JWT token processor required for ServiceAuth and CustomAuth authentications.
 *
 * JWTClass::loop(<auth_data_t *>)
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

/**
 * This information can be taken from the service account JSON file.
 *
 * To download service account file, from the Firebase console, goto project settings,
 * select "Service accounts" tab and click at "Generate new private key" button
 */
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

void timeStatusCB(uint32_t &ts);

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

// Don't forget to call firebase::JWTClass::loop() in the main loop(), otherwise the JWT token process cannot be started.
CustomAuth custom_auth(timeStatusCB, API_KEY, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, "myId" /* UID */, "" /* claims */, 3600 /* expire period in seconds (<3600) */);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA) || defined(ARDUINO_PORTENTA_C33) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#else
ESP_SSLClient ssl_client;
#endif

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

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

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    // We will set the claims to the token we used here (ID token using CustomAuth).
    // We set the values of the claims to math the auth.token varibles values in the security rules condition checking.
    // The claims string must be JSON serialized string when passing to CustomAuth::setClaims or CustomAuth class constructor.

    // For more details about custom claims, please see https://firebase.google.com/docs/auth/admin/custom-claims.
    // See examples/RealtimeDatabase/Sync/CustomClaims/CustomClaims.ino for Realtime Database use case.

    String claims = "{\"premium_account\": true,\"admin\": true}";
    custom_auth.setClaims(claims);

    // Initialize the FirebaseApp or auth task handler.
    // To deinitialize, use deinitializeApp(app).
    Serial.println("Initializing the app...");
    initializeApp(aClient, app, getAuth(custom_auth), asyncCB, "authTask");
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

    // To get the authentication time to live in seconds before expired.
    // app.ttl();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;
        Firebase.printf("Auth Token: %s\n", app.getToken().c_str());
        firebase_token_type type = app.getTokenType();
        if (type == token_type_access)
            Serial.println("Token Type: access token");
        else if (type == token_type_id)
            Serial.println("Token Type: ID token");
        else if (type == token_type_legacy)
            Serial.println("Token Type: legacy token");
        else if (type == token_type_no)
            Serial.println("Token Type: no token");
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

void asyncCB(AsyncResult &aResult) { printResult(aResult); }

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
