/**
 * 2025-01-27
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
#ifndef ASYNC_CLOUD_STORAGE_H
#define ASYNC_CLOUD_STORAGE_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./core/StringUtil.h"

using namespace firebase_ns;

#if defined(ENABLE_CLOUD_STORAGE)

#include "./cloud_storage/DataOptions.h"

class CloudStorage
{

    friend class AppBase;

public:
    using Parent = GoogleCloudStorage::Parent;
    using GetOptions = GoogleCloudStorage::GetOptions;
    using UploadOptions = GoogleCloudStorage::UploadOptions;
    using ListOptions = GoogleCloudStorage::ListOptions;
    using DeleteOptions = GoogleCloudStorage::DeleteOptions;

    std::vector<uint32_t> cVec; // AsyncClient vector

    ~CloudStorage() {}
    explicit CloudStorage(const String &url = "") { this->service_url = url; }

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
    bool download(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, GetOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, &options, nullptr, nullptr, GoogleCloudStorage::cs_download, false);
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
    void download(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, GetOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, &file, &options, nullptr, nullptr, GoogleCloudStorage::cs_download, true);
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
    void download(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, GetOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, &file, &options, nullptr, nullptr, GoogleCloudStorage::cs_download, true);
    }

    /** Upload file to the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to upload.
     * The object is the object to be stored in the Storage bucket.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param options Optional. The GoogleCloudStorage::UploadOptions that holds the information for insert options, properties and types of upload.
     * For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
     * For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool upload(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, UploadOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, nullptr, &options, nullptr, GoogleCloudStorage::cs_uploads, false);
        return aClient.getResult()->lastError.code() == 0;
    }

    /** Upload file to the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to upload.
     * The object is the object to be stored in the Storage bucket.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param options Optional. The GoogleCloudStorage::UploadOptions that holds the information for insert options, properties and types of upload.
     * For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
     * For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties
     * @param aResult The async result (AsyncResult).
     *
     *
     */
    void upload(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, UploadOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, &file, nullptr, &options, nullptr, GoogleCloudStorage::cs_uploads, true);
    }

    /** Upload file to the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to upload.
     * The object is the object to be stored in the Storage bucket.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * @param options Optional. The GoogleCloudStorage::UploadOptions that holds the information for insert options, properties and types of upload.
     * For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
     * For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void upload(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, UploadOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, &file, nullptr, &options, nullptr, GoogleCloudStorage::cs_uploads, true);
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
    bool ota(AsyncClientClass &aClient, const Parent &parent, GetOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_download_ota, false);
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
    void ota(AsyncClientClass &aClient, const Parent &parent, GetOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_download_ota, true);
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
    void ota(AsyncClientClass &aClient, const Parent &parent, GetOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_download_ota, true);
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
    String getMetadata(AsyncClientClass &aClient, const Parent &parent, GetOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_get_meta, false);
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
    void getMetadata(AsyncClientClass &aClient, const Parent &parent, GetOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_get_meta, true);
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
    void getMetadata(AsyncClientClass &aClient, const Parent &parent, GetOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        // file_config_data file;
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_get_meta, true);
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
    String list(AsyncClientClass &aClient, const Parent &parent, ListOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, nullptr, nullptr, &options, GoogleCloudStorage::cs_list, false);
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
    void list(AsyncClientClass &aClient, const Parent &parent, ListOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, nullptr, nullptr, &options, GoogleCloudStorage::cs_list, true);
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
    void list(AsyncClientClass &aClient, const Parent &parent, ListOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, nullptr, nullptr, &options, GoogleCloudStorage::cs_list, true);
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
    bool deleteObject(AsyncClientClass &aClient, const Parent &parent, DeleteOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_delete, false);
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
    void deleteObject(AsyncClientClass &aClient, const Parent &parent, DeleteOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_delete, true);
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
    void deleteObject(AsyncClientClass &aClient, const Parent &parent, DeleteOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_delete, true);
    }

#if defined(FIREBASE_OTA_STORAGE)
    /**
     * Set Arduino OTA Storage.
     *  @param storage The Arduino  OTAStorage class object.
     */
    void setOTAStorage(OTAStorage &storage) { ota_storage_addr = reinterpret_cast<uint32_t>(&storage); }
#endif

