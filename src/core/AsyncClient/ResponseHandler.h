/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_CLIENT_RESPONSE_HANDLER_H
#define CORE_ASYNC_CLIENT_RESPONSE_HANDLER_H

#include <Arduino.h>
#include <Client.h>
#include "./core/Error.h"
#include "./core/AsyncClient/ConnectionHandler.h"
#include "./core/AsyncClient/RequestHandler.h"
#include "./core/Utils/StringUtil.h"

namespace resns
{
    enum data_item_type_t
    {
        header,
        status,
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
    struct response_flags
    {

    public:
        bool keep_alive = false, uploadRange = false;
        bool sse = false, http_response = false, chunks = false, payload_available = false, gzip = false;

        void reset()
        {
            keep_alive = false;
            sse = false;
            chunks = false;
            payload_available = false;
            gzip = false;
            uploadRange = false;
        }
    };

    enum response_stage
    {
        response_stage_undefined,
        response_stage_init,
        response_stage_status_line,
        response_stage_header,
        response_stage_payload,
        response_stage_finished,
        response_stage_error
    };

    struct ResponseContext
    {
    public:
        const int bufSize = 512; // Maximum size of the buffer (including null terminator).
        uint8_t *buf = nullptr;  //  Byte array to store the read data.
        const int hdrSize = 512;
        char *hdr = nullptr;
        String *location = nullptr;
        int totalRead = 0;       //  Reference to a counter tracking total bytes read (caller must reset).
        char endToken = '\n';    // Character to stop reading at (e.g., '\n' for headers/SSE). Pass 0 to read until the buffer is full (binary/body mode).
        long bytesRemState = -1; // [IN/OUT] Stateful counter. Default to -1 (Infinite/Unknown)
        bool isChunked = false;  // Set true if "Transfer-Encoding: chunked" is detected. Enables the internal Hex parser
        response_stage stage = response_stage_undefined;
        bool isUpload = false;
        bool isSSE = false;
        bool isAuth = false;
        long tempSize = 0;
        bool stateChunkSizeRead = false; // Tracks if we have actually read digits for the current chunk size

        void begin()
        {
            newBuf();
            newHdr();
            totalRead = 0;
            endToken = '\n';
            bytesRemState = -1; // Default to -1 (Infinite/Unknown)
            isChunked = false;
            isUpload = false;
            location = nullptr;
            isSSE = false;
            isAuth = false;
            stage = response_stage_init;
            tempSize = 0;
            stateChunkSizeRead = false;
        }

        void newHdr()
        {
            freeHdr();
            hdr = (char *)malloc(hdrSize);
        }

        void newBuf()
        {
            freeBuf();
            buf = (uint8_t *)malloc(bufSize);
        }

        void freeBuf()
        {
            if (buf)
                free(buf);
            buf = nullptr;
        }

        void freeHdr()
        {
            if (hdr)
                free(hdr);
            hdr = nullptr;
        }

        ~ResponseContext()
        {
            freeBuf();
            freeHdr();
        }
    };

    struct auth_error_t
    {
        String string;
        int resp_code = 0;
    };

    int httpCode = 0, statusCode = 0;
    response_flags flags;
    size_t payloadLen = 0, payloadRead = 0, xx = 0;
    auth_error_t error;
    uint8_t *toFill = nullptr;
    uint16_t toFillLen = 0, toFillIndex = 0;
    String val[resns::max_type];
    ResponseContext respCtx;
    Timer read_timer;
    bool auth_data_available = false;

    tcp_client_type client_type = tcpc_sync;
    Client *client = nullptr;
    Memory mem;

    res_handler() {}

    ~res_handler()
    {
        mem.release(&toFill);
        toFillLen = 0;
        toFillIndex = 0;
    }

    void setClient(tcp_client_type client_type, Client *client)
    {
        this->client_type = client_type;
        this->client = client;
    }

