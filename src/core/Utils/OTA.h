/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_UTILS_OTA_H
#define CORE_UTILS_OTA_H

#include <Arduino.h>
#include "./core/Utils/Memory.h"
#include "./core/Utils/Base64.h"
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

    bool decodeBase64OTA(Memory &mem, Base64Util *but, const char *src, int16_t &code)
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
    void setOTAStorage(uint32_t addr) { OTAUpdater.setOTAStorage(addr); }
#endif

#if defined(OTA_UPDATE_ENABLED) && defined(FIREBASE_OTA_UPDATER)
    void prepareDownloadOTA(size_t payloadLen, bool base64, int16_t &code, int command)
    {
        code = 0;
        int size = base64 ? (3 * (payloadLen - 2) / 4) : payloadLen;
        if (!FIREBASE_OTA_UPDATER.begin(size, command))
            code = FIREBASE_ERROR_FW_UPDATE_TOO_LOW_FREE_SKETCH_SPACE;
#if defined(FIREBASE_OTA_STORAGE)
        if (!OTAUpdater.isInit())
            code = FIREBASE_ERROR_FW_UPDATE_OTA_STORAGE_CLASS_OBJECT_UNINITIALIZE;
#endif
    }
#endif
#if defined(OTA_UPDATE_ENABLED) && defined(FIREBASE_OTA_UPDATER)
    bool endDownloadOTA(Base64Util &b64ut, int pad, int16_t &code)
    {
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
    }
#endif
};
#endif