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
#ifndef ASYNC_REQUEST_HANDLER_H
#define ASYNC_REQUEST_HANDLER_H
#include <Arduino.h>
#include "./Config.h"
#include "./core/Storage.h"
#include "./core/Timer.h"


#define FIREBASE_TCP_WRITE_TIMEOUT 30 * 1000

typedef void (*NetworkStatus)(bool &status);
typedef void (*NetworkReconnect)(void);

using namespace firebase;

struct async_request_handler_t
{
public:
    enum http_request_method
    {
        http_undefined,
        http_put,
        http_post,
        http_get,
        http_patch,
        http_delete,
    };

    struct url_info_t
    {
        String host;
        String uri;
        String auth;
    };

    String url, path, header, payload;
    uint16_t port = 443;
    size_t payloadLen = 0;
    uint8_t *data = nullptr;
    file_config_data file_data;
    bool base64 = false;

    size_t dataLen = 0;
    uint32_t payloadIndex = 0;
    uint16_t dataIndex = 0;
    bool ota = false;
    int b64Pad = 0;
    int ota_error = 0;
    http_request_method method = http_undefined;
    String etag;
    Timer send_timer;

    void addNewLine(String &header)
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

    void addGAPIsHostHeader(String &header, PGM_P sub)
    {
        header += FPSTR("Host: ");
        addGAPIsHost(header, sub);
        addNewLine(header);
    }

    void addHostHeader(String &header, PGM_P host)
    {
        header += FPSTR("Host: ");
        header += host;
        addNewLine(header);
    }

    void addContentTypeHeader(String &header, PGM_P v)
    {
        header += FPSTR("Content-Type: ");
        header += v;
        addNewLine(header);
    }

    void addContentLengthHeader(String &header, size_t len)
    {
        header += FPSTR("Content-Length: ");
        header += len;
        addNewLine(header);
    }

    void addUAHeader(String &header)
    {
        header += FPSTR("User-Agent: ESP");
        addNewLine(header);
    }

    void addConnectionHeader(String &header, bool keepAlive)
    {
        header += keepAlive ? FPSTR("Connection: keep-alive") : FPSTR("Connection: close");
        addNewLine(header);
    }

    /* Append the string with first request line (HTTP method) */
    bool addRequestHeaderFirst(String &header, async_request_handler_t::http_request_method method)
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
    void addRequestHeaderLast(String &header)
    {
        header += FPSTR(" HTTP/1.1\r\n");
    }

    /* Append the string with first part of Authorization header */
    void addAuthHeaderFirst(String &header, auth_token_type type)
    {
        header += "Authorization: ";
        if (type == auth_access_token || type == auth_sa_access_token)
            header += "Bearer ";
    }

    void parse(Memory &mem, const String &url, struct async_request_handler_t::url_info_t &info)
    {

        char *host = reinterpret_cast<char *>(mem.alloc(url.length()));
        char *uri = reinterpret_cast<char *>(mem.alloc(url.length()));
        char *auth = reinterpret_cast<char *>(mem.alloc(url.length()));

        int p1 = 0;
        int x = sscanf(url.c_str(), "https://%[^/]/%s", host, uri);
        x ? p1 = 8 : x = sscanf(url.c_str(), "http://%[^/]/%s", host, uri);
        x ? p1 = 7 : x = sscanf(url.c_str(), "%[^/]/%s", host, uri);

        int p2 = 0;
        if (x > 0)
        {
            p2 = String(host).indexOf("?", 0);
            if (p2 != -1)
                x = sscanf(url.c_str() + p1, "%[^?]?%s", host, uri);
        }

        if (strlen(uri) > 0)
        {
#if defined(ENABLE_DATABASE)
            p2 = String(uri).indexOf("auth=", 0);
            if (p2 != -1)
                x = sscanf(uri + p2 + 5, "%[^&]", auth);
#endif
        }

        info.uri = uri;
        info.host = host;
        info.auth = auth;
        mem.release(&host);
        mem.release(&uri);
        mem.release(&auth);
    }

    void idle()
    {
#if defined(ARDUINO_ESP8266_MAJOR) && defined(ARDUINO_ESP8266_MINOR) && defined(ARDUINO_ESP8266_REVISION) && ((ARDUINO_ESP8266_MAJOR == 3 && ARDUINO_ESP8266_MINOR >= 1) || ARDUINO_ESP8266_MAJOR > 3)
        esp_yield();
#else
        delay(0);
#endif
    }

    void feedTimer()
    {
        send_timer.stop();
        send_timer.setInterval(FIREBASE_TCP_WRITE_TIMEOUT);
        send_timer.start();
    }
};

#endif