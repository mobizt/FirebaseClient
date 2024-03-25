/**
 * Created March 24, 2024
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
#include "./core/ObjectWriter.h"

#if defined(ENABLE_FIRESTORE)
#include "./firestore/Values.h"
#if defined(ENABLE_FIRESTORE_QUERY)
#include "./firestore/Query.h"
using namespace FirestoreQuery;
#endif

#define RESOURCE_PATH_BASE FPSTR("<resource_path>")

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
    firebase_firestore_request_type_manage_database,

    firebase_firestore_request_type_get_doc = 300,
    firebase_firestore_request_type_list_doc,
    firebase_firestore_request_type_list_index,
    firebase_firestore_request_type_get_index,

    firebase_firestore_request_type_patch_doc = 400,

    firebase_firestore_request_type_delete_doc = 500,
    firebase_firestore_request_type_delete_index
};

/**
 * A set of field paths on a document. Used to restrict a get or update operation on a document to a subset of its fields. This is different from standard field masks, as this is always scoped to a Document, and takes in account the dynamic nature of Value.
 */
class DocumentMask : public Printable
{
    friend class FirestoreBase;
    friend class GetDocumentOptions;
    friend class PatchDocumentOptions;
    friend class ListDocumentsOptions;
    friend class Documents;

private:
    size_t bufSize = 2;
    String buf[2];
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
    DocumentMask(const String &fieldPaths = "")
    {
        setFieldPaths(fieldPaths);
    }
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
    const char *c_str() const { return buf[0].c_str(); }
    size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void clear() { owriter.clearBuf(buf, bufSize); }
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

    public:
        /**
         * Increment object to use with FieldTransform object class constructor.
         * @param value Adds the given value to the field's current value.
         */
        template <typename T>
        Increment(T value) { owriter.setPair(buf, FPSTR("increment"), value.val()); }
        const char *c_str() const { return buf.c_str(); }
        void clear() { buf.remove(0, buf.length()); }
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

    public:
        /**
         * Maximum object to use with FieldTransform object class constructor.
         * @param value Sets the field to the maximum of its current value and the given value.
         */
        template <typename T>
        Maximum(T value) { owriter.setPair(buf, FPSTR("maximum"), value.c_str()); }
        const char *c_str() const { return buf.c_str(); }
        void clear() { buf.remove(0, buf.length()); }
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
        Minimum(T value) { owriter.setPair(buf, FPSTR("minimum"), value.c_str()); }
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

