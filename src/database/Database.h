/**
 * Created November 10, 2023
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 K. Suwatchai (Mobizt)
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

#ifndef ASYNC_DATABASE_H
#define ASYNC_DATABASE_H
#include <Arduino.h>
#include "AsyncClient/AsyncClient.h"
using namespace std;

class Database
{

public:
    Database(const String &url = "")
    {
        this->dbUrl = url;
    };

    Database &operator=(Database &rhs)
    {
        this->dbUrl = rhs.dbUrl;
        return *this;
    }

    ~Database()
    {
    }

    /**
     * Get value at the node path.
     * @param path The node path to get value.
     * @param aClient The async client.
     * @return value that casts from response payload.
     */

    template <typename T1 = int>
    auto get(AsyncClient &aClient, const String &path) -> typename enable_if<!is_same<T1, void>::value && !is_same<T1, AsyncResult>::value, T1>::type
    {
        AsyncResult result;
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), nullptr, nullptr, &result, NULL);
        asyncRequest(aReq);
        return result.database.to<T1>();
    }

    template <typename T1 = int>
    auto get(AsyncClient &aClient, const String &path, DataOptions &options) -> typename enable_if<!is_same<T1, void>::value && !is_same<T1, AsyncResult>::value, T1>::type
    {
        AsyncResult result;
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), &options, nullptr, &result, NULL);
        asyncRequest(aReq);
        return result.database.to<T1>();
    }

    void get(AsyncClient &aClient, const String &path, AsyncResult &aResult, bool sse = false)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, sse, true, false, false), nullptr, nullptr, &aResult, NULL);
        asyncRequest(aReq);
    }

    void get(AsyncClient &aClient, const String &path, AsyncResultCallback cb, bool sse = false)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, sse, true, false, false), nullptr, nullptr, nullptr, cb);
        asyncRequest(aReq);
    }

    void get(AsyncClient &aClient, const String &path, DataOptions &options, AsyncResult &aResult)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, false, true, false, false), &options, nullptr, &aResult, NULL);
        asyncRequest(aReq);
    }

    void get(AsyncClient &aClient, const String &path, DataOptions &options, AsyncResultCallback cb)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, false, true, false, false), &options, nullptr, nullptr, cb);
        asyncRequest(aReq);
    }

    void get(AsyncClient &aClient, const String &path, file_config_data file, AsyncResultCallback cb)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), nullptr, &file, nullptr, cb);
        asyncRequest(aReq);
    }

    void get(AsyncClient &aClient, const String &path, file_config_data file, AsyncResult &aResult)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), nullptr, &file, &aResult, NULL);
        asyncRequest(aReq);
    }

    bool existed(AsyncClient &aClient, const String &path)
    {
        AsyncResult result;
        DataOptions options;
        options.silent = true;
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), &options, nullptr, &result, NULL);
        asyncRequest(aReq);
        return !result.database.null_etag;
    }

    void ota(AsyncClient &aClient, const String &path, AsyncResultCallback cb)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, false, false, false, true), nullptr, nullptr, nullptr, cb);
        asyncRequest(aReq);
    }

    void ota(AsyncClient &aClient, const String &path, AsyncResult &aResult)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, false, false, false, true), nullptr, nullptr, &aResult, NULL);
        asyncRequest(aReq);
    }

    template <typename T = const char *>
    bool set(AsyncClient &aClient, const String &path, T value)
    {
        AsyncResult result;
        return storeAsync(aClient, path, value, async_request_handler_t::http_put, false, &result, NULL);
    }

    template <typename T = const char *>
    void set(AsyncClient &aClient, const String &path, T value, AsyncResultCallback cb)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_put, true, nullptr, cb);
    }

    template <typename T = const char *>
    void set(AsyncClient &aClient, const String &path, T value, AsyncResult &aResult)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_put, true, &aResult, NULL);
    }

    void set(AsyncClient &aClient, const String &path, file_config_data file, AsyncResultCallback cb)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_put, AsyncClient::slot_options_t(false, false, true, false, true), nullptr, &file, nullptr, cb);
        asyncRequest(aReq);
    }

    void set(AsyncClient &aClient, const String &path, file_config_data file, AsyncResult &aResult)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_put, AsyncClient::slot_options_t(false, false, true, false, true), nullptr, &file, &aResult, nullptr);
        asyncRequest(aReq);
    }

    template <typename T = const char *>
    String push(AsyncClient &aClient, const String &path, T value)
    {
        ValueConverter vcon;
        AsyncResult result;
        String payload;
        vcon.getVal<T>(payload, value);
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClient::slot_options_t(), nullptr, nullptr, &result, NULL);
        asyncRequest(aReq, payload.c_str());
        return result.database.name();
    }

    void push(AsyncClient &aClient, const String &path, file_config_data file, AsyncResultCallback cb)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClient::slot_options_t(false, false, true, false, true), nullptr, &file, nullptr, cb);
        asyncRequest(aReq);
    }

    void push(AsyncClient &aClient, const String &path, file_config_data file, AsyncResult &aResult)
    {
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClient::slot_options_t(false, false, true, false, true), nullptr, &file, &aResult, nullptr);
        asyncRequest(aReq);
    }

    template <typename T = object_t>
    bool update(AsyncClient &aClient, const String &path, const T &value)
    {
        AsyncResult result;
        return storeAsync(aClient, path, value, async_request_handler_t::http_patch, false, &result, NULL);
    }

    template <typename T = object_t>
    void update(AsyncClient &aClient, const String &path, const T &value, AsyncResultCallback cb)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_patch, true, nullptr, cb);
    }

    template <typename T = object_t>
    void update(AsyncClient &aClient, const String &path, const T &value, AsyncResult &aResult)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_patch, true, &aResult, NULL);
    }

    bool remove(AsyncClient &aClient, const String &path)
    {
        AsyncResult result;
        AsyncClient::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_delete, AsyncClient::slot_options_t(), nullptr, nullptr, &result, nullptr);
        asyncRequest(aReq);
        return result.database.null_etag && result.database.data().indexOf("null") > -1;
    }

    void url(const String &url)
    {
        this->dbUrl = url;
    }

    void loop()
    {
        for (size_t clientSlot = 0; clientSlot < clientList.size(); clientSlot++)
        {
            AsyncClient *aClient = reinterpret_cast<AsyncClient *>(clientList[clientSlot]);
            if (aClient)
            {
                for (size_t slot = 0; slot < aClient->slotCount(); slot++)
                    aClient->process(clientList, true);
            }
        }
    }

