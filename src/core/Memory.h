/**
 * Created June 12, 2024
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
#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include <Arduino.h>
#include "./Config.h"

#if defined(ESP8266) && defined(MMU_EXTERNAL_HEAP)
#include <umm_malloc/umm_malloc.h>
#include <umm_malloc/umm_heap_select.h>
#if !defined(ESP8266_USE_EXTERNAL_HEAP)
#define ESP8266_USE_EXTERNAL_HEAP
#endif
#endif

class Memory
{
private:
public:
    Memory() {}
    ~Memory() {}

    // Free reserved memory at pointer.
    void release(void *ptr)
    {
        void **p = reinterpret_cast<void **>(ptr);
        if (*p)
        {
            free(*p);
            *p = 0;
        }
    }

    void *alloc(size_t len, bool clear = true)
    {
        void *p = nullptr;
        size_t newLen = getReservedLen(len);
#if defined(BOARD_HAS_PSRAM)
        if (ESP.getPsramSize() > 0)
            p = reinterpret_cast<void *>(ps_malloc(newLen));
        else
            p = reinterpret_cast<void *>(malloc(newLen));

        if (!p)
            return NULL;
#else

#if defined(ESP8266_USE_EXTERNAL_HEAP)
        ESP.setExternalHeap();
#endif
        p = reinterpret_cast<void *>(malloc(newLen));
        bool nn = p ? true : false;
#if defined(ESP8266_USE_EXTERNAL_HEAP)
        ESP.resetHeap();
#endif

        if (!nn)
            return NULL;

#endif
        if (clear)
            memset(p, 0, newLen);
        return p;
    }

    size_t getReservedLen(size_t len)
    {
        int blen = len + 1;
        int newlen = (blen / 4) * 4;
        if (newlen < blen)
            newlen += 4;
        return (size_t)newlen;
    }
};

#endif