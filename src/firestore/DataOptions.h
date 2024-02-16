/**
 * Created February 16, 2024
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

#define FIRESTORE_RESOURCE_PATH_BASE FPSTR("<resource_path>")

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
    void addMember(String &buf, const String &v, bool isObjValue, const String &token = "}}")
    {
        int p = buf.lastIndexOf(token);
        String str = buf.substring(0, p);
        str += ',';
        if (isObjValue)
        {
            String tmp = v;
            str += tmp.substring(1, tmp.length() - 1);
        }
        else
            str += v;
        str += token;
        buf = str;
    }
    const char *setPair(String &buf, const String &key, const String &value, bool isArrayValue = false)
    {
        buf.remove(0, buf.length());
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

    String getDocPath(const String &document)
    {
        String doc_path = FIRESTORE_RESOURCE_PATH_BASE;
        if (document.length())
        {
            if (document.length() && document[0] != '/')
                doc_path += '/';
            doc_path += document;
        }
        return doc_path;
    }
};

class ParentResource
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
    ParentResource() {}
    ParentResource(const String &projectId, const String &databaseId = "")
    {
        this->projectId = projectId;
        this->databaseId = databaseId;
    }
};

class DocumentMask : public Printable
{
    friend class Firestore;
    friend class GetDocumentOptions;
    friend class patchDocumentOptions;

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
    /**
     * A set of field paths on a document.
     * Used to restrict a get or update operation on a document to a subset of its fields.
     * This is different from standard field masks, as this is always scoped to a Document,
     * and takes in account the dynamic nature of Value.
     *
     * @param mask The list of field paths in the mask. See Document.fields for a field path syntax reference.
     */
    DocumentMask(const String &fieldPaths = "")
    {
        setFieldPaths(fieldPaths);
    }
    void setFieldPaths(const String &fieldPaths)
    {
        if (fieldPaths.length())
        {
            this->mask = fieldPaths;
            JsonHelper jh;
            jh.addTokens(str, "fieldPaths", mask, true);
        }
    }
    const char *c_str() { return str.c_str(); }
    size_t printTo(Print &p) const { return p.print(str.c_str()); }
};

/**
 * A message that can hold any of the supported value types.
 */
namespace Values
{

    class NullValue : public Printable
    {
    private:
        String buf, str;
        FSUT fsut;

