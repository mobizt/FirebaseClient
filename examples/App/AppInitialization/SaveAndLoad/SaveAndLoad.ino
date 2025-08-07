/**
 * The example for storing the authentication credentials to file and loading it for later use.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_FS

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

FileConfig user_auth_file("/user_auth.txt", file_operation_callback);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);

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

#if defined(ENABLE_FS)
    MY_FS.begin();
#endif

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    set_ssl_client_insecure_and_buffer(ssl_client);

#if defined(ENABLE_FS)
    // Check if auth file exists.
    if (MY_FS.exists(getFile(user_auth_file).filename.c_str()))
    {
        // Load auth config from file.
        UserAuth user_auth_file(getFile(user_auth_file));
        Serial.println("Initializing app...");
        initializeApp(aClient, app, getAuth(user_auth_file), auth_debug_print, "🔐 authTask");
    }
    else
    {
        // Load auth from data.
        Serial.println("Initializing app...");
        initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");
        // Save auth to file.
        user_auth.save(getFile(user_auth_file));
    }
#else
    // Load auth from data.
    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "🔐 authTask");
#endif
}

void loop()
{
    // To maintain the authentication and async tasks.
    app.loop();
}