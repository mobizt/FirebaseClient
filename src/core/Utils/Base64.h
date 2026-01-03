/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_UTILS_BASE64_H
#define CORE_UTILS_BASE64_H

#include <Arduino.h>
#include <Client.h>
#if __has_include(<vector>)
#include <vector>
#endif
#include "./core/Updater/OTAUpdater.h"
#include "./core/Updater/OTAUpdater.cpp"
#include "./core/File/BlobWriter.h"
#include "./core/Utils/Memory.h"

static const char firebase_boundary_table[] PROGMEM = "=_abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const unsigned char firebase_base64_table[65] PROGMEM = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

template <typename T>
struct firebase_base64_io_t
{
    // the total bytes of data in output buffer
    int bufWrite = 0;
    // the size of output buffer
    size_t bufLen = 1024;
// for file to write
#if defined(ENABLE_FS)
    File file;
#endif
    // for T array
    T *outT = nullptr;
    // for T vector
    std::vector<T> *outL = nullptr;
    // for client
    Client *outC = nullptr;
    // for blob
    firebase_blob_writer *outB = nullptr;
    // for ota
    bool ota = false;
};

class Base64Util
{
public:
    int getBase64Len(int n)
    {
        int len = (4 * ceil(n / 3.0));
        return len;
    }

    int getBase64Padding(int n)
    {
        int pLen = getBase64Len(n);
        int uLen = ceil(4.0 * n / 3.0);
        return pLen - uLen;
    }

    size_t encodedLength(size_t len) const { return ((len + 2) / 3 * 4) + 1; }

    int decodedLen(const char *src)
    {
        int len = strlen(src), i = len - 1, pad = 0;
        if (len < 4)
            return 0;
        while (i > 0 && src[i--] == '=')
        {
            pad++;
        }
        return (3 * (len / 4)) - pad;
    }

    unsigned char *creatBase64EncBuffer(Memory &mem, bool isURL)
    {
        unsigned char *base64EncBuf = reinterpret_cast<unsigned char *>(mem.alloc(65));
        strcpy_P(reinterpret_cast<char *>(base64EncBuf), reinterpret_cast<const char *>(firebase_base64_table));
        if (isURL)
        {
            base64EncBuf[62] = '-';
            base64EncBuf[63] = '_';
        }
        return base64EncBuf;
    }

    bool updateWrite(uint8_t *data, size_t len)
    {
#if defined(OTA_UPDATE_ENABLED) && defined(FIREBASE_OTA_UPDATER) && (defined(ENABLE_DATABASE) || defined(ENABLE_STORAGE) || defined(ENABLE_CLOUD_STORAGE))
        return FIREBASE_OTA_UPDATER.write(data, len) == len;
#else
        (void)data;
        (void)len;
#endif
        return false;
    }

    unsigned char *creatBase64DecBuffer(Memory &mem)
    {
        unsigned char *base64DecBuf = reinterpret_cast<unsigned char *>(mem.alloc(256, false));
        memset(base64DecBuf, 0x80, 256);
        for (size_t i = 0; i < sizeof(firebase_base64_table) - 1; i++)
            base64DecBuf[firebase_base64_table[i]] = (unsigned char)i;
        base64DecBuf['='] = 0;
        return base64DecBuf;
    }

    template <typename T = uint8_t>
    bool writeOutput(firebase_base64_io_t<T> &out)
    {
        size_t write = out.bufWrite;
        out.bufWrite = 0;

        if (write == 0)
            return true;

        if (out.outC && out.outC->write(reinterpret_cast<uint8_t *>(out.outT), write) == write)
            return true;
#if defined(ENABLE_FS)
        else if (out.file && out.file.write(reinterpret_cast<uint8_t *>(out.outT), write) == write)
            return true;
#endif
        else if (out.outB && out.outB->write(reinterpret_cast<uint8_t *>(out.outT), write) == write)
            return true;

#if defined(OTA_UPDATE_ENABLED)
        else if (out.ota)
        {
            if (updateWrite(reinterpret_cast<uint8_t *>(out.outT), write))
                return true;
        }
#endif
        return false;
    }

    template <typename T = uint8_t>
    bool setOutput(uint8_t val, firebase_base64_io_t<T> &out, T **pos)
    {
        if (out.outT)
        {
#if defined(ENABLE_FS)
            if (out.ota || out.outC || out.file || out.outB)
#else
            if (out.ota || out.outC || out.outB)
#endif
            {
                out.outT[out.bufWrite++] = val;
                if (out.bufWrite == (int)out.bufLen && !writeOutput(out))
                    return false;
            }
            else
                *(*pos)++ = (T)(val);
        }
        else if (out.outL)
            out.outL->push_back(val);

        return true;
    }

    template <typename T>
    bool decode(Memory &mem, const unsigned char *base64DecBuf, const char *src, size_t len, firebase_base64_io_t<T> &out)
    {
        // the maximum chunk size that writes to output is limited by out.bufLen, the minimum is depending on the source length
        bool ret = false;
        unsigned char *block = reinterpret_cast<unsigned char *>(mem.alloc(4, false));
        unsigned char temp;
        size_t i, count;
        int pad = 0;
        size_t extra_pad;
        T *pos = out.outT ? reinterpret_cast<T *>(&out.outT[0]) : nullptr;
        if (len == 0)
            len = strlen(src);

        count = 0;

        for (i = 0; i < len; i++)
        {
            if (base64DecBuf[(int)src[i]] != 0x80)
                count++;
        }

        if (count == 0)
            goto skip;

        extra_pad = (4 - count % 4) % 4;
        count = 0;
        for (i = 0; i < len + extra_pad; i++)
        {
            unsigned char val;

            if (i >= len)
                val = '=';
            else
                val = src[i];

            temp = base64DecBuf[val];

            if (temp == 0x80)
                continue;

            if (val == '=')
                pad++;

            block[count] = temp;
            count++;
            if (count == 4)
            {
                setOutput((block[0] << 2) | (block[1] >> 4), out, &pos);
                count = 0;
                if (pad)
                {
                    if (pad == 1)
                        setOutput((block[1] << 4) | (block[2] >> 2), out, &pos);
                    else if (pad > 2)
                        goto skip;

                    break;
                }
                else
                {
                    setOutput((block[1] << 4) | (block[2] >> 2), out, &pos);
                    setOutput((block[2] << 6) | block[3], out, &pos);
                }
            }
        }

        // write the remaining bytes
        if (out.bufWrite > 0 && !writeOutput(out))
            goto skip;

        ret = true;

    skip:
        mem.release(&block);
        return ret;
    }

