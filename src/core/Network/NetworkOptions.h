#ifndef CORE_NETWORK_NETWORK_OPTIONS_H
#define CORE_NETWORK_NETWORK_OPTIONS_H

#include <Arduino.h>
#include <vector>

#if __has_include(<esp_idf_version.h>)
#include <esp_idf_version.h>
#endif

#if __has_include(<IPAddress.h>)
#include <IPAddress.h>
#endif

#if defined(ESP8266) || defined(CORE_ARDUINO_PICO)

#include <string>

//__GNUC__
//__GNUC_MINOR__
//__GNUC_PATCHLEVEL__

#ifdef __GNUC__
#if __GNUC__ > 4 || __GNUC__ == 10
#if defined(ARDUINO_ESP8266_GIT_VER)
#if ARDUINO_ESP8266_GIT_VER > 0
#define ESP8266_CORE_SDK_V3_X_X
#endif
#endif
#endif
#endif

#endif // ESP8266) || CORE_ARDUINO_PICO

#if !defined(FIREBASE_DISABLE_NATIVE_ETHERNET)

#if defined(ESP32) && __has_include(<ETH.h>)
#include <ETH.h>
#define FIREBASE_LWIP_ETH_IS_AVAILABLE
#elif defined(ESP8266) && defined(ESP8266_CORE_SDK_V3_X_X)

#if __has_include(<LwipIntfDev.h>)
#include <LwipIntfDev.h>
#endif

#if __has_include(<ENC28J60lwIP.h>)&& defined(ENABLE_ESP8266_ENC28J60_ETH)
#define INC_ENC28J60_LWIP
#include <ENC28J60lwIP.h>
#endif

#if __has_include(<W5100lwIP.h>) && defined(ENABLE_ESP8266_W5100_ETH)
#define INC_W5100_LWIP
#include <W5100lwIP.h>
#endif

#if __has_include(<W5500lwIP.h>)&& defined(ENABLE_ESP8266_W5500_ETH)
#define INC_W5500_LWIP
#include <W5500lwIP.h>
#endif

#if defined(INC_ENC28J60_LWIP) || defined(INC_W5100_LWIP) || defined(INC_W5500_LWIP)
#define FIREBASE_LWIP_ETH_IS_AVAILABLE
#endif

#endif

#endif // FIREBASE_DISABLE_NATIVE_ETHERNET

#if __has_include(<Ethernet.h>) ||  (defined(FIREBASE_ETHERNET_MODULE_LIB) && defined(FIREBASE_ETHERNET_MODULE_CLASS))

#if defined(ESP8266)
#undef MAX_SOCK_NUM
#endif

#if (defined(ESP8266) && defined(FIREBASE_DISABLE_NATIVE_ETHERNET)) || (!defined(ESP8266) && !defined(ARDUINO_NANO_RP2040_CONNECT))

#if defined(FIREBASE_ETHERNET_MODULE_LIB) && defined(FIREBASE_ETHERNET_MODULE_CLASS)
#if __has_include(FIREBASE_ETHERNET_MODULE_LIB)
#include FIREBASE_ETHERNET_MODULE_LIB
#define FIREBASE_ETHERNET_MODULE_CLASS_IMPL FIREBASE_ETHERNET_MODULE_CLASS
#elif __has_include(<Ethernet.h>)
#include <Ethernet.h>
#define FIREBASE_ETHERNET_MODULE_CLASS_IMPL Ethernet
#endif
#else
#include <Ethernet.h>
#define FIREBASE_ETHERNET_MODULE_CLASS_IMPL Ethernet
#endif

#if defined(FIREBASE_ETHERNET_MODULE_CLASS_IMPL)

#define FIREBASE_ETHERNET_MODULE_IS_AVAILABLE

// milliseconds
#if !defined(FIREBASE_ETHERNET_MODULE_TIMEOUT)
#define FIREBASE_ETHERNET_MODULE_TIMEOUT 2000
#elif FIREBASE_ETHERNET_MODULE_TIMEOUT <= 0 || FIREBASE_ETHERNET_MODULE_TIMEOUT > 120 * 1000
#undef FIREBASE_ETHERNET_MODULE_TIMEOUT
#define FIREBASE_ETHERNET_MODULE_TIMEOUT 2000
#endif

#endif

#endif

#if defined(ESP8266)
#undef MAX_SOCK_NUM
#endif

#endif // __has_include(<Ethernet.h>) ||  (defined(FIREBASE_ETHERNET_MODULE_LIB) && defined(FIREBASE_ETHERNET_MODULE_CLASS))

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || \
    defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_PORTENTA_C33) ||                \
    defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4) ||         \
    __has_include(<WiFiNINA.h>) ||__has_include(<WiFi101.h>)

#if !defined(FIREBASE_DISABLE_ONBOARD_WIFI)

#define FIREBASE_WIFI_IS_AVAILABLE

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4) || defined(ARDUINO_OPTA)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>) || defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#elif __has_include(<WiFiC3.h>) || defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif __has_include(<WiFi.h>)
#include <WiFi.h>
#endif

