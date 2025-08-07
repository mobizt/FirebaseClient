/**
 * The example to show how to use custom claims and UID with security rules to control Firebase Realtime database access.
 *
 * We use the sync functions in this example because it is easy to describe the processes stype by step.
 *
 * For more details of using claims with security rules to control Firebase services accesses,
 * visit https://firebase.google.com/docs/auth/admin/custom-claims
 *
 * For security rules information.
 * https://firebase.google.com/docs/rules
 * https://firebase.google.com/docs/rules/rules-language
 * https://firebase.google.com/docs/database/security/rules-conditions
 *
 * This example uses the CustomAuth class for authentication for setting our custom UID and additional claims.
 *
 * The LegacyToken class (database secret) was used only for security rules read and modification process.
 *
 * The FirebaseJson library used in this example is available to install in IDE's Library Manager or
 * can be downloaded from https://github.com/mobizt/FirebaseJson.
 *
 * The syntaxes used in this example are described in example/App/AppInitialization/Sync/CustomAuth/CustomAuth.ino
 *
 * For the complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 */

#define ENABLE_CUSTOM_AUTH
#define ENABLE_LEGACY_TOKEN
#define ENABLE_DATABASE

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.
#include <FirebaseJson.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define FIREBASE_CLIENT_EMAIL "CLIENT_EMAIL"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

#define DATABASE_SECRET "DATABASE_SECRET"
#define DATABASE_URL "URL"

void processData(AsyncResult &aResult);
bool mofifyRules(const String &controlPath, const String &readCondition, const String &writeCondition, const String &databaseSecret);

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

