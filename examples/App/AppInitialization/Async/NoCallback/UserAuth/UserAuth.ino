/**
 * SYNTAX:
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
 * SYNTAX:
 *
 * UserAuth::UserAuth(<api_key>, <user_email>, <user_password>, <expire>);
 * UserAuth::UserAuth(<file_config_data>);
 * UserAuth::save(<file_config_data>);
 *
 * <api_key> - API key can be obtained from Firebase console > Project Overview > Project settings.
 * <user_email> - The user Email that in the project.
 * <user_password> - The user password in the project.
 * <expire> - The expiry period in seconds (less than 3600), 3300 is the default value.
 * <file_config_data> -  The filesystem data (file_config_data) obtained from FileConfig class object of file that the UserAuth credentials will be saved to or read from.
 *
 * To use other network interfaces, network data from one of the following Network classes
 * can be assigned.
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

void printResult(AsyncResult &aResult);

bool verifyUser(const String &apiKey, const String &email, const String &password);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);

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

AsyncResult aResult_no_callback;

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

    // You can validate or verify user before initializing the app.
    Serial.print("Verifying user... ");
    bool ret = verifyUser(API_KEY, USER_EMAIL, USER_PASSWORD);
    if (ret)
        Serial.println("ok");
    else
        Serial.println("failed");

    Serial.println("Initializing app...");

    // Initialize the FirebaseApp or auth task handler.
    // To deinitialize, use deinitializeApp(app).
    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;

        // Print authentication info
        Serial.println("Authentication Information");
        Firebase.printf("User UID: %s\n", app.getUid().c_str());
        Firebase.printf("Auth Token: %s\n", app.getToken().c_str());
        Firebase.printf("Refresh Token: %s\n", app.getRefreshToken().c_str());
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
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

bool verifyUser(const String &apiKey, const String &email, const String &password)
{
    if (ssl_client.connected())
        ssl_client.stop();

    String host = "www.googleapis.com";
    bool ret = false;

    if (ssl_client.connect(host.c_str(), 443) > 0)
    {
        String payload = "{\"email\":\"";
        payload += email;
        payload += "\",\"password\":\"";
        payload += password;
        payload += "\",\"returnSecureToken\":true}";

        String header = "POST /identitytoolkit/v3/relyingparty/verifyPassword?key=";
        header += apiKey;
        header += " HTTP/1.1\r\n";
        header += "Host: ";
        header += host;
        header += "\r\n";
        header += "Content-Type: application/json\r\n";
        header += "Content-Length: ";
        header += payload.length();
        header += "\r\n\r\n";

        if (ssl_client.print(header) == header.length())
        {
            if (ssl_client.print(payload) == payload.length())
            {
                unsigned long ms = millis();
                while (ssl_client.connected() && ssl_client.available() == 0 && millis() - ms < 5000)
                {
                    delay(1);
                }

                ms = millis();
                while (ssl_client.connected() && ssl_client.available() && millis() - ms < 5000)
                {
                    String line = ssl_client.readStringUntil('\n');
                    if (line.length())
                    {
                        ret = line.indexOf("HTTP/1.1 200 OK") > -1;
                        break;
                    }
                }
                ssl_client.stop();
            }
        }
    }

    return ret;
}