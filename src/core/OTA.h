/**
 * Created January 11, 2024
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
#ifndef CORE_OTA_H
#define CORE_OTA_H

#include <Arduino.h>
#include "./Config.h"
#include "Memory.h"
#include "Base64.h"
#include "Error.h"

class OtaHelper
{
public:
    void getPad(uint8_t *s, int &len, int &pad)
    {
        pad = -1;
        if (len > 1)
        {
            if (s[len - 1] == '"')
            {
                pad = 0;
                if (len > 2)
                {
                    if (s[len - 2] == '=')
                        pad++;
                }
                if (len > 3)
                {
                    if (s[len - 3] == '=')
                        pad++;
                }
                s[len - 1] = 0;
                len--;
            }
        }
    }

    bool decodeBase64OTA(Memory &mem, Base64Helper *bh, const char *src, size_t len, int &code)
    {

        bool ret = true;
        firebase_base64_io_t<uint8_t> out;
        uint8_t *buf = reinterpret_cast<uint8_t *>(mem.alloc(out.bufLen));
        out.ota = true;
        out.outT = buf;
        unsigned char *base64DecBuf = bh->creatBase64DecBuffer(mem);
        if (!bh->decode<uint8_t>(mem, base64DecBuf, src, strlen(src), out))
        {
            code = FIREBASE_ERROR_FW_UPDATE_WRITE_FAILED;
            ret = false;
        }
        mem.release(&buf);
        mem.release(&base64DecBuf);
        return ret;
    }

    void prepareDownloadOTA(size_t payloadLen, bool base64, int &code)
    {
        code = 0;
#if defined(OTA_UPDATE_ENABLED) && (defined(ESP32) || defined(ESP8266) || defined(MB_ARDUINO_PICO))
        int size = base64 ? (3 * (payloadLen - 2) / 4) : payloadLen;
#if defined(ESP32) || defined(MB_ARDUINO_PICO)
        if (!Update.begin(size))
            code = FIREBASE_ERROR_FW_UPDATE_TOO_LOW_FREE_SKETCH_SPACE;
#elif defined(ESP8266)
        if (!Update.begin(size))
            code = FIREBASE_ERROR_FW_UPDATE_TOO_LOW_FREE_SKETCH_SPACE;

#endif
#endif
    }

    bool endDownloadOTA(int pad, int &code)
    {
        Base64Helper bh;

#if defined(OTA_UPDATE_ENABLED) && (defined(ESP32) || defined(ESP8266) || defined(MB_ARDUINO_PICO))

        // write extra pad
        if (pad > 0)
        {
            uint8_t buf[pad];
            memset(buf, 0, pad);
            bh.updateWrite(buf, pad);
        }

        if (code == 0)
        {
            if (!Update.end())
                code = FIREBASE_ERROR_FW_UPDATE_END_FAILED;
        }

        return code == 0;

#endif
    }
};

#endif