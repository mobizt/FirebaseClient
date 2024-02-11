/**
 * Created February 11, 2024
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
#ifndef FIRESTORE_DATAOPTIONS_H
#define FIRESTORE_DATAOPTIONS_H

#include <Arduino.h>
#include "./Config.h"

#if defined(ENABLE_FIRESTORE)

enum firebase_firestore_request_type
{
    firebase_firestore_request_type_undefined,
    firebase_firestore_request_type_rollback = 200,
    firebase_firestore_request_type_begin_transaction,
    firebase_firestore_request_type_commit_document,
    firebase_firestore_request_type_batch_write_doc,
    firebase_firestore_request_type_run_query,
    firebase_firestore_request_type_list_collection,
    firebase_firestore_request_type_export_docs,
    firebase_firestore_request_type_import_docs,
    firebase_firestore_request_type_create_doc,
    firebase_firestore_request_type_batch_get_doc,
    firebase_firestore_request_type_create_index,

    firebase_firestore_request_type_get_doc = 300,
    firebase_firestore_request_type_list_doc,
    firebase_firestore_request_type_list_index,
    firebase_firestore_request_type_get_index,

    firebase_firestore_request_type_patch_doc = 400,

    firebase_firestore_request_type_delete_doc = 500,
    firebase_firestore_request_type_delete_index
};

typedef struct firebase_firestore_parent_resource_t
{
    friend class Firestore;

private:
    String projectId;
    String databaseId;
    String documentPath;

    void pathResove(String &collectionId, String &documentId)
    {
        size_t count = 0;
        collectionId = this->documentPath;
        int p = collectionId.lastIndexOf("/");
        String _documentPath = this->documentPath;

        for (size_t i = 0; i < _documentPath.length(); i++)
            count += _documentPath[i] == '/' ? 1 : 0;

        if (p > -1 && count % 2 > 0)
        {
            documentId = collectionId.substring(p + 1, collectionId.length());
            collectionId = collectionId.substring(0, p);
        }
    }

public:
    firebase_firestore_parent_resource_t() {}
    firebase_firestore_parent_resource_t(const String &projectId, const String &databaseId)
    {
        this->projectId = projectId;
        this->databaseId = databaseId;
    }

} ParentResource;

typedef struct firebase_firestore_document_mask_t
{
    friend class Firestore;

private:
    String mask;

    String get()
    {
        String buf;
        JsonHelper jh;
        jh.addTokens(buf, jh.toString("fieldPaths"), mask, true);
        return buf;
    }

    String getQuery(const String &mask, bool hasParam)
    {
        String buf;
        URLHelper uh;
        uh.addParamsTokens(buf, String(mask + ".fieldPaths="), this->mask, hasParam);
        return buf;
    }

public:
    firebase_firestore_document_mask_t() {}
    firebase_firestore_document_mask_t(const String &mask)
    {
        this->mask = mask;
    }

} DocumentMask;

namespace Values
{

    struct NullValue
    {

    public:
        NullValue() {}
        const char *c_str() { return (const char *)FPSTR("{\"nullValue\":null}"); }
    };

    struct BooleanValue
    {
    private:
        String buf;

    public:
        BooleanValue(bool value)
        {
            buf = FPSTR("{\"booleanValue\":");
            buf += value ? FPSTR("true") : FPSTR("false");
            buf += '}';
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct IntegerValue
    {

    private:
        String buf;

    public:
        IntegerValue(int v)
        {
            buf = FPSTR("{\"integerValue\":\"");
            buf += v;
            buf += FPSTR("\"}");
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct DoubleValue
    {

    private:
        String buf;

    public:
        DoubleValue(double value)
        {
            buf = FPSTR("{\"doubleValue\":");
            buf += String(value);
            buf += '}';
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct TimestampValue
    {

    private:
        String buf;

    public:
        TimestampValue(const String &value)
        {
            buf = FPSTR("{\"timestampValue\":\"");
            buf += value;
            buf += FPSTR("\"}");
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct StringValue
    {

    private:
        String buf;

    public:
        StringValue(const String &value)
        {
            buf = FPSTR("{\"stringValue\":\"");
            buf += value;
            buf += FPSTR("\"}");
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct BytesValue
    {
    private:
        String buf;

    public:
        BytesValue(const String &value)
        {
            buf = FPSTR("{\"bytesValue\":\"");
            buf += value;
            buf += FPSTR("\"}");
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct ReferenceValue
    {

    private:
        String buf;

    public:
        ReferenceValue(const String &value)
        {
            buf = FPSTR("{\"referenceValue\":\"");
            buf += value;
            buf += FPSTR("\"}");
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct GeoPointValue
    {
    private:
        String buf;

    public:
        GeoPointValue(double lat, double lng)
        {
            buf = FPSTR("{\"geoPointValue\":{\"latitude\":");
            buf += String(lat);
            buf += FPSTR(",\"longitude\":");
            buf += String(lng);
            buf += FPSTR("}}");
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct ArrayValue
    {

    private:
        String buf;
        template <typename T>
        void set(T value)
        {
            buf = FPSTR("{\"arrayValue\":{\"values\":[");
            buf += value.c_str();
            buf += FPSTR("]}}");
        }

    public:
        template <typename T>
        ArrayValue(T value)
        {
            set(value);
        }
        template <typename T>
        ArrayValue &add(T value)
        {
            if (buf.length() == 0)
                set(value);
            else
            {
                int p = buf.lastIndexOf("]}}");
                String str = buf.substring(0, p);
                str += ',';
                str += value.c_str();
                str += FPSTR("]}}");
                buf = str;
            }

            return *this;
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct MapValue
    {
    private:
        String buf;
        template <typename T>
        void set(const String &key, T value)
        {
            buf = FPSTR("{\"mapValue\":{\"fields\":{\"");
            buf += key;
            buf += FPSTR("\":");
            buf += value.c_str();
            buf += FPSTR("}}}");
        }

    public:
        template <typename T>
        MapValue(const String &key, T value)
        {
            set(key, value);
        }
        template <typename T>
        MapValue &add(const String &key, T value)
        {
            if (buf.length() == 0)
                set(key, value);
            else
            {
                int p = buf.lastIndexOf("}}}");
                String str = buf.substring(0, p);
                str += FPSTR(",\"");
                str += key;
                str += FPSTR("\":");
                str += value.c_str();
                str += FPSTR("}}}");
                buf = str;
            }
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
    };

};

namespace FieldTransform
{
    enum ServerValue
    {
        SERVER_VALUE_UNSPECIFIED,
        REQUEST_TIME
    };

    struct Increment
    {
    private:
        String buf;

    public:
        template <typename T>
        Increment(T value)
        {
            buf = FPSTR("{\"increment\":");
            buf += value.c_str();
            buf += '}';
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct Maximum
    {
    private:
        String buf;

    public:
        template <typename T>
        Maximum(T value)
        {
            buf = FPSTR("{\"maximum\":");
            buf += value.c_str();
            buf += '}';
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct Minimum
    {
    private:
        String buf;

    public:
        template <typename T>
        Minimum(T value)
        {
            buf = FPSTR("{\"minimum\":");
            buf += value.c_str();
            buf += '}';
        }
        const char *c_str() { return buf.c_str(); }
    };

    template <typename T = Values::ArrayValue>
    struct AppendMissingElements
    {
    private:
        String buf;

    public:
        AppendMissingElements(T value)
        {
            buf = FPSTR("{\"appendMissingElements\":");
            buf += value.c_str();
            buf += '}';
        }
        const char *c_str() { return buf.c_str(); }
    };

    template <typename T = Values::ArrayValue>
    struct RemoveAllFromArray
    {
    private:
        String buf;

    public:
        RemoveAllFromArray(T value)
        {
            buf = FPSTR("{\"removeAllFromArray\":");
            buf += value.c_str();
            buf += '}';
        }
        const char *c_str() { return buf.c_str(); }
    };
    struct SetToServerValue
    {
    private:
        String buf;

    public:
        SetToServerValue(ServerValue value)
        {
            buf = FPSTR("{\"setToServerValue\":");
            buf += value == SERVER_VALUE_UNSPECIFIED ? FPSTR("\"SERVER_VALUE_UNSPECIFIED\"}") : FPSTR("\"REQUEST_TIME\"}");
        }
        const char *c_str() { return buf.c_str(); }
    };

    struct FieldTransform
    {
    private:
        String buf;
        template <typename T>
        void set(const String &fieldPath, T v)
        {
            buf = FPSTR("{\"fieldPath\":\"");
            buf += fieldPath;
            buf += FPSTR("\",");
            String str = v.c_str();
            buf += str.substring(1, str.length() - 1);
            buf += '}';
        }

    public:
        template <typename T>
        FieldTransform(const String &fieldPath, T v)
        {
            set(fieldPath, v);
        }
        FieldTransform(const String &fieldPath, AppendMissingElements<Values::ArrayValue> v)
        {
            set(fieldPath, v);
        }
        FieldTransform(const String &fieldPath, RemoveAllFromArray<Values::ArrayValue> v)
        {
            set(fieldPath, v);
        }
        const char *c_str() { return buf.c_str(); }
    };

};

typedef struct firebase_firestore_precondition_t
{
    friend class Firestore;

private:
    String buf;

    String getQuery(const String &mask, bool hasParam)
    {
        String tmp;
        if (buf.length())
        {
            tmp += hasParam ? '&' : '?';
            tmp += mask + buf;
        }
        return tmp;
    }

public:
    firebase_firestore_precondition_t() {}
    firebase_firestore_precondition_t(bool exists)
    {
        buf = FPSTR(".exists=");
        buf += exists ? FPSTR("true") : FPSTR("false");
    }

    firebase_firestore_precondition_t(const String &updateTime)
    {
        JsonHelper jh;
        buf = FPSTR(".updateTime=");
        buf += jh.toString(updateTime);
    }

} Precondition;

typedef struct firebase_firestore_document_t
{
    friend class Firestore;

private:
    String name;
    String createTime;
    String updateTime;
    object_t fields;

public:
    firebase_firestore_document_t() {}
    firebase_firestore_document_t(const object_t &fields, const String &name = "", const String &createTime = "", const String &updateTime = "")
    {
        this->name = name;
        this->fields = fields;
        this->createTime = createTime;
        this->updateTime = updateTime;
    }
    String toString()
    {
        String buf;
        JsonHelper jh;
        if (createTime.length())
            jh.addObject(buf, jh.toString("createTime"), jh.toString(createTime));
        if (createTime.length())
            jh.addObject(buf, jh.toString("updateTime"), jh.toString(updateTime));
        jh.addObject(buf, jh.toString("fields"), fields.c_str(), true);
        return buf;
    }

} Document;

class FirestoreOptions
{
    friend class Firestore;

public:
    ParentResource parent;
    String collectionId;
    String documentId;
    DocumentMask mask, updateMask;
    Precondition currentDocument;
    String payload;
    String exists;
    String updateTime;
    String readTime;
    String transaction;
    int pageSize = 10;
    String pageToken;
    String orderBy;
    bool showMissing = false;
    size_t size = 0;
    firebase_firestore_request_type requestType = firebase_firestore_request_type_undefined;
    unsigned long requestTime = 0;

    void copy(FirestoreOptions &rhs)
    {
        this->parent = rhs.parent;
        this->collectionId = rhs.collectionId;
        this->documentId = rhs.documentId;
        this->mask = rhs.mask;
        this->updateMask = rhs.updateMask;
        this->payload = rhs.payload;
        this->exists = rhs.exists;
        this->updateTime = rhs.updateTime;
        this->readTime = rhs.readTime;
        this->transaction = rhs.transaction;
        this->pageSize = rhs.pageSize;
        this->pageToken = rhs.pageToken;
        this->orderBy = rhs.orderBy;
        this->showMissing = rhs.showMissing;
        this->size = rhs.size;
        this->requestType = rhs.requestType;
        this->requestTime = rhs.requestTime;
    }

private:
};

#endif

#endif