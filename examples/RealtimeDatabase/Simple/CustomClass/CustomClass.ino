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

MyFirebase fb;
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

    fb.userBegin(ssl_client1, DATABASE_URL, API_KEY, USER_EMAIL, USER_PASSWORD);

    fb.beginStream(ssl_client2, "/test/string");
}

void loop()
{
    if (fb.ready() && (ms == 0 || millis() - ms > 20000))
    {
        ms = millis();

        count++;

        Serial.println("Setting the string value...");
        fb.setString("/test/string", "Hello " + String(count));

        Serial.println("Getting the string value...");
        Serial.println(fb.getString("/test/string"));
    }

    if (fb.available())
    {
        if (fb.isEvent())
            Serial.printf("Event task: %s, msg: %s, code: %d\n", fb.taskId().c_str(), fb.eventString().c_str(), fb.eventCode());

        if (fb.isDebug())
            Serial.printf("Debug task: %s, msg: %s\n", fb.taskId().c_str(), fb.debugString().c_str());

        if (fb.isError())
            Serial.printf("Error task: %s, msg: %s, code: %d\n", fb.taskId().c_str(), fb.errorString().c_str(), fb.errorCode());

        if (fb.isPayload())
        {
            if (fb.isStream())
            {
                Serial.println("---------------------------");
                Serial.println("Stream data...");
                Serial.printf("task: %s\n", fb.taskId().c_str());
                Serial.printf("event: %s\n", fb.StreamEvent().c_str());
                Serial.printf("path: %s\n", fb.StreamDataPath().c_str());
                Serial.printf("data: %s\n", fb.StreamData().c_str());
                Serial.printf("type: %d\n", fb.StreamDataType());
            }
            else
                Serial.printf("task: %s, payload: %s\n", fb.taskId().c_str(), fb.payload().c_str());
        }
    }
}