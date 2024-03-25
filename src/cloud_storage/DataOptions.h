/**
 * Created March 19, 2024
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
#ifndef GOOGLE_CLOUD_STORAGE_DATA_OPTIONS_H
#define GOOGLE_CLOUD_STORAGE_DATA_OPTIONS_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/JSON.h"
#include "./core/ObjectWriter.h"
#include "./core/AsyncClient/AsyncClient.h"
#include "./core/URL.h"

#if defined(ENABLE_CLOUD_STORAGE)

namespace GoogleCloudStorage
{
    enum google_cloud_storage_request_type
    {
        google_cloud_storage_request_type_undefined,
        google_cloud_storage_request_type_uploads,
        google_cloud_storage_request_type_upload_simple,
        google_cloud_storage_request_type_upload_multipart,
        google_cloud_storage_request_type_upload_resumable_init,
        google_cloud_storage_request_type_upload_resumable_run,
        google_cloud_storage_request_type_download,
        google_cloud_storage_request_type_patch,
        google_cloud_storage_request_type_get_meta,
        google_cloud_storage_request_type_set_meta,
        google_cloud_storage_request_type_update_meta,
        google_cloud_storage_request_type_delete,
        google_cloud_storage_request_type_list,
        google_cloud_storage_request_type_download_ota
    };

    enum upload_type
    {
        upload_type_simple,
        upload_type_multipart,
        upload_type_resumable
    };

    enum PROJECTION_OPTIONS
    {
        full,
        noAcl
    };

    enum ACL_OPTIONS
    {
        authenticatedRead,
        bucketOwnerFullControl,
        bucketOwnerRead,
        _private,
        projectPrivate,
        publicRead
    };

    struct BaseOptions : public Printable
    {

    protected:
        size_t bufSize = 7;
        String buf[7];
        ObjectWriter owriter;
        URLUtil uut;
        StringUtil sut;

        BaseOptions &setBuf()
        {
            owriter.clear(buf[0]);
            for (size_t i = 1; i < bufSize; i++)
            {
                if (buf[i].length())
                {
                    if (buf[0].length())
                        buf[0] += '&';
                    buf[0] += buf[i];
                }
            }
            return *this;
        }

    public:
        BaseOptions &generation(uint64_t value)
        {
            buf[1] = "generation=" + sut.u64Str(value);
            return setBuf();
        }
        BaseOptions &ifGenerationMatch(uint64_t value)
        {
            buf[2] = "ifGenerationMatch=" + sut.u64Str(value);
            return setBuf();
        }
        BaseOptions &ifGenerationNotMatch(uint64_t value)
        {
            buf[3] = "ifGenerationNotMatch=" + sut.u64Str(value);
            return setBuf();
        }
        BaseOptions &ifMetagenerationMatch(uint64_t value)
        {
            buf[4] = "ifMetagenerationMatch=" + sut.u64Str(value);
            return setBuf();
        }
        BaseOptions &ifMetagenerationNotMatch(uint64_t value)
        {
            buf[5] = "ifMetagenerationNotMatch=" + sut.u64Str(value);
            return setBuf();
        }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct GetOptions : public BaseOptions
    {

    public:
        BaseOptions &projection(PROJECTION_OPTIONS value)
        {
            buf[6] = "projection=";
            if (value == PROJECTION_OPTIONS::full)
                buf[6] += "full";
            else if (value == PROJECTION_OPTIONS::noAcl)
                buf[6] += "noAcl";
            return setBuf();
        }

        const char *c_str() const { return buf[0].c_str(); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct DeleteOptions : BaseOptions
    {
    public:
        const char *c_str() const { return buf[0].c_str(); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct InsertOptions : public Printable
    {

    private:
        size_t bufSize = 9;
        String buf[9];
        ObjectWriter owriter;
        StringUtil sut;

        InsertOptions &setBuf()
        {
            owriter.clear(buf[0]);
            for (size_t i = 1; i < bufSize; i++)
            {
                if (buf[i].length())
                {
                    if (buf[0].length())
                        buf[0] += '&';
                    buf[0] += buf[i];
                }
            }
            return *this;
        }

    public:
        InsertOptions &contentEncoding(const String &value)
        {
            buf[1] = "contentEncoding=" + value;
            return setBuf();
        }
        InsertOptions &ifGenerationMatch(uint64_t value)
        {
            buf[2] = "ifGenerationMatch=" + sut.u64Str(value);
            return setBuf();
        }
        InsertOptions &ifGenerationNotMatch(uint64_t value)
        {
            buf[3] = "ifGenerationNotMatch=" + sut.u64Str(value);
            return setBuf();
        }
        InsertOptions &ifMetagenerationMatch(uint64_t value)
        {
            buf[4] = "ifMetagenerationMatch=" + sut.u64Str(value);
            return setBuf();
        }
        InsertOptions &ifMetagenerationNotMatch(uint64_t value)
        {
            buf[5] = "ifMetagenerationNotMatch=" + sut.u64Str(value);
            return setBuf();
        }
        InsertOptions &kmsKeyName(const String &value)
        {
            buf[6] = "contentEncoding=" + value;
            return setBuf();
        }
        InsertOptions &predefinedAcl(ACL_OPTIONS value)
        {
            buf[7] = "predefinedAcl=";
            if (value == ACL_OPTIONS::authenticatedRead)
                buf[7] += "authenticatedRead";
            else if (value == ACL_OPTIONS::bucketOwnerFullControl)
                buf[7] += "bucketOwnerFullControl";
            else if (value == ACL_OPTIONS::bucketOwnerRead)
                buf[7] += "bucketOwnerRead";
            else if (value == ACL_OPTIONS::_private)
                buf[7] += "private";
            else if (value == ACL_OPTIONS::projectPrivate)
                buf[7] += "projectPrivate";
            else if (value == ACL_OPTIONS::publicRead)
                buf[7] += "publicRead";
            return setBuf();
        }
        InsertOptions &projection(PROJECTION_OPTIONS value)
        {
            buf[8] = "projection=";
            if (value == PROJECTION_OPTIONS::full)
                buf[8] += "full";
            else if (value == PROJECTION_OPTIONS::noAcl)
                buf[8] += "noAcl";
            return setBuf();
        }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct ListOptions : public Printable
    {

    private:
        size_t bufSize = 10;
        String buf[10];
        ObjectWriter owriter;
        URLUtil uut;

        ListOptions &setBuf()
        {
            owriter.clear(buf[0]);
            for (size_t i = 1; i < bufSize; i++)
            {
                if (buf[i].length())
                {
                    if (buf[0].length())
                        buf[0] += '&';
                    buf[0] += buf[i];
                }
            }
            return *this;
        }

    public:
        ListOptions &delimiter(const String &value)
        {
            buf[1] = "delimiter=" + value;
            return setBuf();
        }
        ListOptions &endOffset(const String &value)
        {
            buf[2] = "endOffset=" + value;
            return setBuf();
        }
        ListOptions &includeTrailingDelimiter(bool value)
        {
            buf[3] = "includeTrailingDelimiter=" + owriter.getBoolStr(value);
            return setBuf();
        }
        ListOptions &maxResults(uint32_t value)
        {
            buf[4] = "maxResults=" + String(value);
            return setBuf();
        }
        ListOptions &pageToken(const String &value)
        {
            buf[5] = "pageToken=" + value;
            return setBuf();
        }
        ListOptions &prefix(const String &value)
        {
            buf[6] = "prefix=" + value;
            return setBuf();
        }
        ListOptions &projection(const String &value)
        {
            buf[7] = "projection=" + value;
            return setBuf();
        }
        ListOptions &startOffset(const String &value)
        {
            buf[8] = "startOffset=" + value;
            return setBuf();
        }
        ListOptions &versions(bool value)
        {
            buf[9] = "versions=" + owriter.getBoolStr(value);
            return setBuf();
        }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct InsertProperties : public Printable
    {

    private:
        size_t bufSize = 15;
        String buf[15];
        ObjectWriter owriter;
        JSONUtil jut;

        InsertProperties &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        InsertProperties() { metadata(object_t()); }

        // https://cloud.google.com/storage/docs/access-control/lists
        // The item or value will be added to the array or list.
        // To add more items, call this function again.
        InsertProperties &acl(const String &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[1], "acl", value, true);
            return *this;
        }
        InsertProperties &cacheControl(const String &value) { return setObject(buf[2], FPSTR("cacheControl"), value, true, true); }
        InsertProperties &contentDisposition(const String &value) { return setObject(buf[3], FPSTR("contentDisposition"), value, true, true); }
        InsertProperties &contentEncoding(const String &value) { return setObject(buf[4], FPSTR("contentEncoding"), value, true, true); }
        InsertProperties &contentLanguage(const String &value) { return setObject(buf[5], FPSTR("contentLanguage"), value, true, true); }
        InsertProperties &contentType(const String &value) { return setObject(buf[6], FPSTR("contentType"), value, true, true); }
        InsertProperties &crc32c(const String &value) { return setObject(buf[7], FPSTR("crc32c"), value, true, true); }
        InsertProperties &customTime(const String &value) { return setObject(buf[8], FPSTR("customTime"), value, true, true); }
        InsertProperties &eventBasedHold(bool value) { return setObject(buf[9], FPSTR("eventBasedHold"), owriter.getBoolStr(value), false, true); }
        InsertProperties &md5Hash(const String &value) { return setObject(buf[10], FPSTR("md5Hash"), value, true, true); }
        InsertProperties &metadata(const object_t value)
        {
            buf[11] = FPSTR("{\"metadata\":{\"firebaseStorageDownloadTokens\":\"a82781ce-a115-442f-bac6-a52f7f63b3e8\"}}");
            if (strlen(value.c_str()))
                owriter.addMember(buf[11], value.c_str());
            owriter.getBuf(buf, bufSize);
            return *this;
        }
        InsertProperties &retention(const object_t value) { return setObject(buf[12], FPSTR("retention"), value.c_str(), false, true); }
        InsertProperties &storageClass(const String &value) { return setObject(buf[13], FPSTR("storageClass"), value, true, true); }
        InsertProperties &temporaryHold(bool value) { return setObject(buf[14], FPSTR("temporaryHold"), owriter.getBoolStr(value), false, true); }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    class Parent
    {
        friend class Storage;

    private:
        String bucketId, object;

    public:
        Parent() {}
        Parent(const String &bucketId, const String &object = "")
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
        }
        String getObject() const { return object; }
        String getBucketId() const { return bucketId; }
    };

    class DataOptions
    {
        friend class Messaging;

    public:
        String payload, extras;
        GoogleCloudStorage::Parent parent;
        google_cloud_storage_request_type requestType = google_cloud_storage_request_type_undefined;
        unsigned long requestTime = 0;
        void copy(DataOptions &rhs)
        {
            this->extras = rhs.extras;
            this->payload = rhs.payload;
        }

    private:
    };

    struct uploadOptions
    {
        String mime;
        GoogleCloudStorage::upload_type uploadType;
        GoogleCloudStorage::InsertOptions insertOptions;
        GoogleCloudStorage::InsertProperties insertProps;
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
        async_request_data_t(AsyncClientClass *aClient, const String &path, async_request_handler_t::http_request_method method, slot_options_t opt, DataOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
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