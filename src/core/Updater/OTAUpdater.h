/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_UPDATER_OTA_UPDATER_H
#define CORE_UPDATER_OTA_UPDATER_H

#include <Arduino.h>

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
    bool begin(int size, int command = 0);
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