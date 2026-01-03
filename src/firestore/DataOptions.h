/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FIRESTORE_DATA_OPTIONS_H
#define FIRESTORE_DATA_OPTIONS_H

#include <Arduino.h>
#include "./core/Utils/JSON.h"
#include "./core/Utils/ObjectWriter.h"
#include "./core/Utils/URL.h"

#if defined(ENABLE_FIRESTORE)
#include "./firestore/Values.h"
#if defined(ENABLE_FIRESTORE_QUERY)
#include "./firestore/Query.h"
using namespace FirestoreQuery;
#endif

#define RESOURCE_PATH_BASE "<resource_path>"

enum firebase_firestore_request_type
{
    cf_undefined,
    cf_rollback = 200,
    cf_begin_transaction,
    cf_commit_document,
    cf_batch_write_doc,
    cf_run_query,
    cf_list_collection,
    cf_export_docs,
    cf_import_docs,
    cf_create_doc,
    cf_batch_get_doc,
    cf_create_composite_index,
    cf_create_field_index,
    cf_manage_database,
    cf_get_doc = 300,
    cf_list_doc,
    cf_list_index,
    cf_get_index,
    cf_patch_doc = 400,
    cf_delete_doc = 500,
    cf_delete_index
};

/**
 * A set of field paths on a document. Used to restrict a get or update operation on a document to a subset of its fields. This is different from standard field masks, as this is always scoped to a Document, and takes in account the dynamic nature of Value.
 */
class DocumentMask : public BaseO2
{
    friend class FirestoreBase;
    friend class GetDocumentOptions;
    friend class PatchDocumentOptions;
    friend class ListDocumentsOptions;
    friend class Documents;

private:
    ObjectWriter owriter;

    String get()
    {
        String temp;
        JSONUtil jut;
        jut.addTokens(temp, jut.toString("fieldPaths"), buf[1], true);
        return temp;
    }

public:
    /**
     * A set of field paths on a document.
     * Used to restrict a get or update operation on a document to a subset of its fields.
     * This is different from standard field masks, as this is always scoped to a Document,
     * and takes in account the dynamic nature of Value.
     *
     * @param fieldPaths The list of field paths in the mask. See Document.fields (https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document.FIELDS.fields) for a field path syntax reference.
     */
    explicit DocumentMask(const String &fieldPaths = "") { setFieldPaths(fieldPaths); }

    /**
     * A set of field paths on a document.
     * Used to restrict a get or update operation on a document to a subset of its fields.
     * This is different from standard field masks, as this is always scoped to a Document,
     * and takes in account the dynamic nature of Value.
     *
     * @param fieldPaths The list of field paths in the mask. See Document.fields (https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document.FIELDS.fields) for a field path syntax reference.
     */
    void setFieldPaths(const String &fieldPaths)
    {
        if (fieldPaths.length())
        {
            this->buf[1] = fieldPaths;
            JSONUtil jut;
            jut.addTokens(buf[0], "fieldPaths", buf[1], true);
        }
    }
    String getQuery(const String &mask, bool hasParam)
    {
        String temp;
        URLUtil uut;
        uut.addParamsTokens(temp, String(mask + ".fieldPaths="), this->buf[1], hasParam);
        return temp;
    }
};

namespace FieldTransform
{
    // A value that is calculated by the server.
    enum ServerValue
    {
        SERVER_VALUE_UNSPECIFIED,
        // REQUEST_TIME The time at which the server processed the request, with millisecond precision.
        //  If used on multiple fields (same or different documents) in a transaction, all the fields
        // will get the same server timestamp.
        REQUEST_TIME
    };

    /**
     * Adds the given value to the field's current value.
     * This must be an integer or a double value.
     *  If the field is not an integer or double, or if the field does not yet exist, the transformation will set the field to the given value. If either of the given value or the current field value are doubles, both values will be interpreted as doubles. Double arithmetic and representation of double values follow IEEE 754 semantics. If there is positive/negative integer overflow, the field is resolved to the largest magnitude positive/negative integer.
     */
    struct Increment
    {
    private:
        String buf;
        ObjectWriter owriter;
        StringUtil sut;

    public:
        /**
         * Increment object to use with FieldTransform object class constructor.
         * @param value Adds the given value to the field's current value.
         */
        template <typename T>
        explicit Increment(T value) { owriter.setPair(buf, "increment", value.val()); }
        const char *c_str() const { return buf.c_str(); }
        void clear() { sut.clear(buf); }
    };

    /**
     * Sets the field to the maximum of its current value and the given value.
     * This must be an integer or a double value.
     * If the field is not an integer or double, or if the field does not yet exist, the transformation will set the field to the given value. If a maximum operation is applied where the field and the input value are of mixed types (that is - one is an integer and one is a double) the field takes on the type of the larger operand. If the operands are equivalent (e.g. 3 and 3.0), the field does not change. 0, 0.0, and -0.0 are all zero. The maximum of a zero stored value and zero input value is always the stored value. The maximum of any numeric value x and NaN is NaN.
     */
    struct Maximum
    {
    private:
        String buf;
        ObjectWriter owriter;
        StringUtil sut;

    public:
        /**
         * Maximum object to use with FieldTransform object class constructor.
         * @param value Sets the field to the maximum of its current value and the given value.
         */
        template <typename T>
        explicit Maximum(T value) { owriter.setPair(buf, "maximum", value.c_str()); }
        const char *c_str() const { return buf.c_str(); }
        void clear() { sut.clear(buf); }
    };

