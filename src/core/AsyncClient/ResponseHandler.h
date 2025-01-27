/**
 * 2025-01-26
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 K. Suwatchai (Mobizt)
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
#ifndef ASYNC_RESPONSE_HANDLER_H
#define ASYNC_RESPONSE_HANDLER_H
#include <Arduino.h>
#include <Client.h>
#include "RequestHandler.h"
#include "./core/StringUtil.h"

#define FIREBASE_TCP_READ_TIMEOUT_SEC 30 // Do not change

namespace resns
{
    enum data_item_type_t
    {
        header,
        payload,
        etag,
        location,
        max_type
    };
}

struct res_handler
{
private:
    StringUtil sut;

public:
    enum chunk_phase
    {
        READ_CHUNK_SIZE = 0,
        READ_CHUNK_DATA = 1
    };

    struct response_flags
    {

    public:
        bool header_remaining = false, payload_remaining = false, keep_alive = false, uploadRange = false;
        bool sse = false, http_response = false, chunks = false, payload_available = false;

        void reset()
        {
            header_remaining = false;
            payload_remaining = false;
            keep_alive = false;
            sse = false;
            chunks = false;
            payload_available = false;
        }
    };

    struct chunk_info_t
    {
        chunk_phase phase = READ_CHUNK_SIZE;
        int chunkSize = 0;
        int dataLen = 0;
    };

    struct auth_error_t
    {
        String string;
        int resp_code = 0;
    };

    int httpCode = 0;
    response_flags flags;
    size_t payloadLen = 0, payloadRead = 0;
    auth_error_t error;
    uint8_t *toFill = nullptr;
    uint16_t toFillLen = 0, toFillIndex = 0;
    String val[resns::max_type];
    chunk_info_t chunkInfo;
    Timer read_timer;
    bool auth_data_available = false;

    tcp_client_type client_type;
    Client *client = nullptr;
    void *atcp_config = nullptr;

    res_handler() {}

    ~res_handler()
    {
        if (toFill)
            free(toFill);
        toFill = nullptr;
        toFillLen = 0;
        toFillIndex = 0;
    }

    void setClient(tcp_client_type client_type, Client *client, void *atcp_config)
    {
        this->client_type = client_type;
        this->client = client;
        this->atcp_config = atcp_config;
    }

    void clear()
    {
        httpCode = 0;
        flags.reset();
        payloadLen = 0;
        payloadRead = 0;
        error.resp_code = 0;
        sut.clear(error.string);
        if (toFill)
            delete toFill;
        toFill = nullptr;
        toFillLen = 0;
        toFillIndex = 0;
        for (size_t i = 0; i < resns::max_type; i++)
            sut.clear(val[i]);
        chunkInfo.chunkSize = 0;
        chunkInfo.dataLen = 0;
        chunkInfo.phase = READ_CHUNK_SIZE;
    }

    void feedTimer(int interval = -1) { read_timer.feed(interval == -1 ? FIREBASE_TCP_READ_TIMEOUT_SEC : interval); }

    int tcpAvailable()
    {
        if (client_type == tcpc_sync)
            return client ? client->available() : 0;
        else
        {
#if defined(ENABLE_ASYNC_TCP_CLIENT)
            AsyncTCPConfig *async_tcp_config = reinterpret_cast<AsyncTCPConfig *>(atcp_config);
            if (!async_tcp_config && !async_tcp_config->tcpReceive)
                return 0;

            uint8_t buf[1];
            async_tcp_config->tcpReceive(buf, 1, async_tcp_config->filledSize, async_tcp_config->available);

            if (async_tcp_config->filledSize)
            {
                memcpy(async_tcp_config->buff + async_tcp_config->buffPos, buf, async_tcp_config->filledSize);
                async_tcp_config->buffPos++;
            }

            return async_tcp_config->available;
#endif
        }

        return 0;
    }

    int tcpRead()
    {
        if (client_type == tcpc_sync)
            return client ? client->read() : -1;
        else
        {
#if defined(ENABLE_ASYNC_TCP_CLIENT)

            AsyncTCPConfig *async_tcp_config = reinterpret_cast<AsyncTCPConfig *>(atcp_config);
            if (!async_tcp_config && !async_tcp_config->tcpReceive)
                return 0;

            if (async_tcp_config->buffPos)
            {
                uint8_t v = async_tcp_config->buff[0];
                async_tcp_config->buffPos--;

                if (async_tcp_config->buffPos)
                    memmove(async_tcp_config->buff, async_tcp_config->buff + 1, async_tcp_config->buffPos);

                return v;
            }

            async_tcp_config->tcpReceive(async_tcp_config->buff, 1, async_tcp_config->filledSize, async_tcp_config->available);

            if (async_tcp_config->filledSize)
                async_tcp_config->buffPos = async_tcp_config->filledSize;

            return async_tcp_config->filledSize ? async_tcp_config->buff[0] : -1;
#endif
        }

        return 0;
    }

    int tcpRead(uint8_t *buf, size_t size)
    {
        if (client_type == tcpc_sync)
            return client ? client->read(buf, size) : -1;
        else
        {
#if defined(ENABLE_ASYNC_TCP_CLIENT)
            AsyncTCPConfig *async_tcp_config = reinterpret_cast<AsyncTCPConfig *>(atcp_config);
            if (!async_tcp_config && !async_tcp_config->tcpReceive)
                return 0;

            int pos = 0;
            if (async_tcp_config->buffPos)
            {
                int read = async_tcp_config->buffPos;
                if (read > (int)size)
                    read = size;

                memcpy(buf, async_tcp_config->buff, read);
                async_tcp_config->buffPos -= read;

                if (async_tcp_config->buffPos)
                    memmove(async_tcp_config->buff, async_tcp_config->buff + read, async_tcp_config->buffPos);

                if ((int)size <= read)
                    return read;

                pos = read;
                size -= read;
            }

            if (size > 0)
            {
                async_tcp_config->buffPos = 0;
                uint8_t buff[size];
                async_tcp_config->tcpReceive(buff + pos, size, async_tcp_config->filledSize, async_tcp_config->available);
                return async_tcp_config->filledSize ? async_tcp_config->filledSize : -1;
            }

            return -1;
#endif
        }

        return 0;
    }

    void parseRespHeader(String &out, const char *header)
    {
        out.remove(0, out.length());
        if (httpCode > 0)
        {
            int p1 = -1, p2 = -1, p3 = -1;
            p1 = val[resns::header].indexOf(header);
            if (p1 > -1)
                p2 = val[resns::header].indexOf(':', p1);

            if (p2 > -1)
                p3 = val[resns::header].indexOf("\r\n", p2);

            if (p2 > -1 && p3 > -1)
                out = val[resns::header].substring(p2 + 1, p3);

            out.trim();
        }
    }

    int getStatusCode()
    {
        String out;
        int p1 = val[resns::header].indexOf("HTTP/1.");
        if (p1 > -1)
        {
            out = val[resns::header].substring(p1 + 9, val[resns::header].indexOf(' ', p1 + 9));
            return atoi(out.c_str());
        }
        return 0;
    }

    int getChunkSize(String &line)
    {
        if (line.length() == 0)
            readLine(&line);

        int p = line.indexOf(";");
        if (p == -1)
            p = line.indexOf("\r\n");
        if (p != -1)
            chunkInfo.chunkSize = hex2int(line.substring(0, p).c_str());

        return chunkInfo.chunkSize;
    }

    // Returns -1 when complete
    int decodeChunks(String *out)
    {
        if (!client || !out)
            return 0;
        int res = 0, read = 0;
        String line;

        // because chunks might span multiple reads, we need to keep track of where we are in the chunk
        // chunkInfo.dataLen is our current position in the chunk
        // chunkInfo.chunkSize is the total size of the chunk

        // readline() only reads while there is data available, so it might return early
        // when available() is less than the remaining amount of data in the chunk

        // read chunk-size, chunk-extension (if any) and CRLF
        if (chunkInfo.phase == READ_CHUNK_SIZE)
        {
            chunkInfo.phase = READ_CHUNK_DATA;
            chunkInfo.chunkSize = -1;
            chunkInfo.dataLen = 0;
            res = getChunkSize(line);
            payloadLen += res > -1 ? res : 0;
        }
        // read chunk-data and CRLF
        // append chunk-data to entity-body
        else
        {
            // if chunk-size is 0, it's the last chunk, and can be skipped
            if (chunkInfo.chunkSize > 0)
            {
                read = readLine(&line);

                // if we read till a CRLF, we have a chunk (or the rest of it)
                // if the last two bytes are NOT CRLF, we have a partial chunk
                // if we read 0 bytes, read next chunk size

                // check for \n and \r, remove them if present (they're part of the protocol, not the data)
                if (read >= 2 && line[read - 2] == '\r' && line[read - 1] == '\n')
                {
                    // last chunk?
                    if (line[0] == '0')
                        return -1;

                    // remove the \r\n
                    line.remove(line.length() - 2);
                    read -= 2;
                }

                // if we still have data, append it and update the chunkInfo
                if (read)
                {
                    *out += line;
                    chunkInfo.dataLen += read;
                    payloadRead += read;

                    // check if we're done reading this chunk
                    if (chunkInfo.dataLen == chunkInfo.chunkSize)
                        chunkInfo.phase = READ_CHUNK_SIZE;
                }
                // if we read 0 bytes, read next chunk size
                else
                {
                    chunkInfo.phase = READ_CHUNK_SIZE;
                }
            }
            else
            {

                read = readLine(&line);

                // CRLF (end of chunked body)
                if (read == 2 && line[0] == '\r' && line[1] == '\n')
                    res = -1;
                else // another chunk?
                    getChunkSize(line);
            }
        }

        return res;
    }

    bool readStatusLine()
    {
        if (httpCode > 0)
            return false;

        val[resns::header].reserve(1024);

        // The first chunk (line) can be http response status or already connected stream payload
        readLine();
        int status = getStatusCode();
        if (status > 0)
        {
            // http response status
            flags.header_remaining = true;
            httpCode = status;
        }
        return true;
    }

    uint32_t hex2int(const char *hex)
    {
        uint32_t val = 0;
        while (*hex)
        {
            // get current character then increment
            uint8_t byte = *hex++;
            // transform hex character to the 4bit equivalent number, using the ascii table indexes
            if (byte >= '0' && byte <= '9')
                byte = byte - '0';
            else if (byte >= 'a' && byte <= 'f')
                byte = byte - 'a' + 10;
            else if (byte >= 'A' && byte <= 'F')
                byte = byte - 'A' + 10;
            // shift 4 to make space for new digit, and add the 4 bits of the new digit
            val = (val << 4) | (byte & 0xF);
        }
        return val;
    }

    int readLine(String *buf = nullptr)
    {
        if (!buf)
            buf = &val[resns::header];

        int p = 0;
        while (tcpAvailable())
        {
            int res = tcpRead();
            if (res > -1)
            {
                *buf += (char)res;
                p++;
                if (res == '\n')
                    return p;
            }
        }
        return p;
    }

    bool endOfHeader(int read)
    {
        return ((read == 1 && val[resns::header][val[resns::header].length() - 1] == '\n') ||
                (read == 2 && val[resns::header][val[resns::header].length() - 2] == '\r' && val[resns::header][val[resns::header].length() - 1] == '\n'));
    }

    bool readHeader(bool sse, bool clearPayload, bool upload)
    {
        bool ret = endOfHeader(readLine());
        if (ret)
        {
            flags.http_response = true;
            val[resns::etag].remove(0, val[resns::etag].length());
            parseRespHeader(val[resns::etag], "ETag");

            String temp;
            parseRespHeader(temp, "Content-Length");
            payloadLen = atoi(temp.c_str());

            parseRespHeader(temp, "Connection");
            flags.keep_alive = temp.length() && temp.indexOf("keep-alive") > -1;

            parseRespHeader(temp, "Transfer-Encoding");
            flags.chunks = temp.length() && temp.indexOf("chunked") > -1;

            parseRespHeader(temp, "Content-Type");
            flags.sse = temp.length() && temp.indexOf("text/event-stream") > -1;

            clear(clearPayload);

            if (upload)
            {
                parseRespHeader(temp, "Range");
                if (httpCode == FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT && temp.indexOf("bytes=") > -1)
                    flags.uploadRange = true;
            }

            if (httpCode > 0 && httpCode != FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
                flags.payload_remaining = true;

            if (!sse && (httpCode == FIREBASE_ERROR_HTTP_CODE_OK || httpCode == FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT || httpCode == FIREBASE_ERROR_HTTP_CODE_NO_CONTENT) && !flags.chunks && payloadLen == 0)
                flags.payload_remaining = false;
        }
        return ret;
    }

    void clear(bool clearPayload)
    {
        val[resns::header].remove(0, val[resns::header].length());
        if (clearPayload)
            val[resns::payload].remove(0, val[resns::payload].length());
        flags.header_remaining = false;
        flags.payload_remaining = false;
        flags.uploadRange = false;
        payloadRead = 0;
        error.resp_code = 0;
        error.string.remove(0, error.string.length());
        chunkInfo.chunkSize = 0;
        chunkInfo.dataLen = 0;
        chunkInfo.phase = res_handler::READ_CHUNK_SIZE;
    }
};

#endif