    public:
        /**
         * Append the given elements in order if they are not already present in the current field value.
         * If the field is not an array, or if the field does not yet exist, it is first set to the empty array.
         * @param arrayValue The array value object to append.
         */
        AppendMissingElements(T arrayValue) { owriter.setPair(buf, FPSTR("appendMissingElements"), arrayValue.c_str()); }
        const char *c_str() const { return buf.c_str(); }
        void clear() { buf.remove(0, buf.length()); }
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
        RemoveAllFromArray(T arrayValue) { owriter.setPair(buf, FPSTR("removeAllFromArray"), arrayValue.c_str()); }
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
        SetToServerValue(ServerValue enumValue) { owriter.setPair(buf, FPSTR("setToServerValue"), jut.toString(enumValue == SERVER_VALUE_UNSPECIFIED ? FPSTR("SERVER_VALUE_UNSPECIFIED") : FPSTR("REQUEST_TIME"))); }
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
        template <typename T>
        void set(const String &fieldPath, T v)
        {
            jut.addObject(buf, FPSTR("fieldPath"), fieldPath, true);
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
        const char *c_str() const { return buf.c_str(); }
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
    friend class Documents;

private:
    size_t bufSize = 4;
    String buf[4];
    ObjectWriter owriter;
    JSONUtil jut;

    String getQuery(const String &mask)
    {
        buf[1].remove(0, buf[1].length());
        if (buf[2].length())
        {
            buf[1] = FPSTR("?");
            buf[1] += mask;
            buf[1] += FPSTR(".exists=");
            buf[1] += buf[2];
        }

        if (buf[3].length())
        {
            if (buf[1].length())
                buf[1] += '&';
            else
                buf[1] = FPSTR("?");
            buf[1] += mask;
            buf[1] += FPSTR(".updateTime=");
            buf[1] += jut.toString(buf[3]);
        }
        return buf[1];
    }

    Precondition &setObject()
    {
        if (buf[2].length())
        {
            if (buf[0].length() == 0)
                owriter.setPair(buf[0], FPSTR("exists"), buf[2]);
            else
            {
                buf[0][buf[0].length() - 1] = '\0';
                jut.addObject(buf[0], FPSTR("exists"), buf[2], true);
            }
        }

        if (buf[3].length())
        {
            if (buf[0].length() == 0)
                owriter.setPair(buf[0], FPSTR("updateTime"), buf[3]);
            else
            {
                buf[0][buf[0].length() - 1] = '\0';
                jut.addObject(buf[0], FPSTR("updateTime"), buf[3], true);
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
    const char *c_str() const { return buf[0].c_str(); }
    size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void clear() { owriter.clearBuf(buf, bufSize); }
};
/**
 * Firestore document
 */
template <typename T = Values::Value>
class Document : public Printable
{
    friend class FirestoreBase;

private:
    Values::MapValue mv;
    size_t bufSize = 4;
    String buf[4];
    ObjectWriter owriter;
    JSONUtil jut;

    Document &getBuf()
    {
        buf[2] = mv.c_str();
        buf[3].remove(0, buf[3].length());
        if (buf[1].length())
            jut.addObject(buf[3], FPSTR("name"), owriter.makeResourcePath(buf[1]), true, true);
        else
        {
            buf[0] = buf[2];
            return *this;
        }

        buf[0] = buf[3];
        if (buf[2].length())
            owriter.addMember(buf[0], buf[2], "}");

        return *this;
    }

public:
    /**
     * A Firestore document constructor with document resource name.
     * @param name The resource name of the document.
     */
    Document(const String &name = "")
    {
        buf[1] = name;
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
    void setContent(const String &content)
    {
        owriter.clearBuf(buf, bufSize);
        buf[0] = content;
    }
    const char *c_str()
    {
        getBuf();
        return buf[0].c_str();
    }
    size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
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
        firestore_database_mode_create,
        firestore_database_mode_get,
        firestore_database_mode_list,
        firestore_database_mode_patch,
        firestore_database_mode_delete
    };

    /**
     * A Cloud Firestore Database.
     */
    class Database : public Printable
    {
        friend class Databases;

    private:
        size_t bufSize = 9;
        String buf[9];
        ObjectWriter owriter;
        Database &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

        const char *getBuf() { return buf[0].c_str(); }

    public:
        void setContent(const String &content)
        {
            owriter.clearBuf(buf, bufSize);
            buf[0] = content;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }

        /**
         * Set the concurrency control mode to use for this database (used in database creation).
         * @param concurrencyMode The ConcurrencyMode enum.
         */
        Database &concurrencyMode(ConcurrencyMode concurrencyMode)
        {
            if (concurrencyMode == ConcurrencyMode::CONCURRENCY_MODE_UNSPECIFIED)
                return setObject(buf[1], "concurrencyMode", "CONCURRENCY_MODE_UNSPECIFIED", true, true);
            else if (concurrencyMode == ConcurrencyMode::OPTIMISTIC)
                return setObject(buf[1], "concurrencyMode", "OPTIMISTIC", true, true);
            else if (concurrencyMode == ConcurrencyMode::PESSIMISTIC)
                return setObject(buf[1], "concurrencyMode", "PESSIMISTIC", true, true);
            else if (concurrencyMode == ConcurrencyMode::OPTIMISTIC_WITH_ENTITY_GROUPS)
                return setObject(buf[1], "concurrencyMode", "OPTIMISTIC_WITH_ENTITY_GROUPS", true, true);
            return *this;
        }

        /**
         * Set the App Engine integration mode to use for this database (used in database creation).
         * @param appEngineIntegrationMode The AppEngineIntegrationMode enum.
         */
        Database &appEngineIntegrationMode(AppEngineIntegrationMode appEngineIntegrationMode)
        {
            if (appEngineIntegrationMode == AppEngineIntegrationMode::APP_ENGINE_INTEGRATION_MODE_UNSPECIFIED)
                return setObject(buf[2], "appEngineIntegrationMode", "APP_ENGINE_INTEGRATION_MODE_UNSPECIFIED", true, true);
            else if (appEngineIntegrationMode == AppEngineIntegrationMode::ENABLED)
                return setObject(buf[2], "appEngineIntegrationMode", "ENABLED", true, true);
            else if (appEngineIntegrationMode == AppEngineIntegrationMode::_DISABLED)
                return setObject(buf[2], "appEngineIntegrationMode", "DISABLED", true, true);
            return *this;
        }
        /**
         * Set the state of delete protection for the database (used in database creation).
         * @param deleteProtectionState The DeleteProtectionState enum.
         */
        Database &deleteProtectionState(DeleteProtectionState deleteProtectionState)
        {
            if (deleteProtectionState == DeleteProtectionState::DELETE_PROTECTION_STATE_UNSPECIFIED)
                return setObject(buf[3], "deleteProtectionState", "DELETE_PROTECTION_STATE_UNSPECIFIED", true, true);
            else if (deleteProtectionState == DeleteProtectionState::DELETE_PROTECTION_ENABLED)
                return setObject(buf[3], "deleteProtectionState", "DELETE_PROTECTION_ENABLED", true, true);
            else if (deleteProtectionState == DeleteProtectionState::DELETE_PROTECTION_DISABLED)
                return setObject(buf[3], "deleteProtectionState", "DELETE_PROTECTION_DISABLED", true, true);
            return *this;
        }
        /**
         * Set to enable the PITR feature on this database (used in database creation).
         * @param pointInTimeRecoveryEnablement The PointInTimeRecoveryEnablement enum.
         */
        Database &pointInTimeRecoveryEnablement(PointInTimeRecoveryEnablement pointInTimeRecoveryEnablement)
        {
            if (pointInTimeRecoveryEnablement == PointInTimeRecoveryEnablement::POINT_IN_TIME_RECOVERY_ENABLEMENT_UNSPECIFIED)
                return setObject(buf[4], "pointInTimeRecoveryEnablement", "POINT_IN_TIME_RECOVERY_ENABLEMENT_UNSPECIFIED", true, true);
            else if (pointInTimeRecoveryEnablement == PointInTimeRecoveryEnablement::POINT_IN_TIME_RECOVERY_ENABLED)
                return setObject(buf[4], "pointInTimeRecoveryEnablement", "POINT_IN_TIME_RECOVERY_ENABLED", true, true);
            else if (pointInTimeRecoveryEnablement == PointInTimeRecoveryEnablement::POINT_IN_TIME_RECOVERY_DISABLED)
                return setObject(buf[4], "pointInTimeRecoveryEnablement", "POINT_IN_TIME_RECOVERY_DISABLED", true, true);
            return *this;
        }

        /**
         * Set the type of the database (used in database creation). See https://cloud.google.com/datastore/docs/firestore-or-datastore for information about how to choose.
         * @param databaseType The DatabaseType enum.
         */
        Database &databaseType(DatabaseType databaseType)
        {
            if (databaseType == DatabaseType::DATABASE_TYPE_UNSPECIFIED)
                return setObject(buf[5], "type", "DATABASE_TYPE_UNSPECIFIED", true, true);
            else if (databaseType == DatabaseType::FIRESTORE_NATIVE)
                return setObject(buf[5], "type", "FIRESTORE_NATIVE", true, true);
            else if (databaseType == DatabaseType::DATASTORE_MODE)
                return setObject(buf[5], "type", "DATASTORE_MODE", true, true);
            return *this;
        }

        /**
         * Set the location of the database (used in database creation).
         * Available locations are listed at https://cloud.google.com/firestore/docs/locations.
         * @param value The location Id.
         */
        Database &locationId(const String &value) { return setObject(buf[6], "locationId", value, true, true); }

        /**
         * Set the resource name of the Database (used in database creation).
         * Format: projects/{project}/databases/{database}
         * @param value The resource name.
         */
        Database &name(const String &value) { return setObject(buf[7], "name", value, true, true); }

        /**
         * Set the ETag (used in database update and deletion)
         * This checksum is computed by the server based on the value of other fields, and may be sent on update and delete requests to ensure the client has an up-to-date value before proceeding.
         * @param value The ETag.
         */
        Database &etag(const String &value) { return setObject(buf[8], "etag", value, true, true); }
    };

}

/**
 * A transformation of a document
 */
class DocumentTransform : public Printable
{
    friend class FirestoreBase;
    friend class Write;

private:
    String buf;
    ObjectWriter owriter;
    JSONUtil jut;

public:
    /**
     * A transformation of a document.
     * @param document The name of the document to transform.
     * @param fieldTransforms The list of transformations to apply to the fields of the document, in order. This must not be empty.
     */
    DocumentTransform(const String &document, FieldTransform::FieldTransform fieldTransforms)
    {
        jut.addObject(buf, FPSTR("document"), owriter.makeResourcePath(document), true);
        jut.addObject(buf, FPSTR("fieldTransforms"), fieldTransforms.c_str(), false, true);
    }
    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
};

/**
 * A write on a document.
 */
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
    Write(DocumentMask updateMask, Document<Values::Value> update, Precondition currentDocument)
    {
        bool curdoc = strlen(currentDocument.c_str());
        bool updatemask = strlen(updateMask.c_str());
        write_type = firestore_write_type_update;
        jut.addObject(buf, FPSTR("update"), update.c_str(), false, !updatemask && !curdoc);
        if (updatemask)
            jut.addObject(buf, FPSTR("updateMask"), updateMask.c_str(), false, !curdoc);
        if (curdoc)
            jut.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str(), false, true);
    }
    /**
     * A write on a document.
     * @param deletePath A document name to delete.
     */
    Write(const String &deletePath, Precondition currentDocument)
    {
        write_type = firestore_write_type_delete;
        if (strlen(currentDocument.c_str()))
            jut.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str(), false);
        jut.addObject(buf, FPSTR("delete"), owriter.makeResourcePath(deletePath), true, true);
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
            jut.addObject(buf, FPSTR("currentDocument"), currentDocument.c_str(), false);
        jut.addObject(buf, FPSTR("transform"), transform.c_str(), false, true);
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
                jut.addObject(buf, FPSTR("updateTransforms"), owriter.getArrayStr(updateTransforms.c_str()), false, true);
            }
            else
                owriter.addMember(buf, updateTransforms.c_str(), "]}");
            updateTrans = true;
        }

