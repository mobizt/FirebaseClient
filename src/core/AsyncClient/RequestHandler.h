#ifndef CORE_ASYNC_CLIENT_REQUEST_HANDLER_H
#define CORE_ASYNC_CLIENT_REQUEST_HANDLER_H

#include <Arduino.h>
#include "Client.h"
#include "./Config.h"
#include "./core/File/FileConfig.h"
#include "./core/Utils/Timer.h"
#include "./core/Utils/StringUtil.h"
#include "./core/AsyncClient/ConnectionHandler.h"
#include "./core/Utils/URL.h"
#include "./core/Utils/Base64.h"
#if defined(ENABLE_ASYNC_TCP_CLIENT)
#include "./core/AsyncClient/AsyncTCPConfig.h"
#endif

#define FIREBASE_TCP_WRITE_TIMEOUT_SEC 30 // Do not change

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

public:
    String val[reqns::max_type];
    String location;
    app_token_t *app_token = nullptr;
    uint16_t port = 443;
    uint8_t *data = nullptr;
    file_config_data file_data;
    bool base64 = false, ota = false, connected = false;
    uint32_t ul_dl_task_running_addr = 0, ota_storage_addr = 0, payloadLen = 0;
    uint32_t dataLen = 0, payloadIndex = 0;
    uint16_t dataIndex = 0;
    int8_t b64Pad = 0;
    int16_t ota_error = 0;
    reqns::http_request_method method = reqns::http_undefined;
    Timer send_timer;

    tcp_client_type client_type;
    Client *client = nullptr;
    void *atcp_config = nullptr;

    req_handler() {}

    void setClient(tcp_client_type client_type, Client *client, void *atcp_config)
    {
        this->client_type = client_type;
        this->client = client;
        this->atcp_config = atcp_config;
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

    void setContentType(const String &type)
    {
        addContentTypeHeader(type.c_str());
    }

    void setContentLength(size_t len)
    {
        if (method == reqns::http_post || method == reqns::http_put || method == reqns::http_patch)
        {
            addContentLengthHeader(len);
            addNewLine();
        }
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

    String getHost(bool fromReq, String *location = nullptr, String *ext = nullptr)
    {
#if defined(ENABLE_CLOUD_STORAGE)
        String url = fromReq ? val[reqns::url] : file_data.resumable.getLocation();
#else
        String url = fromReq ? val[reqns::url] : *location;
#endif
        URLUtil uut;
        return uut.getHost(url, ext);
    }

    size_t tcpWrite(const uint8_t *data, size_t size)
    {
        if (client_type == tcpc_sync)
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
            {
                val[reqns::header] += customHeader;
                val[reqns::header] += ":";
                val[reqns::header] += file_data.file_size + headerLen;
                val[reqns::header] += "\r\n";
            }
            else
                setContentLength(file_data.file_size);

            closeFile();
        }
    }

    void closeFile()
    {
#if defined(ENABLE_FS)
        if (file_data.file && file_data.file_status == file_config_data::file_status_opened)
        {
            file_data.file_size = 0;
            payloadIndex = 0;
            dataIndex = 0;
            file_data.file_status = file_config_data::file_status_closed;
            file_data.file.close();
        }
#endif
    }

    bool openFile(file_operating_mode mode)
    {
#if defined(ENABLE_FS)
        file_data.cb(file_data.file, file_data.filename.c_str(), mode);
        if (!file_data.file)
            return false;
#else
        return false;
#endif
        file_data.file_status = file_config_data::file_status_opened;
        return true;
    }
};

#endif