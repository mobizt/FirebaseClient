/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef STORAGE_STORAGE_H
#define STORAGE_STORAGE_H

#include <Arduino.h>
#include "./core/FirebaseApp.h"

using namespace firebase_ns;

#if defined(ENABLE_STORAGE)
#include "./storage/DataOptions.h"

class Storage : public AppBase
{
    friend class AppBase;

public:
    using Parent = FirebaseStorage::Parent;

    ~Storage() {}

    explicit Storage(const String &url = "") { this->service_url = url; }

    Storage &operator=(const Storage &rhs)
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
     * Perform the async task repeatedly.
     * Should be placed in main loop function.
     */
    void loop() { loopImpl(); }

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
    bool download(AsyncClientClass &aClient, const Parent &parent, file_config_data &file) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, "", FirebaseStorage::fs_download, false)->lastError.code() == 0; }

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
    void download(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, &file, "", FirebaseStorage::fs_download, true); }

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
    void download(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, &file, "", FirebaseStorage::fs_download, true); }

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
    bool upload(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, const String &mime) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, &file, mime, FirebaseStorage::fs_upload, false)->lastError.code() == 0; }

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
    void upload(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, const String &mime, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, &file, mime, FirebaseStorage::fs_upload, true); }

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
    void upload(AsyncClientClass &aClient, const Parent &parent, file_config_data &file, const String &mime, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, &file, mime, FirebaseStorage::fs_upload, true); }

    /** Perform OTA update using a firmware (object) from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     * @param command The OTA command (optional). 0 or U_FLASH for firmware, 100 or U_FLASHFS (ESP32) or 
     * U_FS (ESP8266 or Raspberry Pi Pico), 101 or U_SPIFFS (ESP32), 
     * 102 for U_FATFS (ESP32), 103 or U_LITTLEFS (ESP32).
     *
     * @return Boolean value, indicates the success of the operation.
     *
     */
    bool ota(AsyncClientClass &aClient, const Parent &parent, int command = 0) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", FirebaseStorage::fs_download_ota, false, command)->lastError.code() == 0; }

    /** Perform OTA update using a firmware (object) from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket IdThe FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     * @param aResult The async result (AsyncResult).
     * @param command The OTA command (optional). 0 or U_FLASH for firmware, 100 or U_FLASHFS (ESP32) or 
     * U_FS (ESP8266 or Raspberry Pi Pico), 101 or U_SPIFFS (ESP32), 
     * 102 for U_FATFS (ESP32), 103 or U_LITTLEFS (ESP32).
     *
     */
    void ota(AsyncClientClass &aClient, const Parent &parent, AsyncResult &aResult, int command = 0) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", FirebaseStorage::fs_download_ota, true, command); }

    /** Perform OTA update using a firmware (object) from the Firebase Storage.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket IdThe FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
     * The bucketid is the Storage bucket Id of object to download.
     * The object is the object in Storage bucket to download.
     * The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     * @param command The OTA command (optional). 0 or U_FLASH for firmware, 100 or U_FLASHFS (ESP32) or 
     * U_FS (ESP8266 or Raspberry Pi Pico), 101 or U_SPIFFS (ESP32), 
     * 102 for U_FATFS (ESP32), 103 or U_LITTLEFS (ESP32).
     *
     */
    void ota(AsyncClientClass &aClient, const Parent &parent, AsyncResultCallback cb, const String &uid = "", int command = 0) { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", FirebaseStorage::fs_download_ota, true, command); }

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
    String getMetadata(AsyncClientClass &aClient, const Parent &parent) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", FirebaseStorage::fs_get_meta, false)->c_str(); }

    /** Get the metadata of object in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to get metadata.
     * The object is the object in Storage bucket to get metadata.
     * @param aResult The async result (AsyncResult).
     *
     */
    void getMetadata(AsyncClientClass &aClient, const Parent &parent, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", FirebaseStorage::fs_get_meta, true); }

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
    void getMetadata(AsyncClientClass &aClient, const Parent &parent, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", FirebaseStorage::fs_get_meta, true); }

    /** List all objects in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id in its constructor.
     * The bucketid is the Storage bucket Id to list all objects.
     *
     * @return String The response payload.
     *
     */
    String list(AsyncClientClass &aClient, const Parent &parent) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", FirebaseStorage::fs_list, false)->c_str(); }

    /** List all objects in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id in its constructor.
     * The bucketid is the Storage bucket Id to list all objects.
     * @param aResult The async result (AsyncResult).
     *
     */
    void list(AsyncClientClass &aClient, const Parent &parent, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", FirebaseStorage::fs_list, true); }

    /** List all objects in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id in its constructor.
     * The bucketid is the Storage bucket Id to list all objects.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void list(AsyncClientClass &aClient, const Parent &parent, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", FirebaseStorage::fs_list, true); }

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
    bool deleteObject(AsyncClientClass &aClient, const Parent &parent) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", FirebaseStorage::fs_delete, false)->lastError.code() == 0; }

    /** Delete the object in Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
     * The bucketid is the Storage bucket Id of object to delete.
     * The object is the object in Storage bucket to delete.
     * @param aResult The async result (AsyncResult).
     *
     */
    void deleteObject(AsyncClientClass &aClient, const Parent &parent, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", FirebaseStorage::fs_delete, true); }

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
    void deleteObject(AsyncClientClass &aClient, const Parent &parent, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", FirebaseStorage::fs_delete, true); }

#if defined(FIREBASE_OTA_STORAGE)
    /**
     * Set Arduino OTA Storage.
     *  @param storage The Arduino  OTAStorage class object.
     */
    void setOTAStorage(OTAStorage &storage) { ota_storage_addr = reinterpret_cast<uint32_t>(&storage); }
#endif

private:
    AsyncResult *sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, file_config_data *file, const String &mime, FirebaseStorage::firebase_storage_request_type requestType, bool async, int command = 0)
    {
        using namespace FirebaseStorage;
        DataOptions options;
        options.requestType = requestType;
        options.parent = parent;

        reqns::http_request_method method = reqns::http_post;

        if (requestType == fs_download || requestType == fs_download_ota || requestType == fs_list || requestType == fs_get_meta)
        {
            method = reqns::http_get;
            if (requestType == fs_download || requestType == fs_download_ota)
                sut.printTo(options.extras, strlen(options.parent.getAccessToken()) + 100, "?alt=media%s%s", strlen(options.parent.getAccessToken()) ? "&token=" : "", strlen(options.parent.getAccessToken()) ? options.parent.getAccessToken() : "");
        }
        else if (requestType == fs_upload || requestType == fs_delete)
        {
            uut.addEncUrl(options.extras, "?name=", parent.getObject());
            if (requestType == fs_delete)
                method = reqns::http_delete;
        }

        req_data aReq(&aClient, method, slot_options_t(false, false, async, false, requestType == fs_download_ota, false), &options, file, result, cb, uid, command);
        if (mime.length() && requestType == fs_upload)
            aReq.mime = mime;
        asyncRequest(aReq);
        return aClient.getResult();
    }

    void asyncRequest(FirebaseStorage::req_data &request, int beta = 0)
    {
        app_token_t *atoken = appToken();
        if (!atoken)
            return request.aClient->setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        request.opt.user_auth = user_auth;
        String extras;

        sut.printTo(request.path, request.options->parent.getBucketId().length(), "/v0/b/%s/o", request.options->parent.getBucketId().c_str());

        if (request.method == reqns::http_get)
            uut.addEncUrl(request.path, "/", request.options->parent.getObject());

        sut.addParams(request.options->extras, extras);
        url("firebasestorage.googleapis.com");

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

            if (request.mime.length())
                sData->request.addContentType(request.mime);

            sData->request.setFileContentLength();
#if defined(ENABLE_FS)
            if (sData->request.file_data.filename.length() > 0 && sData->request.file_data.file_size == 0)
                return request.aClient->setClientError(request, FIREBASE_ERROR_FILE_READ);
#endif
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

    void setFileStatus(async_data *sData, const FirebaseStorage::req_data &request)
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