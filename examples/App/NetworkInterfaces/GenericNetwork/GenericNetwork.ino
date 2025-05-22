/**
 * ABOUT:
 *
 * The bare minimum example for working with any networks.
 *
 * The divices used in this example are WIZnet W5500 SPI Ethernet module and ESP32.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * GenericNetwork::GenericNetwork(<net_connect_callback>, <network_status_callback>);
 *
 * <net_connect_callback> - The network connection callback function.
 * <network_status_callback> - The network status callback function.
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

void netconnect();
void netStatus(bool &status);

GenericNetwork generic_network(netconnect, netStatus);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(generic_network));

void setup()
{
  Serial.begin(115200);

  netconnect();

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

void netconnect()
{
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
}

void netStatus(bool &status)
{
  status = WiFi.status() == WL_CONNECTED;
}