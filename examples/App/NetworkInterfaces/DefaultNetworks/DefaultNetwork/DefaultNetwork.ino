/**
 * ABOUT:
 *
 * The bare minimum example for working with the native WiFi network on any WiFi capable devices.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * DefaultNetwork::DefaultNetwork(<re_connect_option>);
 *
 * <re_connect_option> - The Boolean value set for enabling the WiFi reconnection when the WiFi is disconnected.
 *
 * For WiFi Multi, see examples/NetworkInterfaces/DefaultWiFiNetwork/DefaultWiFiNetwork.ino
 *
 * For native or board built-in Ethernet, see examples/NetworkInterfaces/DefaultEthernetNetwork/DefaultEthernetNetwork.ino
 */

// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
#define ENABLE_USER_CONFIG
#define ENABLE_USER_AUTH

// For network independent usage (disable all network features).
// #define DISABLE_NERWORKS

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, true); // The second parameter is for network reconnection

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
}

void loop()
{
    // To maintain the authentication and async tasks.
    app.loop();
}