    template <typename T>
    bool encodeLast(const unsigned char *base64EncBuf, const unsigned char *in, size_t len, firebase_base64_io_t<T> &out, T **pos)
    {
        if (len > 2)
            return false;

        if (!setOutput(base64EncBuf[in[0] >> 2], out, pos))
            return false;

        if (len == 1)
        {
            if (!setOutput(base64EncBuf[(in[0] & 0x03) << 4], out, pos))
                return false;
            if (!setOutput('=', out, pos))
                return false;
        }
        else
        {
            if (!setOutput(base64EncBuf[((in[0] & 0x03) << 4) | (in[1] >> 4)], out, pos))
                return false;
            if (!setOutput(base64EncBuf[(in[1] & 0x0f) << 2], out, pos))
                return false;
        }

        if (!setOutput('=', out, pos))
            return false;

        return true;
    }

    template <typename T>
    bool encode(const unsigned char *base64EncBuf, uint8_t *src, size_t len, firebase_base64_io_t<T> &out, bool writeAllRemaining = true)
    {
        const unsigned char *end, *in;

        T *pos = out.outT ? reinterpret_cast<T *>(&out.outT[0]) : nullptr;
        in = src;
        end = src + len;

        while (end - in >= 3)
        {
            if (!setOutput(base64EncBuf[in[0] >> 2], out, &pos))
                return false;
            if (!setOutput(base64EncBuf[((in[0] & 0x03) << 4) | (in[1] >> 4)], out, &pos))
                return false;
            if (!setOutput(base64EncBuf[((in[1] & 0x0f) << 2) | (in[2] >> 6)], out, &pos))
                return false;
            if (!setOutput(base64EncBuf[in[2] & 0x3f], out, &pos))
                return false;
            in += 3;
        }

        if (end - in && !encodeLast(base64EncBuf, in, end - in, out, &pos))
            return false;

        if (writeAllRemaining && out.bufWrite > 0 && !writeOutput(out))
            return false;

        return true;
    }

#if defined(ENABLE_FS)
    bool decodeToFile(Memory &mem, const File &file, const char *src)
    {
        firebase_base64_io_t<uint8_t> out;
        out.file = file;
        uint8_t *buf = reinterpret_cast<uint8_t *>(mem.alloc(out.bufLen));
        out.outT = buf;
        unsigned char *base64DecBuf = creatBase64DecBuffer(mem);
        bool ret = decode<uint8_t>(mem, base64DecBuf, src, strlen(src), out);
        mem.release(&buf);
        mem.release(&base64DecBuf);
        return ret;
    }
#endif
    bool decodeToBlob(Memory &mem, firebase_blob_writer *bWriter, const char *src)
    {
        firebase_base64_io_t<uint8_t> out;
        out.outB = bWriter;
        uint8_t *buf = reinterpret_cast<uint8_t *>(mem.alloc(out.bufLen));
        out.outT = buf;
        unsigned char *base64DecBuf = creatBase64DecBuffer(mem);
        bool ret = decode<uint8_t>(mem, base64DecBuf, src, strlen(src), out);
        mem.release(&buf);
        mem.release(&base64DecBuf);
        return ret;
    }

    void encodeUrl(Memory &mem, char *encoded, const unsigned char *string, size_t len)
    {
        size_t i;
        char *p = encoded;
        unsigned char *base64EncBuf = creatBase64EncBuffer(mem, true);

        for (i = 0; i < len - 2; i += 3)
        {
            *p++ = base64EncBuf[(string[i] >> 2) & 0x3F];
            *p++ = base64EncBuf[((string[i] & 0x3) << 4) | ((int)(string[i + 1] & 0xF0) >> 4)];
            *p++ = base64EncBuf[((string[i + 1] & 0xF) << 2) | ((int)(string[i + 2] & 0xC0) >> 6)];
            *p++ = base64EncBuf[string[i + 2] & 0x3F];
        }

        if (i < len)
        {
            *p++ = base64EncBuf[(string[i] >> 2) & 0x3F];
            if (i == (len - 1))
                *p++ = base64EncBuf[((string[i] & 0x3) << 4)];
            else
            {
                *p++ = base64EncBuf[((string[i] & 0x3) << 4) | ((int)(string[i + 1] & 0xF0) >> 4)];
                *p++ = base64EncBuf[((string[i + 1] & 0xF) << 2)];
            }
        }

        *p++ = '\0';
        mem.release(&base64EncBuf);
    }

    char *encodeToChars(Memory &mem, uint8_t *src, size_t len)
    {
        char *encoded = reinterpret_cast<char *>(mem.alloc(encodedLength(len) + 1));
        firebase_base64_io_t<char> out;
        out.outT = encoded;
        unsigned char *base64EncBuf = creatBase64EncBuffer(mem, false);
        encode<char>(base64EncBuf, src, len, out);
        mem.release(&base64EncBuf);
        return encoded;
    }
};
#endif