CustomAuth custom_auth(API_KEY, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, "peter" /* UID */, "" /* claims can be set later */, 3600 /* expire period in seconds (<3600) */);
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

    // Assign the valid time only required for authentication process with ServiceAuth and CustomAuth.
    app.setTime(get_ntp_time());

    // The following process will modify the Realtime Database security rules to allow the read/write accesses only at UsersData/$userId
    // if the auth.token variables are match the custom claims we set.
    //
    // Here is the final security rules we want to set in this example.
    //
    // {
    // 	  "rules": {
    // 		  "examples": {
    // 			  "AccessControl": {
    // 				  "$resource": {
    // 					  "$group": {
    // 						  "$userId": {
    // 							  ".read": "($userId === auth.uid && auth.token.resource === $resource && auth.token.group === $group)",
    // 							  ".write": "($userId === auth.uid && auth.token.resource === $resource && auth.token.group === $group)"
    // 						  }
    // 					  }
    // 				  }
    // 			  }
    // 		  }
    // 	  }
    // }
    //
    // We use $ variable in the rules e.g. $resource, $group, and $userId to capture the path segment that are used to compare with the auth variables
    // that we set e.g. $userId will be compared with UID (auth.uid), $resource will be compared with resource claim (auth.token.resource),
    // and $group will be compared with group claim (auth.token.group).
    //
    // For more information, visit https://firebase.google.com/docs/database/security/rules-conditions#using_variables_to_capture_path_segments
    //
    // =========
    // IMPORTANT
    // =========
    // To allow read/write access only the conditions we set above,
    // you have to remove the following rules from your security rules (if it exists).
    // ".read": "auth != null"
    // ".write": "auth != null"
    // ".read": "true"
    // ".write": "true"
    // ".read": "some other conditions that allow access by date"
    // ".write": "some other conditions that allow access by date"

    String controlPath = "/examples/AccessControl/$resource/$group/$userId";
    String readConditions = "($userId === auth.uid && auth.token.resource === $resource && auth.token.group === $group)";
    String writeConditions = readConditions;

    mofifyRules(controlPath, readConditions, writeConditions, DATABASE_SECRET);

    // We will set the claims to the token we used here (ID token using CustomAuth).
    // We set the values of the claims to math the auth.token varibles values in the security rules conditions.
    // The claims string must be JSON serialized string when passing to CustomAuth::setClaims or CustomAuth class constructor.

    // For more details about custom claims, please see https://firebase.google.com/docs/auth/admin/custom-claims.

    // The resource claim value can be access via auth.token.resource in the security rules.
    // And group claim value can be access via auth.token.group.
    String claims = "{\"resource\":\"products\", \"group\":\"user\"}";
    custom_auth.setClaims(claims);

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(custom_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(custom_auth), 120 * 1000, auth_debug_print);

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && (ms == 0 || millis() - ms > 10000))
    {
        ms = millis();

        // From the UID, claims and security rules we have set,
        // it only allows us to access at examples/Access/products/user/peter/...
        // Because the resource in the claim is products and group claim is user.

        String path = "examples/AccessControl/products/user/";
        path += app.getUid();
        path += "/value";

        Serial.print("Setting the int value to the granted path... ");
        // This should be ok.
        bool status = Database.set<int>(aClient, path, 12345);
        if (status)
            Serial.println("ok");
        else
            Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

        Serial.print("Setting the int value to other locations... ");
        // This should be failed because we write to the path that is not allowed.
        status = Database.set<int>(aClient, "/examples/Set/Int", 12345);
        if (status)
            Serial.println("ok");
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
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
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

/**
 * @param controlPath The path that the which we want to control access.
 * @param readCondition The read access condition.
 * @param writeCondition The write access condition.
 * @param databaseSecret The database secret.
 */
bool mofifyRules(const String &controlPath, const String &readCondition, const String &writeCondition, const String &databaseSecret)
{
    // Use database secret for to allow security rules access.
    // The ServiceAuth (OAuth2.0 access token authorization) can also be used
    // but database secret is more simple for this case.

    LegacyToken legacy_token(databaseSecret);
    initializeApp(aClient, app, getAuth(legacy_token));
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);

    Serial.println("Getting the security rules to check the existing conditions... ");

    String jsonStr = Database.get<String>(aClient, ".settings/rules");

    // If security rules are ready get.
    if (aClient.lastError().code() == 0)
    {
        Serial.println("Success");

        bool ret = true;
        FirebaseJsonData parseResult;
        FirebaseJson currentRules(jsonStr);
        bool readConditionExists = false, writeConditionExists = false;

        String rulePath = "rules";

        if (controlPath.length() && controlPath[0] != '/')
            rulePath += '/';

        rulePath += controlPath;

        // Check the read condition exists or matches
        if (readCondition.length() > 0)
        {
            String readPath = rulePath;
            readPath += "/.read";
            if (currentRules.get(parseResult, readPath.c_str()) && strcmp(parseResult.to<const char *>(), readCondition.c_str()) == 0)
                readConditionExists = true;
        }

        // Check the write condition exists or matches
        if (writeCondition.length() > 0)
        {
            String writePath = rulePath;
            writePath += "/.write";
            if (currentRules.get(parseResult, writePath.c_str()) && strcmp(parseResult.to<const char *>(), writeCondition.c_str()) == 0)
                writeConditionExists = true;
        }

        // Add conditions if they do not exist.
        if (!readConditionExists || !writeConditionExists)
        {
            FirebaseJson addedRules;
            if (!readConditionExists)
                addedRules.add(".read", readCondition);
            if (!writeConditionExists)
                addedRules.add(".write", writeCondition);

            currentRules.set(rulePath, addedRules);

            String modifiedRules;
            currentRules.toString(modifiedRules, true);

            Serial.println("Setting the security rules to add the modified conditions... ");
            bool status = Database.set<object_t>(aClient, ".settings/rules", object_t(modifiedRules));
            if (status)
                Serial.println("Success");
            else
                Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
        }
        else
        {
            Serial.println("The rules exist, nothing to change");
        }

        currentRules.clear();
        return aClient.lastError().code() == 0;
    }
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

    Serial.println("-----------------------------------");

    return false;
}