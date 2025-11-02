#ifndef CORE_OPTIONS_H
#define CORE_OPTIONS_H

#if defined(ENABLE_SERVICE_AUTH) || defined(ENABLE_CUSTOM_AUTH)

#define ENABLE_JWT

#else

#if defined(ENABLE_JWT)

#if !defined(ENABLE_SERVICE_AUTH)
#define ENABLE_SERVICE_AUTH
#endif

#if !defined(ENABLE_CUSTOM_AUTH)
#define ENABLE_CUSTOM_AUTH
#endif

#endif

#endif // ENABLE_SERVICE_AUTH || ENABLE_CUSTOM_AUTH

#if !defined(FIREBASE_ASYNC_CLIENT)
#define FIREBASE_ASYNC_CLIENT AsyncClient
#endif

// -------------------------------
// Build target options
// CORE_ARDUINO_XXXX
// -------------------------------

#if defined(ARDUINO_ARCH_RP2040)

#if defined(ARDUINO_NANO_RP2040_CONNECT)
#ifndef ARDUINO_NANO_RP2040_CONNECT_MODULE
#define ARDUINO_NANO_RP2040_CONNECT_MODULE
#endif
#else
#ifndef ARDUINO_PICO_MODULE
#define ARDUINO_PICO_MODULE
#endif
#endif

#endif // ARDUINO_ARCH_RP2040

#if defined(ESP8266) || defined(ESP32)
#ifndef CORE_ARDUINO_ESP
#define CORE_ARDUINO_ESP
#endif
#endif

#if defined(__arm__)
#ifndef CORE_ARDUINO_ARM
#define CORE_ARDUINO_ARM
#endif
#endif

#if defined(ARDUINO_ARCH_SAMD)
#ifndef CORE_ARDUINO_ARCH_SAMD
#define CORE_ARDUINO_ARCH_SAMD
#endif
#endif

#if defined(ARDUINO_ARCH_RP2040)

#if defined(ARDUINO_NANO_RP2040_CONNECT)
#ifndef CORE_ARDUINO_NANO_RP2040_CONNECT
#define CORE_ARDUINO_NANO_RP2040_CONNECT
#endif
#else
#ifndef CORE_ARDUINO_PICO
#define CORE_ARDUINO_PICO
#endif
#endif

#endif // ARDUINO_ARCH_RP2040

#if defined(TEENSYDUINO)
#ifndef CORE_ARDUINO_TEENSY
#define CORE_ARDUINO_TEENSY
#endif
#endif

// -------------------------------
// Filesystems options
// -------------------------------

#if defined(ENABLE_FS)
#if __has_include(<FS.h>)
#include <FS.h>
#elif __has_include(<SD.h>) && __has_include(<SPI.h>)
#include <SPI.h>
#include <SD.h>
#else
// #undef ENABLE_FS
#endif
#endif // ENABLE_FS

#if defined(ENABLE_FS)

#if (defined(ESP8266) || defined(CORE_ARDUINO_PICO)) || (defined(ESP32) && __has_include(<SPIFFS.h>))

#if !defined(FILE_OPEN_MODE_READ)
#define FILE_OPEN_MODE_READ "r"
#endif

#if !defined(FILE_OPEN_MODE_WRITE)
#define FILE_OPEN_MODE_WRITE "w"
#endif

#if !defined(FILE_OPEN_MODE_APPEND)
#define FILE_OPEN_MODE_APPEND "a"
#endif

#elif __has_include(<SD.h>) && __has_include(<SPI.h>)

#if !defined(FILE_OPEN_MODE_READ)
#define FILE_OPEN_MODE_READ FILE_READ
#endif

#if !defined(FILE_OPEN_MODE_WRITE)
#define FILE_OPEN_MODE_WRITE FILE_WRITE
#endif

#if !defined(FILE_OPEN_MODE_APPEND)
#define FILE_OPEN_MODE_APPEND FILE_WRITE
#endif

#endif // __has_include(<SD.h>) && __has_include(<SPI.h>)

#endif // ENABLE_FS

#if defined(ESP32)
#if defined(ESP_ARDUINO_VERSION)
#if ESP_ARDUINO_VERSION > ESP_ARDUINO_VERSION_VAL(2, 0, 1)
#define ESP32_GT_2_0_1_FS_MEMORY_FIX
#endif
#endif
#endif

// -------------------------------
// OTA options
// -------------------------------

#if defined(ENABLE_OTA) && (defined(ENABLE_DATABASE) || defined(ENABLE_STORAGE) || defined(ENABLE_CLOUD_STORAGE))
#if defined(ESP32)
#include <Update.h>
#elif defined(ESP8266) || defined(CORE_ARDUINO_PICO)
#include <Updater.h>
#endif

#define OTA_UPDATE_ENABLED
#endif

#if __has_include(<time.h>)
#include <time.h>
#endif

// -------------------------------
// Values and limits options
// -------------------------------

#if !defined(FIREBASE_ASYNC_QUEUE_LIMIT)
#if defined(ESP8266)
#define FIREBASE_ASYNC_QUEUE_LIMIT 10
#elif defined(ESP32) || defined(ARDUINO_PICO_MODULE)
#define FIREBASE_ASYNC_QUEUE_LIMIT 20
#else
#define FIREBASE_ASYNC_QUEUE_LIMIT 10
#endif
#endif

#define FIREBASE_DEFAULT_TS 1759214462

#define FIREBASE_RECONNECTION_TIMEOUT_MSEC 5000

#define FIREBASE_NET_RECONNECT_TIMEOUT_SEC 10

#define FIREBASE_SESSION_TIMEOUT_SEC 150

#define FIREBASE_TCP_WRITE_TIMEOUT_SEC 30 // Do not change

#define FIREBASE_TCP_READ_TIMEOUT_SEC 30 // Do not change

#define FIREBASE_AUTH_PLACEHOLDER "<auth_token>"

// Raw chunk size for TCP's read and write operations.
#define FIREBASE_CHUNK_SIZE 2048

// Base64 encoded string chunk size for TCP's write operation.
// This used in Realtime database with File implementation.
#define FIREBASE_BASE64_CHUNK_SIZE 1026

// SSE mode time out in milliseconds.
#define FIREBASE_SSE_TIMEOUT_MS 40 * 1000

// The JWT process timed out due to absent of JWT.loop in loop or 
// too long delay in time status callback.
#define FIREBASE_JWT_TIMEOUT_MS 60 * 1000

#endif