    public:
        /**
         * A null value.
         */
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
        /**
         * A string value.
         *  @param value The string vakue
         */
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
        /**
         * A boolean value.
         *  @param value The boolean value
         */
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
        /**
         * A integer value.
         *  @param value The integer value
         */
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
        /**
         * A double value.
         *  @param value The double value
         */
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
        /**
         * A timestamp value.
         * Precise only to microseconds. When stored, any additional precision is rounded down.
         * @param value The timestamp value string
         */
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
        /**
         * A bytes value.
         * Must not exceed 1 MiB - 89 bytes. Only the first 1,500 bytes are considered by queries.
         * A base64-encoded string.
         * @param value The bytes value string
         */
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
        /**
         * A reference to a document.
         * @param value The resource name of document
         */
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
        /**
         * A geo point value representing a point on the surface of Earth.
         * @param lat The latitude
         * @param lng The longitude
         */
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
        /**
         * An array value.
         * Cannot directly contain another array value, though can contain an map which contains another array.
         * @param value The object except for array value
         */
        template <typename T>
        ArrayValue(T value)
        {
            memset(flags, 0, 11);
            set(value);
        }
        /**
         * Add object to array value
         * @param value The resource name of document
         */
        template <typename T>
        ArrayValue &add(T value)
        {
            if (!isExisted(value))
            {
                if (buf.length() == 0)
                    set(value);
                else
                    fsut.addMember(buf, value.val(), true, "]}");
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

    /**
     * A map value.
     */
    class MapValue : public Printable
    {

    private:
        String buf, str;
        FSUT fsut;
        template <typename T>
        void set(const String &key, T value) { fsut.setPair(buf, FPSTR("fields"), MAP(key, value, true).c_str()); }

    public:
        MapValue() {}
        /**
         * A map value.
         * @param value The map value
         */
        template <typename T>
        MapValue(const String &key, T value) { set(key, value); }
        template <typename T>
        MapValue &add(const String &key, T value)
        {
            if (buf.length() == 0)
                set(key, value);
            else
                fsut.addMember(buf, MAP(key, value, true).c_str(), true);
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return fsut.setPair(str, firestore_const_key[firestore_const_key_mapValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };
    /**
     * A message that can hold any of the supported value types.
     */
    class Value : public Printable
    {
    private:
        String buf;

    public:
        Value() {}
        /**
         * @param value The object value
         */
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
        // REQUEST_TIME The time at which the server processed the request, with millisecond precision.
        //  If used on multiple fields (same or different documents) in a transaction, all the fields
        // will get the same server timestamp.
        REQUEST_TIME
    };

    struct Increment
    {
    private:
        String buf;
        FSUT fsut;

    public:
        /**
         * Increment object to use with FieldTransform object class constructor.
         * @param value Adds the given value to the field's current value.
         */
        template <typename T>
        Increment(T value) { fsut.setPair(buf, FPSTR("increment"), value.val()); }
        const char *c_str() { return buf.c_str(); }
    };

    struct Maximum
    {
    private:
        String buf;
        FSUT fsut;

    public:
        /**
         * Maximum object to use with FieldTransform object class constructor.
         * @param value Sets the field to the maximum of its current value and the given value.
         */
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
        /**
         * Mainimum object to use with FieldTransform object class constructor.
         * @param value Sets the field to the minimum of its current value and the given value.
         */
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
        /**
         * Append the given elements in order if they are not already present in the current field value.
         * If the field is not an array, or if the field does not yet exist, it is first set to the empty array.
         * @param arrayValue The array value object to append.
         */
        AppendMissingElements(T arrayValue) { fsut.setPair(buf, FPSTR("appendMissingElements"), arrayValue.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };

    template <typename T = Values::ArrayValue>
    struct RemoveAllFromArray
    {
    private:
        String buf;
        FSUT fsut;

    public:
        /**
         * Remove all of the given elements from the array in the field.
         * If the field is not an array, or if the field does not yet exist, it is set to the empty array.
         * @param arrayValue The array value object to remove.
         */
        RemoveAllFromArray(T arrayValue) { fsut.setPair(buf, FPSTR("removeAllFromArray"), arrayValue.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };
    /**
     *  Sets the field to the given server value.
     */
    struct SetToServerValue
    {
    private:
        String buf;
        FSUT fsut;
        JsonHelper jh;

    public:
        /**
         * Sets the field to the given server value.
         * @param enumValue The ServerValue enum
         *
         */
        SetToServerValue(ServerValue enumValue) { fsut.setPair(buf, FPSTR("setToServerValue"), jh.toString(enumValue == SERVER_VALUE_UNSPECIFIED ? FPSTR("SERVER_VALUE_UNSPECIFIED") : FPSTR("REQUEST_TIME"))); }
        const char *c_str() { return buf.c_str(); }
    };
    /**
     * A transformation of a field of the document.
     */
    struct FieldTransform
    {
    private:
        String buf;
        JsonHelper jh;
        FSUT fsut;
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
        /**
         * @param fieldPath The path of the field.
         * @param object The Increment, Maximum and Minimum objects.
         */
        template <typename T>
        FieldTransform(const String &fieldPath, T object) { set(fieldPath, object); }
        /**
         * @param fieldPath The path of the field.
         * @param arrayValue Append the given elements in order if they are not already present in the current field value.
         * If the field is not an array, or if the field does not yet exist, it is first set to the empty array.
         */
        FieldTransform(const String &fieldPath, AppendMissingElements<Values::ArrayValue> arrayvalue) { set(fieldPath, arrayvalue); }
        /**
         * @param fieldPath The path of the field.
         * @param arrayValue Remove all of the given elements from the array in the field.
         * If the field is not an array, or if the field does not yet exist, it is set to the empty array.
         */
        FieldTransform(const String &fieldPath, RemoveAllFromArray<Values::ArrayValue> arrayvalue) { set(fieldPath, arrayvalue); }
        const char *c_str() { return buf.c_str(); }
    };

};
/**
 * A precondition on a document, used for conditional operations.
 */
class Precondition : public Printable
{
    friend class Firestore;

private:
    String buf, str;
    FSUT fsut;
    JsonHelper jh;
    String estr;
    String eut;

    String getQuery(const String &mask)
    {
        buf.remove(0, buf.length());
        if (estr.length())
        {
            buf = FPSTR("?");
            buf = mask;
            buf += FPSTR(".exists=");
            buf += estr;
        }

        if (eut.length())
        {
            if (buf.length())
                buf += '&';
            else
                buf = FPSTR("?");
            buf = mask;
            buf += FPSTR(".updateTime=");
            buf += jh.toString(eut);
        }
        return buf;
    }

    void setObject()
    {
        if (estr.length())
        {
            if (str.length() == 0)
                fsut.setPair(str, FPSTR("exists"), estr);
            else
            {
                str[str.length() - 1] = '\0';
                jh.addObject(str, FPSTR("exists"), estr, true);
            }
        }

        if (eut.length())
        {
            if (str.length() == 0)
                fsut.setPair(str, FPSTR("updateTime"), eut);
            else
            {
                str[str.length() - 1] = '\0';
                jh.addObject(str, FPSTR("updateTime"), eut, true);
            }
        }
    }

public:
    Precondition() {}
    /**
     * Set the exists condition.
     * @param value When set to true, the target document must exist.
     * When set to false, the target document must not exist.
     */
    Precondition &exists(bool value)
    {
        this->estr = fsut.getBoolStr(value);
        setObject();
        return *this;
    }
    /**
     * Set the update time condition.
     * @param updateTime A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     * When set, the target document must exist and have been last updated at that time. Timestamp must be microsecond aligned.
     */
    Precondition &updateTime(const String &timestamp)
    {
        this->eut = timestamp;
        setObject();
        return *this;
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
    String buf, map_obj, name_obj, name;
    FSUT fsut;
    JsonHelper jh;

    void getBuf()
    {
        map_obj = mv.c_str();
        name_obj.remove(0, name_obj.length());
        if (name.length())
            jh.addObject(name_obj, FPSTR("name"), jh.toString(fsut.getDocPath(name)), true);
        else
        {
            buf = map_obj;
            return;
        }

        buf = name_obj;
        if (map_obj.length())
            fsut.addMember(buf, map_obj, true, "}");
    }

public:
    /**
     * A Firestore document constructor with document resource name.
     * @param name The resource name of the document.
     */
    Document(const String &name = "")
    {
        this->name = name;
        getBuf();
    }
    /**
     * A Firestore document constructor with object.
     * @param key The key of an object.
     * @param value The value of an object.
     */
    Document(const String &key, T value)
    {
        mv.add(key, value);
        getBuf();
    }
    /**
     * Add the object to Firestore document.
     * @param key The key of an object.
     * @param value The value of an object.
     */
    Document &add(const String &key, T value)
    {
        mv.add(key, value);
        getBuf();
        return *this;
    }
    /**
     * Set the document resource name.
     * @param name The resource name of the document.
     */
    void setName(const String &name)
    {
        this->name = name;
        getBuf();
    }
    const char *c_str()
    {
        getBuf();
        return buf.c_str();
    }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class DocumentTransform : public Printable
{
    friend class Firestore;
    friend class Write;

private:
    String buf;
    FSUT fsut;
    JsonHelper jh;

public:
    /**
     * A transformation of a document.
     * @param document The name of the document to transform.
     * @param fieldTransforms The list of transformations to apply to the fields of the document, in order. This must not be empty.
     */
    DocumentTransform(const String &document, FieldTransform::FieldTransform fieldTransforms)
    {
        jh.addObject(buf, FPSTR("document"), jh.toString(fsut.getDocPath(document)));
        jh.addObject(buf, FPSTR("fieldTransforms"), fieldTransforms.c_str(), true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class Write : public Printable
{
    friend class Firestore;
    friend class Writes;

private:
    enum firestore_write_type
    {
        firestore_write_type_undefined,
        firestore_write_type_update,
        firestore_write_type_delete,
        firestore_write_type_transform
    };
    String buf;
    JsonHelper jh;
    FSUT fsut;
    firestore_write_type write_type = firestore_write_type_undefined;
    bool updateTrans = false;

public:
    /**
     * A write on a document.
     * @param updateMask The fields to update in this write. This field can be set only when the operation is update.
     * If the mask is not set for an update and the document exists, any existing data will be overwritten.
     * If the mask is set and the document on the server has fields not covered by the mask, they are left unchanged.
     * Fields referenced in the mask, but not present in the input document, are deleted from the document on the server. The field paths in this mask must not contain a reserved field name.
     * @param update A document to write.
     * @param currentDocument An optional precondition on the document. The write will fail if this is set and not met by the target document.
     */
    Write(DocumentMask updateMask, Document<Values::Value> update, Precondition currentDocument)
    {
        bool curdoc = strlen(currentDocument.c_str());
        bool updatemask = strlen(updateMask.c_str());
        write_type = firestore_write_type_update;
        jh.addObject(buf, FPSTR("update"), update.c_str(), !updatemask && !curdoc);
        if (updatemask)
            jh.addObject(buf, FPSTR("updateMask"), updateMask.c_str(), !curdoc);
        if (curdoc)
            jh.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str(), true);
    }
    /**
     * A write on a document.
     * @param deletePath A document name to delete.
     */
    Write(const String &deletePath, Precondition currentDocument)
    {
        write_type = firestore_write_type_delete;
        if (strlen(currentDocument.c_str()))
            jh.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str());
        jh.addObject(buf, FPSTR("delete"), jh.toString(fsut.getDocPath(deletePath)), true);
    }

    /**
     * A write on a document.
     * @param transform Applies a transformation to a document.
     * @param currentDocument An optional precondition on the document. The write will fail if this is set and not met by the target document.
     */
    Write(DocumentTransform transform, Precondition currentDocument)
    {
        write_type = firestore_write_type_transform;
        if (strlen(currentDocument.c_str()))
            jh.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str());
        jh.addObject(buf, FPSTR("transform"), transform.c_str(), true);
    }
    /**
     * Add the update transform.
     * @param updateTransforms The transforms to perform after update.
     * This field can be set only when the operation is update.
     * If present, this write is equivalent to performing update and transform to
     * the same document atomically and in order.
     */
    Write &addUpdateTransform(FieldTransform::FieldTransform updateTransforms)
    {
        if (write_type == firestore_write_type_update)
        {
            if (!updateTrans)
            {
                buf[buf.length() - 1] = '\0';
                jh.addObject(buf, FPSTR("updateTransforms"), fsut.getArrayStr(updateTransforms.c_str()), true);
            }
            else
                fsut.addMember(buf, updateTransforms.c_str(), true, "]}");
            updateTrans = true;
        }

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
    /**
     * The writes to apply.
     * @param write A write on a document.
     * @param transaction A base64-encoded string. If set, applies all writes in this transaction, and commits it.
     *
     */
    Writes(Write write, const String &transaction = "")
    {
        if (transaction.length())
            jh.addObject(buf, FPSTR("transaction"), jh.toString(transaction));
        jh.addObject(buf, FPSTR("writes"), fsut.getArrayStr(write.c_str()), true);
    }

    /**
     * The writes to apply.
     * @param write A write on a document.
     * @param labels Labels associated with this batch write.
     * An object containing a list of "key": value pairs.
     */
    Writes(Write write, Values::MapValue labels)
    {
        if (strlen(labels.c_str()))
            jh.addObject(buf, FPSTR("labels"), labels.c_str());
        jh.addObject(buf, FPSTR("writes"), fsut.getArrayStr(write.c_str()), true);
    }

    /**
     * Add the write.
     * @param write A write on a document.
     */
    Writes &add(Write write)
    {
        FSUT fsut;
        fsut.addMember(buf, write.c_str(), true, "]}");
        return *this;
    }

    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

/**
 * Options for a transaction that can be used to read and write documents.
 */
class ReadWrite : public Printable
{
private:
    String buf;
    JsonHelper jh;

public:
    /**
     * @param retryTransaction An optional transaction to retry.
     * A base64-encoded string.
     */
    ReadWrite(const String &retryTransaction)
    {
        if (retryTransaction.length())
            jh.addObject(buf, "retryTransaction", jh.toString(retryTransaction), true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

/**
 * Options for a transaction that can only be used to read documents.
 */
class ReadOnly : public Printable
{
private:
    String buf;
    JsonHelper jh;

public:
    ReadOnly() {}
    /**
     * @param readTime Timestamp. Reads documents at the given time.
     */
    ReadOnly(const String &readTime)
    {
        if (readTime.length())
            jh.addObject(buf, "readTime", jh.toString(readTime), true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

/**
 * Options for creating a new transaction.
 */
class TransactionOptions : public Printable
{
private:
    String buf;
    JsonHelper jh;

public:
    TransactionOptions() {}
    /**
     * @param readOnly The transaction can only be used for read operations.
     */
    TransactionOptions(ReadOnly readOnly)
    {
        if (strlen(readOnly.c_str()))
            jh.addObject(buf, "readOnly", readOnly.c_str(), true);
    }
    /**
     * @param readWrite The transaction can be used for both read and write operations.
     */
    TransactionOptions(ReadWrite readWrite)
    {
        if (strlen(readWrite.c_str()))
            jh.addObject(buf, "readWrite", readWrite.c_str(), true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class EximDocumentOptions : public Printable
{
private:
    String buf;
    URLHelper uh;
    JsonHelper json;

public:
    EximDocumentOptions(const String &collectionIds, const String &bucketID, const String &storagePath)
    {
        String uriPrefix;
        uh.addGStorageURL(uriPrefix, bucketID, storagePath);
        json.addObject(buf, json.toString("inputUriPrefix"), json.toString(uriPrefix));
        json.addTokens(buf, json.toString("collectionIds"), collectionIds, true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class GetDocumentOptions : public Printable
{
private:
    String buf;
    URLHelper uh;

public:
    GetDocumentOptions() {}
    GetDocumentOptions(DocumentMask mask, const String &transaction = "", const String &readTime = "")
    {
        bool hasParam = false;
        if (strlen(mask.c_str()))
            buf = mask.getQuery("mask", hasParam);
        if (transaction.length())
            uh.addParam(buf, FPSTR("transaction"), transaction, hasParam);
        if (readTime.length())
            uh.addParam(buf, FPSTR("readTime"), readTime, hasParam);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class BatchGetDocumentOptions : public Printable
{
private:
    String buf, doc, msk, trans, newtrans, rt;
    JsonHelper jh;
    FSUT fsut;

    void setbuf()
    {
        buf.remove(0, buf.length());
        if (doc.length())
        {
            if (buf.length() == 0)
                buf = doc.c_str();
            else
                fsut.addMember(buf, doc, true, "}");
        }

        if (msk.length())
        {
            if (buf.length() == 0)
                buf = msk.c_str();
            else
                fsut.addMember(buf, msk, true, "}");
        }

        if (trans.length())
        {
            if (buf.length() == 0)
                buf = trans.c_str();
            else
                fsut.addMember(buf, trans, true, "}");
        }

        if (newtrans.length())
        {
            if (buf.length() == 0)
                buf = newtrans.c_str();
            else
                fsut.addMember(buf, newtrans, true, "}");
        }

        if (rt.length())
        {
            if (buf.length() == 0)
                buf = rt.c_str();
            else
                fsut.addMember(buf, rt, true, "}");
        }
    }

public:
    BatchGetDocumentOptions() {}
    /**
     * @param document The names of the documents to retrieve.
     */
    void addDocument(const String &document)
    {
        if (doc.length() == 0)
        {
            String str;
            jh.addArray(str, jh.toString(fsut.getDocPath(document)), true);
            jh.addObject(doc, "documents", str, true);
        }
        else
            fsut.addMember(doc, jh.toString(fsut.getDocPath(document)), false, "]}");
        setbuf();
    }
    /**
     * @param mask The fields to return. If not set, returns all fields.
     */
    void mask(DocumentMask mask)
    {
        jh.addObject(msk, "mask", mask.c_str(), true);
        setbuf();
    }
    /**
     * @param transaction Timestamp Reads documents in a transaction.
     */
    void transaction(const String &transaction)
    {
        if (transaction.length() && newtrans.length() == 0 && rt.length() == 0)
        {
            jh.addObject(trans, "transaction", jh.toString(transaction), true);
            setbuf();
        }
    }
    /**
     * @param transOptions Starts a new transaction and reads the documents. Defaults to a read-only transaction.
     * The new transaction ID will be returned as the first response in the stream.
     */
    void newTransaction(TransactionOptions transOptions)
    {
        if (strlen(transOptions.c_str()) && trans.length() == 0 && rt.length() == 0)
        {
            jh.addObject(newtrans, "newTransaction", transOptions.c_str(), true);
            setbuf();
        }
    }
    /**
     * @param readTime Timestamp. Reads documents as they were at the given time.
     */
    void readTime(const String &readTime)
    {
        if (readTime.length() && trans.length() == 0 && newtrans.length() == 0)
        {
            jh.addObject(rt, "readTime", jh.toString(readTime), true);
            setbuf();
        }
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class patchDocumentOptions : public Printable
{
private:
    String buf;
    URLHelper uh;

public:
    patchDocumentOptions(DocumentMask updateMask, DocumentMask mask, Precondition currentDocument)
    {
        bool hasParam = false;
        if (strlen(updateMask.c_str()))
            buf = updateMask.getQuery("updateMask", hasParam);
        if (strlen(mask.c_str()))
            buf += mask.getQuery("mask", hasParam);
        if (strlen(currentDocument.c_str()))
            uh.addParam(buf, FPSTR("currentDocument"), currentDocument.c_str(), hasParam);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class ListDocumentsOptions : public Printable
{
private:
    String buf;
    int pz = 0;
    String ptk;
    String ordby;
    DocumentMask msk;
    String sms;
    FSUT fsut;

public:
    ListDocumentsOptions() {}
    ListDocumentsOptions &pageSize(int pageSize)
    {
        pz = pageSize;
        return *this;
    }
    ListDocumentsOptions &pageToken(const String &pageToken)
    {
        ptk = pageToken;
        return *this;
    }
    ListDocumentsOptions &orderBy(const String orderBy)
    {
        ordby = orderBy;
        return *this;
    }
    ListDocumentsOptions &mask(DocumentMask mask)
    {
        msk = mask;
        return *this;
    }
    ListDocumentsOptions &showMissing(bool value)
    {
        sms = fsut.getBoolStr(value);
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
    String extras;
    String payload;
    firebase_firestore_request_type requestType = firebase_firestore_request_type_undefined;
    unsigned long requestTime = 0;

    void copy(FirestoreOptions &rhs)
    {
        this->parent = rhs.parent;
        this->collectionId = rhs.collectionId;
        this->documentId = rhs.documentId;
        this->extras = rhs.extras;
        this->payload = rhs.payload;
    }

private:
};

#endif

#endif