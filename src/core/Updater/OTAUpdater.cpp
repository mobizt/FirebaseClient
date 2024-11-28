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

#ifndef OTA_UPDATER_CPP
#define OTA_UPDATER_CPP

#include <Arduino.h>
#include "./core/Updater/OTAUpdater.h"

#if defined(FIREBASE_OTA_STORAGE)

OTAUpdaterClass::OTAUpdaterClass() {}
OTAUpdaterClass::~OTAUpdaterClass() {}

bool OTAUpdaterClass::begin(int size)
{
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