    /**
     * Sets the field to the minimum of its current value and the given value.
     * This must be an integer or a double value.
     * If the field is not an integer or double, or if the field does not yet exist, the transformation will set the field to the input value. If a minimum operation is applied where the field and the input value are of mixed types (that is - one is an integer and one is a double) the field takes on the type of the smaller operand. If the operands are equivalent (e.g. 3 and 3.0), the field does not change. 0, 0.0, and -0.0 are all zero. The minimum of a zero stored value and zero input value is always the stored value. The minimum of any numeric value x and NaN is NaN.
     */
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
        explicit Minimum(T value) { owriter.setPair(buf, "minimum", value.c_str()); }
        const char *c_str() const { return buf.c_str(); }
    };

    /**
     * Append the given elements in order if they are not already present in the current field value. If the field is not an array, or if the field does not yet exist, it is first set to the empty array.
     * Equivalent numbers of different types (e.g. 3L and 3.0) are considered equal when checking if a value is missing. NaN is equal to NaN, and Null is equal to Null. If the input contains multiple equivalent values, only the first will be considered.
     * The corresponding transform_result will be the null value.
     */
    template <typename T = Values::ArrayValue>
    struct AppendMissingElements
    {
    private:
        String buf;
        ObjectWriter owriter;
        StringUtil sut;

    public:
        /**
         * Append the given elements in order if they are not already present in the current field value.
         * If the field is not an array, or if the field does not yet exist, it is first set to the empty array.
         * @param arrayValue The array value object to append.
         */
        explicit AppendMissingElements(const T &arrayValue) { owriter.setPair(buf, "appendMissingElements", arrayValue.c_str()); }
        const char *c_str() const { return buf.c_str(); }
        void clear() { sut.clear(buf); }
    };

    /**
     * Remove all of the given elements from the array in the field. If the field is not an array, or if the field does not yet exist, it is set to the empty array.
     * Equivalent numbers of the different types (e.g. 3L and 3.0) are considered equal when deciding whether an element should be removed. NaN is equal to NaN, and Null is equal to Null. This will remove all equivalent values if there are duplicates.
     * The corresponding transform_result will be the null value.
     */
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
        explicit RemoveAllFromArray(const T &arrayValue) { owriter.setPair(buf, "removeAllFromArray", arrayValue.c_str()); }
        const char *c_str() const { return buf.c_str(); }
    };

    /**
     *  Sets the field to the given server value.
     */
    struct SetToServerValue
    {
    private:
        String buf;
        ObjectWriter owriter;
        JSONUtil jut;

    public:
        /**
         * Sets the field to the given server value.
         * @param enumValue The ServerValue enum
         *
         */
        explicit SetToServerValue(ServerValue enumValue) { owriter.setPair(buf, "setToServerValue", jut.toString(enumValue == SERVER_VALUE_UNSPECIFIED ? "SERVER_VALUE_UNSPECIFIED" : "REQUEST_TIME")); }
        const char *c_str() const { return buf.c_str(); }
    };

    /**
     * A transformation of a field of the document.
     */
    struct FieldTransform : public Printable
    {
    private:
        String buf;
        JSONUtil jut;
        ObjectWriter owriter;
        StringUtil sut;

        template <typename T>
        void set(const String &fieldPath, T v)
        {
            jut.addObject(buf, "fieldPath", fieldPath, true);
            buf += ',';
            String str = v.c_str();
            buf += str.substring(1, str.length() - 1);
            buf += '}';
        }

    public:
        /**
         * @param fieldPath The path of the field.
         * @param object The Increment, Maximum and Minimum objects.
         * object is a union field transform_type from one of setToServerValue, increment, maximum, minimum, appendMissingElements, and removeAllFromArray
         */
        template <typename T>
        explicit FieldTransform(const String &fieldPath, T object) { set(fieldPath, object); }

        /**
         * @param fieldPath The path of the field.
         * @param arrayValue Append the given elements in order if they are not already present in the current field value.
         * If the field is not an array, or if the field does not yet exist, it is first set to the empty array.
         */

        explicit FieldTransform(const String &fieldPath, AppendMissingElements<Values::ArrayValue> arrayvalue) { set(fieldPath, arrayvalue); }

        /**
         * @param fieldPath The path of the field.
         * @param arrayValue Remove all of the given elements from the array in the field.
         * If the field is not an array, or if the field does not yet exist, it is set to the empty array.
         */
        explicit FieldTransform(const String &fieldPath, RemoveAllFromArray<Values::ArrayValue> arrayvalue) { set(fieldPath, arrayvalue); }
        const char *c_str() const { return buf.c_str(); }
        size_t printTo(Print &p) const override { return p.print(buf.c_str()); }
        void clear() { sut.clear(buf); }
    };

};
/**
 * A precondition on a document, used for conditional operations.
 */
class Precondition : public BaseO4
{
    friend class FirestoreBase;
    friend class Documents;

private:
    ObjectWriter owriter;
    JSONUtil jut;
    StringUtil sut;

