/**
 * The bare minimum example for using user authentication with custom UID and claims.
 *
 * The service account JSON key file will be used in this example.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_CUSTOM_AUTH
#define ENABLE_FS

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

FileConfig sa_file("/sa_file.json", file_operation_callback);

CustomAuth custom_auth(getFile(sa_file), API_KEY, "myId");

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

#if defined(ENABLE_FS)
    MY_FS.begin();
#endif

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    set_ssl_client_insecure_and_buffer(ssl_client);

    // Assign the valid time only required for authentication process with ServiceAuth and CustomAuth.
    app.setTime(get_ntp_time());

    String claims = "{\"premium_account\": true,\"admin\": true}";
    custom_auth.setClaims(claims);

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(custom_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait (recommend for ESP8266).
    // initializeApp(aClient, app, getAuth(custom_auth), 120 * 1000, auth_debug_print);
}

void loop()
{
    // To maintain the authentication and async tasks.
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;
        print_token_type(app);
    }
}