        return *this;
    }
    void setContent(const String &content) { buf = content; }
    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
};
/**
 * Class that represent the object that contains a write or list of writes, transaction and label to use with batch write.
 */
class Writes : public Printable
{
private:
    String buf;
    JSONUtil jut;
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
            jut.addObject(buf, FPSTR("transaction"), transaction, true);
        jut.addObject(buf, FPSTR("writes"), owriter.getArrayStr(write.c_str()), false, true);
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
            jut.addObject(buf, FPSTR("labels"), labels.c_str(), false);
        jut.addObject(buf, FPSTR("writes"), owriter.getArrayStr(write.c_str()), false, true);
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
    void setContent(const String &content) { buf = content; }
    const char *c_str() const { return buf.c_str(); }
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
    JSONUtil jut;

public:
    /**
     * @param retryTransaction An optional transaction to retry.
     * A base64-encoded string.
     */
    ReadWrite(const String &retryTransaction)
    {
        if (retryTransaction.length())
            jut.addObject(buf, FPSTR("retryTransaction"), retryTransaction, true, true);
    }
    const char *c_str() const { return buf.c_str(); }
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
    JSONUtil jut;

public:
    ReadOnly() {}
    /**
     * @param readTime Timestamp. Reads documents at the given time.
     */
    ReadOnly(const String &readTime)
    {
        if (readTime.length())
            jut.addObject(buf, "readTime", readTime, true, true);
    }
    const char *c_str() const { return buf.c_str(); }
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
    JSONUtil jut;

public:
    TransactionOptions() {}
    /**
     * @param readOnly The transaction can only be used for read operations.
     */
    TransactionOptions(ReadOnly readOnly)
    {
        if (strlen(readOnly.c_str()))
            jut.addObject(buf, "readOnly", readOnly.c_str(), false, true);
    }
    /**
     * @param readWrite The transaction can be used for both read and write operations.
     */
    TransactionOptions(ReadWrite readWrite)
    {
        if (strlen(readWrite.c_str()))
            jut.addObject(buf, "readWrite", readWrite.c_str(), false, true);
    }
    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
};

