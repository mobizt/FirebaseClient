/**
 * Created March 17, 2024
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
#ifndef ASYNC_RESPONSE_HANDLER_H
#define ASYNC_RESPONSE_HANDLER_H
#include <Arduino.h>
#include <Client.h>
#include "RequestHandler.h"

#define FIREBASE_TCP_READ_TIMEOUT_SEC 30 // Do not change

namespace res_hndlr_ns
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

struct async_response_handler_t
{
public:
    enum chunk_phase
    {
        READ_CHUNK_SIZE = 0,
        READ_CHUNK_DATA = 1
    };

    struct response_flags
    {
    public:
        bool header_remaining = false;
        bool payload_remaining = false;
        bool keep_alive = false;
        bool sse = false;
        bool http_response = false;
        bool chunks = false;
        bool payload_available = false;

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
    size_t payloadLen = 0;
    size_t payloadRead = 0;
    auth_error_t error;
    uint8_t *toFill = nullptr;
    uint16_t toFillLen = 0;
    uint16_t toFillIndex = 0;
    String val[res_hndlr_ns::max_type];
    chunk_info_t chunkInfo;
    Timer read_timer;
    bool auth_data_available = false;

    async_response_handler_t()
    {
    }

    ~async_response_handler_t()
    {
        if (toFill)
            free(toFill);
        toFill = nullptr;
        toFillLen = 0;
        toFillIndex = 0;
    }

    void clear()
    {
        httpCode = 0;
        flags.reset();
        payloadLen = 0;
        payloadRead = 0;
        error.resp_code = 0;
        error.string.remove(0, error.string.length());
        if (toFill)
            delete toFill;
        toFill = nullptr;
        toFillLen = 0;
        toFillIndex = 0;
        for (size_t i = 0; i < res_hndlr_ns::max_type; i++)
            val[i].remove(0, val[i].length());
        chunkInfo.chunkSize = 0;
        chunkInfo.dataLen = 0;
        chunkInfo.phase = READ_CHUNK_SIZE;
    }

    void feedTimer(int interval = -1)
    {
        read_timer.feed(interval == -1 ? FIREBASE_TCP_READ_TIMEOUT_SEC : interval);
    }

    int tcpAvailable(async_request_handler_t::tcp_client_type client_type, Client *client, void *atcp_config)
    {
        if (client_type == async_request_handler_t::tcp_client_type_sync)
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

    int tcpRead(async_request_handler_t::tcp_client_type client_type, Client *client, void *atcp_config)
    {
        if (client_type == async_request_handler_t::tcp_client_type_sync)
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

    int tcpRead(async_request_handler_t::tcp_client_type client_type, Client *client, void *atcp_config, uint8_t *buf, size_t size)
    {
        if (client_type == async_request_handler_t::tcp_client_type_sync)
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
};

#endif