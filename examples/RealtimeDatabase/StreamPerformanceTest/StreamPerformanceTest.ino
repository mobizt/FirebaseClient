/**
 * ABOUT:
 *
 * The Realtime Database Stream performance test example.
 *
 * This example will show how fast your device gets the Stream event.
 *
 * You will get all continouse data changes events from your Realtime Database without data lost.
 *
 * Open the index.html file with web browser and follow the instructions on that page to test.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 */

#include <Arduino.h>
#include <WiFi.h>

#include <FirebaseClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define DATABASE_URL "URL"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);

DefaultNetwork network;
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);
FirebaseApp app;

#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client1, ssl_client2;

using AsyncClient = AsyncClientClass;
AsyncClient aClient1(ssl_client1, getNetwork(network)), aClient2(ssl_client2, getNetwork(network));
RealtimeDatabase Database;

uint32_t expected_count = 0, counter = 0, sum = 0, delayMs = 0;
uint8_t ledPin = 0;
int option = 0;
bool sendAck = false, ledStatus = false;
String msg, masterMs, deviceIP;

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

    ssl_client1.setInsecure();
    ssl_client2.setInsecure();
    initializeApp(aClient2, app, getAuth(user_auth));
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
    Database.get(aClient1, "/test/performance", asyncCB, true, "streamTask");

    IPAddress ip = WiFi.localIP(); // Can use toString
    char buff[20];
    snprintf(buff, 20, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    deviceIP = buff;
}

void loop()
{
    app.loop();
    Database.loop();
    if (app.ready() && sendAck)
    {
        sendAck = false;
        Database.set<String>(aClient2, "/test/ack", msg, asyncCB, "setTask");
    }
}

void asyncCB(AsyncResult &aResult) { printResult(aResult); }

void printResult(AsyncResult &aResult)
{
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
        RealtimeDatabaseResult &RTDB = aResult.to<RealtimeDatabaseResult>();

        if (RTDB.isStream())
        {
            if (RTDB.type() == 0 /* null */)
                return;

            if (RTDB.type() == 5 /* string */ && RTDB.dataPath() == "/chat")
            {
                String op = RTDB.to<String>();

                if (op.indexOf("hello-") > -1)
                {
                    option = 0;
                    int start = op.indexOf("-");
                    masterMs = op.substring(start + 1);
                    sendAck = true;
                    msg = "hello-" + masterMs + "-" + deviceIP;
                }

                if (op.indexOf("print-all-") > -1 || op.indexOf("print-count-") > -1)
                {
                    // print-all-expectedCount-masterMs
                    // print-count-expectedCount-masterMs
                    option = op.indexOf("print-all-") > -1 ? 1 : 2;
                    int start = op.indexOf("-");
                    start = op.indexOf("-", start + 1);
                    int end = op.indexOf("-", start + 1);
                    masterMs = op.substring(op.lastIndexOf("-") + 1);
                    expected_count = op.substring(start + 1, end).toInt();

                    sendAck = true;
                    msg = "accepted-" + masterMs + "-" + deviceIP;
                    if (option == 1)
                        Serial.println();
                }
                else if (op.indexOf("blink-") > -1)
                {
                    // blink-pin-delayMs-expectedCount-helloMs
                    option = 3;
                    int start = op.indexOf("-");
                    int end = op.indexOf("-", start + 1);
                    ledPin = op.substring(start + 1, end).toInt();
                    start = op.indexOf("-", start + 1);
                    end = op.indexOf("-", start + 1);
                    delayMs = op.substring(start + 1, end).toInt();
                    start = op.indexOf("-", start + 1);
                    end = op.indexOf("-", start + 1);
                    expected_count = op.substring(start + 1, end).toInt();
                    masterMs = op.substring(op.lastIndexOf("-") + 1);
                    pinMode(ledPin, OUTPUT);
                    Firebase.printf("\nBlink LED: %d, Delay: %d ms\n", ledPin, delayMs);
                    sendAck = true;
                    msg = "accepted-" + masterMs + "-" + deviceIP;
                }

                if (option > 0)
                {
                    // Clear counter and sum values
                    counter = 0;
                    sum = 0;
                }
            }
            else
            {
                counter++;

                if (RTDB.type() == 1 /* int */ && RTDB.dataPath().length())
                {
                    sum += RTDB.to<int>();
                    if (option == 1)
                        Firebase.printf("counter: %d\ndata: %d\nsum: %d\n", counter, RTDB.to<int>(), sum);
                    else if (option == 3)
                    {
                        // Blink
                        digitalWrite(ledPin, ledStatus);
                        ledStatus = !ledStatus;
                        delay(delayMs);
                    }

                    if (expected_count == counter)
                    {
                        sendAck = true;
                        msg = "done-" + masterMs + "-" + deviceIP;
                        if (option == 2)
                            Firebase.printf("\ncounter: %d\nsum: %d\n", counter, sum);
                        Serial.println("Done!");
                    }
                }
            }
        }
    }
}