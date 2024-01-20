/**
 * Created November 8, 2023
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 K. Suwatchai (Mobizt)
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

#if !defined(DEFAULT_DEBUG_PORT)
#define DEFAULT_DEBUG_PORT Serial
#endif

#if defined(DISABLE_ALL_OPTIONS)

#undef ENABLE_DATABASE
#undef ENABLE_FIRESTORE
#undef ENABLE_MESSAGING
#undef ENABLE_STORAGE
#undef ENABLE_CLOUD_STORAGE
#undef ENABLE_FUNCTIONS
#undef ENABLE_PSRAM
#undef ENABLE_NTP_TIME
#undef ENABLE_ERROR_STRING
#undef ENABLE_OTA
#undef ENABLE_FS
#undef ENABLE_KEEPALIVE
#undef DEFAULT_DEBUG_PORT

#endif


#if defined(DISABLE_DATABASE)
#undef ENABLE_DATABASE
#endif

#if defined(DISABLE_FIRESTORE)
#undef ENABLE_FIRESTORE
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

#if defined(DISABLE_NPT_TIME)
#undef ENABLE_NTP_TIME
#endif

#if defined(DISABLE_ERROR_STRING)
#undef ENABLE_ERROR_STRING
#endif

#if defined(DISABLE_OTA)
#undef ENABLE_OTA
#endif

#if defined(DISABLE_FS)
#undef ENABLE_FS
#endif

#if defined(DISABLE_KEEPALIVE)
#undef ENABLE_KEEPALIVE
#endif

#if defined(DISABLE_DEBUG)
#undef DEFAULT_DEBUG_PORT
#endif


#if !defined(FIREBASE_RTDB_STREAM_TASK_STACK_SIZE)
#define FIREBASE_RTDB_STREAM_TASK_STACK_SIZE 8192
#endif

#if !defined(FIREBASE_MAX_BLOB_PAYLOAD_SIZE)
#define FIREBASE_MAX_BLOB_PAYLOAD_SIZE 1024
#endif

#if !defined(FIREBASE_MAX_ASYNC_QUEUE)
#define FIREBASE_MAX_ASYNC_QUEUE 20
#endif

#if defined(ENABLE_PSRAM)
#define FIREBASE_USE_PSRAM
#define MB_STRING_USE_PSRAM
#endif


#if defined(ENABLE_FS)
#include <FS.h>
#define FILEOBJ File
#else
class dmFile : public Stream
{
public:
    operator bool() { return 0; }
    size_t write(uint8_t) { return 0; }
    size_t write(const uint8_t *buf, size_t size) { return 0; }
    int available() { return 0; }
    int read() { return 0; }
    int peek() { return 0; }
    void flush() {}
    int read(uint8_t *buf, size_t size) { return 0; }
    void close() {}
    size_t position() { return 0; }
    size_t size() { return 0; }
};
#define FILEOBJ dmFile
#endif

#if defined(ENABLE_OTA) && (defined(ENABLE_DATABASE) || defined(ENABLE_STORAGE) || defined(ENABLE_CLOUD_STORAGE))
#if defined(ESP32)
#include <Update.h>
#elif defined(ESP8266) || defined(MB_ARDUINO_PICO)
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

#endif