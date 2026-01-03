/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CLOUD_STORAGE_CLOUD_STORAGE_H
#define CLOUD_STORAGE_CLOUD_STORAGE_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./core/Utils/StringUtil.h"

using namespace firebase_ns;

#if defined(ENABLE_CLOUD_STORAGE)

#include "./cloud_storage/DataOptions.h"

class CloudStorage : public AppBase
{
    friend class AppBase;

public:
    using Parent = GoogleCloudStorage::Parent;
    using GetOptions = GoogleCloudStorage::GetOptions;
    using UploadOptions = GoogleCloudStorage::UploadOptions;
    using ListOptions = GoogleCloudStorage::ListOptions;
    using DeleteOptions = GoogleCloudStorage::DeleteOptions;

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
    void resetApp() { resetAppImpl(); }

    /**
     * Perform the async task repeatedly (DEPRECATED).
     */
    void loop() { loopImpl(); }

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
    bool download(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, GetOptions &options) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, &options, nullptr, nullptr, GoogleCloudStorage::cs_download, false, 0)->lastError.code() == 0; }

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
    void download(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, GetOptions &options, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, &file, &options, nullptr, nullptr, GoogleCloudStorage::cs_download, true, 0); }

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
    void download(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, GetOptions &options, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, &file, &options, nullptr, nullptr, GoogleCloudStorage::cs_download, true, 0); }

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
    bool upload(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, UploadOptions &options) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, nullptr, &options, nullptr, GoogleCloudStorage::cs_uploads, false, 0)->lastError.code() == 0; }

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
    void upload(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, UploadOptions &options, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, &file, nullptr, &options, nullptr, GoogleCloudStorage::cs_uploads, true, 0); }

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
    void upload(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, UploadOptions &options, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, &file, nullptr, &options, nullptr, GoogleCloudStorage::cs_uploads, true, 0); }

    /** Perform OTA update using a firmware (object) from the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     * @param command The OTA command (optional). 0 or U_FLASH for firmware, 100 or U_FLASHFS (ESP32) or
     * U_FS (ESP8266 or Raspberry Pi Pico), 101 or U_SPIFFS (ESP32),
     * 102 for U_FATFS (ESP32), 103 or U_LITTLEFS (ESP32).
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool ota(AsyncClientClass &aClient, const Parent &parent, GetOptions &options, int command = 0) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_download_ota, false, command)->lastError.code() == 0; }

    /** Perform OTA update using a firmware (object) from the Google Cloud Storage.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * @param options Optional. The GoogleCloudStorage::GetOptions that holds the get options.
     * For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters
     * @param aResult The async result (AsyncResult).
     * @param command The OTA command (optional). 0 or U_FLASH for firmware, 100 or U_FLASHFS (ESP32) or
     * U_FS (ESP8266 or Raspberry Pi Pico), 101 or U_SPIFFS (ESP32),
     * 102 for U_FATFS (ESP32), 103 or U_LITTLEFS (ESP32).
     *
     */
    void ota(AsyncClientClass &aClient, const Parent &parent, GetOptions &options, AsyncResult &aResult, int command) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_download_ota, true, command); }

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
     * @param command The OTA command (optional). 0 or U_FLASH for firmware, 100 or U_FLASHFS (ESP32) or
     * U_FS (ESP8266 or Raspberry Pi Pico), 101 or U_SPIFFS (ESP32),
     * 102 for U_FATFS (ESP32), 103 or U_LITTLEFS (ESP32).
     *
     */
    void ota(AsyncClientClass &aClient, const Parent &parent, GetOptions &options, AsyncResultCallback cb, const String &uid = "", int command = 0) { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_download_ota, true, command); }

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
    String getMetadata(AsyncClientClass &aClient, const Parent &parent, GetOptions &options) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_get_meta, false, 0)->c_str(); }

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
    void getMetadata(AsyncClientClass &aClient, const Parent &parent, GetOptions &options, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_get_meta, true, 0); }

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
    void getMetadata(AsyncClientClass &aClient, const Parent &parent, GetOptions &options, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_get_meta, true, 0); }

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
    String list(AsyncClientClass &aClient, const Parent &parent, ListOptions &options) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, nullptr, nullptr, &options, GoogleCloudStorage::cs_list, false, 0)->c_str(); }

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
    void list(AsyncClientClass &aClient, const Parent &parent, ListOptions &options, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, nullptr, nullptr, &options, GoogleCloudStorage::cs_list, true, 0); }

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
    void list(AsyncClientClass &aClient, const Parent &parent, ListOptions &options, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, nullptr, nullptr, &options, GoogleCloudStorage::cs_list, true, 0); }

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
    bool deleteObject(AsyncClientClass &aClient, const Parent &parent, DeleteOptions &options) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_delete, false, 0)->lastError.code() == 0; }

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
    void deleteObject(AsyncClientClass &aClient, const Parent &parent, DeleteOptions &options, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_delete, true, 0); }

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
    void deleteObject(AsyncClientClass &aClient, const Parent &parent, DeleteOptions &options, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, &options, nullptr, nullptr, GoogleCloudStorage::cs_delete, true, 0); }

#if defined(FIREBASE_OTA_STORAGE)
    /**
     * Set Arduino OTA Storage.
     *  @param storage The Arduino  OTAStorage class object.
     */
    void setOTAStorage(OTAStorage &storage) { ota_storage_addr = reinterpret_cast<uint32_t>(&storage); }