    const char *getQuery(const String &mask)
    {
        sut.clear(buf[1]);
        if (buf[2].length())
            sut.printTo(buf[1], mask.length() + buf[2].length(), "?%s.exists=%s", mask.c_str(), buf[2].c_str());
        if (buf[3].length())
            sut.printTo(buf[1], mask.length() + jut.toString(buf[3]).length(), "%s%s.updateTime=%s", buf[1].length() ? "&" : "?", mask.c_str(), jut.toString(buf[3]).c_str());
        return buf[1].c_str();
    }

    Precondition &setObject()
    {
        if (buf[2].length())
        {
            if (buf[0].length() == 0)
                owriter.setPair(buf[0], "exists", buf[2]);
            else
            {
                buf[0][buf[0].length() - 1] = '\0';
                jut.addObject(buf[0], "exists", buf[2], true);
            }
        }

        if (buf[3].length())
        {
            if (buf[0].length() == 0)
                owriter.setPair(buf[0], "updateTime", buf[3]);
            else
            {
                buf[0][buf[0].length() - 1] = '\0';
                jut.addObject(buf[0], "updateTime", buf[3], true);
            }
        }

        return *this;
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
        this->buf[2] = owriter.getBoolStr(value);
        return setObject();
    }

    /**
     * Set the update time condition.
     * @param updateTime A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     * When set, the target document must exist and have been last updated at that time. Timestamp must be microsecond aligned.
     */
    Precondition &updateTime(const String &timestamp)
    {
        this->buf[3] = timestamp;
        return setObject();
    }
};

/**
 * Firestore document
 */
template <typename T = Values::Value>
class Document : public BaseO4
{
    friend class FirestoreBase;

private:
    Values::MapValue mv;
    ObjectWriter owriter;
    JSONUtil jut;
    StringUtil sut;

    Document &getBuf()
    {
        buf[2] = mv.c_str();
        sut.clear(buf[3]);
        if (buf[1].length())
            jut.addObject(buf[3], "name", owriter.makeResourcePath(buf[1]), true, true);
        else
        {
            buf[0] = buf[2];
            return *this;
        }

        buf[0] = buf[3];
        if (buf[2].length())
            owriter.addMember(buf[0], buf[2], false, "}");

        return *this;
    }

public:
    /**
     * A Firestore document constructor with document resource name.
     * @param name The resource name of the document.
     */
    explicit Document(const String &name = "")
    {
        buf[1] = name;
        getBuf();
    }

    /**
     * A Firestore document constructor with `Values::Value` object.
     * @param key The key of an object.
     * @param value The value of an `Values::Value` object.
     */
    explicit Document(const String &key, T value)
    {
        mv.add(key, value);
        getBuf();
    }

    /**
     * Add the `Values::Value` object to Firestore document.
     * @param key The key of an object.
     * @param value The value of an `Values::Value` object.
     */
    Document &add(const String &key, T value)
    {
        mv.add(key, value);
        return getBuf();
    }

    /**
     * Set the document resource name.
     * @param name The resource name of the document.
     */
    void setName(const String &name)
    {
        buf[1] = name;
        getBuf();
    }

    const char *c_str() const { return buf[0].c_str(); }

    void clear()
    {
        owriter.clearBuf(buf, bufSize);
        mv.clear();
    }
};

namespace Firestore
{
    // The type of concurrency control mode for transactions.
    enum ConcurrencyMode
    {
        CONCURRENCY_MODE_UNSPECIFIED, // Not used.
        // Use optimistic concurrency control by default. This mode is available for Cloud Firestore databases.
        OPTIMISTIC,
        // This is the default setting for Cloud Firestore.
        PESSIMISTIC,
        // Use optimistic concurrency control with entity groups by default. This is the only available mode for Cloud Datastore. This mode is also available for Cloud Firestore with Datastore Mode but is not recommended.
        OPTIMISTIC_WITH_ENTITY_GROUPS
    };

    // The type of App Engine integration mode.
    enum AppEngineIntegrationMode
    {
        APP_ENGINE_INTEGRATION_MODE_UNSPECIFIED, // Not used
        // If an App Engine application exists in the same region as this database, App Engine configuration will impact this database. This includes disabling of the application & database, as well as disabling writes to the database.
        ENABLED,
        // App Engine has no effect on the ability of this database to serve requests. This is the default setting for databases created with the Firestore API.
        _DISABLED
    };

    // The delete protection state of the database.
    enum DeleteProtectionState
    {
        DELETE_PROTECTION_STATE_UNSPECIFIED, // The default value. Delete protection type is not specified
        DELETE_PROTECTION_DISABLED,          //	Delete protection is disabled
        DELETE_PROTECTION_ENABLED            // Delete protection is enabled
    };

    // Point In Time Recovery feature enablement.
    enum PointInTimeRecoveryEnablement
    {
        POINT_IN_TIME_RECOVERY_ENABLEMENT_UNSPECIFIED, //	Not used.
        // Reads are supported on selected versions of the data from within the past 7 days:
        // [] Reads against any timestamp within the past hour
        // [] Reads against 1-minute snapshots beyond 1 hour and within 7 days
        // versionRetentionPeriod and earliestVersionTime can be used to determine the supported versions.
        POINT_IN_TIME_RECOVERY_ENABLED,
        POINT_IN_TIME_RECOVERY_DISABLED // Reads are supported on any version of the data from within the past 1 hour.
    };

