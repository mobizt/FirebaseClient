/**
 * The bare minimum code example for using the Realtime Database service in a synchronous mode.
 *
 * The generally required steps are explained below. This example is best suited for simple, single-threaded applications.
 * For multi-tasking or RTOS environments, please see the `RealTimeDatabase_Async` example.
 *
 * Step 1. Include the required Firebase defines and libraries.
 * =================================================================
 *
 * Step 2. Define the authentication configuration (identifier) object.
 * ======================================================================
 * UserAuth (user authentication with email/password) is the basic authentication method for the Realtime Database,
 * Firebase Storage, and Firestore services. It stores the email, password, and API key for the authentication process.
 *
 * Step 3. Define the authentication handler object.
 * ==================================================
 * The FirebaseApp object works as the authentication handler.
 * It manages the authentication process and token refresh cycles.
 *
 * Step 4. Define the SSL client.
 * ================================
 * The SSL client handles the server connection and data transfer.
 *
 * Step 5. Define the Async Client.
 * ================================
 * The library's synchronous functions still require an AsyncClient object.
 *
 * Step 6. Define the class that provides the Firebase/Google Cloud service.
 * ============================================================================
 * The Firebase/Google Cloud service classes provide the member functions for database operations.
 *
 * Step 7. Start the authentication process.
 * ==========================================
 * At this step, the authentication credentials will be used by `initializeApp` to begin
 * the authentication process.
 *
 * Step 8. Bind the FirebaseApp (authentication handler) with your Firebase/Google Cloud service object.
 * =========================================================================================================
 *
 * Step 9. Set the Realtime Database URL (for the Realtime Database only).
 * ========================================================================
 *
 * Step 10. Maintain authentication in the loop.
 * =============================================
 * `app.loop()` is required to manage the authentication token refresh cycle.
 *
 * Step 11. Check the authentication status before use.
 * ====================================================
 * Before calling a Firebase service function, the `FirebaseApp::ready()` function should return true.
 *
 * Step 12. Call the Firebase service functions synchronously.
 * ===========================================================
 * In synchronous mode, the function call (e.g., `Database.set<String>(...)`) will not return until the server has responded.
 * This blocks the execution of your code, but simplifies the logic.
 */

// Step 1
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// Step 2
UserAuth user_auth("WEB_API_KEY", "UESR_EMAIL", "USER_PASSWORD");

// Step 3
FirebaseApp app;

// Step 4
// Use SSL client for sync tasks.
WiFiClientSecure ssl_client;

// Step 5
// Use AsyncClient for sync tasks.
using AsyncClient = AsyncClientClass;
AsyncClient async_client(ssl_client);

// Step 6
RealtimeDatabase Database;

bool onetimeTest = false;


void setup()
{
    Serial.begin(115200);

    WiFi.begin("WIFI_AP", "WIFI_PASSWORD");

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

    // Skip certificate verification
    ssl_client.setInsecure();

    // Note: The setConnectionTimeout and setHandshakeTimeout functions are
    // deprecated in modern ESP32 cores and may cause compiler errors.
    // They are left here for compatibility with older core versions.
    ssl_client.setConnectionTimeout(1000);
    ssl_client.setHandshakeTimeout(5);

    // Step 7
    initializeApp(async_client, app, getAuth(user_auth), NULL, "authTask");

    // Step 8
    app.getApp<RealtimeDatabase>(Database);

    // Step 9
    Database.url("DATABASE_URL");
}

void loop()
{
    // Step 10
    app.loop();

    // Step 11
    if (app.ready() && !onetimeTest)
    {
        onetimeTest = true;

        // Step 12
        // Realtime Database set value (Synchronous).
        // ===========================================
        // This is a blocking call. Your code will pause here until the
        // server responds or the operation times out.
        bool status = Database.set<double>(async_client, "/examples/BareMinimum/data/set3", 199.538739);
        if (status)
            Serial.println("Value set complete.");
        else
            Firebase.printf("Error, msg: %s, code: %d\n", async_client.lastError().message().c_str(), async_client.lastError().code());

        // Realtime Database get value (Synchronous).
        // ===========================================
        // This is a blocking call.
        String value = Database.get<String>(async_client, "/examples/BareMinimum/data/set3");
        if (async_client.lastError().code() == 0)
        {
            Serial.println("Value get complete.");
            Serial.println(value);
        }
        else
            Firebase.printf("Error, msg: %s, code: %d\n", async_client.lastError().message().c_str(), async_client.lastError().code());
    }
}
