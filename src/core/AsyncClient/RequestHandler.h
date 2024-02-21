/**
 * Created February 21, 2024
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
#ifndef ASYNC_REQUEST_HANDLER_H
#define ASYNC_REQUEST_HANDLER_H
#include <Arduino.h>
#include "./Config.h"
#include "./core/FileConfig.h"
#include "./core/Timer.h"
#include "Client.h"
#include "./core/AuthConfig.h"

#if defined(ENABLE_ASYNC_TCP_CLIENT)
#include "./core/AsyncTCPConfig.h"
#endif

#define FIREBASE_TCP_WRITE_TIMEOUT_SEC 30

#define FIREBASE_AUTH_PLACEHOLDER (const char *)FPSTR("<auth_token>")

#if defined(ESP8266)
#define FIREBASE_ASYNC_QUEUE_LIMIT 3
#elif defined(ESP32) || defined(ARDUINO_PICO_MODULE)
#define FIREBASE_ASYNC_QUEUE_LIMIT 5
#else
#define FIREBASE_ASYNC_QUEUE_LIMIT 2
#endif

typedef void (*NetworkStatus)(bool &status);
typedef void (*NetworkReconnect)(void);

using namespace firebase;

struct async_request_handler_t
{
public:
    enum tcp_client_type
    {
        tcp_client_type_sync,
        tcp_client_type_async
    };

    enum http_request_method
    {
        http_undefined,
        http_put,
        http_post,
        http_get,
        http_patch,
        http_delete,
    };

    String url, path, etag;
    String header, payload;
    app_token_t *app_token = nullptr;
    uint16_t port = 443;
    uint8_t *data = nullptr;
    file_config_data file_data;
    bool base64 = false;
    bool ota = false;
    uint32_t payloadLen = 0;
    uint32_t dataLen = 0;
    uint32_t payloadIndex = 0;
    uint16_t dataIndex = 0;
    int8_t b64Pad = 0;
    int16_t ota_error = 0;
    http_request_method method = http_undefined;
    Timer send_timer;

    async_request_handler_t()
    {
    }

    void clear()
    {
        url.remove(0, url.length());
        path.remove(0, etag.length());
        etag.remove(0, path.length());
        header.remove(0, header.length());
        payload.remove(0, payload.length());
        port = 443;
        if (data)
            delete data;
        data = nullptr;
        file_data.clear();
        base64 = false;
        ota = false;
        payloadLen = 0;
        dataLen = 0;
        payloadIndex = 0;
        dataIndex = 0;
        b64Pad = 0;
        ota_error = 0;
        method = http_undefined;
    }

    void addNewLine()
    {
        header += "\r\n";
    }

    void addGAPIsHost(String &str, PGM_P sub)
    {
        str += sub;
        if (str[str.length() - 1] != '.')
            str += ".";
        str += FPSTR("googleapis.com");
    }

    void addGAPIsHostHeader(PGM_P sub)
    {
        header += FPSTR("Host: ");
        addGAPIsHost(header, sub);
        addNewLine();
    }

    void addHostHeader(PGM_P host)
    {
        header += FPSTR("Host: ");
        header += host;
        addNewLine();
    }

    void addContentTypeHeader(PGM_P v)
    {
        header += FPSTR("Content-Type: ");
        header += v;
        addNewLine();
    }

    void addContentLengthHeader(size_t len)
    {
        header += FPSTR("Content-Length: ");
        header += len;
        addNewLine();
    }

    void addUAHeader()
    {
        header += FPSTR("User-Agent: ESP");
        addNewLine();
    }

    void addConnectionHeader(bool keepAlive)
    {
        header += keepAlive ? FPSTR("Connection: keep-alive") : FPSTR("Connection: close");
        addNewLine();
    }

    /* Append the string with first request line (HTTP method) */
    bool addRequestHeaderFirst(async_request_handler_t::http_request_method method)
    {
        bool post = false;
        switch (method)
        {
        case async_request_handler_t::http_get:
            header += FPSTR("GET");
            break;
        case async_request_handler_t::http_post:
            header += FPSTR("POST");
            post = true;
            break;

        case async_request_handler_t::http_patch:
            header += FPSTR("PATCH");
            post = true;
            break;

        case async_request_handler_t::http_delete:
            header += FPSTR("DELETE");
            break;

        case async_request_handler_t::http_put:
            header += FPSTR("PUT");
            break;

        default:
            break;
        }

        if (method == async_request_handler_t::http_get || method == async_request_handler_t::http_post || method == async_request_handler_t::http_patch || method == async_request_handler_t::http_delete || method == async_request_handler_t::http_put)
            header += FPSTR(" ");

        return post;
    }

    /* Append the string with last request line (HTTP version) */
    void addRequestHeaderLast()
    {
        header += FPSTR(" HTTP/1.1\r\n");
    }

    /* Append the string with first part of Authorization header */
    void addAuthHeaderFirst(auth_token_type type)
    {
        header += FPSTR("Authorization: ");
        if (type == auth_access_token || type == auth_sa_access_token)
            header += FPSTR("Bearer ");
        else if (type == auth_user_id_token || type == auth_id_token || type == auth_custom_token)
            header += FPSTR("Firebase ");
        else
            header += FPSTR("key=");
    }

    void feedTimer(int interval = -1)
    {
        send_timer.feed(interval == -1 ? FIREBASE_TCP_WRITE_TIMEOUT_SEC : interval);
    }

    size_t tcpWrite(async_request_handler_t::tcp_client_type client_type, Client *client, void *atcp_config, uint8_t *data, size_t size)
    {
        if (client_type == tcp_client_type_sync)
            return client ? client->write(data, size) : 0;
        else
        {

#if defined(ENABLE_ASYNC_TCP_CLIENT)

            AsyncTCPConfig *async_tcp_config = reinterpret_cast<AsyncTCPConfig *>(atcp_config);
            if (!async_tcp_config && !async_tcp_config->tcpSend)
                return 0;

            uint32_t sent = 0;
            async_tcp_config->tcpSend(data, size, sent);
            return sent;
#endif
        }
        return 0;
    }
};

#endif