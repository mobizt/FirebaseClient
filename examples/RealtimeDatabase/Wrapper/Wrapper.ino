/**
 * The example show how to create a wrapper class for Firebase Realtime Database that is easier to use.
*/

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include "MyFirebase.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"

#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

WiFiClientSecure ssl_client1, ssl_client2;

MyFirebase firebase;
int count = 0;
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

    ssl_client1.setInsecure();
    ssl_client2.setInsecure();

    firebase.userBegin(ssl_client1, DATABASE_URL, API_KEY, USER_EMAIL, USER_PASSWORD);
    firebase.beginStream(ssl_client2, "/test/string");
}

void loop()
{
    if (firebase.ready() && (ms == 0 || millis() - ms > 20000))
    {
        ms = millis();
        count++;

        Serial.println("Setting the string value...");
        firebase.setString("/test/string", "Hello " + String(count));

        Serial.println("Getting the string value...");
        Serial.println(firebase.getString("/test/string"));
    }

    if (firebase.available())
    {
        if (firebase.isEvent())
            Serial.printf("Event task: %s, msg: %s, code: %d\n", firebase.taskId().c_str(), firebase.eventString().c_str(), firebase.eventCode());

        if (firebase.isDebug())
            Serial.printf("Debug task: %s, msg: %s\n", firebase.taskId().c_str(), firebase.debugString().c_str());

        if (firebase.isError())
            Serial.printf("Error task: %s, msg: %s, code: %d\n", firebase.taskId().c_str(), firebase.errorString().c_str(), firebase.errorCode());

        if (firebase.isPayload())
        {
            if (firebase.isStream())
            {
                Serial.println("---------------------------");
                Serial.println("Stream data...");
                Serial.printf("task: %s\n", firebase.taskId().c_str());
                Serial.printf("event: %s\n", firebase.StreamEvent().c_str());
                Serial.printf("path: %s\n", firebase.StreamDataPath().c_str());
                Serial.printf("data: %s\n", firebase.StreamData().c_str());
                Serial.printf("type: %d\n", firebase.StreamDataType());
            }
            else
                Serial.printf("task: %s, payload: %s\n", firebase.taskId().c_str(), firebase.payload().c_str());
        }
    }
}