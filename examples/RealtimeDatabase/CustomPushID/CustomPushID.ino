/**
 * The example to simulate push operation with set and UUID.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

void processData(AsyncResult &aResult);
String genUUID();

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult databaseResult;

unsigned long ms = 0;

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

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && (ms == 0 || millis() - ms > 60000))
    {
        ms = millis();

        String path = "/examples/CustomPush/data2/" + genUUID();

        // Push the int value

        Serial.println("Pushing the int value... ");
        String name = Database.push<int>(aClient, "/examples/CustomPush/data1", 12345);
        if (aClient.lastError().code() == 0)
            Firebase.printf("Success, Name: %s\n", name.c_str());
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

        Serial.println("Pushing (setting) the int value with custom UUID... ");

        // Set the int value with custom UUID (your simulate push operation)
        bool status = Database.set<int>(aClient, path, 12345);
        if (status)
            Serial.println("Success");
        else
           Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    }
}

void processData(AsyncResult &aResult)
{
    // Exits when no result is available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        if (aResult.to<RealtimeDatabaseResult>().name().length())
            Firebase.printf("task: %s, name: %s\n", aResult.uid().c_str(), aResult.to<RealtimeDatabaseResult>().name().c_str());
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

String genUUID()
{
    // This is how Firebase generate UUID when you calling Push.

    // Push UUID generator, https://gist.github.com/mikelehen/3596a30bd69384624c11

    /**
     * Fancy ID generator that creates 20-character string identifiers with the following properties:
     *
     * 1. They're based on timestamp so that they sort *after* any existing ids.
     * 2. They contain 72-bits of random data after the timestamp so that IDs won't collide with other clients' IDs.
     * 3. They sort *lexicographically* (so the timestamp is converted to characters that will sort properly).
     * 4. They're monotonically increasing.  Even if you generate more than one in the same timestamp, the
     *    latter ones will sort after the former ones.  We do this by using the previous random bits
     *    but "incrementing" them by 1 (only in the case of a timestamp collision).
     */

    // Modeled after base64 web-safe chars, but ordered by ASCII.
    static char PUSH_CHARS[] = "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";

    // Timestamp of last push, used to prevent local collisions if you push twice in one ms.
    static long long lastPushTime = 0;

    // We generate 72-bits of randomness which get turned into 12 characters and appended to the
    // timestamp to prevent collisions with other clients.  We store the last characters we
    // generated because in the event of a collision, we'll use those same characters except
    // "incremented" by one.
    char lastRandChars[72] = "";
    char timeStampChars[9] = "";

    uint32_t ts = get_ntp_time();

    long long now = ts * 1000LL;

    srand(now);

    bool duplicateTime = (now == lastPushTime);
    lastPushTime = now;

    for (int i = 7; i >= 0; i--)
    {
        timeStampChars[i] = PUSH_CHARS[(int)(now % 64)];
        now = now / 64;
    }

    // We should have converted the entire timestamp.
    if (now != 0)
        return String();

    timeStampChars[8] = '\0';

    String id = timeStampChars;

    if (!duplicateTime)
    {
        for (int i = 0; i < 12; i++)
        {
            double fl = ((double)rand() / (double)(RAND_MAX + 1.0)) * 64;
            lastRandChars[i] = (char)floor(fl);
        }
    }
    else
    {
        // If the timestamp hasn't changed since last push, use the same random number, except incremented by 1.
        int val = 0;
        for (int i = 11; i >= 0 && lastRandChars[i] == 63; i--)
        {
            val = i;
            lastRandChars[i] = 0;
        }

        if (val >= 0)
            lastRandChars[val]++;
    }

    for (int i = 0; i < 12; i++)
        id += PUSH_CHARS[(int)lastRandChars[i]];

    // Length should be 20.
    if (id.length() != 20)
        return String();

    return id;
}