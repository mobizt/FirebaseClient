/**
 * ABOUT:
 *
 * The bare minimum example for working with multi-AP WiFi network
 * on any WiFi capable devices.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * DefaultWiFiNetwork::DefaultWiFiNetwork(<FirebaseWiFi>, <re_connect_option>);
 *
 * <FirebaseWiFi> - The FirebaseWiFi class object that used for keeping the network credentials (WiFi AP and WiFi password).
 * <re_connect_option> - The Boolean value set for enabling the WiFi reconnection when the WiFi is disconnected.
 *
 * This type of network class is for WiFi Multi or device that use WiFi class that does not support WiFi reconnection.
 *
 * For normal WiFi, see examples/NetworkInterfaces/DefaultNetwork/DefaultNetwork.ino
 */

// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
#define ENABLE_USER_CONFIG
#define ENABLE_USER_AUTH

// For network independent usage (disable all network features).
// #define DISABLE_NERWORKS

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID1 "WIFI_AP1"
#define WIFI_PASSWORD1 "WIFI_PASSWORD1"

#define WIFI_SSID2 "WIFI_AP2"
#define WIFI_PASSWORD2 "WIFI_PASSWORD2"

#define WIFI_SSID3 "WIFI_AP3"
#define WIFI_PASSWORD3 "WIFI_PASSWORD3"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

FirebaseWiFi wifimulti;

DefaultWiFiNetwork default_network(wifimulti, true /* reconnect network */);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(default_network));

void setup()
{
    Serial.begin(115200);

    wifimulti.addAP(WIFI_SSID1, WIFI_PASSWORD1);
    wifimulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
    wifimulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);

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