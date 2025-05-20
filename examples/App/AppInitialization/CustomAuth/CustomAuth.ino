/**
 * ABOUT:
 *
 * The bare minimum example for using user authentication with custom UID and claims.
 *
 * This example requires the service account credentials that obtains from the JSON key file.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * CustomAuth::CustomAuth(<api_key>, <client_email>, <project_id>, <private_key>, <user_id>, <scope>, <claims>, <expire>);
 *
 * <api_key> - The web API key of project.
 * <client_email> - The service account client Email.
 * <project_id> - The service account project ID.
 * <private_key> - The service account private key.
 * <user_id> - The unique identifier of the signed-in user must be a string, between 1-128 characters long, inclusive. Shorter uids offer better performance.
 * <claims> - Optional custom claims to include in the Security Rules auth / request.auth variables.
 * <expire> - The expiry period in seconds (less than 3600), 3300 is the default value.
 *
 * 2.------------------------
 *
 * CustomAuth::CustomAuth(<TimeStatusCallback>, <api_key>, <client_email>, <project_id>, <private_key>, <user_id>, <scope>, <claims>, <expire>);
 *
 * <TimeStatusCallback> - The time status callback that provide the UNIX timestamp value used for JWT token signing.
 * <api_key> - The web API key of project.
 * <client_email> - The service account client Email.
 * <project_id> - The service account project ID.
 * <private_key> - The service account private key.
 * <user_id> - The user ID.
 * <scope> - The OAuth scopes.
 * <claims> - The OAuth claims. For more details about claims, please visit https://firebase.google.com/docs/auth/admin/custom-claims.
 * <expire> - The expiry period in seconds (less than 3600), 3300 is the default value.
 *
 * See examples/RealtimeDatabase/AccessControl/AccessControl.ino for how it can work with security rules for database access control.
 *
 * 3.------------------------
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
 * 4.------------------------
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
 * The valid time is required for private key signing process in JWT token generation.
 * In case TimeStatusCallback was not assigned in the CustomAuth class constructor,
 * use FirebaseApp::setTime before calling initializeApp.
 *
 * The timeoutMs is the timeout for waiting the authentication process to be done.
 *
 * By providing AsyncResultCallback in the initializeApp function, the debug information will be collected and send to AsyncResultCallback
 * function immediately.
 */


// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
#define ENABLE_USER_CONFIG
#define ENABLE_CUSTOM_AUTH

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

CustomAuth custom_auth(API_KEY, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, "user" /* UID */, "" /* claims */, 3600 /* expire period in seconds (<3600) */);

FirebaseApp app;

SSL_CLIENT ssl_client;

// This uses built-in core WiFi/Ethernet for network connection.
// See examples/App/NetworkInterfaces for more network examples.
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

    // Assign the valid time only required for authentication process with ServiceAuth and CustomAuth.
    app.setTime(get_ntp_time());

    String claims = "{\"premium_account\": true,\"admin\": true}";
    custom_auth.setClaims(claims);

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(custom_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(custom_auth), 120 * 1000, auth_debug_print);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && !taskComplete)
    {
        taskComplete = true;
        print_token_type(app);
    }
}