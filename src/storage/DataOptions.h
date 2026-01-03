/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef STORAGE_DATA_OPTIONS_H
#define STORAGE_DATA_OPTIONS_H

#include <Arduino.h>
#include "./core/Utils/JSON.h"
#include "./core/Utils/ObjectWriter.h"
#include "./core/AsyncClient/AsyncClient.h"

#if defined(ENABLE_STORAGE)
namespace FirebaseStorage
{
    enum firebase_storage_request_type
    {
        fs_undefined,
        fs_upload,
        fs_download,
        fs_get_meta,
        fs_delete,
        fs_list,
        fs_download_ota
    };

    class Parent
    {
        friend class Storage;

    private:
        String bucketId, object, accessToken;

    public:
        Parent() {}
        explicit Parent(const String &bucketId, const String &object = "", const String &accessToken = "")
        {
            URLUtil uut;
            this->bucketId = uut.getHost(bucketId);
            this->object = object.length() && object[0] == '/' ? object.substring(1, object.length() - 1) : object;
            this->accessToken = accessToken;
        }
        String getObject() const { return object; }
        String getBucketId() const { return bucketId; }
        const char *getAccessToken() const { return accessToken.c_str(); }
    };

    class DataOptions
    {
    public:
        String payload, extras;
        FirebaseStorage::Parent parent;
        firebase_storage_request_type requestType = fs_undefined;
        unsigned long requestTime = 0;

        void copy(const DataOptions &rhs)
        {
            this->extras = rhs.extras;
            this->payload = rhs.payload;
        }

    private:
    };

    struct req_data
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path, uid, mime;
        reqns::http_request_method method = reqns::http_undefined;
        slot_options_t opt;
        DataOptions *options = nullptr;
        file_config_data *file = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        int command = 0;
        req_data() {}
        explicit req_data(AsyncClientClass *aClient, reqns::http_request_method method, slot_options_t opt, DataOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "", int command = 0)
        {
            this->aClient = aClient;
            this->method = method;
            this->opt = opt;
            this->options = options;
            this->file = file;
            this->aResult = aResult;
            this->cb = cb;
            this->uid = uid;
            this->command = command;
        }
    };
}
#endif
#endif