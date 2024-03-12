/**
 * Created February 9, 2024
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
#ifndef ASYNC_STORAGE_H
#define ASYNC_STORAGE_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
using namespace std;

using namespace firebase;

#if defined(ENABLE_STORAGE)

#include "./storage/DataOptions.h"

class Storage
{
private:
    AsyncClientClass *aClient = nullptr;
    String service_url;
    String path;
    String uid;
    uint32_t app_addr = 0;
    app_token_t *app_token = nullptr;

public:
    ~Storage(){};
    Storage(const String &url = "")
    {
        this->service_url = url;
    };

    Storage &operator=(Storage &rhs)
    {
        this->service_url = rhs.service_url;
        return *this;
    }

    /**
     * Set the Firebase Storage URL
     * @param url The Firebase Storage URL.
     */
    void url(const String &url)
    {
        this->service_url = url;
    }

    void setApp(uint32_t app_addr, app_token_t *app_token)
    {
        this->app_addr = app_addr;
        this->app_token = app_token;
    }

    app_token_t *appToken()
    {
        List vec;
        return vec.existed(aVec, app_addr) ? app_token : nullptr;
    }

    /**
     * Perform the async task repeatedly.
     * Should be places in main loop function.
     */
    void loop()
    {
        for (size_t i = 0; i < cVec.size(); i++)
        {
            AsyncClientClass *aClient = reinterpret_cast<AsyncClientClass *>(cVec[i]);
            if (aClient)
            {
                aClient->process(true);
                aClient->handleRemove();
            }
        }
    }

    /** Download object from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data file)
    {
        AsyncResult result;
        sendRequest(aClient, &result, NULL, "", parent, file, FirebaseStorage::firebase_storage_request_type_download, false);
        return result.lastError.code() == 0;
    }

    /** Download object from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param aResult The async result (AsyncResult).
     *
     */
    void download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data file, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, file, FirebaseStorage::firebase_storage_request_type_download, true);
    }

    /** Download object from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data file, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, file, FirebaseStorage::firebase_storage_request_type_download, true);
    }

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const FirebaseStorage::Parent &parent, file_config_data &file, FirebaseStorage::firebase_storage_request_type requestType, bool async)
    {
        FirebaseStorage::DataOptions options;
        options.requestType = requestType;
        options.parent = parent;

        async_request_handler_t::http_request_method method = async_request_handler_t::http_post;

        if (requestType == FirebaseStorage::firebase_storage_request_type_download ||
            requestType == FirebaseStorage::firebase_storage_request_type_download_ota ||
            requestType == FirebaseStorage::firebase_storage_request_type_list ||
            requestType == FirebaseStorage::firebase_storage_request_type_get_meta)
        {
            method = async_request_handler_t::http_get;
            if (requestType == FirebaseStorage::firebase_storage_request_type_download ||
                requestType == FirebaseStorage::firebase_storage_request_type_download_ota)
                options.extras += "?alt=media";
        }

        FirebaseStorage::async_request_data_t aReq(&aClient, path, method, slot_options_t(false, false, async, false, false, false), &options, &file, result, cb, uid);
        asyncRequest(aReq);
    }

    void asyncRequest(FirebaseStorage::async_request_data_t &request, int beta = 0)
    {
        app_token_t *app_token = appToken();

        if (!app_token)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = app_token;
        String extras;

        request.path = "/v0/b/";
        request.path += request.options->parent.getBucketId();
        request.path += "/o/";
        request.path += request.options->parent.getObject();

        addParams(request, extras);

        url(FPSTR("firebasestorage.googleapis.com"));

        async_data_item_t *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        request.aClient->newRequest(sData, service_url, request.path, extras, request.method, request.opt, request.uid);

        if (request.file)
        {
            sData->request.file_data.copy(*request.file);
            sData->request.base64 = false;
            request.aClient->setFileContentLength(sData);
        }
        else if (request.options->payload.length())
        {
            sData->request.val[req_hndlr_ns::payload] = request.options->payload;
            request.aClient->setContentLength(sData, request.options->payload.length());
        }

        setFileStatus(sData, request);

        if (request.cb)
            sData->cb = request.cb;

        if (request.aResult)
            sData->setRefResult(request.aResult);

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void setClientError(FirebaseStorage::async_request_data_t &request, int code)
    {
        AsyncResult *aResult = request.aResult;

        if (!aResult)
            aResult = new AsyncResult();

        aResult->error_available = true;
        aResult->lastError.setClientError(code);

        if (request.cb)
            request.cb(*aResult);

        if (!request.aResult)
        {
            delete aResult;
            aResult = nullptr;
        }
    }

    void addParams(FirebaseStorage::async_request_data_t &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }

    void setFileStatus(async_data_item_t *sData, FirebaseStorage::async_request_data_t &request)
    {
        if (sData->request.file_data.filename.length())
        {
            sData->download = request.method == async_request_handler_t::http_get;
            sData->upload = request.method == async_request_handler_t::http_post ||
                            request.method == async_request_handler_t::http_put ||
                            request.method == async_request_handler_t::http_patch;
        }
    }
};

#endif

#endif