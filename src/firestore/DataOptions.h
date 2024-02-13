/**
 * Created February 13, 2024
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


enum firestore_const_key_type
{
    firestore_const_key_nullValue,
    firestore_const_key_booleanValue,
    firestore_const_key_integerValue,
    firestore_const_key_doubleValue,
    firestore_const_key_timestampValue,
    firestore_const_key_stringValue,
    firestore_const_key_bytesValue,
    firestore_const_key_referenceValue,
    firestore_const_key_geoPointValue,
    firestore_const_key_arrayValue,
    firestore_const_key_mapValue,
    firestore_const_key_maxType
};

struct firebase_firestore_const_key_t
{
    char text[15];
};

const struct firebase_firestore_const_key_t firestore_const_key[firestore_const_key_maxType] PROGMEM = {
    "nullValue",
    "booleanValue",
    "integerValue",
    "doubleValue",
    "timestampValue",
    "stringValue",
    "bytesValue",
    "referenceValue",
    "geoPointValue",
    "arrayValue",
    "mapValue"};

class FSUT
{
private:
    JsonHelper jh;

public:
    void addArray(String &buf, const String &v)
    {
        int p = buf.lastIndexOf("]}");
        String str = buf.substring(0, p);
        str += ',';
        str += v;
        str += FPSTR("]}");
        buf = str;
    }
    void addObject(String &buf, const String &v)
    {
        int p = buf.lastIndexOf("}}");
        String str = buf.substring(0, p);
        str += ',';
        String tmp = v;
        str += tmp.substring(1, tmp.length() - 1);
        str += FPSTR("}}");
        buf = str;
    }
    const char *setPair(String &buf, const String &key, const String &value, bool isArrayValue = false)
    {
        buf.remove(0,buf.length());
        jh.addObject(buf, key, isArrayValue ? getArrayStr(value) : value, true);
        return buf.c_str();
    }
    void setBool(String &buf, bool value) { buf = getBoolStr(value); }

    String getBoolStr(bool value) { return value ? FPSTR("true") : FPSTR("false"); }

    String getArrayStr(const String &value)
    {
        String str = FPSTR("[");
        str += value;
        str += ']';
        return str;
    }

    void setString(String &buf, const String &value)
    {
        buf = FPSTR("\"");
        buf += value;
        buf += '"';
    }
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
    firebase_firestore_parent_resource_t(const String &projectId, const String &databaseId = "")
    {
        this->projectId = projectId;
        this->databaseId = databaseId;
    }

} ParentResource;

class DocumentMask : public Printable
{
    friend class Firestore;

private:
    String mask;
    String str;

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
    DocumentMask() {}
    DocumentMask(const String &mask)
    {
        this->mask = mask;
        JsonHelper jh;
        jh.addTokens(str, "fieldPaths", mask, true);
    }
    const char *c_str() { return str.c_str(); }
    size_t printTo(Print &p) const { return p.print(str.c_str()); }
};

namespace Values
{

    class NullValue : public Printable
    {
    private:
        String buf, str;
        FSUT fsut;

    public:
        NullValue() { buf = FPSTR("null"); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_nullValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class StringValue : public Printable
    {

    private:
        String buf, str;
        FSUT fsut;

    public:
        StringValue(const String &value) { fsut.setString(buf, value); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_stringValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class BooleanValue : public Printable
    {
    private:
        String buf, str;
        FSUT fsut;

    public:
        BooleanValue(bool value) { fsut.setBool(buf, value); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_booleanValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class IntegerValue : public Printable
    {

    private:
        String buf, str;
        FSUT fsut;

    public:
        IntegerValue(int value) { buf = StringValue(String(value)).c_str(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_integerValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class DoubleValue : public Printable
    {

    private:
        String buf, str;
        FSUT fsut;

    public:
        DoubleValue(double value) { buf = String(value); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_doubleValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class TimestampValue : public Printable
    {

    private:
        String buf, str;
        FSUT fsut;

    public:
        TimestampValue(const String &value) { buf = StringValue(value).c_str(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_timestampValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class BytesValue : public Printable
    {
    private:
        String buf, str;
        FSUT fsut;

    public:
        BytesValue(const String &value) { buf = StringValue(value).c_str(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_bytesValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class ReferenceValue : public Printable
    {

    private:
        String buf, str;
        FSUT fsut;

    public:
        ReferenceValue(const String &value) { buf = StringValue(value).c_str(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_referenceValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class GeoPointValue : public Printable
    {
    private:
        String buf, str;
        FSUT fsut;
        JsonHelper jh;

    public:
        GeoPointValue(double lat, double lng)
        {
            jh.addObject(buf, FPSTR("latitude"), String(lat));
            jh.addObject(buf, FPSTR("longitude"), String(lng), true);
        }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_geoPointValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class ArrayValue : public Printable
    {

    private:
        String buf, str;
        FSUT fsut;
        bool flags[11];

        template <typename T>
        bool isExisted(T value)
        {
            String tmp = value.val();
             for (size_t i = 0; i <= firestore_const_key_mapValue; i++)
            {
                if (tmp.indexOf(firestore_const_key[i].text) > -1)
                {
                    if (flags[i])
                        return true;
                    flags[i] = 1;
                }
            }

            return false;
        }
        template <typename T>
        void set(T value)
        {
            if (isExisted(value))
                return;
            fsut.setPair(buf, FPSTR("values"), value.val(), true);
        }

    public:
        template <typename T>
        ArrayValue(T value)
        {
            memset(flags, 0, 11);
            set(value);
        }
        template <typename T>
        ArrayValue &add(T value)
        {
            if (!isExisted(value))
            {
                if (buf.length() == 0)
                    set(value);
                else
                    fsut.addArray(buf, value.val());
            }
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_arrayValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    struct MAP
    {
    private:
        String buf;
        FSUT fsut;

    public:
        template <typename T>
        MAP(const String &key, T value, bool val) { fsut.setPair(buf, key, val ? value.val() : value.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };

    class MapValue : public Printable
    {

    private:
        String buf, str;
        FSUT fsut;
        template <typename T>
        void set(const String &key, T value) { fsut.setPair(buf, FPSTR("fields"), MAP(key, value, true).c_str()); }

    public:
        MapValue() {}
        template <typename T>
        MapValue(const String &key, T value) { set(key, value); }
        template <typename T>
        MapValue &add(const String &key, T value)
        {
            if (buf.length() == 0)
                set(key, value);
            else
                fsut.addObject(buf, MAP(key, value, true).c_str());
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_mapValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class Value : public Printable
    {
    private:
        String buf;

    public:
        Value() {}
        template <typename T>
        Value(T value) { buf = value.val(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
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
        FSUT fsut;

    public:
        template <typename T>
        Increment(T value) { fsut.setPair(buf, FPSTR("increment"), value.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };

    struct Maximum
    {
    private:
        String buf;
        FSUT fsut;

    public:
        template <typename T>
        Maximum(T value) { fsut.setPair(buf, FPSTR("maximum"), value.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };

    struct Minimum
    {
    private:
        String buf;
        FSUT fsut;

    public:
        template <typename T>
        Minimum(T value) { fsut.setPair(buf, FPSTR("minimum"), value.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };

    template <typename T = Values::ArrayValue>
    struct AppendMissingElements
    {
    private:
        String buf;
        FSUT fsut;

    public:
        AppendMissingElements(T value) { fsut.setPair(buf, FPSTR("appendMissingElements"), value.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };

    template <typename T = Values::ArrayValue>
    struct RemoveAllFromArray
    {
    private:
        String buf;
        FSUT fsut;

    public:
        RemoveAllFromArray(T value) { fsut.setPair(buf, FPSTR("removeAllFromArray"), value.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };
    struct SetToServerValue
    {
    private:
        String buf;
        FSUT fsut;
        JsonHelper jh;

    public:
        SetToServerValue(ServerValue value) { fsut.setPair(buf, FPSTR("setToServerValue"), jh.toString(value == SERVER_VALUE_UNSPECIFIED ? FPSTR("SERVER_VALUE_UNSPECIFIED") : FPSTR("REQUEST_TIME"))); }
        const char *c_str() { return buf.c_str(); }
    };

    struct FieldTransform
    {
    private:
        String buf;
        JsonHelper jh;
        template <typename T>
        void set(const String &fieldPath, T v)
        {
            jh.addObject(buf, FPSTR("fieldPath"), jh.toString(fieldPath));
            buf += ',';
            String str = v.c_str();
            buf += str.substring(1, str.length() - 1);
            buf += '}';
        }

    public:
        template <typename T>
        FieldTransform(const String &fieldPath, T v) { set(fieldPath, v); }
        FieldTransform(const String &fieldPath, AppendMissingElements<Values::ArrayValue> v) { set(fieldPath, v); }
        FieldTransform(const String &fieldPath, RemoveAllFromArray<Values::ArrayValue> v) { set(fieldPath, v); }
        const char *c_str() { return buf.c_str(); }
    };

};

class Precondition : public Printable
{
    friend class Firestore;

private:
    String buf, str;
    FSUT fsut;
    JsonHelper jh;

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
    Precondition() {}
    Precondition(bool exists)
    {
        buf = FPSTR(".exists=");
        buf += fsut.getBoolStr(exists);
        fsut.setPair(str, FPSTR("exists"), fsut.getBoolStr(exists));
    }

    Precondition(const String &updateTime)
    {
        buf = FPSTR(".updateTime=");
        buf += jh.toString(updateTime);
        fsut.setPair(str, FPSTR("updateTime"), jh.toString(updateTime));
    }
    const char *c_str() { return str.c_str(); }
    size_t printTo(Print &p) const { return p.print(str.c_str()); }
};

template <typename T = Values::Value>
class Document : public Printable
{
    friend class Firestore;

private:
    Values::MapValue mv;
    String buf;

public:
    Document() {}
    Document(const String &key, T value)
    {
        mv.add(key, value);
        buf = mv.c_str();
    }
    Document &add(const String &key, T value)
    {
        mv.add(key, value);
        buf = mv.c_str();
        return *this;
    }
    const char *c_str() { return mv.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class DocumentTransform : public Printable
{
private:
    String buf;
    JsonHelper jh;

public:
    DocumentTransform(const String &document, FieldTransform::FieldTransform fieldTransforms)
    {
        jh.addObject(buf, FPSTR("document"), jh.toString(document));
        jh.addObject(buf, FPSTR("fieldTransforms"), fieldTransforms.c_str(), true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class Write : public Printable
{
private:
    String buf;
    JsonHelper jh;
    FSUT fsut;

    void set(DocumentMask &updateMask, FieldTransform::FieldTransform &updateTransforms, Precondition &currentDocument)
    {
        jh.addObject(buf, FPSTR("updateMask"), updateMask.c_str());
        jh.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str());
        jh.addObject(buf, FPSTR("updateTransforms"), fsut.getArrayStr(currentDocument.c_str()), true);
    }

public:
    Write(DocumentMask updateMask, FieldTransform::FieldTransform updateTransforms, Precondition currentDocument, Document<Values::Value> update)
    {
        jh.addObject(buf, FPSTR("update"), update.c_str());
        set(updateMask, updateTransforms, currentDocument);
    }

    Write(DocumentMask updateMask, FieldTransform::FieldTransform updateTransforms, Precondition currentDocument, const String &Delete)
    {
        jh.addObject(buf, FPSTR("delete"), jh.toString(Delete));
        set(updateMask, updateTransforms, currentDocument);
    }

    Write(DocumentMask updateMask, FieldTransform::FieldTransform updateTransforms, Precondition currentDocument, DocumentTransform transform)
    {
        jh.addObject(buf, FPSTR("transform"), transform.c_str());
        set(updateMask, updateTransforms, currentDocument);
    }

    Write &add(FieldTransform::FieldTransform updateTransforms)
    {
        fsut.addArray(buf, updateTransforms.c_str());
        return *this;
    }

    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class Writes : public Printable
{
private:
    String buf;
    JsonHelper jh;
    FSUT fsut;

public:
    Writes(Write write, const String &transaction = "")
    {
        if (transaction.length())
            jh.addObject(buf, FPSTR("transaction"), jh.toString(transaction));
        jh.addObject(buf, FPSTR("writes"), fsut.getArrayStr(write.c_str()), true);
    }

    Writes &add(Write write)
    {
        FSUT fsut;
        fsut.addArray(buf, write.c_str());
        return *this;
    }

    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

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