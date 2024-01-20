#ifndef ASYNC_REQUEST_HANDLER_H
#define ASYNC_REQUEST_HANDLER_H
#include <Arduino.h>
#include "./Config.h"
#include "./core/Storage.h"
#include "./core/URL.h"

#define FIREBASE_TCP_WRITE_TIMEOUT 30 *1000;

typedef void (*NetworkStatus)(bool &status);
typedef void (*NetworkReconnect)(void);

#if defined(ENABLE_DATABASE)

class Filter
{
    friend class DataOptions;
    friend class Database;
    friend class AsyncClient;

private:
    void copy(Filter &rhs)
    {
        this->uri = rhs.uri;
        this->complete = rhs.complete;
    }
    void toString(String &buf)
    {
        String v = "\"";
        v += buf;
        buf = v;
        buf += '"';
    }
    bool hasParams = true;
    bool complete = false;
    URLHelper uh;
    String uri;

public:
    Filter(){};
    ~Filter() { clear(); };

    Filter &orderBy(const String &val)
    {

        complete = true;
        String ob = "\"";
        ob += val;
        ob += "\"";
        uh.addParam(uri, "orderBy", ob, hasParams);
        return *this;
    }

    template <typename T = int>
    Filter &limitToFirst(T val)
    {
        uh.addParam(uri, "limitToFirst", String(val), hasParams);
        return *this;
    }

    template <typename T = int>
    Filter &limitToLast(T val)
    {
        uh.addParam(uri, "limitToLast", String(val), hasParams);
        return *this;
    }

    Filter &startAt(const String &val)
    {
        String sa = "\"";
        sa += val;
        sa += "\"";
        uh.addParam(uri, "startAt", sa, hasParams);
        return *this;
    }

    Filter &startAt(int val)
    {
        uh.addParam(uri, "startAt", String(val), hasParams);
        return *this;
    }

    Filter &endAt(const String &val)
    {
        String sa = "\"";
        sa += val;
        sa += "\"";
        uh.addParam(uri, "endAt", sa, hasParams);
        return *this;
    }

    Filter &endAt(int val)
    {
        uh.addParam(uri, "endAt", String(val), hasParams);
        return *this;
    }

    Filter &equalTo(const String &val)
    {
        String sa = "\"";
        sa += val;
        sa += "\"";
        uh.addParam(uri, "equalTo", sa, hasParams);
        return *this;
    }

    Filter &equalTo(int val)
    {
        uh.addParam(uri, "equalTo", String(val), hasParams);
        return *this;
    }

    Filter &clear()
    {
        uri.clear();
        complete = false;
        return *this;
    }
};

class DataOptions
{
    friend class Database;

public:
    uint32_t readTimeout = 0;
    String writeSizeLimit;
    bool shallow = false;
    bool silent = false;
    bool classicRequest = false;
    String customHeaders;
    String ETAG;
    Filter filter;

    void copy(DataOptions &rhs)
    {
        this->readTimeout = rhs.readTimeout;
        this->writeSizeLimit = rhs.writeSizeLimit;
        this->shallow = rhs.shallow;
        this->silent = rhs.silent;
        this->classicRequest = rhs.classicRequest;
        this->customHeaders = rhs.customHeaders;
        this->ETAG = rhs.ETAG;
        this->filter.copy(rhs.filter);
        this->ota = rhs.ota;
        this->base64 = rhs.base64;
    }

    void clear()
    {
        readTimeout = 0;
        writeSizeLimit.clear();
        shallow = false;
        silent = false;
        classicRequest = false;
        customHeaders.clear();
        ETAG.clear();
        filter.clear();
    }

private:
    bool base64 = false;
    bool ota = false;
};

#endif

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
    unsigned long last_request_ms = 0;
    unsigned long request_tmo = FIREBASE_TCP_WRITE_TIMEOUT;
};

#endif