    // The type of the database. See https://cloud.google.com/datastore/docs/firestore-or-datastore for information about how to choose.
    enum DatabaseType
    {
        DATABASE_TYPE_UNSPECIFIED, //	The default value. This value is used if the database type is omitted.
        FIRESTORE_NATIVE,          //	Firestore Native Mode
        DATASTORE_MODE             //	Firestore in Datastore Mode.
    };

    enum firestore_database_mode
    {
        cf_mode_create,
        cf_mode_get,
        cf_mode_list,
        cf_mode_patch,
        cf_mode_delete
    };

    const struct firebase_ns::key_str_30 _ConcurrencyMode[ConcurrencyMode::OPTIMISTIC_WITH_ENTITY_GROUPS + 1] PROGMEM = {"CONCURRENCY_MODE_UNSPECIFIED", "OPTIMISTIC", "PESSIMISTIC", "OPTIMISTIC_WITH_ENTITY_GROUPS"};
    const struct firebase_ns::key_str_40 _AppEngineIntegrationMode[AppEngineIntegrationMode::_DISABLED + 1] PROGMEM = {"APP_ENGINE_INTEGRATION_MODE_UNSPECIFIED", "ENABLED", "DISABLED"};
    const struct firebase_ns::key_str_40 _DeleteProtectionState[DeleteProtectionState::DELETE_PROTECTION_ENABLED + 1] PROGMEM = {"DELETE_PROTECTION_STATE_UNSPECIFIED", "DELETE_PROTECTION_DISABLED", "DELETE_PROTECTION_ENABLED"};
    const struct firebase_ns::key_str_50 _PointInTimeRecoveryEnablement[PointInTimeRecoveryEnablement::POINT_IN_TIME_RECOVERY_DISABLED + 1] PROGMEM = {"POINT_IN_TIME_RECOVERY_ENABLEMENT_UNSPECIFIED", "POINT_IN_TIME_RECOVERY_ENABLED", "POINT_IN_TIME_RECOVERY_DISABLED"};
    const struct firebase_ns::key_str_30 _DatabaseType[DatabaseType::DATASTORE_MODE + 1] PROGMEM = {"DATABASE_TYPE_UNSPECIFIED", "FIRESTORE_NATIVE", "DATASTORE_MODE"};

    /**
     * A Cloud Firestore Database.
     */
    class Database : public BaseO10
    {
        friend class Databases;

    public:
        // This value represents the item to add to an array.
        // Set the concurrency control mode to use for this database (used in database creation).
        Database &concurrencyMode(ConcurrencyMode value) { return wr.append<Database &, const char *>(*this, _ConcurrencyMode[value].text, buf, bufSize, 1, __func__); }

        // This value represents the item to add to an array.
        // Set the App Engine integration mode to use for this database (used in database creation).
        Database &appEngineIntegrationMode(AppEngineIntegrationMode value) { return wr.append<Database &, const char *>(*this, _AppEngineIntegrationMode[value].text, buf, bufSize, 2, __func__); }

        // This value represents the item to add to an array.
        // Set the state of delete protection for the database (used in database creation).
        Database &deleteProtectionState(DeleteProtectionState value) { return wr.append<Database &, const char *>(*this, _DeleteProtectionState[value].text, buf, bufSize, 3, __func__); }

        // This value represents the item to add to an array.
        // Set to enable the PITR feature on this database (used in database creation).
        Database &pointInTimeRecoveryEnablement(PointInTimeRecoveryEnablement value) { return wr.append<Database &, const char *>(*this, _PointInTimeRecoveryEnablement[value].text, buf, bufSize, 4, __func__); }

        // This value represents the item to add to an array.
        // Set the type of the database (used in database creation). See https://cloud.google.com/datastore/docs/firestore-or-datastore for information about how to choose.
        Database &databaseType(DatabaseType value) { return wr.append<Database &, const char *>(*this, _DatabaseType[value].text, buf, bufSize, 5, __func__); }

        // This value represents the item to add to an array.
        // Set the location of the database (used in database creation).
        // Available locations are listed at https://cloud.google.com/firestore/docs/locations.
        Database &locationId(const String &value) { return wr.append<Database &, String>(*this, value, buf, bufSize, 6, __func__); }

        // This value represents the item to add to an array.
        // Set the resource name of the Database (used in database creation).
        // Format: projects/{project}/databases/{database}
        Database &name(const String &value) { return wr.append<Database &, String>(*this, value, buf, bufSize, 7, __func__); }

        // This value represents the item to add to an array.
        // Set the ETag (used in database update and deletion)
        // This checksum is computed by the server based on the value of other fields, and may be sent on update and delete requests to ensure the client has an up-to-date value before proceeding.
        Database &etag(const String &value) { return wr.append<Database &, String>(*this, value, buf, bufSize, 8, __func__); }
    };

}

/**
 * A transformation of a document
 */
class DocumentTransform : public BaseO1
{
    friend class FirestoreBase;
    friend class Write;

private:
    ObjectWriter owriter;
    JSONUtil jut;

public:
    /**
     * A transformation of a document.
     * @param document The name of the document to transform.
     * @param fieldTransforms The list of transformations to apply to the fields of the document, in order. This must not be empty.
     */
    explicit DocumentTransform(const String &document, FieldTransform::FieldTransform fieldTransforms)
    {
        jut.addObject(buf, "document", owriter.makeResourcePath(document), true);
        jut.addObject(buf, "fieldTransforms", fieldTransforms.c_str(), false, true);
    }
};

/**
 * A write on a document.
 */
