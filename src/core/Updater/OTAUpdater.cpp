/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_UPDATER_OTA_UPDATER_CPP
#define CORE_UPDATER_OTA_UPDATER_CPP

#include <Arduino.h>
#include "./core/Updater/OTAUpdater.h"

#if defined(FIREBASE_OTA_STORAGE)

OTAUpdaterClass::OTAUpdaterClass() {}

OTAUpdaterClass::~OTAUpdaterClass() {}

bool OTAUpdaterClass::begin(int size, int command)
{
    void(command);
#if defined(FIREBASE_OTA_STORAGE)
    if (storage && storage->open(size) > 0)
        return true;
#endif
    return false;
}

bool OTAUpdaterClass::end()
{
    close();
    return true;
}

void OTAUpdaterClass::setOTAStorage(uint32_t addr)
{
    this->addr = addr;
    storage = reinterpret_cast<OTAStorage *>(addr);
}

bool OTAUpdaterClass::isInit() { return addr > 0; }

size_t OTAUpdaterClass::write(uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
        write(data[i]);
    return len;
}

size_t OTAUpdaterClass::write(uint8_t b)
{
    if (storage)
        storage->write(b);
    return 1;
}

void OTAUpdaterClass::close()
{
    if (storage)
        storage->close();
}

void OTAUpdaterClass::apply()
{
    if (storage)
        storage->apply();
}

OTAUpdaterClass OTAUpdater;
#endif
#endif