private:
    StringUtil sut;
    String service_url, path, uid;
    // FirebaseApp address and FirebaseApp vector address
    uint32_t app_addr = 0, avec_addr = 0, ul_dl_task_running_addr = 0, ota_storage_addr = 0;
    app_token_t *app_token = nullptr;
    Memory mem;

    void url(const String &url) { this->service_url = url; }

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

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, file_config_data *file, const GoogleCloudStorage::BaseOptions *baseOptions, GoogleCloudStorage::UploadOptions *UploadOptions, const GoogleCloudStorage::ListOptions *listOptions, GoogleCloudStorage::google_cloud_storage_request_type requestType, bool async)
    {
        GoogleCloudStorage::DataOptions options;
        options.requestType = requestType;
        options.parent = parent;

        if (UploadOptions && strlen(UploadOptions->insertProps.c_str()) && UploadOptions->uploadType == GoogleCloudStorage::upload_type_resumable)
            options.payload = UploadOptions->insertProps.c_str();

        reqns::http_request_method method = reqns::http_post;

        if (requestType == GoogleCloudStorage::cs_download ||
            requestType == GoogleCloudStorage::cs_download_ota ||
            requestType == GoogleCloudStorage::cs_list ||
            requestType == GoogleCloudStorage::cs_get_meta)
        {
            method = reqns::http_get;
            if (requestType == GoogleCloudStorage::cs_download ||
                requestType == GoogleCloudStorage::cs_download_ota)
                options.extras += "?alt=media";
        }
        else if (requestType == GoogleCloudStorage::cs_uploads)
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
            if (UploadOptions && UploadOptions->uploadType == GoogleCloudStorage::upload_type_resumable && sz >= 256 * 1024)
                options.extras += "resumable";
            else
            {
                options.extras += "media";
                sut.clear(options.payload);
            }
        }
        else if (requestType == GoogleCloudStorage::cs_delete)
            method = reqns::http_delete;

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
        else if (UploadOptions && strlen(UploadOptions->insertOptions.c_str()))
        {
            options.extras += options.extras.length() ? '&' : '?';
            options.extras += UploadOptions->insertOptions.c_str();
        }

        GoogleCloudStorage::req_data aReq(&aClient, path, method, slot_options_t(false, false, async, false, requestType == GoogleCloudStorage::cs_download_ota, false), &options, file, result, cb, uid);

        if (UploadOptions && UploadOptions->mime.length() && requestType == GoogleCloudStorage::cs_uploads)
            aReq.mime = UploadOptions->mime;

        asyncRequest(aReq);
    }

    void asyncRequest(GoogleCloudStorage::req_data &request, int beta = 0)
    {
        app_token_t *atoken = appToken();

        if (!atoken)
            return request.aClient->setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        String extras;

        if (request.method == reqns::http_post || request.method == reqns::http_put)
            request.path += "/upload";

        request.path += "/storage/v1/b/";
        request.path += request.options->parent.getBucketId();
        request.path += "/o";

        if ((request.method == reqns::http_get || request.method == reqns::http_delete) && request.options->parent.getObject().length())
        {
            URLUtil uut;
            request.path += "/";
            request.path += uut.encode(request.options->parent.getObject());
        }

        addParams(request, extras);

        url(FPSTR("storage.googleapis.com"));

        async_data *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return request.aClient->setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

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
                    sData->request.val[reqns::payload] = request.options->payload;
                    sData->request.val[reqns::header] += FPSTR("X-Upload-Content-Type: ");
                    sData->request.val[reqns::header] += request.mime;
                    sData->request.val[reqns::header] += "\r\n";
                    sData->request.setFileContentLength(0, "X-Upload-Content-Length");
                    sData->request.setContentType("application/json; charset=UTF-8");
                    sData->request.setContentLength(request.options->payload.length());

                    sData->request.file_data.resumable.setSize(sData->request.file_data.file_size ? sData->request.file_data.file_size : sData->request.file_data.data_size);
                    sData->request.file_data.resumable.updateRange();
                }
            }
            else
            {
                if (request.mime.length())
                    sData->request.setContentType(request.mime);
                sData->request.setFileContentLength(0);
            }

            if (sData->request.file_data.filename.length() > 0 && sData->request.file_data.file_size == 0)
                return request.aClient->setClientError(request, FIREBASE_ERROR_FILE_READ);

            if (request.options->extras.indexOf("uploadType=media") == -1)
            {
                URLUtil uut;
                sData->aResult.upload_data.downloadUrl = uut.downloadURL(request.options->parent.getBucketId(), request.options->parent.getObject());
            }
        }
        else if (request.options->payload.length())
        {
            sData->request.val[reqns::payload] = request.options->payload;
            sData->request.setContentLength(request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        request.aClient->addRemoveClientVec(reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(request.aClient->rVec)));

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void addParams(const GoogleCloudStorage::req_data &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }

    void setFileStatus(async_data *sData, const GoogleCloudStorage::req_data &request)
    {
        if ((request.file && (request.file->filename.length() || request.file->data_size)) || request.opt.ota)
        {
            sData->download = request.method == reqns::http_get;
            sData->upload = request.method == reqns::http_post ||
                            request.method == reqns::http_put ||
                            request.method == reqns::http_patch;
        }
    }
};

#endif

#endif