class EximDocumentOptions : public Printable
{
private:
    String buf;
    URLUtil uut;
    JSONUtil json;

public:
    EximDocumentOptions(const String &collectionIds, const String &bucketID, const String &storagePath)
    {
        String uriPrefix;
        uut.addGStorageURL(uriPrefix, bucketID, storagePath);
        json.addObject(buf, "inputUriPrefix", uriPrefix, true);
        json.addTokens(buf, "collectionIds", collectionIds, true);
    }
    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
};

class GetDocumentOptions : public Printable
{
private:
    String buf;
    URLUtil uut;

public:
    GetDocumentOptions() {}
    GetDocumentOptions(DocumentMask mask, const String &transaction = "", const String &readTime = "")
    {
        bool hasParam = false;
        if (strlen(mask.c_str()))
            buf = mask.getQuery("mask", hasParam);
        if (transaction.length())
            uut.addParam(buf, FPSTR("transaction"), transaction, hasParam);
        if (readTime.length())
            uut.addParam(buf, FPSTR("readTime"), readTime, hasParam);
    }
    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
};

class BatchGetDocumentOptions : public Printable
{
private:
    size_t bufSize = 6;
    String buf[6];
    ObjectWriter owriter;

    void setObject(String &buf_n, const String &key, const String &value, bool isString, bool last) { owriter.setObject(buf, bufSize, buf_n, key, value, isString, last); }

public:
    BatchGetDocumentOptions() {}
    /**
     * @param document The names of the documents to retrieve.
     * The item or value will be added to the array or list.
     * To add more items, call this function again.
     */
    void addDocument(const String &document) { owriter.addMapArrayMember(buf, bufSize, buf[1], "documents", owriter.makeResourcePath(document, true), false); }
    /**
     * @param mask The fields to return. If not set, returns all fields.
     */
    void mask(DocumentMask mask) { setObject(buf[2], "mask", mask.c_str(), false, true); }
    /**
     * @param transaction Timestamp Reads documents in a transaction.
     */
    void transaction(const String &transaction)
    {
        // Union field
        if (transaction.length() && buf[4].length() == 0 && buf[5].length() == 0)
            setObject(buf[3], "transaction", transaction, true, true);
    }
    /**
     * @param transOptions Starts a new transaction and reads the documents. Defaults to a read-only transaction.
     * The new transaction ID will be returned as the first response in the stream.
     */
    void newTransaction(TransactionOptions transOptions)
    {
        // Union field
        if (strlen(transOptions.c_str()) && buf[3].length() == 0 && buf[5].length() == 0)
            setObject(buf[4], "newTransaction", transOptions.c_str(), false, true);
    }
    /**
     * @param readTime Timestamp. Reads documents as they were at the given time.
     */
    void readTime(const String &readTime)
    {
        // Union field
        if (readTime.length() && buf[3].length() == 0 && buf[4].length() == 0)
            setObject(buf[5], "readTime", readTime, true, true);
    }
    void setContent(const String &content)
    {
        owriter.clearBuf(buf, bufSize);
        buf[0] = content;
    }
    const char *c_str() const { return buf[0].c_str(); }
    size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void clear() { owriter.clearBuf(buf, bufSize); }
};

