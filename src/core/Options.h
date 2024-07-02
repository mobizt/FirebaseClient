/**
 * Created July 1, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef FIREBASE_BUILD_OPTIONS_H
#define FIREBASE_BUILD_OPTIONS_H

#define FIREBASE_DEFAULT_TS 1618971013

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

#endif

#if defined(DISABLE_ALL_OPTIONS)

#undef ENABLE_DATABASE
#undef ENABLE_FIRESTORE
#undef ENABLE_FIRESTORE_QUERY
#undef ENABLE_MESSAGING
#undef ENABLE_STORAGE
#undef ENABLE_CLOUD_STORAGE
#undef ENABLE_FUNCTIONS
#undef ENABLE_PSRAM
#undef ENABLE_OTA
#undef ENABLE_FS
#undef ENABLE_SERVICE_AUTH
#undef ENABLE_CUSTOM_AUTH
#undef ENABLE_USER_AUTH
#undef ENABLE_ACCESS_TOKEN
#undef ENABLE_CUSTOM_TOKEN
#undef ENABLE_ID_TOKEN
#undef ENABLE_LEGACY_TOKEN
#undef ENABLE_JWT

#endif

#if defined(DISABLE_DATABASE)
#undef ENABLE_DATABASE
#endif

#if defined(DISABLE_FIRESTORE)
#undef ENABLE_FIRESTORE
#undef ENABLE_FIRESTORE_QUERY
#endif

#if defined(DISABLE_FIRESTORE_QUERY)
#undef ENABLE_FIRESTORE_QUERY
#endif

#if defined(DISABLE_MESSAGING)
#undef ENABLE_MESSAGING
#endif

#if defined(DISABLE_STORAGE)
#undef ENABLE_STORAGE
#endif

#if defined(DISABLE_CLOUD_STORAGE)
#undef ENABLE_CLOUD_STORAGE
#endif

#if defined(DISABLE_FUNCTIONS)
#undef ENABLE_FUNCTIONS
#endif

#if defined(DISABLE_PSRAM)
#undef ENABLE_PSRAM
#endif

#if defined(DISABLE_OTA)
#undef ENABLE_OTA
#endif

#if defined(DISABLE_FS)
#undef ENABLE_FS
#endif

#if defined(DISABLE_SERVICE_AUTH)
#undef ENABLE_SERVICE_AUTH
#endif

#if defined(DISABLE_CUSTOM_AUTH)
#undef ENABLE_CUSTOM_AUTH
#endif

#if defined(DISABLE_USER_AUTH)
#undef ENABLE_USER_AUTH
#endif

#if defined(DISABLE_ACCESS_TOKEN)
#undef ENABLE_ACCESS_TOKEN
#endif

#if defined(DISABLE_CUSTOM_TOKEN)
#undef ENABLE_CUSTOM_TOKEN
#endif

#if defined(DISABLE_ID_TOKEN)
#undef ENABLE_ID_TOKEN
#endif

#if defined(DISABLE_LEGACY_TOKEN)
#undef ENABLE_LEGACY_TOKEN
#endif

#if defined(ENABLE_PSRAM)
#define FIREBASE_USE_PSRAM
#endif

#if !defined(FPSTR)
#define FPSTR
#endif

/// CORE_ARDUINO_XXXX macro for MCU build target.

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

#endif

#if defined(TEENSYDUINO)
#ifndef CORE_ARDUINO_TEENSY
#define CORE_ARDUINO_TEENSY
#endif
#endif

#if defined(ENABLE_FS)
#if __has_include(<FS.h>)
#include <FS.h>
#define FILEOBJ File
#elif __has_include(<SD.h>) && __has_include(<SPI.h>)
#include <SPI.h>
#include <SD.h>
#define FILEOBJ File
#else
#undef ENABLE_FS
#endif
#endif

#if defined(ENABLE_FS)

#if (defined(ESP8266) || defined(CORE_ARDUINO_PICO)) || (defined(ESP32) && __has_include(<SPIFFS.h>))
#ifndef FLASH_SUPPORTS
#define FLASH_SUPPORTS
#endif

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

#endif

#endif

#if __has_include(<time.h>)
#include <time.h>
#endif

#if defined(ENABLE_OTA) && (defined(ENABLE_DATABASE) || defined(ENABLE_STORAGE) || defined(ENABLE_CLOUD_STORAGE))
#if defined(ESP32)
#include <Update.h>
#elif defined(ESP8266) || defined(CORE_ARDUINO_PICO)
#include <Updater.h>
#endif
#define OTA_UPDATE_ENABLED
#endif

#if defined(ESP32)
#if defined(ESP_ARDUINO_VERSION)
#if ESP_ARDUINO_VERSION > ESP_ARDUINO_VERSION_VAL(2, 0, 1)
#define ESP32_GT_2_0_1_FS_MEMORY_FIX
#endif
#endif
#endif

#if defined(ENABLE_SERVICE_AUTH) || defined(ENABLE_CUSTOM_AUTH)
#define ENABLE_JWT
#endif

#if defined(ENABLE_JWT)

#if !defined(ENABLE_SERVICE_AUTH)
#define ENABLE_SERVICE_AUTH
#endif

#if !defined(ENABLE_CUSTOM_AUTH)
#define ENABLE_CUSTOM_AUTH
#endif

#endif

#if !defined(FIREBASE_ASYNC_CLIENT)
#define FIREBASE_ASYNC_CLIENT AsyncClient
#endif

#endif