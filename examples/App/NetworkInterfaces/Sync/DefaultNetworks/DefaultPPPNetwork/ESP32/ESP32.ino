

/**
 * This example is for ESP32 Core v3.x.x and the SIM modules.
 *
 * WaveShare SIM7600 with hardware Flow Control
 *
 * ESP32    WaveShare SIM7600
 *
 * GPIO25   RST
 * GPIO21   TX
 * GPIO22   RX
 * GPIO26   RTS
 * GPIO27   CTS
 * GND      GND
 *
 * SIM800 basic module with just TX,RX and RST
 *
 * GPIO0   RST
 * GPIO2   TX
 * GPIO19   RX
 * GND      GND
 *
 * The PPP ESP32 example was taken from
 * https://github.com/espressif/arduino-esp32/blob/master/libraries/PPP/examples/PPP_Basic/PPP_Basic.ino
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 *
 */

#include <Arduino.h>

#include <PPP.h>

#include <FirebaseClient.h>

#include <NetworkClientSecure.h>

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

void authHandler();

void printResult(AsyncResult &aResult);

DefaultPPPNetwork default_network;

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

NetworkClientSecure ssl_client;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(default_network));

AsyncResult aResult_no_callback;

#define PPP_MODEM_APN "YourAPN"
#define PPP_MODEM_PIN "0000" // or NULL

// WaveShare SIM7600 HW Flow Control
#define PPP_MODEM_RST 25
#define PPP_MODEM_RST_LOW false // active HIGH
#define PPP_MODEM_RST_DELAY 200
#define PPP_MODEM_TX 21
#define PPP_MODEM_RX 22
#define PPP_MODEM_RTS 26
#define PPP_MODEM_CTS 27
#define PPP_MODEM_FC ESP_MODEM_FLOW_CONTROL_HW
#define PPP_MODEM_MODEL PPP_MODEM_SIM7600

// SIM800 basic module with just TX,RX and RST
// #define PPP_MODEM_RST     0
// #define PPP_MODEM_RST_LOW true //active LOW
// #define PPP_MODEM_TX      2
// #define PPP_MODEM_RX      19
// #define PPP_MODEM_RTS     -1
// #define PPP_MODEM_CTS     -1
// #define PPP_MODEM_FC      ESP_MODEM_FLOW_CONTROL_NONE
// #define PPP_MODEM_MODEL   PPP_MODEM_SIM800

bool firebaseConfigReady = false;

void onEvent(arduino_event_id_t event, arduino_event_info_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_PPP_START:
        Serial.println("PPP Started");
        break;
    case ARDUINO_EVENT_PPP_CONNECTED:
        Serial.println("PPP Connected");
        break;
    case ARDUINO_EVENT_PPP_GOT_IP:
        Serial.println("PPP Got IP");
        break;
    case ARDUINO_EVENT_PPP_LOST_IP:
        Serial.println("PPP Lost IP");
        break;
    case ARDUINO_EVENT_PPP_DISCONNECTED:
        Serial.println("PPP Disconnected");
        break;
    case ARDUINO_EVENT_PPP_STOP:
        Serial.println("PPP Stopped");
        break;
    default:
        break;
    }
}

void setup()
{
    Serial.begin(115200);

    // This delay is needed in case ETH_CLK_MODE was set to ETH_CLOCK_GPIO0_IN,
    // to allow the external clock source to be ready before initialize the Ethernet.
    delay(500);

    Serial.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    // Listen for modem events
    Network.onEvent(onEvent);

    // Configure the modem
    PPP.setApn(PPP_MODEM_APN);
    PPP.setPin(PPP_MODEM_PIN);
    PPP.setResetPin(PPP_MODEM_RST, PPP_MODEM_RST_LOW, PPP_MODEM_RST_DELAY);
    PPP.setPins(PPP_MODEM_TX, PPP_MODEM_RX, PPP_MODEM_RTS, PPP_MODEM_CTS, PPP_MODEM_FC);

    Serial.println("Starting the modem. It might take a while!");
    PPP.begin(PPP_MODEM_MODEL);

    Serial.print("Manufacturer: ");
    Serial.println(PPP.cmd("AT+CGMI", 10000));
    Serial.print("Model: ");
    Serial.println(PPP.moduleName());
    Serial.print("IMEI: ");
    Serial.println(PPP.IMEI());

    bool attached = PPP.attached();
    if (!attached)
    {
        int i = 0;
        unsigned int s = millis();
        Serial.print("Waiting to connect to network");
        while (!attached && ((++i) < 600))
        {
            Serial.print(".");
            delay(100);
            attached = PPP.attached();
        }
        Serial.print((millis() - s) / 1000.0, 1);
        Serial.println("s");
        attached = PPP.attached();
    }

    Serial.print("Attached: ");
    Serial.println(attached);
    Serial.print("State: ");
    Serial.println(PPP.radioState());
    if (attached)
    {
        Serial.print("Operator: ");
        Serial.println(PPP.operatorName());
        Serial.print("IMSI: ");
        Serial.println(PPP.IMSI());
        Serial.print("RSSI: ");
        Serial.println(PPP.RSSI());
        int ber = PPP.BER();
        if (ber > 0)
        {
            Serial.print("BER: ");
            Serial.println(ber);
            Serial.print("NetMode: ");
            Serial.println(PPP.networkMode());
        }

        Serial.println("Switching to data mode...");
        PPP.mode(ESP_MODEM_MODE_CMUX); // Data and Command mixed mode
        if (!PPP.waitStatusBits(ESP_NETIF_CONNECTED_BIT, 1000))
        {
            Serial.println("Failed to connect to internet!");
        }
        else
        {
            Serial.println("Connected to internet!");
        }
    }
    else
    {
        Serial.println("Failed to connect to network!");
    }
}

void setConfig()
{
    if (firebaseConfigReady)
        return;

    firebaseConfigReady = true;

    Serial.println("Initializing app...");

    ssl_client.setInsecure();

    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);
}

void loop()
{
    if (PPP.connected())
    {
        setConfig();
    }

    authHandler();

    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();

    printResult(aResult_no_callback);
}

void authHandler()
{
    // Blocking authentication handler with timeout
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // The JWT token processor required for ServiceAuth and CustomAuth authentications.
        // JWT is a static object of JWTClass and it's not thread safe.
        // In multi-threaded operations (multi-FirebaseApp), you have to define JWTClass for each FirebaseApp,
        // and set it to the FirebaseApp via FirebaseApp::setJWTProcessor(<JWTClass>), before calling initializeApp.
        JWT.loop(app.getAuth());
        printResult(aResult_no_callback);
    }
}

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
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}