class PatchDocumentOptions : public Printable
{
private:
    String buf;
    URLUtil uut;

public:
    PatchDocumentOptions(DocumentMask updateMask, DocumentMask mask, Precondition currentDocument)
    {
        bool hasParam = false;
        if (strlen(updateMask.c_str()))
            buf = updateMask.getQuery("updateMask", hasParam);
        if (strlen(mask.c_str()))
            buf += mask.getQuery("mask", hasParam);
        if (strlen(currentDocument.c_str()))
            uut.addParam(buf, FPSTR("currentDocument"), currentDocument.c_str(), hasParam);
    }
    void setContent(const String &content) { buf = content; }
    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    void clear() { buf.remove(0, buf.length()); }
};

#if defined(ENABLE_FIRESTORE_QUERY)

class QueryOptions : public Printable
{
private:
    size_t bufSize = 5;
    String buf[5];
    ObjectWriter owriter;

    void setObject(String &buf_n, const String &key, const String &value, bool isString, bool last) { owriter.setObject(buf, bufSize, buf_n, key, value, isString, last); }

public:
    QueryOptions() {}

    /**
     * A structured query.
     * @param structuredQuery A structured query.
     */
    void structuredQuery(StructuredQuery structuredQuery) { setObject(buf[1], "newTransaction", structuredQuery.c_str(), false, true); }
    /**
     * @param transaction Run the query within an already active transaction.
     */
    void transaction(const String &transaction)
    {
        // Union field
        if (transaction.length() && buf[3].length() == 0 && buf[4].length() == 0)
            setObject(buf[2], "transaction", transaction, true, true);
    }
    /**
     * @param transOptions Starts a new transaction and reads the documents. Defaults to a read-only transaction.
     * The new transaction ID will be returned as the first response in the stream.
     */
    void newTransaction(TransactionOptions transOptions)
    {
        // Union field
        if (strlen(transOptions.c_str()) && buf[2].length() == 0 && buf[4].length() == 0)
            setObject(buf[3], "newTransaction", transOptions.c_str(), false, true);
    }
    /**
     * @param readTime Timestamp. Reads documents as they were at the given time.
     * This must be a microsecond precision timestamp within the past one hour,
     * or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     */
    void readTime(const String &readTime)
    {
        // Union field
        if (readTime.length() && buf[2].length() == 0 && buf[3].length() == 0)
            setObject(buf[4], "readTime", readTime, true, true);
    }
    void setContent(const String &content)
    {
        owriter.clearBuf(buf, bufSize);
        buf[0] = content;
    }
    const char *c_str() const { return buf[0].c_str(); }
    size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void clear() { owriter.clearBuf(buf, bufSize); }
};

