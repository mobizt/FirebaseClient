/**
 * This is example for how to disable the re-authentication and force the authentication processes.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 300 /* expired in 300 sec */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

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

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);

    // Disable auto authentication
    // From UserAuth class constructor defined above, the expire period was 300 seconds (5 min).
    // Then the library will not re-authenticate after 300 seconds or auth token was expired (60 minutes).
    // The auth token will be expired in 60 minutes.
    app.autoAuthenticate(false);
}

void loop()
{
    // To maintain the authentication and async tasks.
    app.loop();

    if (app.ready())
    {
        // We force the auth token to be refreshed after 60 seconds.
        if (app.ttl() <= 300 - 60)
        {
            app.authenticate();
        }
    }
}