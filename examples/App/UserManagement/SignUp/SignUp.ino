/**
 * ABOUT:
 *
 * The example to sign up a new user.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * signup(<AsyncClient>, <FirebaseApp>, <user_auth_data>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <FirebaseApp> - The FirebaseApp class object to handle authentication/authorization task.
 * <user_auth_data> - The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
 */

// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
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

    UserAccount user(API_KEY);

    set_ssl_client_insecure_and_buffer(ssl_client);

    Serial.println("Signing up...");
    signup(aClient, app, getAuth(user.email("user@gmail.com").password("user_password")), auth_debug_print, "signupTask");

    // Or sign up and wait.
    // signup(aClient, app, getAuth(user.email("user@gmail.com").password("user_password")), 120 * 1000, auth_debug_print);
}

void loop()
{
    // To maintain the authentication and async tasks.
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;
        Firebase.printf("UID: %s\nID token: %s\nRefresh token: %s\n", app.getUid().c_str(), app.getToken().c_str(), app.getRefreshToken().c_str());
    }
}