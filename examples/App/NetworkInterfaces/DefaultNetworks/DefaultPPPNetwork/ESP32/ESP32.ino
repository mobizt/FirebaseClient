

/**
 * ABOUT:
 *
 * The bare minimum example for working with the ESP32 Arduino Core (v3.x.x) PPP network.
 *
 * The divices used in this example are WaveShare SIM7600 GSM module and ESP32.
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
 * LilyGO TTGO T-A7670 development board (ESP32 with SIMCom A7670)
 * GPIO5    RST
 * GPIO26   TX
 * GPIO27   RX
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
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 */

// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
#define ENABLE_USER_CONFIG
#define ENABLE_USER_AUTH

// For network independent usage (disable all network features).
// #define DISABLE_NERWORKS

#include <PPP.h>
#include <NetworkClientSecure.h>
#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

DefaultPPPNetwork default_network;

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

NetworkClientSecure ssl_client;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(default_network));

#define PPP_MODEM_APN "YourAPN"
#define PPP_MODEM_PIN "0000" // or NULL

// WaveShare SIM7600 HW Flow Control
#define PPP_MODEM_RST 25
#define PPP_MODEM_RST_LOW true // active LOW
#define PPP_MODEM_RST_DELAY 200
#define PPP_MODEM_TX 21
#define PPP_MODEM_RX 22
#define PPP_MODEM_RTS 26
#define PPP_MODEM_CTS 27
#define PPP_MODEM_FC ESP_MODEM_FLOW_CONTROL_HW
#define PPP_MODEM_MODEL PPP_MODEM_SIM7600

// LilyGO TTGO T-A7670 development board (ESP32 with SIMCom A7670)
// #define PPP_MODEM_RST 5
// #define PPP_MODEM_RST_LOW false // active HIGH
// #define PPP_MODEM_RST_DELAY 200
// #define PPP_MODEM_TX 26
// #define PPP_MODEM_RX 27
// #define PPP_MODEM_RTS -1
// #define PPP_MODEM_CTS -1
// #define PPP_MODEM_FC ESP_MODEM_FLOW_CONTROL_NONE
// #define PPP_MODEM_MODEL PPP_MODEM_SIM7600

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

    // Resetting the modem
#if defined(PPP_MODEM_RST)
    pinMode(PPP_MODEM_RST, PPP_MODEM_RST_LOW ? OUTPUT_OPEN_DRAIN : OUTPUT);
    digitalWrite(PPP_MODEM_RST, PPP_MODEM_RST_LOW);
    delay(100);
    digitalWrite(PPP_MODEM_RST, !PPP_MODEM_RST_LOW);
    delay(3000);
    digitalWrite(PPP_MODEM_RST, PPP_MODEM_RST_LOW);
#endif

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

    ssl_client.setInsecure();

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);
}

void loop()
{
    if (PPP.connected())
    {
        setConfig();
    }

    // To maintain the authentication and async tasks.
    app.loop();
}