#endif

class ListDocumentsOptions : public Printable
{
private:
    size_t bufSize = 6;
    String buf[6];
    int pagesize = 0;
    DocumentMask msk;
    ObjectWriter owriter;
    URLUtil uut;

    ListDocumentsOptions &set()
    {
        bool hasParam = false;
        owriter.clear(buf[0]);

        if (pagesize > 0)
            uut.addParam(buf[0], FPSTR("pageSize"), String(pagesize), hasParam);
        if (buf[1].length() > 0)
            uut.addParam(buf[0], FPSTR("pageToken"), buf[1], hasParam);
        if (buf[2].length() > 0)
            uut.addParam(buf[0], FPSTR("orderBy"), buf[2], hasParam);
        if (strlen(msk.c_str()) > 0)
            buf[0] += msk.getQuery("mask", hasParam);
        if (buf[3].length() > 0)
            uut.addParam(buf[0], FPSTR("showMissing"), buf[3], hasParam);
        if (buf[4].length() > 0)
            uut.addParam(buf[0], FPSTR("transaction"), buf[4], hasParam);
        if (buf[5].length() > 0)
            uut.addParam(buf[0], FPSTR("readTime"), buf[5], hasParam);

        return *this;
    }

public:
    ListDocumentsOptions() {}
    /**
     * Optional. The maximum number of documents to return in a single response.
     * Firestore may return fewer than this value.
     * @param size The maximum number of documents to return in a single response.
     */
    ListDocumentsOptions &pageSize(int size)
    {
        pagesize = size;
        return set();
    }
    /**
     * Optional. A page token, received from a previous documents.list response.
     * @param pageToken Optional. A page token, received from a previous documents.list response.
     */
    ListDocumentsOptions &pageToken(const String &pageToken)
    {
        buf[1] = pageToken;
        return set();
    }
    /**
     * Optional. The optional ordering of the documents to return.
     * For example: priority desc, __name__ desc.
     * @param orderBy
     */
    ListDocumentsOptions &orderBy(const String orderBy)
    {
        buf[2] = orderBy;
        return set();
    }
    /**
     * Optional. The fields to return. If not set, returns all fields.
     * If a document has a field that is not present in this mask, that field will not be returned in the response.
     * @param mask The fields to return.
     */
    ListDocumentsOptions &mask(DocumentMask mask)
    {
        msk = mask;
        return set();
    }
    /**
     * If the list should show missing documents.
     * A document is missing if it does not exist, but there are sub-documents nested underneath it. When true, such missing documents will be returned with a key but will not have fields, createTime, or updateTime set.
     * Requests with showMissing may not specify where or orderBy.
     * @param value
     */
    ListDocumentsOptions &showMissing(bool value)
    {
        buf[3] = owriter.getBoolStr(value);
        return set();
    }
    /**
     * Perform the read as part of an already active transaction.
     * A base64-encoded string.
     * @param transaction
     */
    ListDocumentsOptions &transaction(const String transaction)
    {
        buf[4] = transaction;
        return set();
    }
    /**
     * Perform the read at the provided time.
     * This must be a microsecond precision timestamp within the past one hour,
     * or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     * @param readTime
     */
    ListDocumentsOptions &readTime(const String readTime)
    {
        buf[5] = readTime;
        return set();
    }
    void setContent(const String &content)
    {
        owriter.clearBuf(buf, bufSize);
        buf[0] = content;
    }
    const char *c_str() const { return buf[0].c_str(); }
    size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void clear()
    {
        owriter.clearBuf(buf, bufSize);
        msk.clear();
    }
};

