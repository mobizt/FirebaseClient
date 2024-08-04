/**
 * Created August 4, 2024
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

using namespace firebase;

#if defined(ENABLE_STORAGE)

#include "./storage/DataOptions.h"

class Storage
{

    friend class AppBase;

public:
    std::vector<uint32_t> cVec; // AsyncClient vector

    ~Storage() {}
    explicit Storage(const String &url = "")
    {
        this->service_url = url;
    };

    Storage &operator=(const Storage &rhs)
    {
        this->service_url = rhs.service_url;
        this->app_token = rhs.app_token;
        return *this;
    }

    /**
     * Unbind or remove the FirebaseApp
     */
    void resetApp()
    {
        this->app_addr = 0;
        this->app_token = nullptr;
        this->avec_addr = 0; // AsyncClient vector (list) address
        this->ul_dl_task_running_addr = 0;
    }

    /**
     * Perform the async task repeatedly.
     * Should be placed in main loop function.
     */
    void loop()
    {
        for (size_t i = 0; i < cVec.size(); i++)
        {
            AsyncClientClass *client = reinterpret_cast<AsyncClientClass *>(cVec[i]);
            if (client)
            {
                client->process(true);
                client->handleRemove();
            }
        }
    }

    /** Download object from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, "", FirebaseStorage::firebase_storage_request_type_download, false);
        return aClient.getResult()->lastError.code() == 0;
    }

    /** Download object from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket IdThe FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param aResult The async result (AsyncResult).
     *
     */
    void download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, &file, "", FirebaseStorage::firebase_storage_request_type_download, true);
    }

    /** Download object from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, &file, "", FirebaseStorage::firebase_storage_request_type_download, true);
    }

    /** Upload file to the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to upload.
     * The object is the object to be stored in the Storage bucket.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param mime The file MIME type
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool upload(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, const String &mime)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, mime, FirebaseStorage::firebase_storage_request_type_upload, false);
        return aClient.getResult()->lastError.code() == 0;
    }

    /** Upload file to the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to upload.
     * The object is the object to be stored in the Storage bucket.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param mime The file MIME type
     * @param aResult The async result (AsyncResult).
     *
     *
     */
    void upload(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, const String &mime, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, &file, mime, FirebaseStorage::firebase_storage_request_type_upload, true);
    }

    /** Upload file to the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
     * The bucketid is the Storage bucket Id of object to upload.
     * The object is the object to be stored in the Storage bucket.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param mime The file MIME type
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void upload(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, const String &mime, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, &file, mime, FirebaseStorage::firebase_storage_request_type_upload, true);
    }

    /** Perform OTA update using a firmware (object) from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool ota(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_download_ota, false);
        return aClient.getResult()->lastError.code() == 0;
    }

    /** Perform OTA update using a firmware (object) from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket IdThe FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     * @param aResult The async result (AsyncResult).
     *
     */
    void ota(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_download_ota, true);
    }

    /** Perform OTA update using a firmware (object) from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket IdThe FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor. 
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void ota(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_download_ota, true);
    }

    /** Get the metadata of object in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to get metadata.
     * The object is the object in Storage bucket to get metadata.
     *
     * @return String The response payload.
     *
     */
    String getMetadata(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_get_meta, false);
        return aClient.getResult()->c_str();
    }

    /** Get the metadata of object in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to get metadata.
     * The object is the object in Storage bucket to get metadata.
     * @param aResult The async result (AsyncResult).
     *
     */
    void getMetadata(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_get_meta, true);
    }

    /** Get the metadata of object in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to get metadata.
     * The object is the object in Storage bucket to get metadata.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void getMetadata(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_get_meta, true);
    }

    /** List all objects in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id in its constructor.
     * The bucketid is the Storage bucket Id to list all objects.
     *
     * @return String The response payload.
     *
     */
    String list(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_list, false);
        return aClient.getResult()->c_str();
    }

    /** List all objects in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id in its constructor.
     * The bucketid is the Storage bucket Id to list all objects.
     * @param aResult The async result (AsyncResult).
     *
     */
    void list(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_list, true);
    }

    /** List all objects in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id in its constructor.
     * The bucketid is the Storage bucket Id to list all objects.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void list(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_list, true);
    }

    /** Delete the object in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to delete.
     * The object is the object in Storage bucket to delete.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool deleteObject(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_delete, false);
        return aClient.getResult()->lastError.code() == 0;
    }

    /** Delete the object in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to delete.
     * The object is the object in Storage bucket to delete.
     * @param aResult The async result (AsyncResult).
     *
     */
    void deleteObject(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_delete, true);
    }

    /** Delete the object in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to delete.
     * The object is the object in Storage bucket to delete.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void deleteObject(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", FirebaseStorage::firebase_storage_request_type_delete, true);
    }

#if defined(FIREBASE_OTA_STORAGE)
    /**
     * Set Arduino OTA Storage.
     *  @param storage The Arduino  OTAStorage class object.
     */
    void setOTAStorage(OTAStorage &storage) { ota_storage_addr = reinterpret_cast<uint32_t>(&storage); }
