/*
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef ESP_SSL_CLIENT_MEMORY_H
#define ESP_SSL_CLIENT_MEMORY_H
#include <Arduino.h>

static size_t esp_sslclient_get_reserve_len(size_t len)
{
    int blen = len + 1;
    int newlen = (blen / 4) * 4;
    if (newlen < blen)
        newlen += 4;
    return (size_t)newlen;
}

static void *esp_sslclient_malloc(size_t len)
{
    void *p = NULL;
    size_t newLen = esp_sslclient_get_reserve_len(len);

#if defined(BOARD_HAS_PSRAM)
    if (ESP.getPsramSize() > 0)
        p = reinterpret_cast<void *>(ps_malloc(newLen));
    else
        p = reinterpret_cast<void *>(malloc(newLen));
#else

#if defined(ESP8266_USE_EXTERNAL_HEAP)
    ESP.setExternalHeap();
#endif

    p = reinterpret_cast<void *>(malloc(newLen));
#if defined(ESP8266_USE_EXTERNAL_HEAP)
    ESP.resetHeap();
#endif

#endif
    return p;
}

static void esp_sslclient_free(void *ptr)
{
    void **p = reinterpret_cast<void **>(ptr);
    if (*p)
    {
        free(*p);
        *p = 0;
    }
}

static void *esp_sslclient_realloc(void *ptr, size_t sz)
{
    size_t newLen = esp_sslclient_get_reserve_len(sz);
#if defined(BOARD_HAS_PSRAM)
    if (ESP.getPsramSize() > 0)
        ptr = reinterpret_cast<void *>(ps_realloc(ptr, newLen));
    else
        ptr = reinterpret_cast<void *>(realloc(ptr, newLen));
#else

#if defined(ESP8266_USE_EXTERNAL_HEAP)
    ESP.setExternalHeap();
#endif

    ptr = reinterpret_cast<void *>(realloc(ptr, newLen));

#if defined(ESP8266_USE_EXTERNAL_HEAP)
    ESP.resetHeap();
#endif

#endif
    return ptr;
}

#endif