class Write : public BaseO1
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
    JSONUtil jut;
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
    explicit Write(const DocumentMask &updateMask, const Document<Values::Value> &update, const Precondition &currentDocument)
    {
        bool curdoc = strlen(currentDocument.c_str());
        bool updatemask = strlen(updateMask.c_str());
        write_type = firestore_write_type_update;
        jut.addObject(buf, "update", update.c_str(), false, !updatemask && !curdoc);
        if (updatemask)
            jut.addObject(buf, "updateMask", updateMask.c_str(), false, !curdoc);
        if (curdoc)
            jut.addObject(buf, "currentDocument", currentDocument.c_str(), false, true);
    }

    /**
     * A write on a document.
     * @param deletePath A document name to delete.
     */
    explicit Write(const String &deletePath, const Precondition &currentDocument)
    {
        write_type = firestore_write_type_delete;
        if (strlen(currentDocument.c_str()))
            jut.addObject(buf, "currentDocument", currentDocument.c_str(), false);
        jut.addObject(buf, "delete", owriter.makeResourcePath(deletePath), true, true);
    }

    /**
     * A write on a document.
     * @param transform Applies a transformation to a document.
     * @param currentDocument An optional precondition on the document. The write will fail if this is set and not met by the target document.
     */
    explicit Write(const DocumentTransform &transform, const Precondition &currentDocument)
    {
        write_type = firestore_write_type_transform;
        if (strlen(currentDocument.c_str()))
            jut.addObject(buf, "currentDocument", currentDocument.c_str(), false);
        jut.addObject(buf, "transform", transform.c_str(), false, true);
    }

    /**
     * Add the update transform.
     * @param updateTransforms The transforms to perform after update.
     * This field can be set only when the operation is update.
     * If present, this write is equivalent to performing update and transform to
     * the same document atomically and in order.
     */
    Write &addUpdateTransform(const FieldTransform::FieldTransform &updateTransforms)
    {
        if (write_type == firestore_write_type_update)
        {
            if (!updateTrans)
            {
                buf[buf.length() - 1] = '\0';
                jut.addObject(buf, "updateTransforms", owriter.getArrayStr(updateTransforms.c_str()), false, true);
            }
            else
                owriter.addMember(buf, updateTransforms.c_str(), false, "]}");
            updateTrans = true;
        }

        return *this;
    }
};

/**
 * Class that represent the object that contains a write or list of writes, transaction and label to use with batch write.
 */
class Writes : public BaseO1
{
private:
    JSONUtil jut;
    ObjectWriter owriter;

public:
    /**
     * The writes to apply.
     * @param write A write on a document.
     * @param transaction A base64-encoded string. If set, applies all writes in this transaction, and commits it.
     *
     */
    explicit Writes(Write write, const String &transaction = "")
    {
        if (transaction.length())
            jut.addObject(buf, "transaction", transaction, true);
        jut.addObject(buf, "writes", owriter.getArrayStr(write.c_str()), false, true);
    }

    /**
     * The writes to apply.
     * @param write A write on a document.
     * @param labels Labels associated with this batch write.
     * An object containing a list of "key": value pairs.
     */
    explicit Writes(const Write &write, const Values::MapValue &labels)
    {
        if (strlen(labels.c_str()))
            jut.addObject(buf, "labels", labels.c_str(), false);
        jut.addObject(buf, "writes", owriter.getArrayStr(write.c_str()), false, true);
    }

    /**
     * Add the write.
     * @param write A write on a document.
     */
    Writes &add(const Write &write)
    {
        ObjectWriter owr;
        owr.addMember(buf, write.c_str(), false, "]}");
        return *this;
    }
};

/**
 * Options for a transaction that can be used to read and write documents.
 */
class ReadWrite : public BaseO1
{
private:
    JSONUtil jut;

public:
    /**
     * @param retryTransaction An optional transaction to retry.
     * A base64-encoded string.
     */
    explicit ReadWrite(const String &retryTransaction)
    {
        if (retryTransaction.length())
            jut.addObject(buf, "retryTransaction", retryTransaction, true, true);
    }
};

/**
 * Options for a transaction that can only be used to read documents.
 */
class ReadOnly : public BaseO1
{
private:
    JSONUtil jut;

public:
    ReadOnly() {}

    /**
     * @param readTime Timestamp. Reads documents at the given time.
     */
    explicit ReadOnly(const String &readTime)
    {
        if (readTime.length())
            jut.addObject(buf, "readTime", readTime, true, true);
    }
};

/**
 * Options for creating a new transaction.
 */
class TransactionOptions : public BaseO1
{
private:
    JSONUtil jut;

public:
    TransactionOptions() {}

    /**
     * @param readOnly The transaction can only be used for read operations.
     */
    explicit TransactionOptions(const ReadOnly &readOnly)
    {
        if (strlen(readOnly.c_str()))
            jut.addObject(buf, "readOnly", readOnly.c_str(), false, true);
    }

    /**
     * @param readWrite The transaction can be used for both read and write operations.
     */
    explicit TransactionOptions(const ReadWrite &readWrite)
    {
        if (strlen(readWrite.c_str()))
            jut.addObject(buf, "readWrite", readWrite.c_str(), false, true);
    }
};