#if !defined(ARDUINO_RASPBERRY_PI_PICO_W) && \
    !defined(CORE_ARDUINO_ARCH_SAMD) &&      \
    !defined(CORE_ARDUINO_MBED_PORTENTA) &&  \
    !defined(ARDUINO_UNOWIFIR4) &&           \
    !defined(ARDUINO_PORTENTA_C33) &&        \
    !defined(ARDUINO_NANO_RP2040_CONNECT) && \
    !defined(ARDUINO_GIGA) &&                \
    !defined(ARDUINO_PORTENTA_H7_M7) &&      \
    !defined(ARDUINO_PORTENTA_H7_M4) &&      \
    !defined(ARDUINO_OPTA)

#define FIREBASE_HAS_WIFI_DISCONNECT
#endif

#if defined(ARDUINO_PORTENTA_C33)
#define CONST_STRING_CAST char * // C33 WiFi library SSID is char array
#else
#define CONST_STRING_CAST const char *
#endif

// WiFiS3 getTime is currently return 0 (not implemented)
#if __has_include(<WiFiNINA.h>) || __has_include(<WiFi101.h>)
#define FIREBASE_HAS_WIFI_TIME
#endif

#if defined(CORE_ARDUINO_PICO) && __has_include(<WiFiMulti.h>)
#include <WiFiMulti.h>
#define FIREBASE_HAS_WIFIMULTI
#endif

#if defined(ESP8266) && __has_include(<ESP8266WiFiMulti.h>)
#include <ESP8266WiFiMulti.h>
#define FIREBASE_HAS_WIFIMULTI
#endif

#endif // FIREBASE_DISABLE_ONBOARD_WIFI

#endif // ESP32...

#if defined(TINY_GSM_MODEM_SIM800) ||     \
    defined(TINY_GSM_MODEM_SIM808) ||     \
    defined(TINY_GSM_MODEM_SIM868) ||     \
    defined(TINY_GSM_MODEM_SIM900) ||     \
    defined(TINY_GSM_MODEM_SIM7000) ||    \
    defined(TINY_GSM_MODEM_SIM7000) ||    \
    defined(TINY_GSM_MODEM_SIM7000SSL) || \
    defined(TINY_GSM_MODEM_SIM7070) ||    \
    defined(TINY_GSM_MODEM_SIM7080) ||    \
    defined(TINY_GSM_MODEM_SIM7090) ||    \
    defined(TINY_GSM_MODEM_SIM5320) ||    \
    defined(TINY_GSM_MODEM_SIM5360) ||    \
    defined(TINY_GSM_MODEM_SIM5300) ||    \
    defined(TINY_GSM_MODEM_SIM7100) ||    \
    defined(TINY_GSM_MODEM_SIM7600) ||    \
    defined(TINY_GSM_MODEM_SIM7800) ||    \
    defined(TINY_GSM_MODEM_SIM7500) ||    \
    defined(TINY_GSM_MODEM_UBLOX) ||      \
    defined(TINY_GSM_MODEM_SARAR4) ||     \
    defined(TINY_GSM_MODEM_M95) ||        \
    defined(TINY_GSM_MODEM_BG96) ||       \
    defined(TINY_GSM_MODEM_A6) ||         \
    defined(TINY_GSM_MODEM_A7) ||         \
    defined(TINY_GSM_MODEM_M590) ||       \
    defined(TINY_GSM_MODEM_MC60) ||       \
    defined(TINY_GSM_MODEM_MC60E) ||      \
    defined(TINY_GSM_MODEM_XBEE) ||       \
    defined(TINY_GSM_MODEM_SEQUANS_MONARCH) ||       \
    defined(TINY_GSM_FORK_LIBRARY)
#define FIREBASE_TINYGSM_IS_AVAILABLE
#endif // TINY_GSM_MODEM_SIM800 ...

#if defined(FIREBASE_TINYGSM_IS_AVAILABLE) && __has_include(<TinyGsmClient.h>)
#include <TinyGsmClient.h>
#define FIREBASE_GSM_MODEM_IS_AVAILABLE
#endif

#if !defined(FIREBASE_DISABLE_NATIVE_PPP) && defined(ESP_ARDUINO_VERSION) /* ESP32 core >= v2.0.x */
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)

#if __has_include(<PPP.h>)
#include <PPP.h>
#define FIREBASE_NATIVE_PPP_IS_AVAILABLE
#endif

#endif
#endif // FIREBASE_DISABLE_NATIVE_PPP

#if defined(FIREBASE_WIFI_IS_AVAILABLE)
#define WIFI_CONNECTED (WiFi.status() == WL_CONNECTED)
#else
#define WIFI_CONNECTED false
#endif

#if defined(FIREBASE_NATIVE_PPP_IS_AVAILABLE)
#define PPP_CONNECTED PPP.connected()
#else
#define PPP_CONNECTED false
#endif

#endif // CORE_NETWORK_NETWORK_OPTIONS_H