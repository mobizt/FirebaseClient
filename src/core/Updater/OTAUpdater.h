/**
 * Created November 28, 2024
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

#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <Arduino.h>
#include "./Config.h"


#if __has_include(<OTAStorage.h>)

#include <OTAStorage.h>
#define FIREBASE_OTA_STORAGE
#define FIREBASE_OTA_UPDATER OTAUpdater

#elif defined(ESP32) || defined(ESP8266) || defined(CORE_ARDUINO_PICO)
#define FIREBASE_OTA_UPDATER Update
#endif

#if defined(FIREBASE_OTA_STORAGE)

class OTAUpdaterClass
{

public:
    OTAUpdaterClass();
    ~OTAUpdaterClass();
    bool begin(int size);
    bool end();
    size_t write(uint8_t *data, size_t len);
    void setOTAStorage(uint32_t addr);
    bool isInit();

private:
    OTAStorage *storage = nullptr;

    uint32_t addr = 0;
    size_t write(uint8_t b);
    void close();
    void apply();
};

extern OTAUpdaterClass OTAUpdater;

#endif

#endif