class EximDocumentOptions : public BaseO1
{
private:
    URLUtil uut;
    JSONUtil json;

public:
    explicit EximDocumentOptions(const String &collectionIds, const String &bucketID, const String &storagePath)
    {
        String uriPrefix;
        uut.addGStorageURL(uriPrefix, bucketID, storagePath);
        json.addObject(buf, "inputUriPrefix", uriPrefix, true);
        json.addTokens(buf, "collectionIds", collectionIds, true);
    }
};

/**
 * This class used in Documents::get function represents the query parameters
 */
class GetDocumentOptions : public BaseO1
{
private:
    URLUtil uut;

public:
    GetDocumentOptions() {}
    explicit GetDocumentOptions(DocumentMask mask, const String &transaction = "", const String &readTime = "")
    {
        bool hasParam = false;
        if (strlen(mask.c_str()))
            buf = mask.getQuery("mask", hasParam);
        if (transaction.length())
            uut.addParam(buf, "transaction", transaction, hasParam);
        if (readTime.length())
            uut.addParam(buf, "readTime", readTime, hasParam);
    }
};

/**
 * This class used in Documents::batchGet function represents the JSON representation of request body
 */
class BatchGetDocumentOptions : public BaseO4
{
private:
    ObjectWriter owriter;

public:
    BatchGetDocumentOptions() {}

    // This value represents the item to add to an array.
    // The names of the documents to retrieve.
    // The item or value will be added to the array or list.
    BatchGetDocumentOptions &documents(const String &value) { return wr.append<BatchGetDocumentOptions &, String>(*this, owriter.makeResourcePath(value), buf, bufSize, 1, __func__); }

    // The fields to return. If not set, returns all fields.
    BatchGetDocumentOptions &mask(const DocumentMask &value) { return wr.set<BatchGetDocumentOptions &, DocumentMask>(*this, value, buf, bufSize, 2, __func__); }

    // Union field consistency_selector
    // Timestamp Reads documents in a transaction.
    BatchGetDocumentOptions &transaction(const String &value) { return wr.set<BatchGetDocumentOptions &, String>(*this, value, buf, bufSize, 3, __func__); }

    // Union field consistency_selector
    // Starts a new transaction and reads the documents. Defaults to a read-only transaction.
    // The new transaction ID will be returned as the first response in the stream.
    BatchGetDocumentOptions &newTransaction(const TransactionOptions &value) { return wr.set<BatchGetDocumentOptions &, TransactionOptions>(*this, value, buf, bufSize, 3, __func__); }

    // Union field consistency_selector
    // Timestamp. Reads documents as they were at the given time.
    BatchGetDocumentOptions &readTime(const String &value) { return wr.set<BatchGetDocumentOptions &, String>(*this, value, buf, bufSize, 3, __func__); }
};

class PatchDocumentOptions : public BaseO1
{
private:
    URLUtil uut;

public:
    explicit PatchDocumentOptions(DocumentMask updateMask, DocumentMask mask, const Precondition &currentDocument)
    {
        bool hasParam = false;
        if (strlen(updateMask.c_str()))
            buf = updateMask.getQuery("updateMask", hasParam);
        if (strlen(mask.c_str()))
            buf += mask.getQuery("mask", hasParam);
        if (strlen(currentDocument.c_str()))
            uut.addParam(buf, "currentDocument", currentDocument.c_str(), hasParam);
    }
};

#if defined(ENABLE_FIRESTORE_QUERY)

// Ref https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents/runQuery

class ExplainOptions : public BaseO1
{

public:
    ExplainOptions() {}

    // Optional. Whether to execute this query.
    // When false (the default), the query will be planned, returning only metrics from the planning stages.
    // When true, the query will be planned and executed, returning the full query results along with both planning and execution stage metrics.
    ExplainOptions &analyze(bool value) { return wr.add<ExplainOptions &, bool>(*this, value, buf, __func__); }
};

/**
 * This class used in Documents::runQuery function represents the JSON representation of the request body.
 */
class QueryOptions : public BaseO4
{
public:
    QueryOptions() {}

    // Optional. Explain options for the query. If set, additional query statistics will be returned. If not, only query results will be returned.
    QueryOptions &explainOptions(const ExplainOptions &value) { return wr.set<QueryOptions &, ExplainOptions>(*this, value, buf, bufSize, 1, __func__); }

    // A structured query.
    QueryOptions &structuredQuery(const StructuredQuery &value) { return wr.set<QueryOptions &, StructuredQuery>(*this, value, buf, bufSize, 2, __func__); }

    // Union field consistency_selector
    // Run the query within an already active transaction.
    QueryOptions &transaction(const String &value) { return wr.set<QueryOptions &, String>(*this, value, buf, bufSize, 3, __func__); }

    // Union field consistency_selector
    // Starts a new transaction and reads the documents. Defaults to a read-only transaction.
    // The new transaction ID will be returned as the first response in the stream.
    QueryOptions &newTransaction(const TransactionOptions &value) { return wr.set<QueryOptions &, TransactionOptions>(*this, value, buf, bufSize, 3, __func__); }

    // Union field consistency_selector
    // Timestamp. Reads documents as they were at the given time.
    // This must be a microsecond precision timestamp within the past one hour,or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
    QueryOptions &readTime(const String &value) { return wr.set<QueryOptions &, String>(*this, value, buf, bufSize, 3, __func__); }
};

#endif

// Ref https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents/list

/**
 * This class used in Documents::list function represents the query parametes.
 */