#endif

private:
    String path, uid;
    Memory mem;

    AsyncResult *sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, file_config_data *file, const GoogleCloudStorage::BaseOptions *baseOptions, GoogleCloudStorage::UploadOptions *UploadOptions, const GoogleCloudStorage::ListOptions *listOptions, GoogleCloudStorage::google_cloud_storage_request_type requestType, bool async, int command)
    {
        using namespace GoogleCloudStorage;
        DataOptions options;
        options.requestType = requestType;
        options.parent = parent;

        if (UploadOptions && strlen(UploadOptions->insertProps.c_str()) && UploadOptions->uploadType == upload_type_resumable)
            options.payload = UploadOptions->insertProps.c_str();

        reqns::http_request_method method = reqns::http_post;

        if (requestType == cs_download || requestType == cs_download_ota || requestType == cs_list || requestType == cs_get_meta)
        {
            method = reqns::http_get;
            if (requestType == cs_download || requestType == cs_download_ota)
                options.extras += "?alt=media";
        }
        else if (requestType == cs_uploads)
        {
            uut.addEncUrl(options.extras, "?name=", parent.getObject());

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
            if (UploadOptions && UploadOptions->uploadType == upload_type_resumable && sz >= 256 * 1024)
                options.extras += "resumable";
            else
            {
                options.extras += "media";
                sut.clear(options.payload);
            }
        }
        else if (requestType == cs_delete)
            method = reqns::http_delete;

        if (baseOptions)
            uut.addUrl(options.extras, baseOptions->c_str());
        else if (listOptions)
            uut.addUrl(options.extras, listOptions->c_str());
        else if (UploadOptions)
            uut.addUrl(options.extras, UploadOptions->insertOptions.c_str());

        req_data aReq(&aClient, path, method, slot_options_t(false, false, async, false, requestType == cs_download_ota, false), &options, file, result, cb, uid, command);

        if (UploadOptions && UploadOptions->mime.length() && requestType == cs_uploads)
            aReq.mime = UploadOptions->mime;

        asyncRequest(aReq);
        return aClient.getResult();
    }

    void asyncRequest(GoogleCloudStorage::req_data &request, int beta = 0)
    {
        app_token_t *atoken = appToken();

        if (!atoken)
            return request.aClient->setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        request.opt.user_auth = user_auth;
        String extras;

        sut.printTo(request.path, 200, "%s/storage/v1/b/%s/o", request.method == reqns::http_post || request.method == reqns::http_put ? "/upload" : "", request.options->parent.getBucketId().c_str());
        if ((request.method == reqns::http_get || request.method == reqns::http_delete) && request.options->parent.getObject().length())
            uut.addEncUrl(request.path, "/", request.options->parent.getObject());

        sut.addParams(request.options->extras, extras);
        url("storage.googleapis.com");

        async_data *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return request.aClient->setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        request.aClient->newRequest(sData, service_url, request.path, extras, request.method, request.opt, request.uid, "");

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
            sData->request.command = request.command;
        }

        if (request.file && sData->upload)
        {
            sData->request.base64 = false;

            if (request.options->payload.length())
            {
                if (request.options->extras.indexOf("uploadType=resumable") > -1)
                {
                    StringUtil sut;
                    sData->request.val[reqns::payload] = request.options->payload;
                    sData->request.setFileContentLength(0, "X-Upload-Content-Length");
                    sut.printTo(sData->request.val[reqns::header], 100, "X-Upload-Content-Type: %s\r\nContent-Type: application/json; charset=UTF-8\r\nContent-Length: %d\r\n\r\n", request.mime.c_str(), request.options->payload.length());
                    sData->request.file_data.resumable.setSize(sData->request.file_data.file_size ? sData->request.file_data.file_size : sData->request.file_data.data_size);
                    sData->request.file_data.resumable.updateRange();
                }
            }
            else
            {
                if (request.mime.length())
                    sData->request.addContentType(request.mime);
                sData->request.setFileContentLength();
            }

#if defined(ENABLE_FS)
            if (sData->request.file_data.filename.length() > 0 && sData->request.file_data.file_size == 0)
                return request.aClient->setClientError(request, FIREBASE_ERROR_FILE_READ);
#endif

            if (request.options->extras.indexOf("uploadType=media") == -1)
                sData->aResult.upload_data.downloadUrl = uut.downloadURL(request.options->parent.getBucketId(), request.options->parent.getObject());
        }
        else if (request.options->payload.length())
        {
            sData->request.val[reqns::payload] = request.options->payload;
            sData->request.setContentLengthFinal(request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        request.aClient->addRemoveClientVec(reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(request.aClient->getResultList())));

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void setFileStatus(async_data *sData, const GoogleCloudStorage::req_data &request)
    {
        bool isFile = request.file && request.file->data && request.file->data_size > 0;
#if defined(ENABLE_FS)
        isFile |= request.file && request.file->filename.length() > 0;
#endif
        if (isFile || request.opt.ota)
        {
            sData->download = request.method == reqns::http_get;
            sData->upload = request.method == reqns::http_post || request.method == reqns::http_put || request.method == reqns::http_patch;
        }
    }
};
#endif
#endif