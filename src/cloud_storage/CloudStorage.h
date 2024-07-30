/**
 * Created July 30, 2024
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
#ifndef ASYNC_CLOUD_STORAGE_H
#define ASYNC_CLOUD_STORAGE_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"

using namespace firebase;

#if defined(ENABLE_CLOUD_STORAGE)

#include "./cloud_storage/DataOptions.h"

class CloudStorage
{

    friend class AppBase;

public:
    std::vector<uint32_t> cVec; // AsyncClient vector

    ~CloudStorage() {}
    explicit CloudStorage(const String &url = "")
    {
        this->service_url = url;
    };

    CloudStorage &operator=(const CloudStorage &rhs)
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

    /** Download object from the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool download(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::GetOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_download, false);
        return aClient.getResult()->lastError.code() == 0;
    }

    /** Download object from the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     * @param aResult The async result (AsyncResult).
     *
     */
    void download(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::GetOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, &file, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_download, true);
    }

    /** Download object from the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void download(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::GetOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, &file, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_download, true);
    }

    /** Upload file to the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to upload.
     * The object is the object to be stored in the Storage bucket.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param options Optional. The GoogleCloudStorage::uploadOptions that holds the information for insert options, properties and types of upload.
     * For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
     * For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool upload(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::uploadOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, nullptr, &options, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_uploads, false);
        return aClient.getResult()->lastError.code() == 0;
    }

    /** Upload file to the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to upload.
     * The object is the object to be stored in the Storage bucket.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param options Optional. The GoogleCloudStorage::uploadOptions that holds the information for insert options, properties and types of upload.
     * For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
     * For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties
     * @param aResult The async result (AsyncResult).
     *
     *
     */
    void upload(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::uploadOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, &file, nullptr, &options, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_uploads, true);
    }

    /** Upload file to the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to upload.
     * The object is the object to be stored in the Storage bucket.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param options Optional. The GoogleCloudStorage::uploadOptions that holds the information for insert options, properties and types of upload.
     * For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
     * For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void upload(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::uploadOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, &file, nullptr, &options, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_uploads, true);
    }

    /** Perform OTA update using a firmware (object) from the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool ota(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_download_ota, false);
        return aClient.getResult()->lastError.code() == 0;
    }

    /** Perform OTA update using a firmware (object) from the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     * @param aResult The async result (AsyncResult).
     *
     */
    void ota(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_download_ota, true);
    }

    /** Perform OTA update using a firmware (object) from the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void ota(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_download_ota, true);
    }

    /** Get the metadata of object in Google Cloud Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to get metadata.
     * The object is the object in Storage bucket to get metadata.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     *
     * @return String The response payload.
     *
     */
    String getMetadata(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_get_meta, false);
        return aClient.getResult()->c_str();
    }

    /** Get the metadata of object in Google Cloud Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to get metadata.
     * The object is the object in Storage bucket to get metadata.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     * @param aResult The async result (AsyncResult).
     *
     */
    void getMetadata(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_get_meta, true);
    }

    /** Get the metadata of object in Google Cloud Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to get metadata.
     * The object is the object in Storage bucket to get metadata.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void getMetadata(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        // file_config_data file;
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_get_meta, true);
    }

    /** List all objects in Google Cloud Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id in its constructor.
     * The bucketid is the Storage bucket Id to list all objects.
     * @param options Optional. The GoogleCloudStorage::ListOptions that holds the list options.
     * For the list options, see https://cloud.google.com/storage/docs/json_api/v1/objects/list#optional-parameters
     * @return String The response payload.
     *
     */
    String list(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::ListOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, nullptr, nullptr, &options, GoogleCloudStorage::google_cloud_storage_request_type_list, false);
        return aClient.getResult()->c_str();
    }

    /** List all objects in Google Cloud Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id in its constructor.
     * The bucketid is the Storage bucket Id to list all objects.
     * @param options Optional. The GoogleCloudStorage::ListOptions that holds the list options.
     * For the list options, see https://cloud.google.com/storage/docs/json_api/v1/objects/list#optional-parameters
     * @param aResult The async result (AsyncResult).
     *
     */
    void list(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::ListOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, nullptr, nullptr, &options, GoogleCloudStorage::google_cloud_storage_request_type_list, true);
    }

    /** List all objects in Google Cloud Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id in its constructor.
     * The bucketid is the Storage bucket Id to list all objects.
     * @param options Optional. The GoogleCloudStorage::ListOptions that holds the list options.
     * For the list options, see https://cloud.google.com/storage/docs/json_api/v1/objects/list#optional-parameters
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void list(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::ListOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, nullptr, nullptr, &options, GoogleCloudStorage::google_cloud_storage_request_type_list, true);
    }

    /** Delete the object in Google Cloud Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to delete.
     * The object is the object in Storage bucket to delete.
     * @param options Optional. The GoogleCloudStorage::DeleteOptions that holds the delete options.
     * For the delete options, see see https://cloud.google.com/storage/docs/json_api/v1/objects/delete#optional-parameters
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool deleteObject(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::DeleteOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_delete, false);
        return aClient.getResult()->lastError.code() == 0;
    }

    /** Delete the object in Google Cloud Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to delete.
     * The object is the object in Storage bucket to delete.
     * @param options Optional. The GoogleCloudStorage::DeleteOptions that holds the delete options.
     * For the delete options, see see https://cloud.google.com/storage/docs/json_api/v1/objects/delete#optional-parameters
     * @param aResult The async result (AsyncResult).
     *
     */
    void deleteObject(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::DeleteOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_delete, true);
    }

    /** Delete the object in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to delete.
     * The object is the object in Storage bucket to delete.
     * @param options Optional. The GoogleCloudStorage::DeleteOptions that holds the delete options.
     * For the delete options, see see https://cloud.google.com/storage/docs/json_api/v1/objects/delete#optional-parameters
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void deleteObject(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::DeleteOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::google_cloud_storage_request_type_delete, true);
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
    Memory mem;

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

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const GoogleCloudStorage::Parent &parent, file_config_data *file, const GoogleCloudStorage::BaseOptions *baseOptions, GoogleCloudStorage::uploadOptions *uploadOptions, const GoogleCloudStorage::ListOptions *listOptions, GoogleCloudStorage::google_cloud_storage_request_type requestType, bool async)
    {
        GoogleCloudStorage::DataOptions options;
        options.requestType = requestType;
        options.parent = parent;

        if (uploadOptions && strlen(uploadOptions->insertProps.c_str()) && uploadOptions->uploadType == GoogleCloudStorage::upload_type_resumable)
            options.payload = uploadOptions->insertProps.c_str();

        async_request_handler_t::http_request_method method = async_request_handler_t::http_post;

        if (requestType == GoogleCloudStorage::google_cloud_storage_request_type_download ||
            requestType == GoogleCloudStorage::google_cloud_storage_request_type_download_ota ||
            requestType == GoogleCloudStorage::google_cloud_storage_request_type_list ||
            requestType == GoogleCloudStorage::google_cloud_storage_request_type_get_meta)
        {
            method = async_request_handler_t::http_get;
            if (requestType == GoogleCloudStorage::google_cloud_storage_request_type_download ||
                requestType == GoogleCloudStorage::google_cloud_storage_request_type_download_ota)
                options.extras += "?alt=media";
        }
        else if (requestType == GoogleCloudStorage::google_cloud_storage_request_type_uploads)
        {
            URLUtil uut;
            options.extras += "?name=";
            options.extras += uut.encode(parent.getObject());

            size_t sz = 0;
#if defined(ENABLE_FS)
            if (file && file->cb && file->filename.length())
            {
                file->cb(file->file, file->filename.c_str(), file_mode_open_read);
                sz = file->file.size();
                file->file.close();
            }
#endif
            if (file && file->data_size)
                sz = file->data_size;

            options.extras += "&uploadType=";

            // resumable upload is only for file bigger than 256k
            if (uploadOptions && uploadOptions->uploadType == GoogleCloudStorage::upload_type_resumable && sz >= 256 * 1024)
                options.extras += "resumable";
            else
            {
                options.extras += "media";
                options.payload.remove(0, options.payload.length());
            }
        }
        else if (requestType == GoogleCloudStorage::google_cloud_storage_request_type_delete)
            method = async_request_handler_t::http_delete;

        if (baseOptions && strlen(baseOptions->c_str()))
        {
            options.extras += options.extras.length() ? '&' : '?';
            options.extras += baseOptions->c_str();
        }
        else if (listOptions && strlen(listOptions->c_str()))
        {
            options.extras += options.extras.length() ? '&' : '?';
            options.extras += listOptions->c_str();
        }
        else if (uploadOptions && strlen(uploadOptions->insertOptions.c_str()))
        {
            options.extras += options.extras.length() ? '&' : '?';
            options.extras += uploadOptions->insertOptions.c_str();
        }

        GoogleCloudStorage::async_request_data_t aReq(&aClient, path, method, slot_options_t(false, false, async, false, requestType == GoogleCloudStorage::google_cloud_storage_request_type_download_ota, false), &options, file, result, cb, uid);

        if (uploadOptions && uploadOptions->mime.length() && requestType == GoogleCloudStorage::google_cloud_storage_request_type_uploads)
            aReq.mime = uploadOptions->mime;

        asyncRequest(aReq);
    }

    void asyncRequest(GoogleCloudStorage::async_request_data_t &request, int beta = 0)
    {
        app_token_t *atoken = appToken();

        if (!atoken)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        String extras;

        if (request.method == async_request_handler_t::http_post || request.method == async_request_handler_t::http_put)
            request.path += "/upload";

        request.path += "/storage/v1/b/";
        request.path += request.options->parent.getBucketId();
        request.path += "/o";

        if ((request.method == async_request_handler_t::http_get || request.method == async_request_handler_t::http_delete) && request.options->parent.getObject().length())
        {
            URLUtil uut;
            request.path += "/";
            request.path += uut.encode(request.options->parent.getObject());
        }

        addParams(request, extras);

        url(FPSTR("storage.googleapis.com"));

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

            if (request.options->payload.length())
            {
                if (request.options->extras.indexOf("uploadType=resumable") > -1)
                {
                    sData->request.val[req_hndlr_ns::payload] = request.options->payload;
                    sData->request.val[req_hndlr_ns::header] += FPSTR("X-Upload-Content-Type: ");
                    sData->request.val[req_hndlr_ns::header] += request.mime;
                    sData->request.val[req_hndlr_ns::header] += "\r\n";
                    request.aClient->setFileContentLength(sData, 0, "X-Upload-Content-Length");
                    request.aClient->setContentType(sData, "application/json; charset=UTF-8");
                    request.aClient->setContentLength(sData, request.options->payload.length());

                    sData->request.file_data.resumable.setSize(sData->request.file_data.file_size ? sData->request.file_data.file_size : sData->request.file_data.data_size);
                    sData->request.file_data.resumable.updateRange();
                }
            }
            else
            {
                if (request.mime.length())
                    request.aClient->setContentType(sData, request.mime);
                request.aClient->setFileContentLength(sData, 0);
            }

            if (sData->request.file_data.filename.length() > 0 && sData->request.file_data.file_size == 0)
                return setClientError(request, FIREBASE_ERROR_FILE_READ);

            if (request.options->extras.indexOf("uploadType=media") == -1)
            {
                URLUtil uut;
                sData->aResult.upload_data.downloadUrl = uut.downloadURL(request.options->parent.getBucketId(), request.options->parent.getObject());
            }
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

    void setClientError(GoogleCloudStorage::async_request_data_t &request, int code)
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

    void addParams(const GoogleCloudStorage::async_request_data_t &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }

    void setFileStatus(async_data_item_t *sData, const GoogleCloudStorage::async_request_data_t &request)
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