    void clear()
    {
        httpCode = 0;
        flags.reset();
        payloadLen = 0;

        if (toFill)
            delete toFill;
        toFill = nullptr;
        toFillLen = 0;
        toFillIndex = 0;

        for (size_t i = 0; i < resns::max_type; i++)
            sut.clear(val[i]);

        payloadRead = 0;
        error.resp_code = 0;
        sut.clear(error.string);
    }

    void clearHeader(bool clearPayload)
    {
        sut.clear(val[resns::header]);
        if (clearPayload)
            sut.clear(val[resns::payload]);

        payloadRead = 0;
        error.resp_code = 0;
        sut.clear(error.string);
    }

    void feedTimer(int interval = -1) { read_timer.feed(interval == -1 ? FIREBASE_TCP_READ_TIMEOUT_SEC : interval); }

    int tcpAvailable()
    {
        if (client_type == tcpc_sync)
            return client ? client->available() : 0;

        return 0;
    }

    int tcpRead(uint8_t *buf, size_t size)
    {
        if (client_type == tcpc_sync)
            return client ? client->read(buf, size) : -1;
        return 0;
    }

    /**
     * @brief Reads data from a network stream with support for HTTP Chunked Encoding,
     * Content-Length limits, and Line-based parsing (SSE).
     * * This function acts as a filter. If respCtx.isChunked is true, it strips HTTP hex
     * headers and returns only the payload. It handles timeouts and connection
     * checks automatically.
     * @tparam TSRC Source stream type (e.g., WiFiClient, EthernetClient)
     * @tparam TSNK Debug sink type (e.g., HardwareSerial)
     * @param source      Pointer to the network client to read from.
     * @param sink        Pointer to a Serial object for debug logging (can be NULL).
     * - If respCtx.isChunked=true:  Internal state (counts bytes left in current chunk).
     * @param respCtx    Reference to a ResponseContext struct that maintains state across calls.
     * Must initialize to 0 before starting body.
     * - If respCtx.isChunked=false: Acts as 'Content-Length'.
     * Set to positive value to stop reading after N bytes.
     * Set to -1 to read indefinitely (until close).
     * @return int
     * > 0 : Number of bytes written to buffer.
     * 0 : No data currently available (keep waiting).
     * -1 : Connection closed / Stream finished.
     * -2 : Timeout (data transfer stalled for >5000ms).
     */
    template <typename TSRC, typename TSNK>
    static int readResponse(TSRC *source, TSNK *sink, ResponseContext &respCtx)
    {
        char *buffer = (char *)respCtx.buf;
        const char endToken = respCtx.stage == response_stage_payload ? 0 : respCtx.endToken;
        size_t pos = 0;
        int c;

        unsigned long ms_start = millis();
        unsigned long timeout = 5000;

        // Reset state if we are switching out of chunked mode
        if (!respCtx.isChunked)
        {
            respCtx.stateChunkSizeRead = false;
            respCtx.tempSize = 0;
        }

        if (!source->connected() && source->available() == 0 && respCtx.totalRead == 0)
            return -1; // Socket closed

        while ((source->connected() || source->available()) && (millis() - ms_start < timeout))
        {
            // Content-Length Limit Check (Non-Chunked)
            if (!respCtx.isChunked && respCtx.bytesRemState == 0)
            {
                buffer[pos] = '\0';
                if (sink)
                    sink->print(buffer);
                return (int)pos;
            }

            if (!source->available())
            {
                sys_idle();
                continue;
            }

            c = source->read();
            if (c == -1)
                continue;

            if (respCtx.stage == response_stage_payload) // payload read
            {
                if (respCtx.isChunked)
                {
                    if (respCtx.bytesRemState == 0)
                    {
                        // We are reading the Hex Size Line
                        if (c == '\r')
                            continue; // ignore CR

                        if (c == '\n')
                        {
                            // If we hit a newline but haven't seen any digits yet,
                            // it is just the trailing whitespace of the previous chunk.
                            if (!respCtx.stateChunkSizeRead)
                                continue;

                            // If we saw digits and the value is 0, it's the actual End of Stream.
                            if (respCtx.tempSize == 0)
                                return (int)pos;

                            // Otherwise, lock in the new chunk size
                            respCtx.bytesRemState = respCtx.tempSize;
                            respCtx.tempSize = 0;
                            respCtx.stateChunkSizeRead = false; // Reset for next chunk
                        }
                        else
                        {
                            // Parse Hex
                            int val = -1;
                            if (c >= '0' && c <= '9')
                                val = c - '0';
                            else if (c >= 'a' && c <= 'f')
                                val = c - 'a' + 10;
                            else if (c >= 'A' && c <= 'F')
                                val = c - 'A' + 10;

                            if (val >= 0)
                            {
                                respCtx.tempSize = (respCtx.tempSize * 16) + val;
                                respCtx.stateChunkSizeRead = true; // Mark that we found a valid digit
                            }
                        }
                        ms_start = millis(); // Reset timeout while parsing headers
                        continue;            // Don't write these bytes to buffer
                    }
                    respCtx.bytesRemState--;
                }
                else if (respCtx.bytesRemState > 0)
                    respCtx.bytesRemState--;
            }

            buffer[pos++] = (char)c;
            respCtx.totalRead++;

            if ((endToken != 0 && c == endToken) || pos >= respCtx.bufSize - 1 ||
                (respCtx.stage == response_stage_payload && ((source->available() == 0) || (!respCtx.isChunked && respCtx.bytesRemState == 0))))
            {
                buffer[pos] = '\0';
                if (sink)
                    sink->print(buffer);

                return (int)pos;
            }

            ms_start = millis();
        }

        return millis() - ms_start < timeout ? 0 : -2;
    }

