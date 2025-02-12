/**
 * ABOUT:
 *
 * The bare minimum non-blocking (async) example for working mobile GSM/4G/5G network using TinyGsmClient.h.
 *
 * The divices used in this example are LilyGO TTGO T-A7670 development board (SIMA7670) and ESP32.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * GSMNetwork::GSMNetwork(<modem>, <gsm_pin>, <apn>, <user>, <password>);
 *
 * <modem> - The pointer to TinyGsm modem object. Modem should be initialized and/or set mode before transfering data.
 * <gsm_pin> - The SIM pin.
 * <apn> - The GPRS APN (Access Point Name).
 * <user> - The GPRS user.
 * <password> - The GPRS password.
 *
 * In ESP32 Core v3.x.x, PPP devices are natively supported.
 * See examples/App/NetworkInterfaces/Async/NoCallback/DefaultNetworks/DefaultPPPNetwork/ESP32/ESP32.ino
 */

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7600 // SIMA7670 Compatible with SIM7600 AT instructions

// 📍######### IMPORTANT ! #########
// The macro TINY_GSM_MODEM_SIM7600 should be defined in src/FirebaseConfig.h or user sefined config at src/UserConfig.h

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "YourAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

#define UART_BAUD 115200

// LilyGO TTGO T-A7670 development board (ESP32 with SIMCom A7670)
#define SIM_MODEM_RST 5
#define SIM_MODEM_RST_LOW false // active HIGH
#define SIM_MODEM_RST_DELAY 200
#define SIM_MODEM_TX 26
#define SIM_MODEM_RX 27

// Include TinyGsmClient.h first and followed by FirebaseClient.h
#include <TinyGsmClient.h>
#include <FirebaseClient.h>

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

TinyGsm modem(SerialAT);

TinyGsmClient gsm_client(modem);

// This is a library's internal SSL client.
// You can use any SSL Client that works with GSM library.
// The ESP_SSLClient uses PSRAM by default (if it is available), for PSRAM usage, see https://github.com/mobizt/FirebaseClient#memory-options
// For ESP_SSLClient documentation, see https://github.com/mobizt/ESP_SSLClient
ESP_SSLClient ssl_client;

GSMNetwork gsm_network(&modem, GSM_PIN, apn, gprsUser, gprsPass);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(gsm_network));

AsyncResult aResult_no_callback;

void printResult(AsyncResult &aResult);

void setup()
{
    Serial.begin(115200);

    // Resetting the modem
#if defined(SIM_MODEM_RST)
    pinMode(SIM_MODEM_RST, SIM_MODEM_RST_LOW ? OUTPUT_OPEN_DRAIN : OUTPUT);
    digitalWrite(SIM_MODEM_RST, SIM_MODEM_RST_LOW);
    delay(100);
    digitalWrite(SIM_MODEM_RST, !SIM_MODEM_RST_LOW);
    delay(3000);
    digitalWrite(SIM_MODEM_RST, SIM_MODEM_RST_LOW);
#endif

    DBG("Wait...");

    delay(3000);

    SerialAT.begin(UART_BAUD, SERIAL_8N1, SIM_MODEM_RX, SIM_MODEM_TX);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    DBG("Initializing modem...");
    if (!modem.init())
    {
        DBG("Failed to restart modem, delaying 10s and retrying");
        return;
    }

    /*
    2 Automatic
    13 GSM Only
    14 WCDMA Only
    38 LTE Only
    */
    modem.setNetworkMode(38);
    if (modem.waitResponse(10000L) != 1)
    {
        DBG(" setNetworkMode faill");
    }

    String name = modem.getModemName();
    DBG("Modem Name:", name);

    String modemInfo = modem.getModemInfo();
    DBG("Modem Info:", modemInfo);

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl_client.setInsecure();

    ssl_client.setDebugLevel(1);

    ssl_client.setBufferSizes(2048 /* rx */, 1024 /* tx */);

    ssl_client.setClient(&gsm_client);

    Serial.println("Initializing the app...");
    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();

    printResult(aResult_no_callback);
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