class ListCollectionIdsOptions : public Printable
{
private:
    size_t bufSize = 4;
    String buf[4];
    ObjectWriter owriter;

    ListCollectionIdsOptions &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
    {
        owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
        return *this;
    }

public:
    ListCollectionIdsOptions() {}
    /**
     * The maximum number of results to return.
     * @param pageSize The maximum number of results to return.
     */
    ListCollectionIdsOptions &pageSize(int pageSize) { return setObject(buf[1], FPSTR("pageSize"), String(pageSize), false, true); }
    /**
     * A page token. Must be a value from ListCollectionIdsResponse.
     * @param pageToken A page token. Must be a value from ListCollectionIdsResponse.
     */
    ListCollectionIdsOptions &pageToken(const String &pageToken) { return setObject(buf[2], FPSTR("pageToken"), pageToken, true, true); }
    /**
     * Reads documents as they were at the given time.
     * This must be a microsecond precision timestamp within the past one hour,
     * or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     * @param readTime Timestamp
     */
    ListCollectionIdsOptions &readTime(const String readTime) { return setObject(buf[3], FPSTR("readTime"), readTime, true, true); }
    void setContent(const String &content)
    {
        owriter.clearBuf(buf, bufSize);
        buf[0] = content;
    }
    const char *c_str() const { return buf[0].c_str(); }
    size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void clear() { owriter.clearBuf(buf, bufSize); }
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
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;

