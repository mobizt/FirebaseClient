/**
 * The example to sign up anonymously as a guest.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

unsigned long ms = 0;
int cnt = 0;

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

    UserAccount user(API_KEY);

    set_ssl_client_insecure_and_buffer(ssl_client);

    Serial.println("Anonymous signing up...");

    // For anonymous sign up.
    signup(aClient, app, getAuth(user), auth_debug_print, "signupTask");

    // Or anonymous sign up and wait.
    // signup(aClient, app, getAuth(user), 120 * 1000, auth_debug_print);
}

void loop()
{
    // To maintain the authentication and async tasks.
    app.loop();

    if (app.ready() && (millis() - ms > 30 * 1000 || ms == 0))
    {
        cnt++;
        ms = millis();

        Serial.println("App is authenticated as anonymous...");
        Firebase.printf("User UID: %s\n", app.getUid().c_str());

        app.isAuthenticated();

        if (cnt == 3)
        {
            Serial.println("Deleting anonymous user...");
            UserAccount user(API_KEY);
            deleteUser(aClient, app, getAuth(user.idToken(app.getToken())), auth_debug_print, "deleteUserTask");
            
            // Or delete user and wait.
            // deleteUser(aClient, app, getAuth(user.idToken(app.getToken())), 120 * 1000, auth_debug_print);
        }
    }
}