private:
    String dbUrl;
    vector<uint32_t> clientList;

    template <typename T = object_t>
    bool storeAsync(AsyncClient &aClient, const String &path, const T &value, async_request_handler_t::http_request_method mode, bool async, AsyncResult *aResult, AsyncResultCallback cb)
    {
        ValueConverter vcon;
        String payload;
        vcon.getVal<object_t>(payload, value);
        DataOptions options;
        if (!async)
            options.silent = true;
        AsyncClient::async_request_data_t aReq(&aClient, path, mode, AsyncClient::slot_options_t(false, false, async, false, false), &options, nullptr, aResult, cb);
        asyncRequest(aReq, payload.c_str());
        if (!async)
            return aResult->lastError.code() == 0;
        return true;
    }

    void asyncRequest(AsyncClient::async_request_data_t &request, const char *payload = "")
    {
        String extras = ".json?auth=" + request.aClient->getToken();
        addParams(true, extras, request.method, request.options, request.file);
        AsyncClient::async_data_item_t *sData = request.aClient->newSlot(clientList, dbUrl, request.path, extras, request.method, request.opt);
        if (request.file)
        {
            sData->request.file_data.copy(*request.file);
            sData->request.base64 = true;
            request.aClient->setFileContentLength(sData);
        }
        else if (strlen(payload))
        {
            sData->request.payload = payload;
            request.aClient->setContentLength(sData, strlen(payload));
        }

        if (request.opt.ota)
        {
            sData->request.ota = true;
            sData->request.base64 = true;
        }

        if (request.cb)
            sData->cb = request.cb;
        if (request.aResult)
            sData->refResult = request.aResult;

        request.aClient->process(clientList, sData->async);
    }

    void addParams(bool hasQueryParams, String &extras, async_request_handler_t::http_request_method method, DataOptions *options, bool isFile)
    {
        URLHelper uh;

        if (options)
        {
            if (options->readTimeout > 0)
                uh.addParam(extras, "timeout", String(options->readTimeout) + "ms", hasQueryParams);

            uh.addParam(extras, "writeSizeLimit", options->writeSizeLimit, hasQueryParams);

            if (options->shallow)
                uh.addParam(extras, "shallow", "true", hasQueryParams, true);
        }

        if ((options && options->silent) || ((method == async_request_handler_t::http_put || method == async_request_handler_t::http_post || method == async_request_handler_t::http_patch) && isFile))
            uh.addParam(extras, "print", "silent", hasQueryParams, true);

        if (options && options->filter.complete)
            extras += options->filter.uri;
    }
};

#endif