#ifndef USER_DEBUG_PRINT_H
#define USER_DEBUG_PRINT_H

#include <FirebaseClient.h>

// WiFi library used in the examples
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>) || defined(ARDUINO_NANO_RP2040_CONNECT)
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

// SSL Client used in the examples
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFiClientSecure.h>
#define SSL_CLIENT WiFiClientSecure
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA) || defined(ARDUINO_PORTENTA_C33) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiSSLClient.h>
#define SSL_CLIENT WiFiSSLClient
#else
#define SSL_CLIENT ESP_SSLClient
#endif

// Set some SSL client for skipping server certificate verification.
void set_ssl_client_insecure_and_buffer(SSL_CLIENT &client)
{
#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    client.setInsecure();
#if defined(ESP8266)
    client.setBufferSizes(4096, 1024);
#endif
#else
    (void)client;
#endif
}

// Filesystem and callback function used in the examples.
#if defined(ENABLE_FS)
// In ESP32 Core SDK v3.x.x, to use filesystem in this library,
// the File object should be defined globally
// and the library's internal defined FS object should be set with
// this global FS object in fileCallback function.
#include <FS.h>
File myFile;

#if defined(ESP32)
#include <SPIFFS.h>
#endif
#define MY_FS SPIFFS

void file_operation_callback(File &file, const char *filename, file_operating_mode mode)
{
    // FILE_OPEN_MODE_READ, FILE_OPEN_MODE_WRITE and FILE_OPEN_MODE_APPEND are defined in this library
    // MY_FS is defined in this example
    switch (mode)
    {
    case file_mode_open_read:
        myFile = MY_FS.open(filename, FILE_OPEN_MODE_READ);
        break;
    case file_mode_open_write:
        myFile = MY_FS.open(filename, FILE_OPEN_MODE_WRITE);
        break;
    case file_mode_open_append:
        myFile = MY_FS.open(filename, FILE_OPEN_MODE_APPEND);
        break;
    case file_mode_remove:
        MY_FS.remove(filename);
        break;
    default:
        break;
    }
    // Set the internal FS object with global File object.
    file = myFile;
}

void print_file_content(const String &filename)
{
    File file = MY_FS.open(filename, FILE_OPEN_MODE_READ);
    int i = 0;
    if (file)
    {
        while (file.available())
        {
            if (i > 0 && i % 16 == 0)
                Serial.println();

            int v = file.read();

            if (v < 16)
                Serial.print("0");

            Serial.print(v, HEX);
            Serial.print(" ");
            i++;
        }
        Serial.println();
        file.close();
    }
}

#endif

// Debug information printing
void auth_debug_print(AsyncResult &aResult)
{
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
}

// Function to get NTP server time.
uint32_t get_ntp_time()
{
    uint32_t ts = 0;
    Serial.print("Getting time from NTP server... ");
#if defined(ESP8266) || defined(ESP32) || defined(CORE_ARDUINO_PICO)
    int max_try = 10, retry = 0;
    while (time(nullptr) < FIREBASE_DEFAULT_TS && retry < max_try)
    {
        configTime(3 * 3600, 0, "pool.ntp.org");
        unsigned long m = millis();
        while (time(nullptr) < FIREBASE_DEFAULT_TS && millis() - m < 10 * 1000)
        {
            delay(100);
            ts = time(nullptr);
        }
        Serial.print(ts == 0 ? " failed, retry... " : "");
        retry++;
    }
    ts = time(nullptr);
#elif __has_include(<WiFiNINA.h>) || __has_include(<WiFi101.h>)
    ts = WiFi.getTime();
#endif

    Serial.println(ts > 0 ? "success" : "failed");
    return ts;
}

// Token type information printing
void print_token_type(FirebaseApp &app)
{
    Firebase.printf("Auth Token: %s\n", app.getToken().c_str());
    firebase_token_type type = app.getTokenType();
    if (type == token_type_access)
        Serial.println("Type: access token");
    else if (type == token_type_id)
        Serial.println("Type: ID token");
    else if (type == token_type_legacy)
        Serial.println("Type: legacy token");
    else if (type == token_type_no)
        Serial.println("Type: no token");
}

#endif
