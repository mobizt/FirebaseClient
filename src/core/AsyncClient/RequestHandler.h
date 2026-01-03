/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_CLIENT_REQUEST_HANDLER_H
#define CORE_ASYNC_CLIENT_REQUEST_HANDLER_H

#include <Arduino.h>
#include <Client.h>
#include "./core/File/FileConfig.h"
#include "./core/Utils/Timer.h"
#include "./core/Utils/StringUtil.h"
#include "./core/Utils/URL.h"
#include "./core/Utils/Base64.h"
#include "./core/AsyncClient/ConnectionHandler.h"
#include "./core/Auth/Token/AppToken.h"

#if defined(ENABLE_FS)
#if __has_include(<SPIFFS.h>)
#include <SPIFFS.h>
#endif
#endif

#if defined(ENABLE_GZIP) && defined(FIREBASE_GZIP_LIB)
#include <stdlib.h>
#include <stdio.h>
extern "C"
{
    #include FIREBASE_GZIP_LIB
}
#define EXTRAS_HEADERS "Accept-Encoding: gzip\r\n"
#else
#define EXTRAS_HEADERS ""
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
    Base64Util b64ut;
    URLUtil uut;

public:
    String val[reqns::max_type];
    String location;
    app_token_t *app_token = nullptr;
    uint16_t port = 443;
    uint8_t *data = nullptr;
    file_config_data file_data;
    bool base64 = false, ota = false, connected = false;
    int command = 0;
    uint32_t ul_dl_task_running_addr = 0, ota_storage_addr = 0, payloadLen = 0;
    uint32_t dataLen = 0, payloadIndex = 0;
    uint16_t dataIndex = 0;
    int8_t b64Pad = 0;
    int16_t ota_error = 0;
    reqns::http_request_method method = reqns::http_undefined;
    Timer send_timer;

    tcp_client_type client_type = tcpc_sync;
    Client *client = nullptr;

    req_handler() {}

    void setClient(tcp_client_type client_type, Client *client)
    {
        this->client_type = client_type;
        this->client = client;
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

    void addNewLine() { val[reqns::header] += "\r\n"; }
    void addHostHeader(const String &host) { sut.printTo(val[reqns::header], host.length() + String(EXTRAS_HEADERS).length(), "Host: %s\r\n%s", host.c_str(), EXTRAS_HEADERS); }
    void addConnectionHeader(bool keepAlive) { sut.printTo(val[reqns::header], 50, "Connection: %s\r\n", keepAlive ? "keep-alive" : "close"); }
    void addContentType(const String &type) { sut.printTo(val[reqns::header], type.length(), "Content-Type: %s\r\n", type.c_str()); }
    void setContentLengthFinal(size_t len) { sut.printTo(val[reqns::header], 30, "Content-Length: %d\r\n\r\n", (int)len); }
    void addRequestHeader(reqns::http_request_method method, const String &path, const String &extras)
    {
        switch (method)
        {
        case reqns::http_get:
            val[reqns::header] += "GET ";
            break;
        case reqns::http_post:
            val[reqns::header] += "POST ";
            break;
        case reqns::http_patch:
            val[reqns::header] += "PATCH ";
            break;
        case reqns::http_delete:
            val[reqns::header] += "DELETE ";
            break;
        case reqns::http_put:
            val[reqns::header] += "PUT ";
            break;
        default:
            break;
        }
        sut.printTo(val[reqns::header], 300, "%s%s%s HTTP/1.1\r\n", path.length() == 0 || (path.length() && path[0] != '/') ? "/" : "", path.c_str(), extras.c_str());
    }

    /* Append the string with first part of Authorization header */
    void addAuthHeader(auth_token_type type)
    {
        val[reqns::header] += "Authorization: ";
        if (type == auth_access_token || type == auth_sa_access_token)
            val[reqns::header] += "Bearer ";
        else if (type == auth_user_id_token || type == auth_id_token || type == auth_custom_token || type == auth_sa_custom_token)
            val[reqns::header] += "Firebase ";
        else
            val[reqns::header] += "key=";
        val[reqns::header] += FIREBASE_AUTH_PLACEHOLDER;
        val[reqns::header] += "\r\n";
    }

    void feedTimer(int interval = -1) { send_timer.feed(interval == -1 ? FIREBASE_TCP_WRITE_TIMEOUT_SEC : interval); }

    String getHost(bool fromReq, String *location = nullptr, String *ext = nullptr)
    {
#if defined(ENABLE_CLOUD_STORAGE)
        String url = fromReq ? val[reqns::url] : file_data.resumable.getLocation();
#else
        String url = fromReq ? val[reqns::url] : (location ? *location : "");
#endif
        return uut.getHost(url, ext);
    }

    size_t tcpWrite(const uint8_t *data, size_t size)
    {
        if (client_type == tcpc_sync)
            return client ? client->write(data, size) : 0;
        return 0;
    }

    // This will set the Content-Lenght header with actual file size and header len.
    // Note: If no custom header assigned, the new line will append to the header.
    void setFileContentLength(int headerLen = 0, const String &customHeader = "")
    {
        size_t sz = 0;
#if defined(ENABLE_FS)
        if (file_data.cb && file_data.filename.length())
        {
            file_data.cb(file_data.file, file_data.filename.c_str(), file_mode_open_read);
            sz = file_data.file.size();
        }
#endif
        if (file_data.data_size && file_data.data)
            sz = file_data.data_size;

        if (sz > 0)
        {
            file_data.file_size = base64 ? 2 + b64ut.getBase64Len(sz) : sz;
            if (customHeader.length())
                sut.printTo(val[reqns::header], customHeader.length() + 30, "%s:%d\r\n", customHeader.c_str(), file_data.file_size + headerLen);
            else
                sut.printTo(val[reqns::header], 30, "Content-Length: %d\r\n\r\n", (int)file_data.file_size);
#if defined(ENABLE_FS)
            closeFile();
#endif
        }
    }

#if defined(ENABLE_FS)
    void closeFile()
    {
        if (file_data.file && file_data.file_status == file_config_data::file_status_opened)
        {
            file_data.file_size = 0;
            payloadIndex = 0;
            dataIndex = 0;
            file_data.file_status = file_config_data::file_status_closed;
            file_data.file.close();
        }
    }

    bool openFile(file_operating_mode mode)
    {

        file_data.cb(file_data.file, file_data.filename.c_str(), mode);
        if (!file_data.file)
            return false;

        file_data.file_status = file_config_data::file_status_opened;
        return true;
    }
#endif
};

#endif