        IndexField &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        IndexField() {}
        /**
         * The path of the field. Must match the field path specification described by
         * [google.firestore.v1beta1.Document.fields][fields]. Special field path __name__
         * may be used by itself or at the end of a path. __type__ may be used only at the end of path.
         * @param fieldPath
         */
        IndexField &fieldPath(const String &fieldPath) { return setObject(buf[1], "fieldPath", fieldPath, true, true); }
        /**
         * The field's mode.
         * @param mode
         */
        IndexField &mode(IndexMode::Mode mode)
        {
            if (mode == IndexMode::ASCENDING)
                return setObject(buf[2], "mode", "ASCENDING", true, true);
            else if (mode == IndexMode::DESCENDING)
                return setObject(buf[2], "mode", "DESCENDING", true, true);
            else if (mode == IndexMode::ARRAY_CONTAINS)
                return setObject(buf[2], "mode", "ARRAY_CONTAINS", true, true);
            return *this;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * An index definition.
     */
    class Index : public Printable
    {
    private:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;

        Index &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
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
        Index &collectionId(const String &collectionId) { return setObject(buf[1], FPSTR("collectionId"), collectionId, true, true); }
        /**
         * Add the field to index.
         * @param field the field to index.
         * The item or value will be added to the array or list.
         * To add more items, call this function again.
         */
        Index &addField(IndexField field)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[2], FPSTR("fields"), field.c_str(), false);
            return *this;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
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
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;

        VectorConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        /**
         * Required. The vector dimension this configuration applies to.
         * The resulting index will only include vectors of this dimension, and can be used for vector search with the same dimension.
         * @param value the field to index.
         */
        VectorConfig &dimension(int value) { return setObject(buf[1], "dimension", String(value), false, true); }
        /**
         * Indicates the vector index is a flat index.
         */
        VectorConfig &flat() { return setObject(buf[2], "flat", "{}", false, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * A field in an index. The fieldPath describes which field is indexed, the value_mode describes how the field value is indexed.
     */
    class IndexField : public Printable
    {
    private:
        size_t bufSize = 5;
        String buf[5];
        ObjectWriter owriter;

        IndexField &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        IndexField() {}
        /**
         * Can be name. For single field indexes, this must match the name of the field or may be omitted.
         * @param fieldPath
         */
        IndexField &fieldPath(const String &fieldPath) { return setObject(buf[1], "fieldPath", fieldPath, true, true); }
        /**
         * Indicates that this field supports ordering by the specified order or comparing using =, !=, <, <=, >, >=.
         * @param order
         */
        IndexField &order(Order order)
        {
            // Union field
            if (buf[3].length() == 0 && buf[4].length() == 0)
            {
                if (order == Order::ORDER_UNSPECIFIED)
                    return setObject(buf[2], "order", "ORDER_UNSPECIFIED", true, true);
                else if (order == Order::DESCENDING)
                    return setObject(buf[2], "order", "DESCENDING", true, true);
                else if (order == Order::ASCENDING)
                    return setObject(buf[2], "order", "ASCENDING", true, true);
            }
            return *this;
        }

        /**
         * Indicates that this field supports ordering by the specified order or comparing using =, !=, <, <=, >, >=.
         * @param arrayConfig
         */
        IndexField &arrayConfig(ArrayConfig arrayConfig)
        {
            // Union field
            if (buf[2].length() == 0 && buf[4].length() == 0)
            {
                if (arrayConfig == ArrayConfig::ARRAY_CONFIG_UNSPECIFIED)
                    return setObject(buf[3], "arrayConfig", "ARRAY_CONFIG_UNSPECIFIED", true, true);
                else if (arrayConfig == ArrayConfig::CONTAINS)
                    return setObject(buf[3], "arrayConfig", "CONTAINS", true, true);
            }
            return *this;
        }

        /**
         * Indicates that this field supports nearest neighbors and distance operations on vector.
         * @param vectorConfig
         */
        IndexField &vectorConfig(VectorConfig vectorConfig)
        {
            // Union field
            if (buf[2].length() == 0 && buf[3].length() == 0)
                return setObject(buf[4], "vectorConfig", vectorConfig.c_str(), false, true);
            return *this;
        }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * Cloud Firestore indexes enable simple and complex queries against documents in a database.
     */
    class Index : public Printable
    {
    private:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;

        Index &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
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
                return setObject(buf[1], "queryScope", "QUERY_SCOPE_UNSPECIFIED", true, true);
            else if (queryScope == QueryScope::COLLECTION)
                return setObject(buf[1], "queryScope", "COLLECTION", true, true);
            else if (queryScope == QueryScope::COLLECTION_GROUP)
                return setObject(buf[1], "queryScope", "COLLECTION_GROUP", true, true);
            else if (queryScope == QueryScope::COLLECTION_RECURSIVE)
                return setObject(buf[1], "queryScope", "COLLECTION_RECURSIVE", true, true);
            return *this;
        }

        /**
         * Indexes with a collection query scope specified allow queries against a collection that is the child of a specific document, specified at query time, and that has the same collection id.
         * @param apiScope The query scope enum.
         */
        Index &apiScope(ApiScope apiScope)
        {
            if (apiScope == ApiScope::ANY_API)
                return setObject(buf[2], "apiScope", "ANY_API", true, true);
            else if (apiScope == ApiScope::DATASTORE_MODE_API)
                return setObject(buf[2], "apiScope", "DATASTORE_MODE_API", true, true);
            return *this;
        }

        /**
         * Add the field that supported by this index.
         * @param field the field to add.
         * The item or value will be added to the array or list.
         * To add more items, call this function again.
         */
        Index &addField(IndexField field)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[3], FPSTR("fields"), field.c_str(), false);
            return *this;
        }
        void setContent(const String &content)
        {
            owriter.clearBuf(buf, bufSize);
            buf[0] = content;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
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
        Parent(const String &projectId, const String &databaseId = "")
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
        String getDatabaseId() const { return databaseId.c_str(); }
        String getProjectId() const { return projectId.c_str(); }
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
        firebase_firestore_request_type requestType = firebase_firestore_request_type_undefined;
        unsigned long requestTime = 0;

        void copy(DataOptions &rhs)
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