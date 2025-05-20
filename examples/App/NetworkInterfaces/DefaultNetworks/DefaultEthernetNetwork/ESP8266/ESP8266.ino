/**
 * ABOUT:
 *
 * The bare minimum example for working with the ESP8266 Arduino Core Ethernet network
 * using ENC28J60lwIP.h, Wiznet5100lwIP.h and Wiznet5500lwIP.h.
 *
 * The divices used in this example are ENC28J60 Ethernet module and ESP8266.
 *
 * The ENC28J60 Ethernet module and ESP8266 board, SPI port wiring connection.
 *
 * ESP8266 (Wemos D1 Mini or NodeMCU)        ENC28J60
 *
 * GPIO12 (D6) - MISO                        SO
 * GPIO13 (D7) - MOSI                        SI
 * GPIO14 (D5) - SCK                         SCK
 * GPIO16 (D0) - CS                          CS
 * GND                                       GND
 * 3V3                                       VCC
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * DefaultEthernetNetwork::DefaultEthernetNetwork(<Firebase_SPI_ETH_Module>);
 *
 * <Firebase_SPI_ETH_Module> - The Firebase_SPI_ETH_Module class object that keeping the pointer to lwip driver for each
 * SPI Ethernet module (e.g. ENC28J60lwIP, W5100lwIP and W5500lwIP).
 */

// To define build options in your sketch,
// adding the following macros before FirebaseClient.h
#define ENABLE_USER_CONFIG
#define ENABLE_USER_AUTH
#define ENABLE_ESP8266_ENC28J60_ETH

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#include <WiFiClientSecure.h>

#include <ENC28J60lwIP.h>
// #include <W5100lwIP.h>
// #include <W5500lwIP.h>

#include <LwipEthernet.h>

/** Don't gorget to define this in src/FirebaseConfig.h or src/UserConfig.h
  #define ENABLE_ESP8266_ENC28J60_ETH
*/

/** For PlatformIO IDE, please set the lib_ldf_mode or lib_deps in platformio.ini as the following.
  lib_ldf_mode = chain+
  lib_deps = arduino-libraries/Ethernet @ 2.0.2
*/

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

#define ETH_CS_PIN 16 // D0

ENC28J60lwIP eth(ETH_CS_PIN);
// Wiznet5100lwIP eth(ETH_CS_PIN);
// Wiznet5500lwIP eth(ETH_CS_PIN);

Firebase_SPI_ETH_Module spi_eth;

DefaultEthernetNetwork default_network(spi_eth);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

WiFiClientSecure ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(default_network));

void setup()
{
    Serial.begin(115200);

    // https://github.com/esp8266/Arduino/blob/master/libraries/lwIP_Ethernet/examples/EthClient/EthClient.ino

    Serial.print("Connecting to Ethernet... ");

    if (!ethInitDHCP(eth))
    {
        Serial.println("no hardware found!");
        while (1)
        {
            delay(1000);
        }
    }

    while (!eth.connected())
    {
        Serial.printf(".");
        delay(500);
    }

    Firebase.printf("\nEthernet: IP Address: %s\n", eth.localIP().toString().c_str());

    /* Assign the pointer to Ethernet module lwip interface */
#if defined(ENABLE_ESP8266_ENC28J60_ETH)
    spi_eth.enc28j60 = &eth;
#endif
#if defined(ENABLE_ESP8266_W5100_ETH)
    spi_eth.w5100 = &eth;
#endif
#if defined(ENABLE_ESP8266_W5500_ETH)
    spi_eth.w5500 = &eth;
#endif

    Serial.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl_client.setInsecure();
    ssl_client.setBufferSizes(4096, 1024);

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);
}

void loop()
{
    // To maintain the authentication and async tasks.
    app.loop();
}