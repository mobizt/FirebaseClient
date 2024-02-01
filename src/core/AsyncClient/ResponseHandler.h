/**
 * Created February 1, 2024
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

#define FIREBASE_TCP_READ_TIMEOUT 30 * 1000

struct async_response_handler_t
{
public:
    enum response_type
    {
        RES_UNDEFINED = -1,
        RES_ERROR = 0,
        RES_OK = 1,
    };
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
        response_type resp_type = RES_UNDEFINED;
        int resp_code = 0;
    };

    int httpCode = 0;
    response_flags flags;
    size_t payloadLen = 0;
    size_t payloadRead = 0;
    auth_error_t error;
    unsigned long dataTime = 0;
    uint8_t *toFill = nullptr;
    uint16_t toFillLen = 0;
    uint16_t toFillIndex = 0;
    String header, payload, location, etag;
    chunk_info_t chunkInfo;
    Timer read_timer;

    ~async_response_handler_t()
    {
        if (toFill)
            free(toFill);
        toFill = nullptr;
        toFillLen = 0;
        toFillIndex = 0;
    }

    void feedTimer()
    {
        read_timer.stop();
        read_timer.setInterval(FIREBASE_TCP_READ_TIMEOUT);
        read_timer.start();
    }
};

#endif