#endif

private:
    String service_url;
    String path;
    String uid;
    // FirebaseApp address and FirebaseApp vector address
    uint32_t app_addr = 0, avec_addr = 0;
    uint32_t ul_dl_task_running_addr = 0;
    uint32_t ota_storage_addr = 0;
    app_token_t *app_token = nullptr;

    void url(const String &url)
    {
        this->service_url = url;
    }

    void setApp(uint32_t app_addr, app_token_t *app_token, uint32_t avec_addr, uint32_t ul_dl_task_running_addr)
    {
        this->app_addr = app_addr;
        this->app_token = app_token;
        this->avec_addr = avec_addr; // AsyncClient vector (list) address
        this->ul_dl_task_running_addr = ul_dl_task_running_addr;
    }

    app_token_t *appToken()
    {
        if (avec_addr > 0)
        {
            const std::vector<uint32_t> *aVec = reinterpret_cast<std::vector<uint32_t> *>(avec_addr);
            List vec;
            return vec.existed(*aVec, app_addr) ? app_token : nullptr;
        }
        return nullptr;
    }

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const FirebaseStorage::Parent &parent, file_config_data *file, const String &mime, FirebaseStorage::firebase_storage_request_type requestType, bool async)
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
            {
                options.extras += "?alt=media";
                if (options.parent.getAccessToken().length())
                {
                    options.extras += "&token=";
                    options.extras += options.parent.getAccessToken();
                }
            }
        }
        else if (requestType == FirebaseStorage::firebase_storage_request_type_upload || requestType == FirebaseStorage::firebase_storage_request_type_delete)
        {
            URLUtil uut;
            options.extras += "?name=";
            options.extras += uut.encode(parent.getObject());

            if (requestType == FirebaseStorage::firebase_storage_request_type_delete)
                method = async_request_handler_t::http_delete;
        }

        FirebaseStorage::async_request_data_t aReq(&aClient, path, method, slot_options_t(false, false, async, false, requestType == FirebaseStorage::firebase_storage_request_type_download_ota, false), &options, file, result, cb, uid);

        if (mime.length() && requestType == FirebaseStorage::firebase_storage_request_type_upload)
            aReq.mime = mime;

        asyncRequest(aReq);
    }

    void asyncRequest(FirebaseStorage::async_request_data_t &request, int beta = 0)
    {
        app_token_t *atoken = appToken();

        if (!atoken)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        String extras;

        request.path = "/v0/b/";
        request.path += request.options->parent.getBucketId();
        request.path += "/o";

        if (request.method == async_request_handler_t::http_get)
        {
            URLUtil uut;
            request.path += "/";
            request.path += uut.encode(request.options->parent.getObject());
        }

        addParams(request, extras);

        url(FPSTR("firebasestorage.googleapis.com"));

        async_data_item_t *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        request.aClient->newRequest(sData, service_url, request.path, extras, request.method, request.opt, request.uid);

        if (request.file)
            sData->request.file_data.copy(*request.file);

        setFileStatus(sData, request);

        if (request.opt.ota)
        {
            sData->request.ota = true;
            sData->request.base64 = false;
            sData->aResult.download_data.ota = true;
            sData->request.ul_dl_task_running_addr = ul_dl_task_running_addr;
            sData->request.ota_storage_addr = ota_storage_addr;
        }

        if (request.file && sData->upload)
        {
            sData->request.base64 = false;

            if (request.mime.length())
                request.aClient->setContentType(sData, request.mime);

            request.aClient->setFileContentLength(sData, 0);

            if (sData->request.file_data.filename.length() > 0 && sData->request.file_data.file_size == 0)
                return setClientError(request, FIREBASE_ERROR_FILE_READ);

            URLUtil uut;
            sData->aResult.upload_data.downloadUrl = uut.downloadURL(request.options->parent.getBucketId(), request.options->parent.getObject());
        }
        else if (request.options->payload.length())
        {
            sData->request.val[req_hndlr_ns::payload] = request.options->payload;
            request.aClient->setContentLength(sData, request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        request.aClient->addRemoveClientVec(reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(request.aClient->rVec)));

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void setClientError(FirebaseStorage::async_request_data_t &request, int code)
    {
        AsyncResult *aResult = request.aResult;

        if (!aResult)
            aResult = new AsyncResult();

        aResult->lastError.setClientError(code);

        if (request.cb)
            request.cb(*aResult);

        if (!request.aResult)
        {
            delete aResult;
            aResult = nullptr;
        }
    }

    void addParams(const FirebaseStorage::async_request_data_t &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }

    void setFileStatus(async_data_item_t *sData, const FirebaseStorage::async_request_data_t &request)
    {
        if ((request.file && (request.file->filename.length() || request.file->data_size)) || request.opt.ota)
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