/**
 * Created March 10, 2024
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

#include "./core/JSON.h"

#if defined(ENABLE_FIRESTORE)

#include "./firestore/ObjectWriter.h"
#include "./firestore/Values.h"
#if defined(ENABLE_FIRESTORE_QUERY)
#include "./firestore/Query.h"
using namespace FirestoreQuery;
#endif

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
    firebase_firestore_request_type_create_composite_index,
    firebase_firestore_request_type_create_field_index,

    firebase_firestore_request_type_get_doc = 300,
    firebase_firestore_request_type_list_doc,
    firebase_firestore_request_type_list_index,
    firebase_firestore_request_type_get_index,

    firebase_firestore_request_type_patch_doc = 400,

    firebase_firestore_request_type_delete_doc = 500,
    firebase_firestore_request_type_delete_index
};

class ParentResource
{
    friend class FirestoreBase;
    friend class FirestoreDocuments;

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
    friend class FirestoreBase;
    friend class GetDocumentOptions;
    friend class patchDocumentOptions;
    friend class ListDocumentsOptions;
    friend class FirestoreDocuments;

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
    void clear()
    {
        str.remove(0, str.length());
        mask.remove(0, mask.length());
    }
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
        ObjectWriter owriter;

    public:
        /**
         * Increment object to use with FieldTransform object class constructor.
         * @param value Adds the given value to the field's current value.
         */
        template <typename T>
        Increment(T value) { owriter.setPair(buf, FPSTR("increment"), value.val()); }
        const char *c_str() { return buf.c_str(); }
        void clear() { buf.remove(0, buf.length()); }
    };

    struct Maximum
    {
    private:
        String buf;
        ObjectWriter owriter;

    public:
        /**
         * Maximum object to use with FieldTransform object class constructor.
         * @param value Sets the field to the maximum of its current value and the given value.
         */
        template <typename T>
        Maximum(T value) { owriter.setPair(buf, FPSTR("maximum"), value.c_str()); }
        const char *c_str() { return buf.c_str(); }
        void clear() { buf.remove(0, buf.length()); }
    };

    struct Minimum
    {
    private:
        String buf;
        ObjectWriter owriter;

    public:
        /**
         * Mainimum object to use with FieldTransform object class constructor.
         * @param value Sets the field to the minimum of its current value and the given value.
         */
        template <typename T>
        Minimum(T value) { owriter.setPair(buf, FPSTR("minimum"), value.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };

    template <typename T = Values::ArrayValue>
    struct AppendMissingElements
    {
    private:
        String buf;
        ObjectWriter owriter;

    public:
        /**
         * Append the given elements in order if they are not already present in the current field value.
         * If the field is not an array, or if the field does not yet exist, it is first set to the empty array.
         * @param arrayValue The array value object to append.
         */
        AppendMissingElements(T arrayValue) { owriter.setPair(buf, FPSTR("appendMissingElements"), arrayValue.c_str()); }
        const char *c_str() { return buf.c_str(); }
        void clear() { buf.remove(0, buf.length()); }
    };

    template <typename T = Values::ArrayValue>
    struct RemoveAllFromArray
    {
    private:
        String buf;
        ObjectWriter owriter;

    public:
        /**
         * Remove all of the given elements from the array in the field.
         * If the field is not an array, or if the field does not yet exist, it is set to the empty array.
         * @param arrayValue The array value object to remove.
         */
        RemoveAllFromArray(T arrayValue) { owriter.setPair(buf, FPSTR("removeAllFromArray"), arrayValue.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };
    /**
     *  Sets the field to the given server value.
     */
    struct SetToServerValue
    {
    private:
        String buf;
        ObjectWriter owriter;
        JsonHelper jh;

    public:
        /**
         * Sets the field to the given server value.
         * @param enumValue The ServerValue enum
         *
         */
        SetToServerValue(ServerValue enumValue) { owriter.setPair(buf, FPSTR("setToServerValue"), jh.toString(enumValue == SERVER_VALUE_UNSPECIFIED ? FPSTR("SERVER_VALUE_UNSPECIFIED") : FPSTR("REQUEST_TIME"))); }
        const char *c_str() { return buf.c_str(); }
    };
    /**
     * A transformation of a field of the document.
     */
    struct FieldTransform : public Printable
    {
    private:
        String buf;
        JsonHelper jh;
        ObjectWriter owriter;
        template <typename T>
        void set(const String &fieldPath, T v)
        {
            jh.addObject(buf, FPSTR("fieldPath"), fieldPath, true);
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
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear() { buf.remove(0, buf.length()); }
    };

};
/**
 * A precondition on a document, used for conditional operations.
 */
class Precondition : public Printable
{
    friend class FirestoreBase;
    friend class FirestoreDocuments;

private:
    String buf, str;
    ObjectWriter owriter;
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
                owriter.setPair(str, FPSTR("exists"), estr);
            else
            {
                str[str.length() - 1] = '\0';
                jh.addObject(str, FPSTR("exists"), estr, true);
            }
        }

        if (eut.length())
        {
            if (str.length() == 0)
                owriter.setPair(str, FPSTR("updateTime"), eut);
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
        this->estr = owriter.getBoolStr(value);
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
    void clear()
    {
        buf.remove(0, buf.length());
        str.remove(0, str.length());
        eut.remove(0, eut.length());
        estr.remove(0, estr.length());
    }
};

template <typename T = Values::Value>
class Document : public Printable
{
    friend class FirestoreBase;

private:
    Values::MapValue mv;
    String buf, map_obj, name_obj, name;
    ObjectWriter owriter;
    JsonHelper jh;

    void getBuf()
    {
        map_obj = mv.c_str();
        name_obj.remove(0, name_obj.length());
        if (name.length())
            jh.addObject(name_obj, FPSTR("name"), owriter.getDocPath(name), true, true);
        else
        {
            buf = map_obj;
            return;
        }

        buf = name_obj;
        if (map_obj.length())
            owriter.addMember(buf, map_obj, "}");
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
    void clear()
    {
        buf.remove(0, buf.length());
        map_obj.remove(0, map_obj.length());
        name_obj.remove(0, name_obj.length());
        name.remove(0, name.length());
        mv.clear();
    }
};

class DocumentTransform : public Printable
{
    friend class FirestoreBase;
    friend class Write;

private:
    String buf;
    ObjectWriter owriter;
    JsonHelper jh;

public:
    /**
     * A transformation of a document.
     * @param document The name of the document to transform.
     * @param fieldTransforms The list of transformations to apply to the fields of the document, in order. This must not be empty.
     */
    DocumentTransform(const String &document, FieldTransform::FieldTransform fieldTransforms)
    {
        jh.addObject(buf, FPSTR("document"), owriter.getDocPath(document), true);
        jh.addObject(buf, FPSTR("fieldTransforms"), fieldTransforms.c_str(), false, true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
};

class Write : public Printable
{
    friend class FirestoreBase;
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
    ObjectWriter owriter;
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
        jh.addObject(buf, FPSTR("update"), update.c_str(), false, !updatemask && !curdoc);
        if (updatemask)
            jh.addObject(buf, FPSTR("updateMask"), updateMask.c_str(), false, !curdoc);
        if (curdoc)
            jh.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str(), false, true);
    }
    /**
     * A write on a document.
     * @param deletePath A document name to delete.
     */
    Write(const String &deletePath, Precondition currentDocument)
    {
        write_type = firestore_write_type_delete;
        if (strlen(currentDocument.c_str()))
            jh.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str(), false);
        jh.addObject(buf, FPSTR("delete"), owriter.getDocPath(deletePath), true, true);
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
            jh.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str(), false);
        jh.addObject(buf, FPSTR("transform"), transform.c_str(), false, true);
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
                jh.addObject(buf, FPSTR("updateTransforms"), owriter.getArrayStr(updateTransforms.c_str()), false, true);
            }
            else
                owriter.addMember(buf, updateTransforms.c_str(), "]}");
            updateTrans = true;
        }

        return *this;
    }

    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
};

class Writes : public Printable
{
private:
    String buf;
    JsonHelper jh;
    ObjectWriter owriter;

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
            jh.addObject(buf, FPSTR("transaction"), transaction, true);
        jh.addObject(buf, FPSTR("writes"), owriter.getArrayStr(write.c_str()), false, true);
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
            jh.addObject(buf, FPSTR("labels"), labels.c_str(), false);
        jh.addObject(buf, FPSTR("writes"), owriter.getArrayStr(write.c_str()), false, true);
    }

    /**
     * Add the write.
     * @param write A write on a document.
     */
    Writes &add(Write write)
    {
        ObjectWriter owriter;
        owriter.addMember(buf, write.c_str(), "]}");
        return *this;
    }

    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
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
            jh.addObject(buf, FPSTR("retryTransaction"), retryTransaction, true, true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
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
            jh.addObject(buf, "readTime", readTime, true, true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
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
            jh.addObject(buf, "readOnly", readOnly.c_str(), false, true);
    }
    /**
     * @param readWrite The transaction can be used for both read and write operations.
     */
    TransactionOptions(ReadWrite readWrite)
    {
        if (strlen(readWrite.c_str()))
            jh.addObject(buf, "readWrite", readWrite.c_str(), false, true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
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
        json.addObject(buf, "inputUriPrefix", uriPrefix, true);
        json.addTokens(buf, "collectionIds", collectionIds, true);
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
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
    ObjectWriter owriter;

    void setbuf()
    {
        buf.remove(0, buf.length());
        if (doc.length())
        {
            if (buf.length() == 0)
                buf = doc.c_str();
            else
                owriter.addMember(buf, doc, "}");
        }

        if (msk.length())
        {
            if (buf.length() == 0)
                buf = msk.c_str();
            else
                owriter.addMember(buf, msk, "}");
        }

        if (trans.length())
        {
            if (buf.length() == 0)
                buf = trans.c_str();
            else
                owriter.addMember(buf, trans, "}");
        }

        if (newtrans.length())
        {
            if (buf.length() == 0)
                buf = newtrans.c_str();
            else
                owriter.addMember(buf, newtrans, "}");
        }

        if (rt.length())
        {
            if (buf.length() == 0)
                buf = rt.c_str();
            else
                owriter.addMember(buf, rt, "}");
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
            jh.addArray(str, owriter.getDocPath(document), true, true);
            jh.addObject(doc, "documents", str, false, true);
        }
        else
            owriter.addMember(doc, owriter.getDocPath(document, true), "]}");
        setbuf();
    }
    /**
     * @param mask The fields to return. If not set, returns all fields.
     */
    void mask(DocumentMask mask)
    {
        jh.addObject(msk, "mask", mask.c_str(), false, true);
        setbuf();
    }
    /**
     * @param transaction Timestamp Reads documents in a transaction.
     */
    void transaction(const String &transaction)
    {
        if (transaction.length() && newtrans.length() == 0 && rt.length() == 0)
        {
            jh.addObject(trans, "transaction", transaction, true, true);
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
            jh.addObject(newtrans, "newTransaction", transOptions.c_str(), false, true);
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
            jh.addObject(rt, "readTime", readTime, true, true);
            setbuf();
        }
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear()
    {
        buf.remove(0, buf.length());
        doc.remove(0, doc.length());
        msk.remove(0, msk.length());
        trans.remove(0, trans.length());
        newtrans.remove(0, newtrans.length());
        rt.remove(0, rt.length());
    }
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
    void clear() { buf.remove(0, buf.length()); }
};

#if defined(ENABLE_FIRESTORE_QUERY)

class QueryOptions : public Printable
{
private:
    String buf, query, trans, newtrans, rt;
    JsonHelper jh;
    ObjectWriter owriter;

    void setbuf()
    {
        buf.remove(0, buf.length());

        if (query.length())
        {
            if (buf.length() == 0)
                buf = query.c_str();
            else
                owriter.addMember(buf, query, "}");
        }

        if (trans.length())
        {
            if (buf.length() == 0)
                buf = trans.c_str();
            else
                owriter.addMember(buf, trans, "}");
        }

        if (newtrans.length())
        {
            if (buf.length() == 0)
                buf = newtrans.c_str();
            else
                owriter.addMember(buf, newtrans, "}");
        }

        if (rt.length())
        {
            if (buf.length() == 0)
                buf = rt.c_str();
            else
                owriter.addMember(buf, rt, "}");
        }
    }

public:
    QueryOptions() {}

    /**
     * A structured query.
     * @param structuredQuery A structured query.
     */
    void structuredQuery(StructuredQuery structuredQuery)
    {
        jh.addObject(query, "structuredQuery", structuredQuery.c_str(), false, true);
        setbuf();
    }
    /**
     * @param transaction Run the query within an already active transaction.
     */
    void transaction(const String &transaction)
    {
        if (transaction.length() && newtrans.length() == 0 && rt.length() == 0)
        {
            jh.addObject(trans, "transaction", transaction, true, true);
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
            jh.addObject(newtrans, "newTransaction", transOptions.c_str(), false, true);
            setbuf();
        }
    }
    /**
     * @param readTime Timestamp. Reads documents as they were at the given time.
     * This must be a microsecond precision timestamp within the past one hour,
     * or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     */
    void readTime(const String &readTime)
    {
        if (readTime.length() && trans.length() == 0 && newtrans.length() == 0)
        {
            jh.addObject(rt, "readTime", readTime, true, true);
            setbuf();
        }
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear()
    {
        buf.remove(0, buf.length());
        query.remove(0, query.length());
        trans.remove(0, trans.length());
        newtrans.remove(0, newtrans.length());
        rt.remove(0, rt.length());
    }
};

#endif

class ListDocumentsOptions : public Printable
{
private:
    String buf;
    int pz = 0;
    String ptk, ordby, sms, tsc, rt;
    DocumentMask msk;
    ObjectWriter owriter;
    URLHelper uh;

    void set()
    {
        bool hasParam = false;
        buf.remove(0, buf.length());

        if (pz > 0)
            uh.addParam(buf, FPSTR("pageSize"), String(pz), hasParam);
        if (ptk.length() > 0)
            uh.addParam(buf, FPSTR("pageToken"), ptk, hasParam);
        if (ordby.length() > 0)
            uh.addParam(buf, FPSTR("orderBy"), ordby, hasParam);
        if (strlen(msk.c_str()) > 0)
            buf += msk.getQuery("mask", hasParam);
        if (sms.length() > 0)
            uh.addParam(buf, FPSTR("showMissing"), sms, hasParam);
        if (tsc.length() > 0)
            uh.addParam(buf, FPSTR("transaction"), tsc, hasParam);
        if (rt.length() > 0)
            uh.addParam(buf, FPSTR("readTime"), rt, hasParam);
    }

public:
    ListDocumentsOptions() {}
    /**
     * Optional. The maximum number of documents to return in a single response.
     * Firestore may return fewer than this value.
     * @param pageSize The maximum number of documents to return in a single response.
     */
    ListDocumentsOptions &pageSize(int pageSize)
    {
        pz = pageSize;
        set();
        return *this;
    }
    /**
     * Optional. A page token, received from a previous documents.list response.
     * @param pageToken Optional. A page token, received from a previous documents.list response.
     */
    ListDocumentsOptions &pageToken(const String &pageToken)
    {
        ptk = pageToken;
        set();
        return *this;
    }
    /**
     * Optional. The optional ordering of the documents to return.
     * For example: priority desc, __name__ desc.
     * @param orderBy
     */
    ListDocumentsOptions &orderBy(const String orderBy)
    {
        ordby = orderBy;
        set();
        return *this;
    }
    /**
     * Optional. The fields to return. If not set, returns all fields.
     * If a document has a field that is not present in this mask, that field will not be returned in the response.
     * @param mask The fields to return.
     */
    ListDocumentsOptions &mask(DocumentMask mask)
    {
        msk = mask;
        set();
        return *this;
    }
    /**
     * If the list should show missing documents.
     * A document is missing if it does not exist, but there are sub-documents nested underneath it. When true, such missing documents will be returned with a key but will not have fields, createTime, or updateTime set.
     * Requests with showMissing may not specify where or orderBy.
     * @param value
     */
    ListDocumentsOptions &showMissing(bool value)
    {
        sms = owriter.getBoolStr(value);
        set();
        return *this;
    }
    /**
     * Perform the read as part of an already active transaction.
     * A base64-encoded string.
     * @param transaction
     */
    ListDocumentsOptions &transaction(const String transaction)
    {
        tsc = transaction;
        set();
        return *this;
    }
    /**
     * Perform the read at the provided time.
     * This must be a microsecond precision timestamp within the past one hour,
     * or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     * @param readTime
     */
    ListDocumentsOptions &readTime(const String readTime)
    {
        rt = readTime;
        set();
        return *this;
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear()
    {
        buf.remove(0, buf.length());
        msk.clear();
        ptk.remove(0, ptk.length());
        ordby.remove(0, ordby.length());
        sms.remove(0, sms.length());
        tsc.remove(0, tsc.length());
        rt.remove(0, rt.length());
    }
};

class ListCollectionIdsOptions : public Printable
{
private:
    String buf;
    String pz, ptk, rt;
    ObjectWriter owriter;
    JsonHelper jh;

    void set()
    {
        buf.remove(0, buf.length());

        if (pz.length() > 0)
        {
            if (buf.length() == 0)
                buf = pz;
            else
                owriter.addMember(buf, pz, "}");
        }
        if (ptk.length() > 0)
        {
            if (buf.length() == 0)
                buf = ptk;
            else
                owriter.addMember(buf, ptk, "}");
        }
        if (rt.length() > 0)
        {
            if (buf.length() == 0)
                buf = rt;
            else
                owriter.addMember(buf, rt, "}");
        }
    }

public:
    ListCollectionIdsOptions() {}
    /**
     * The maximum number of results to return.
     * @param pageSize The maximum number of results to return.
     */
    ListCollectionIdsOptions &pageSize(int pageSize)
    {
        jh.addObject(pz, FPSTR("pageSize"), String(pageSize), false, true);
        set();
        return *this;
    }
    /**
     * A page token. Must be a value from ListCollectionIdsResponse.
     * @param pageToken A page token. Must be a value from ListCollectionIdsResponse.
     */
    ListCollectionIdsOptions &pageToken(const String &pageToken)
    {
        jh.addObject(ptk, FPSTR("pageToken"), pageToken, true, true);
        set();
        return *this;
    }
    /**
     * Reads documents as they were at the given time.
     * This must be a microsecond precision timestamp within the past one hour,
     * or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     * @param readTime Timestamp
     */
    ListCollectionIdsOptions &readTime(const String readTime)
    {
        jh.addObject(rt, FPSTR("readTime"), readTime, true, true);
        set();
        return *this;
    }
    const char *c_str() { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear()
    {
        buf.remove(0, buf.length());
        ptk.remove(0, ptk.length());
        pz.remove(0, pz.length());
        rt.remove(0, rt.length());
    }
};

namespace DatabaseIndex
{
    namespace IndexMode
    {
        /**
         * The mode determines how a field is indexed.
         */
        enum Mode
        {
            MODE_UNSPECIFIED, // The mode is unspecified.
            ASCENDING,        // The field's values are indexed so as to support sequencing in ascending order
            // and also query by <, >, <=, >=, and =.
            DESCENDING, // The field's values are indexed so as to support sequencing in descending
            // order and also query by <, >, <=, >=, and =.
            ARRAY_CONTAINS // The field's array values are indexed so as to support membership using ARRAY_CONTAINS queries.
        };

    }

    /**
     * A field of an index.
     */
    class IndexField : public Printable
    {
    private:
        String buf;
        String fp, md;
        ObjectWriter owriter;
        JsonHelper jh;

        void set()
        {
            buf.remove(0, buf.length());
            if (fp.length() > 0)
            {
                if (buf.length() == 0)
                    buf = fp;
                else
                    owriter.addMember(buf, fp, "}");
            }
            if (md.length() > 0)
            {
                if (buf.length() == 0)
                    buf = md;
                else
                    owriter.addMember(buf, md, "}");
            }
        }

    public:
        IndexField() {}
        /**
         * The path of the field. Must match the field path specification described by
         * [google.firestore.v1beta1.Document.fields][fields]. Special field path __name__
         * may be used by itself or at the end of a path. __type__ may be used only at the end of path.
         * @param fieldPath
         */
        IndexField &fieldPath(const String &fieldPath)
        {
            jh.addObject(fp, "fieldPath", fieldPath, true, true);
            set();
            return *this;
        }
        /**
         * The field's mode.
         * @param mode
         */
        IndexField &mode(IndexMode::Mode mode)
        {
            if (mode == IndexMode::ASCENDING)
                jh.addObject(md, "mode", "ASCENDING", true, true);
            else if (mode == IndexMode::DESCENDING)
                jh.addObject(md, "mode", "DESCENDING", true, true);
            else if (mode == IndexMode::ARRAY_CONTAINS)
                jh.addObject(md, "mode", "ARRAY_CONTAINS", true, true);
            set();
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear()
        {
            buf.remove(0, buf.length());
            fp.remove(0, fp.length());
            md.remove(0, md.length());
        }
    };

    /**
     * An index definition.
     */
    class Index : public Printable
    {
    private:
        String buf;
        String collid, flds, flds_ar;
        ObjectWriter owriter;
        JsonHelper jh;

        void set()
        {
            buf.remove(0, buf.length());
            if (collid.length() > 0)
            {
                if (buf.length() == 0)
                    buf = collid;
                else
                    owriter.addMember(buf, collid, "}");
            }
            if (flds.length() > 0)
            {
                if (buf.length() == 0)
                    buf = flds;
                else
                    owriter.addMember(buf, flds, "}");
            }
        }

    public:
        Index(const String &collId)
        {
            if (collId.length())
                collectionId(collId);
        }
        /**
         * The collection ID to which this index applies. Required.
         * @param collectionId The collection ID to which this index applies. Required.
         */
        Index &collectionId(const String &collectionId)
        {
            jh.addObject(collid, FPSTR("collectionId"), collectionId, true, true);
            set();
            return *this;
        }
        /**
         * Add the field to index.
         * @param field the field to index.
         */
        Index &addField(IndexField field)
        {
            flds.remove(0, flds.length());
            if (flds_ar.length() == 0)
                jh.addArray(flds_ar, field.c_str(), false, true);
            else
                owriter.addMember(flds_ar, field.c_str(), "]");
            jh.addObject(flds, FPSTR("fields"), flds_ar, false, true);
            set();
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear()
        {
            buf.remove(0, buf.length());
            collid.remove(0, collid.length());
            flds.remove(0, flds.length());
            flds_ar.remove(0, flds_ar.length());
        }
    };

}

namespace CollectionGroupsIndex
{
    // Query Scope defines the scope at which a query is run.
    enum QueryScope
    {
        QUERY_SCOPE_UNSPECIFIED, // The query scope is unspecified. Not a valid option.
        COLLECTION,              // Indexes with a collection query scope specified allow queries against a collection that is the child of a specific document, specified at query time, and that has the collection id specified by the index.
        COLLECTION_GROUP,        // Indexes with a collection group query scope specified allow queries against all collections that has the collection id specified by the index.
        COLLECTION_RECURSIVE     // Include all the collections's ancestor in the index. Only available for Datastore Mode databases.
    };

    // API Scope defines the APIs (Firestore Native, or Firestore in Datastore Mode) that are supported for queries.
    enum ApiScope
    {
        ANY_API,           // The index can only be used by the Firestore Native query API. This is the default.
        DATASTORE_MODE_API // The index can only be used by the Firestore in Datastore Mode query API.
    };

    // The supported orderings.
    enum Order
    {
        ORDER_UNSPECIFIED, // The ordering is unspecified. Not a valid option.
        ASCENDING,         // The field is ordered by ascending field value.
        DESCENDING         // The field is ordered by descending field value.
    };

    // The supported array value configurations.
    enum ArrayConfig
    {
        ARRAY_CONFIG_UNSPECIFIED, // The index does not support additional array queries.
        CONTAINS                  // The index supports array containment queries.
    };

    class VectorConfig : public Printable
    {
    private:
        String buf;
        String dim, flat_str;
        ObjectWriter owriter;
        JsonHelper jh;

        void set()
        {
            buf.remove(0, buf.length());
            if (dim.length() > 0)
            {
                if (buf.length() == 0)
                    buf = dim;
                else
                    owriter.addMember(buf, dim, "}");
            }
            if (flat_str.length() > 0)
            {
                if (buf.length() == 0)
                    buf = flat_str;
                else
                    owriter.addMember(buf, flat_str, "}");
            }
        }

    public:
        /**
         * Required. The vector dimension this configuration applies to.
         * The resulting index will only include vectors of this dimension, and can be used for vector search with the same dimension.
         * @param value the field to index.
         */
        void dimenrion(int value)
        {
            dim.remove(0, dim.length());
            jh.addObject(dim, "dimenrion", String(value), false, true);
            set();
        }
        /**
         * Indicates the vector index is a flat index.
         */
        void flat()
        {
            flat_str = "{\"flat\":{}}";
            set();
        }
        const char *c_str() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear()
        {
            buf.remove(0, buf.length());
            dim.remove(0, dim.length());
            flat_str.remove(0, flat_str.length());
        }
    };

    /**
     * A field in an index. The fieldPath describes which field is indexed, the value_mode describes how the field value is indexed.
     */
    class IndexField : public Printable
    {
    private:
        String buf;
        String fp, ordr, arcfg, vcfg;
        ObjectWriter owriter;
        JsonHelper jh;

        void set()
        {
            buf.remove(0, buf.length());
            if (fp.length() > 0)
            {
                if (buf.length() == 0)
                    buf = fp;
                else
                    owriter.addMember(buf, fp, "}");
            }
            if (ordr.length() > 0)
            {
                if (buf.length() == 0)
                    buf = ordr;
                else
                    owriter.addMember(buf, ordr, "}");
            }
            else if (arcfg.length() > 0)
            {
                if (buf.length() == 0)
                    buf = arcfg;
                else
                    owriter.addMember(buf, arcfg, "}");
            }
            else if (vcfg.length() > 0)
            {
                if (buf.length() == 0)
                    buf = vcfg;
                else
                    owriter.addMember(buf, vcfg, "}");
            }
        }

    public:
        IndexField() {}
        /**
         * Can be name. For single field indexes, this must match the name of the field or may be omitted.
         * @param fieldPath
         */
        IndexField &fieldPath(const String &fieldPath)
        {
            jh.addObject(fp, "fieldPath", fieldPath, true, true);
            set();
            return *this;
        }
        /**
         * Indicates that this field supports ordering by the specified order or comparing using =, !=, <, <=, >, >=.
         * @param order
         */
        IndexField &order(Order order)
        {
            if (order == Order::ORDER_UNSPECIFIED)
                jh.addObject(ordr, "order", "ORDER_UNSPECIFIED", true, true);
            else if (order == Order::DESCENDING)
                jh.addObject(ordr, "order", "DESCENDING", true, true);
            else if (order == Order::ASCENDING)
                jh.addObject(ordr, "order", "ASCENDING", true, true);
            set();
            return *this;
        }

        /**
         * Indicates that this field supports ordering by the specified order or comparing using =, !=, <, <=, >, >=.
         * @param arrayConfig
         */
        IndexField &arrayConfig(ArrayConfig arrayConfig)
        {
            if (arrayConfig == ArrayConfig::ARRAY_CONFIG_UNSPECIFIED)
                jh.addObject(arcfg, "arrayConfig", "ARRAY_CONFIG_UNSPECIFIED", true, true);
            else if (arrayConfig == ArrayConfig::CONTAINS)
                jh.addObject(arcfg, "arrayConfig", "CONTAINS", true, true);
            set();
            return *this;
        }

        /**
         * Indicates that this field supports nearest neighbors and distance operations on vector.
         * @param vectorConfig
         */
        IndexField &vectorConfig(VectorConfig vectorConfig)
        {
            jh.addObject(vcfg, "vectorConfig", vectorConfig.c_str(), false, true);
            set();
            return *this;
        }

        const char *c_str() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear()
        {
            buf.remove(0, buf.length());
            fp.remove(0, fp.length());
            arcfg.remove(0, arcfg.length());
            vcfg.remove(0, vcfg.length());
            ordr.remove(0, ordr.length());
        }
    };

    /**
     * Cloud Firestore indexes enable simple and complex queries against documents in a database.
     */
    class Index : public Printable
    {
    private:
        String buf;
        String qrscope, apiscope, flds, flds_ar;
        ObjectWriter owriter;
        JsonHelper jh;

        void set()
        {
            buf.remove(0, buf.length());
            if (qrscope.length() > 0)
            {
                if (buf.length() == 0)
                    buf = qrscope;
                else
                    owriter.addMember(buf, qrscope, "}");
            }
            if (apiscope.length() > 0)
            {
                if (buf.length() == 0)
                    buf = apiscope;
                else
                    owriter.addMember(buf, apiscope, "}");
            }
            if (flds.length() > 0)
            {
                if (buf.length() == 0)
                    buf = flds;
                else
                    owriter.addMember(buf, flds, "}");
            }
        }

    public:
        Index() {}

        /**
         * Indexes with a collection query scope specified allow queries against a collection that is the child of a specific document, specified at query time, and that has the same collection id.
         * @param queryScope The query scope enum.
         */
        Index &queryScope(QueryScope queryScope)
        {
            if (queryScope == QueryScope::QUERY_SCOPE_UNSPECIFIED)
                jh.addObject(qrscope, "queryScope", "QUERY_SCOPE_UNSPECIFIED", true, true);
            else if (queryScope == QueryScope::COLLECTION)
                jh.addObject(qrscope, "queryScope", "COLLECTION", true, true);
            else if (queryScope == QueryScope::COLLECTION_GROUP)
                jh.addObject(qrscope, "queryScope", "COLLECTION_GROUP", true, true);
            else if (queryScope == QueryScope::COLLECTION_RECURSIVE)
                jh.addObject(qrscope, "queryScope", "COLLECTION_RECURSIVE", true, true);
            set();
            return *this;
        }

        /**
         * Indexes with a collection query scope specified allow queries against a collection that is the child of a specific document, specified at query time, and that has the same collection id.
         * @param apiScope The query scope enum.
         */
        Index &apiScope(ApiScope apiScope)
        {
            if (apiScope == ApiScope::ANY_API)
                jh.addObject(apiscope, "apiScope", "ANY_API", true, true);
            else if (apiScope == ApiScope::DATASTORE_MODE_API)
                jh.addObject(apiscope, "apiScope", "DATASTORE_MODE_API", true, true);
            set();
            return *this;
        }

        /**
         * Add the field that supported by this index.
         * @param field the field to add.
         */
        Index &addField(IndexField field)
        {
            flds.remove(0, flds.length());
            if (flds_ar.length() == 0)
                jh.addArray(flds_ar, field.c_str(), false, true);
            else
                owriter.addMember(flds_ar, field.c_str(), "]");
            jh.addObject(flds, FPSTR("fields"), flds_ar, false, true);
            set();
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear()
        {
            buf.remove(0, buf.length());
            qrscope.remove(0, qrscope.length());
            apiscope.remove(0, apiscope.length());
            flds.remove(0, flds.length());
            flds_ar.remove(0, flds_ar.length());
        }
    };

}

class FirestoreOptions
{
    friend class FirestoreBase;

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