class ListDocumentsOptions : public BaseO6
{
private:
    int pagesize = 0;
    DocumentMask msk;
    ObjectWriter owriter;
    URLUtil uut;

    ListDocumentsOptions &set()
    {
        bool hasParam = false;
        owriter.clear(buf[0]);

        if (pagesize > 0)
            uut.addParam(buf[0], "pageSize", String(pagesize), hasParam);
        if (buf[1].length() > 0)
            uut.addParam(buf[0], "pageToken", buf[1], hasParam);
        if (buf[2].length() > 0)
            uut.addParam(buf[0], "orderBy", buf[2], hasParam);
        if (strlen(msk.c_str()) > 0)
            buf[0] += msk.getQuery("mask", hasParam);
        if (buf[3].length() > 0)
            uut.addParam(buf[0], "showMissing", buf[3], hasParam);
        if (buf[4].length() > 0)
            uut.addParam(buf[0], "transaction", buf[4], hasParam);
        if (buf[5].length() > 0)
            uut.addParam(buf[0], "readTime", buf[5], hasParam);

        return *this;
    }

public:
    ListDocumentsOptions() {}

    // Optional. The maximum number of documents to return in a single response.
    // Firestore may return fewer than this value.
    ListDocumentsOptions &pageSize(int value)
    {
        pagesize = value;
        return set();
    }

    // Optional. A page token, received from a previous documents.list response.
    ListDocumentsOptions &pageToken(const String &value)
    {
        buf[1] = value;
        return set();
    }

    // Optional. The optional ordering of the documents to return.
    // For example: priority desc, __name__ desc.
    ListDocumentsOptions &orderBy(const String value)
    {
        buf[2] = value;
        return set();
    }

    // Optional. The fields to return. If not set, returns all fields.
    // If a document has a field that is not present in this mask, that field will not be returned in the response.
    ListDocumentsOptions &mask(const DocumentMask &value)
    {
        msk = value;
        return set();
    }

    // If the list should show missing documents.
    // A document is missing if it does not exist, but there are sub-documents nested underneath it. When true, such missing documents will be returned with a key but will not have fields, createTime, or updateTime set.
    // Requests with showMissing may not specify where or orderBy.
    ListDocumentsOptions &showMissing(bool value)
    {
        buf[3] = owriter.getBoolStr(value);
        return set();
    }

    // Perform the read as part of an already active transaction.
    // A base64-encoded string.
    ListDocumentsOptions &transaction(const String value)
    {
        buf[4] = value;
        return set();
    }

    // Perform the read at the provided time.
    // This must be a microsecond precision timestamp within the past one hour,or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
    ListDocumentsOptions &readTime(const String value)
    {
        buf[5] = value;
        return set();
    }
};

// Ref https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents/listCollectionIds

/**
 * This class used in Documents::listCollectionIds function represents the JSON representation of the request body.
 */
class ListCollectionIdsOptions : public BaseO4
{

public:
    ListCollectionIdsOptions() {}

    /**
     * The maximum number of results to return.
     * @param value The maximum number of results to return.
     */
    ListCollectionIdsOptions &pageSize(int value) { return wr.set<ListCollectionIdsOptions &, int>(*this, value, buf, bufSize, 1, __func__); }

    /**
     * A page token. Must be a value from ListCollectionIdsResponse.
     * @param value A page token. Must be a value from ListCollectionIdsResponse.
     */
    ListCollectionIdsOptions &pageToken(const String &value) { return wr.set<ListCollectionIdsOptions &, String>(*this, value, buf, bufSize, 2, __func__); }

    /**
     * Reads documents as they were at the given time.
     * This must be a microsecond precision timestamp within the past one hour,
     * or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     * @param value Timestamp
     */
    ListCollectionIdsOptions &readTime(const String value) { return wr.set<ListCollectionIdsOptions &, String>(*this, value, buf, bufSize, 3, __func__); }
};

// Ref https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes
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

        const struct firebase_ns::key_str_30 _Mode[ARRAY_CONTAINS + 1] PROGMEM = {"MODE_UNSPECIFIED", "ASCENDING", "DESCENDING", "ARRAY_CONTAINS"};
    }

    /**
     * A field of an index.
     */
    class IndexField : public BaseO4
    {

    public:
        IndexField() {}

        // The path of the field. Must match the field path specification described by [google.firestore.v1beta1.Document.fields][fields]. Special field path __name__ may be used by itself or at the end of a path. __type__ may be used only at the end of path.
        IndexField &fieldPath(const String &value) { return wr.set<IndexField &, String>(*this, value, buf, bufSize, 1, __func__); }

        // The field's mode.
        IndexField &mode(IndexMode::Mode value) { return wr.set<IndexField &, const char *>(*this, IndexMode::_Mode[value].text, buf, bufSize, 2, __func__); }
    };

    /**
     * An index definition.
     */
    class Index : public BaseO4
    {
    public:
        explicit Index(const String &collId = "")
        {
            if (collId.length())
                collectionId(collId);
        }

        // The collection ID to which this index applies. Required.
        Index &collectionId(const String &value) { return wr.set<Index &, String>(*this, value, buf, bufSize, 1, __func__); }

        // This value represents the item to add to an array.
        //  The field to index.
        Index &fields(const IndexField &value) { return wr.append<Index &, IndexField>(*this, value, buf, bufSize, 2, __func__); }
    };

}

