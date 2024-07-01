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

#ifndef UPDATER_ARDUINO_CPP
#define UPDATER_ARDUINO_CPP

#include <Arduino.h>
#include "./Config.h"
#include "./core/Updater/UpdaterArduino.h"

UpdaterArduino::UpdaterArduino() {}
UpdaterArduino::~UpdaterArduino() {}

bool UpdaterArduino::begin(int size)
{
#if defined(FIREBASE_UPDATER_INTERNAL_STORAGE)
    if (FIREBASE_UPDATER_INTERNAL_STORAGE.open(size) > 0)
        return true;
#endif
    return false;
}

bool UpdaterArduino::end()
{
    close();
    return true;
}

size_t UpdaterArduino::write(uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
        write(data[i]);
    return len;
}

size_t UpdaterArduino::write(uint8_t b)
{
#if defined(FIREBASE_UPDATER_INTERNAL_STORAGE)
    FIREBASE_UPDATER_INTERNAL_STORAGE.write(b);
#endif
    return 1;
}

void UpdaterArduino::close()
{
#if defined(FIREBASE_UPDATER_INTERNAL_STORAGE)
    FIREBASE_UPDATER_INTERNAL_STORAGE.close();
#endif
}

void UpdaterArduino::apply()
{
#if defined(FIREBASE_UPDATER_INTERNAL_STORAGE)
    FIREBASE_UPDATER_INTERNAL_STORAGE.apply();
#endif
}

#if defined(FIREBASE_UPDATER_INTERNAL_STORAGE)
UpdaterArduino Update;
#endif

#endif