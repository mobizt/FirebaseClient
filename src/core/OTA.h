/**
 * Created November 28, 2024
 *
 * For MCU build target (CORE_ARDUINO_XXXX), see Options.h.
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
#include "./core/Memory.h"
#include "./core/Base64.h"
#include "./core/Error.h"

class OTAUtil
{
public:
    void getPad(uint8_t *s, int &len, int8_t &pad)
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

    bool decodeBase64OTA(Memory &mem, Base64Util *but, const char *src, size_t len, int16_t &code)
    {
        bool ret = true;
        firebase_base64_io_t<uint8_t> out;
        uint8_t *buf = reinterpret_cast<uint8_t *>(mem.alloc(out.bufLen));
        out.ota = true;
        out.outT = buf;
        unsigned char *base64DecBuf = but->creatBase64DecBuffer(mem);
        if (!but->decode<uint8_t>(mem, base64DecBuf, src, strlen(src), out))
        {
            code = FIREBASE_ERROR_FW_UPDATE_WRITE_FAILED;
            ret = false;
        }
        mem.release(&buf);
        mem.release(&base64DecBuf);
        return ret;
    }

#if defined(FIREBASE_OTA_STORAGE)
    void setOTAStorage(uint32_t addr)
    {
        OTAUpdater.setOTAStorage(addr);
    }
#endif

    void prepareDownloadOTA(size_t payloadLen, bool base64, int16_t &code)
    {
        code = 0;
#if defined(OTA_UPDATE_ENABLED) && defined(FIREBASE_OTA_UPDATER)
        int size = base64 ? (3 * (payloadLen - 2) / 4) : payloadLen;
        if (!FIREBASE_OTA_UPDATER.begin(size))
            code = FIREBASE_ERROR_FW_UPDATE_TOO_LOW_FREE_SKETCH_SPACE;
#if defined(FIREBASE_OTA_STORAGE)
        if (!OTAUpdater.isInit())
            code = FIREBASE_ERROR_FW_UPDATE_OTA_STORAGE_CLASS_OBJECT_UNINITIALIZE;
#endif
#endif
    }

    bool endDownloadOTA(Base64Util &b64ut, int pad, int16_t &code)
    {
#if defined(OTA_UPDATE_ENABLED) && defined(FIREBASE_OTA_UPDATER)
        // write extra pad
        if (pad > 0)
        {
            uint8_t buf[pad];
            memset(buf, 0, pad);
            b64ut.updateWrite(buf, pad);
        }

        if (code == 0)
        {
            if (!FIREBASE_OTA_UPDATER.end())
                code = FIREBASE_ERROR_FW_UPDATE_END_FAILED;
        }

        return code == 0;

#endif
        return false;
    }
};

#endif