// Ref https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes

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

    const struct firebase_ns::key_str_30 _QueryScope[QueryScope::COLLECTION_RECURSIVE + 1] PROGMEM = {"QUERY_SCOPE_UNSPECIFIED", "COLLECTION", "COLLECTION_GROUP", "COLLECTION_RECURSIVE"};
    const struct firebase_ns::key_str_20 _ApiScope[ApiScope::DATASTORE_MODE_API + 1] PROGMEM = {"ANY_API", "DATASTORE_MODE_API"};
    const struct firebase_ns::key_str_30 _Order[Order::DESCENDING + 1] PROGMEM = {"ORDER_UNSPECIFIED", "ASCENDING", "DESCENDING"};
    const struct firebase_ns::key_str_30 _ArrayConfig[ArrayConfig::CONTAINS + 1] PROGMEM = {"ARRAY_CONFIG_UNSPECIFIED", "CONTAINS"};

    // Ref https://firebase.google.com/docs/firestore/reference/rest/Shared.Types/FieldOperationMetadata#VectorConfig

    /**
     * The index configuration to support vector search operations
     */
    class VectorConfig : public BaseO4
    {
    public:
        // Required. The vector dimension this configuration applies to.
        // The resulting index will only include vectors of this dimension, and can be used for vector search with the same dimension.
        VectorConfig &dimension(int value) { return wr.set<VectorConfig &, int>(*this, value, buf, bufSize, 1, __func__); }

        // Indicates the vector index is a flat index.
        VectorConfig &flat() { return wr.set<VectorConfig &, const char *>(*this, "{}", buf, bufSize, 2, __func__); }
    };

    // Ref https://firebase.google.com/docs/firestore/reference/rest/Shared.Types/FieldOperationMetadata#IndexField

    /**
     * A field in an index. The fieldPath describes which field is indexed, the value_mode describes how the field value is indexed.
     */
    class IndexField : public BaseO4
    {
    public:
        IndexField() {}

        // Can be name. For single field indexes, this must match the name of the field or may be omitted.
        IndexField &fieldPath(const String &value) { return wr.set<IndexField &, String>(*this, value, buf, bufSize, 1, __func__); }

        // Union field value_mode
        // Indicates that this field supports ordering by the specified order or comparing using =, !=, <, <=, >, >=.
        IndexField &order(Order value) { return wr.set<IndexField &, const char *>(*this, _Order[value].text, buf, bufSize, 2, __func__); }

        // Union field value_mode
        // Indicates that this field supports ordering by the specified order or comparing using =, !=, <, <=, >, >=.
        IndexField &arrayConfig(ArrayConfig value) { return wr.set<IndexField &, const char *>(*this, _ArrayConfig[value].text, buf, bufSize, 2, __func__); }

        // Union field value_mode
        // Indicates that this field supports nearest neighbors and distance operations on vector.
        IndexField &vectorConfig(const VectorConfig &value) { return wr.set<IndexField &, VectorConfig>(*this, value, buf, bufSize, 2, __func__); }
    };

    /**
     * Cloud Firestore indexes enable simple and complex queries against documents in a database.
     */
    class Index : public BaseO4
    {
    public:
        Index() {}

        // Indexes with a collection query scope specified allow queries against a collection that is the child of a specific document, specified at query time, and that has the same collection id.
        Index &queryScope(QueryScope value) { return wr.set<Index &, const char *>(*this, _QueryScope[value].text, buf, bufSize, 1, __func__); }

        // Indexes with a collection query scope specified allow queries against a collection that is the child of a specific document, specified at query time, and that has the same collection id.
        Index &apiScope(ApiScope value) { return wr.set<Index &, const char *>(*this, _ApiScope[value].text, buf, bufSize, 2, __func__); }

        // This value represents the item to add to an array.
        // Add the field that supported by this index.
        Index &fields(const IndexField &value) { return wr.append<Index &, IndexField>(*this, value, buf, bufSize, 3, __func__); }

        // Obsoleted, use fields instead.
        Index &addField(const IndexField &value) { return fields(value); }
    };

}

namespace Firestore
{
    class Parent
    {
        friend class FirestoreBase;
        friend class Documents;

    private:
        String projectId;
        String databaseId;
        String documentPath;
        bool databaseIdParam = false;

    public:
        Parent() {}
        explicit Parent(const String &projectId, const String &databaseId = "")
        {
            this->projectId = projectId;
            this->databaseId = databaseId;
        }

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
        void setDocPath(const String &docPath) { documentPath = docPath; }
        const char *getDatabaseId() const { return databaseId.c_str(); }
        const char *getProjectId() const { return projectId.c_str(); }
        void setDatabaseIdParam(bool value) { databaseIdParam = value; }
        bool isDatabaseIdParam() { return databaseIdParam; }
    };

    class DataOptions
    {
        friend class FirestoreBase;

    public:
        Parent parent;
        String collectionId;
        String documentId;
        String extras;
        String payload;
        firebase_firestore_request_type requestType = cf_undefined;
        unsigned long requestTime = 0;

        void copy(const DataOptions &rhs)
        {
            this->parent = rhs.parent;
            this->collectionId = rhs.collectionId;
            this->documentId = rhs.documentId;
            this->extras = rhs.extras;
            this->payload = rhs.payload;
        }

    private:
    };

}
#endif
#endif