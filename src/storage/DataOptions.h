/**
 * Created June 12, 2024
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
#ifndef FIREBASE_STORAGE_DATA_OPTIONS_H
#define FIREBASE_STORAGE_DATA_OPTIONS_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/JSON.h"
#include "./core/ObjectWriter.h"
#include "./core/AsyncClient/AsyncClient.h"

#if defined(ENABLE_STORAGE)

namespace FirebaseStorage
{
    enum firebase_storage_request_type
    {
        firebase_storage_request_type_undefined,
        firebase_storage_request_type_upload,
        firebase_storage_request_type_download,
        firebase_storage_request_type_get_meta,
        firebase_storage_request_type_delete,
        firebase_storage_request_type_list,
        firebase_storage_request_type_download_ota
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
            this->object = object;
            this->bucketId = bucketId;

            if (this->bucketId.length() && this->bucketId.indexOf("://") > -1)
                this->bucketId.remove(0, this->bucketId.indexOf("://") + 3);

            if (this->bucketId.length())
            {
                if (this->bucketId.indexOf("://") > -1)
                    this->bucketId.remove(0, this->bucketId.indexOf("://") + 3);

                if (this->bucketId.length() && this->bucketId[this->bucketId.length() - 1] == '/')
                    this->bucketId.remove(this->bucketId.length() - 1, 1);
            }

            if (this->object.length() && this->object[0] == '/')
                this->object.remove(0, 1);

            this->accessToken = accessToken;
        }
        String getObject() const { return object; }
        String getBucketId() const { return bucketId; }
        String getAccessToken() const { return accessToken; }
    };

    class DataOptions
    {

    public:
        String payload, extras;
        FirebaseStorage::Parent parent;
        firebase_storage_request_type requestType = firebase_storage_request_type_undefined;
        unsigned long requestTime = 0;

        void copy(const DataOptions &rhs)
        {
            this->extras = rhs.extras;
            this->payload = rhs.payload;
        }

    private:
    };

    struct async_request_data_t
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path;
        String uid;
        String mime;
        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;
        slot_options_t opt;
        DataOptions *options = nullptr;
        file_config_data *file = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        async_request_data_t() {}
        explicit async_request_data_t(AsyncClientClass *aClient, const String &path, async_request_handler_t::http_request_method method, slot_options_t opt, DataOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
        {
            this->aClient = aClient;
            this->path = path;
            this->method = method;
            this->opt = opt;
            this->options = options;
            this->file = file;
            this->aResult = aResult;
            this->cb = cb;
            this->uid = uid;
        }
    };

}

#endif

#endif