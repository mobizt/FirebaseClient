/**
 * ABOUT:
 *
 * The bare minimum example for using user authentication with email and password.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
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
 *
 * 2.------------------------
 *
 * initializeApp(<AsyncClient>, <FirebaseApp>, <user_auth_data>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <FirebaseApp> - The authentication and access token handler.
 * <user_auth_data> - The user auth data (user_auth_data struct) that holds the user input sign-in credentials and token.
 *
 * The <user_auth_data> can be obtained from the following sign-in credentials, access key, auth token providers classs via getAuth function i.e.
 * CustomAuth, ServiceAuth, UserAuth, NoAuth, CustomToken, AccessToken, IDToken, LegacyToken.
 *
 * <AsyncResultCallback> - The AsyncResultCallback function for debugging.
 * <uid> - Optional The user defined task UID for debugging.
 *
 * 3.------------------------
 *
 * initializeApp(<AsyncClient>, <FirebaseApp>, <user_auth_data>, <timeoutMs>, <AsyncResultCallback>);
 *
 * <AsyncClient> - The async client.
 * <FirebaseApp> - The authentication and access token handler.
 * <user_auth_data> - The user auth data (user_auth_data struct) that holds the user input sign-in credentials and token.
 *
 * The <user_auth_data> can be obtained from the following sign-in credentials, access key, auth token providers classs via getAuth function i.e.
 * CustomAuth, ServiceAuth, UserAuth, NoAuth, CustomToken, AccessToken, IDToken, LegacyToken.
 *
 * <timeoutMs> - Optional. The await timeout in milliseconds.
 * <AsyncResultCallback> - Optional. The async result callback (AsyncResultCallback) for debugging.
 *
 * NOTE:
 *
 * The timeoutMs is the timeout for waiting the authentication process to be done.
 *
 * By providing AsyncResultCallback in the initializeApp function, the debug information will be collected and send to AsyncResultCallback
 * function immediately.
 */

// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
#define ENABLE_USER_AUTH

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

bool verifyUser(const String &apiKey, const String &email, const String &password);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

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

    set_ssl_client_insecure_and_buffer(ssl_client);

    // You can validate or verify user before Initializing app.
    Serial.print("Verifying the current user... ");
    bool ret = verifyUser(API_KEY, USER_EMAIL, USER_PASSWORD);
    Serial.println(ret ? "success" : "failed");

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);
}

void loop()
{
    // To maintain the authentication process.
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;

        // Print authentication info
        Serial.println("Authentication Information");
        Firebase.printf("User UID: %s\n", app.getUid().c_str());
        Firebase.printf("Auth Token: %s\n", app.getToken().c_str());
        Firebase.printf("Refresh Token: %s\n", app.getRefreshToken().c_str());
        print_token_type(app);
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