    void readMetaData()
    {
        if (respCtx.stage == response_stage_finished || respCtx.stage == response_stage_payload)
            return;

        if (!respCtx.buf)
            respCtx.newBuf();

        int len = readResponse<Client, Client>(client, nullptr, respCtx);

        if (httpCode == 0 && len > 0)
        {
            int code = getStatusCode((const char *)respCtx.buf);
            if (code > 0)
            {
                sut.clear(val[resns::header]);
                sut.clear(val[resns::payload]);
                httpCode = code;
                respCtx.stage = response_stage_header;
                return;
            }
        }
        readHeader(len);
    }

    void readHeader(int len)
    {
        if (respCtx.stage == response_stage_header)
        {
            if (strcmp((const char *)respCtx.buf, "\r\n") == 0 || len <= 0)
            {
                respCtx.freeHdr();
                clearHeader(!respCtx.isAuth);
                if (httpCode == FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
                    respCtx.stage = response_stage_finished;
                else
                    respCtx.stage = response_stage_payload;
            }
            else
            {
                if (!respCtx.hdr)
                    respCtx.newHdr();

                flags.uploadRange = false;
                flags.http_response = true;

                if (parseHeaders((const char *)respCtx.buf, "ETag", respCtx.hdr, respCtx.hdrSize))
                    val[resns::etag] = respCtx.hdr;

                if (respCtx.location && parseHeaders((const char *)respCtx.buf, "Location", respCtx.hdr, respCtx.hdrSize))
                    *respCtx.location = respCtx.hdr;

                if (parseHeaders((const char *)respCtx.buf, "Content-Length", respCtx.hdr, respCtx.hdrSize))
                {
                    payloadLen = atoi(respCtx.hdr);
                    respCtx.bytesRemState = payloadLen;
                }

                if (parseHeaders((const char *)respCtx.buf, "Connection", respCtx.hdr, respCtx.hdrSize) && strstr(respCtx.hdr, "keep-alive"))
                    flags.keep_alive = true;

                if (parseHeaders((const char *)respCtx.buf, "Transfer-Encoding", respCtx.hdr, respCtx.hdrSize) && strstr(respCtx.hdr, "chunked"))
                {
                    respCtx.isChunked = true;
                    respCtx.bytesRemState = 0; // Initialize chunk logic to "expecting header"
                    flags.chunks = true;
                }

                if (parseHeaders((const char *)respCtx.buf, "Content-Type", respCtx.hdr, respCtx.hdrSize) && strstr(respCtx.hdr, "text/event-stream"))
                    flags.sse = true;

                if (parseHeaders((const char *)respCtx.buf, "Content-Encoding", respCtx.hdr, respCtx.hdrSize) && strstr(respCtx.hdr, "gzip"))
                    flags.gzip = true;

                if (respCtx.isUpload)
                {
                    if (httpCode == FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT && parseHeaders((const char *)respCtx.buf, "Range", respCtx.hdr, respCtx.hdrSize) && strstr(respCtx.hdr, "bytes="))
                        flags.uploadRange = true;
                }
            }
        }
    }

    void readPayload()
    {
        if (respCtx.stage == response_stage_finished)
            return;

        if (respCtx.stage == response_stage_payload)
        {
            if (!respCtx.buf)
                respCtx.newBuf();

            if (client->connected() || client->available())
            {
                int len = readResponse<Client, Client>(client, nullptr, respCtx);

                if (len < 0 || (!respCtx.isChunked && respCtx.bytesRemState == 0 && len == 0))
                {
                    // Timeout or Socket Closed
                    respCtx.stage = response_stage_finished;
                    respCtx.freeBuf();
                }
                else
                {
                    if (len > 0)
                    {
                        payloadRead += len;
                        respCtx.buf[len] = '\0';
                        reserveString();
                        val[resns::payload] += (const char *)respCtx.buf;
                    }

                    if (!respCtx.isChunked || (respCtx.isChunked && len == 0))
                    {
                        respCtx.stage = response_stage_finished;
                        respCtx.freeBuf();
                    }
                }
            }
        }
    }

    void reserveString()
    {
        // String memory reservation is needed to handle large data in external memory.
#if defined(ENABLE_PSRAM) && ((defined(ESP8266) && defined(MMU_EXTERNAL_HEAP)) || (defined(ESP32) && defined(BOARD_HAS_PSRAM)))
        String old = val[resns::payload];
        val[resns::payload].remove(0, val[resns::payload].length());
        val[resns::payload].reserve(payloadRead + 1);
        val[resns::payload] = old;
#endif
    }

    int getStatusCode(const char *status_line)
    {
        if (status_line == NULL)
            return -1;
        int i = 0;
        while (status_line[i] != '\0' && status_line[i] != ' ')
            i++;

        if (status_line[i] == '\0')
            return -1;
        i++;

        char code_str[4] = {0};
        int j = 0;
        while (status_line[i] != '\0' && j < 3 && isdigit((unsigned char)status_line[i]))
            code_str[j++] = status_line[i++];

        if (j != 3)
            return -1;

        return atoi(code_str);
    }

    int parseHeaders(const char *src, const char *key, char *out_buf, size_t out_len)
    {
        const char *p = src;
        size_t key_len = strlen(key);

        while (*p != '\0')
        {
            while (*p == '\r' || *p == '\n')
                p++;

            if (*p == '\0')
                break;

            if (strncasecmp(p, key, key_len) == 0 && p[key_len] == ':')
            {
                p += key_len + 1;

                while (*p == ' ' || *p == '\t')
                    p++;

                size_t i = 0;
                while (*p != '\r' && *p != '\n' && *p != '\0')
                {
                    if (i < out_len - 1)
                        out_buf[i++] = *p;
                    p++;
                }
                out_buf[i] = '\0';
                return 1;
            }
            while (*p != '\0' && *p != '\r' && *p != '\n')
                p++;
        }
        return 0;
    }
};

#endif