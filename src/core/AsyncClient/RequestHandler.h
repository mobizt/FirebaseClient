/**
 * 2025-01-25
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
#ifndef ASYNC_REQUEST_HANDLER_H
#define ASYNC_REQUEST_HANDLER_H
#include <Arduino.h>
#include "./Config.h"
#include "./core/FileConfig.h"
#include "./core/Timer.h"
#include "Client.h"
#include "./core/AuthConfig.h"
#include "./core/StringUtil.h"

#if defined(ENABLE_ASYNC_TCP_CLIENT)
#include "./core/AsyncTCPConfig.h"
#endif

#define FIREBASE_TCP_WRITE_TIMEOUT_SEC 30 // Do not change

#define FIREBASE_RECONNECTION_TIMEOUT_MSEC 5000

#define FIREBASE_SESSION_TIMEOUT_SEC 150

#define FIREBASE_AUTH_PLACEHOLDER FPSTR("<auth_token>")

#if !defined(FIREBASE_ASYNC_QUEUE_LIMIT)
#if defined(ESP8266)
#define FIREBASE_ASYNC_QUEUE_LIMIT 10
#elif defined(ESP32) || defined(ARDUINO_PICO_MODULE)
#define FIREBASE_ASYNC_QUEUE_LIMIT 20
#else
#define FIREBASE_ASYNC_QUEUE_LIMIT 10
#endif
#endif

typedef void (*NetworkStatus)(bool &status);
typedef void (*NetworkReconnect)(void);

using namespace firebase_ns;

namespace reqns
{
    enum data_item_type_t
    {
        url,
        path,
        etag,
        header,
        payload,
        max_type
    };

    enum tcp_client_type
    {
        tcpc_none,
        tcpc_sync,
        tcpc_async
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
}

struct req_handler
{
private:
    StringUtil sut;

public:
    String val[reqns::max_type];
    String location;
    app_token_t *app_token = nullptr;
    uint16_t port = 443;
    uint8_t *data = nullptr;
    file_config_data file_data;
    bool base64 = false, ota = false;
    uint32_t ul_dl_task_running_addr = 0, ota_storage_addr = 0, payloadLen = 0;
    uint32_t dataLen = 0, payloadIndex = 0;
    uint16_t dataIndex = 0;
    int8_t b64Pad = 0;
    int16_t ota_error = 0;
    reqns::http_request_method method = reqns::http_undefined;
    Timer send_timer;

    req_handler()
    {
    }

    void clear()
    {

        for (size_t i = 0; i < reqns::max_type; i++)
            sut.clear(val[i]);
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
        method = reqns::http_undefined;
    }

    void addNewLine()
    {
        val[reqns::header] += "\r\n";
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
        val[reqns::header] += FPSTR("Host: ");
        addGAPIsHost(val[reqns::header], sub);
        addNewLine();
    }

    void addHostHeader(PGM_P host)
    {
        val[reqns::header] += FPSTR("Host: ");
        val[reqns::header] += host;
        addNewLine();
    }

    void addContentTypeHeader(PGM_P v)
    {
        val[reqns::header] += FPSTR("Content-Type: ");
        val[reqns::header] += v;
        addNewLine();
    }

    void addContentLengthHeader(size_t len)
    {
        val[reqns::header] += FPSTR("Content-Length: ");
        val[reqns::header] += len;
        addNewLine();
    }

    void addUAHeader()
    {
        val[reqns::header] += FPSTR("User-Agent: ESP");
        addNewLine();
    }

    void addConnectionHeader(bool keepAlive)
    {
        val[reqns::header] += keepAlive ? FPSTR("Connection: keep-alive") : FPSTR("Connection: close");
        addNewLine();
    }

    /* Append the string with first request line (HTTP method) */
    bool addRequestHeaderFirst(reqns::http_request_method method)
    {
        bool post = false;
        switch (method)
        {
        case reqns::http_get:
            val[reqns::header] += FPSTR("GET");
            break;
        case reqns::http_post:
            val[reqns::header] += FPSTR("POST");
            post = true;
            break;

        case reqns::http_patch:
            val[reqns::header] += FPSTR("PATCH");
            post = true;
            break;

        case reqns::http_delete:
            val[reqns::header] += FPSTR("DELETE");
            break;

        case reqns::http_put:
            val[reqns::header] += FPSTR("PUT");
            break;

        default:
            break;
        }

        if (method == reqns::http_get || method == reqns::http_post || method == reqns::http_patch || method == reqns::http_delete || method == reqns::http_put)
            val[reqns::header] += FPSTR(" ");

        return post;
    }

    /* Append the string with last request line (HTTP version) */
    void addRequestHeaderLast()
    {
        val[reqns::header] += FPSTR(" HTTP/1.1\r\n");
    }

    /* Append the string with first part of Authorization header */
    void addAuthHeaderFirst(auth_token_type type)
    {
        val[reqns::header] += FPSTR("Authorization: ");
        if (type == auth_access_token || type == auth_sa_access_token)
            val[reqns::header] += FPSTR("Bearer ");
        else if (type == auth_user_id_token || type == auth_id_token || type == auth_custom_token || type == auth_sa_custom_token)
            val[reqns::header] += FPSTR("Firebase ");
        else
            val[reqns::header] += FPSTR("key=");
    }

    void feedTimer(int interval = -1)
    {
        send_timer.feed(interval == -1 ? FIREBASE_TCP_WRITE_TIMEOUT_SEC : interval);
    }

    size_t tcpWrite(reqns::tcp_client_type client_type, Client *client, void *atcp_config, const uint8_t *data, size_t size)
    {
        if (client_type == reqns::tcpc_sync)
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