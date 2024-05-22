/**
 * SYNTAX:
 *
 * GSMNetwork::GSMNetwork(<modem>, <gsm_pin>, <apn>, <user>, <password>);
 *
 * <modem> - The pointer to TinyGsm modem object. Modem should be initialized and/or set mode before transfering data.
 * <gsm_pin> - The SIM pin.
 * <apn> - The GPRS APN (Access Point Name).
 * <user> - The GPRS user.
 * <password> - The GPRS password.
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 *
 */

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7600 // SIMA7670 Compatible with SIM7600 AT instructions

// 📍######### IMPORTANT ! #########
// The macro TINY_GSM_MODEM_SIM7600 should be defined in src/Config.h or user sefined config at src/UserConfig.h

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

#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 600         // Time ESP32 will go to sleep (in seconds)

#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 26
#define PIN_RX 27
#define PWR_PIN 4
#define BAT_ADC 35
#define BAT_EN 12
#define PIN_RI 33
#define PIN_DTR 25
#define RESET 5

#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13

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

// This is a library internal SSL client.
// You can use any SSL Client that works with GSM library.
// The ESP_SSLClient uses PSRAM by default (if it is available), for PSRAM usage, see https://github.com/mobizt/FirebaseClient#memory-options
// For ESP_SSLClient documentation, see https://github.com/mobizt/ESP_SSLClient
ESP_SSLClient ssl_client;

GSMNetwork gsm_network(&modem, GSM_PIN, apn, gprsUser, gprsPass);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(gsm_network));

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

void setup()
{

    Serial.begin(115200);

    delay(10);
    pinMode(BAT_EN, OUTPUT);
    digitalWrite(BAT_EN, HIGH);

    // A7670 Reset
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, LOW);
    delay(100);
    digitalWrite(RESET, HIGH);
    delay(3000);
    digitalWrite(RESET, LOW);

    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(100);
    digitalWrite(PWR_PIN, HIGH);
    delay(1000);
    digitalWrite(PWR_PIN, LOW);

    DBG("Wait...");

    delay(3000);

    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

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

    Serial.println("Initializing app...");

    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "authTask");
}

void loop()
{
    // The async task handler should run inside the main loop
    // without blocking delay or bypassing with millis code blocks.

    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();
}

void asyncCB(AsyncResult &aResult)
{
    // WARNING!
    // Do not put your codes inside the callback and printResult.

    printResult(aResult);
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