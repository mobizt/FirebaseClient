/**
 * Created February 17, 2024
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
#ifndef FIRESTORE_FILTER_H
#define FIRESTORE_FILTER_H

#include <Arduino.h>
#include "./Config.h"

#if defined(ENABLE_FIRESTORE)

#include "./firestore/FSUT.h"

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

namespace FirestoreQuery
{
    class Filter;

    namespace CompositFilterOperator
    {
        /**
         * A composite filter operator.
         */
        enum OPERATOR_TYPE
        {
            OPERATOR_UNSPECIFIED, // Unspecified. This value must not be used.
            AND,                  // Documents are required to satisfy all of the combined filters.
            OR                    // Documents are required to satisfy at least one of the combined filters.
        };
    }

    namespace UnaryFilterOperator
    {
        /**
         * A unary operator.
         */
        enum OPERATOR_TYPE
        {
            OPERATOR_UNSPECIFIED,
            IS_NAN,     // Unspecified. This value must not be used.
            IS_NULL,    // The given field is equal to NULL.
            IS_NOT_NAN, // The given field is not equal to NaN.
            IS_NOT_NULL // The given field is not equal to NULL.

        };
    }

    namespace FieldFilterOperator
    {
        /**
         * A field filter operator.
         */
        enum OPERATOR_TYPE
        {
            OPERATOR_UNSPECIFIED,  // Unspecified. This value must not be used.
            LESS_THAN,             // The given field is less than the given value.
            LESS_THAN_OR_EQUAL,    // The given field is less than or equal to the given value.
            GREATER_THAN,          // The given field is greater than the given value.
            GREATER_THAN_OR_EQUAL, // The given field is greater than or equal to the given value.
            EQUAL,                 // The given field is equal to the given value.
            NOT_EQUAL,             // The given field is not equal to the given value.
            ARRAY_CONTAINS,        // The given field is an array that contains the given value.
            IN,                    // The given field is equal to at least one value in the given array.
            ARRAY_CONTAINS_ANY,    // The given field is an array that contains any of the values in the given array.
            NOT_IN                 // The value of the field is not in the given array.
        };
    }

    namespace FilterSort
    {
        /**
         * A sort direction.
         */
        enum Direction
        {
            DIRECTION_UNSPECIFIED, // Unspecified.
            ASCENDING,             // Ascending.
            DESCENDING             // Descending.
        };
    }

    /**
     * A reference to a field in a document, ex: stats.operations.
     */
    class FieldReference : public Printable
    {
    private:
        String buf;
        JsonHelper jh;

    public:
        /**
         * @param fieldPath A reference to a field in a document.
         */
        FieldReference(const String &fieldPath)
        {
            jh.addObject(buf, "fieldPath", jh.toString(fieldPath), true);
        }

        const char *c_str() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    };

    /**
     * The projection of document's fields to return.
     */
    class Projection : public Printable
    {
    private:
        String buf, str;
        FSUT fsut;
        JsonHelper jh;
        void set(FieldReference &field)
        {
            if (str.length() == 0)
                jh.addArray(str, field.c_str(), true);
            else
                fsut.addMember(str, field.c_str(), false, "]");
            buf.remove(0, buf.length());
            jh.addObject(buf, "fields", str, true);
        }

    public:
        /**
         * @param fieldPath A reference to a field in a document.
         */
        Projection(FieldReference field)
        {
            set(field);
        }
        /**
         *  Add FieldReference to Projection
         * @param fieldPath A reference to a field in a document.
         */
        Projection &add(FieldReference field)
        {
            set(field);
            return *this;
        }

        const char *c_str() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    };

    /**
     * An order on a field.
     */
    class Order : public Printable
    {
    private:
        String buf, field_str, direction_str;
        FSUT fsut;
        JsonHelper jh;
        void set();

    public:
        Order();
        Order(FieldReference field, FilterSort::Direction direction);
        Order &field(FieldReference field);
        Order &direction(FilterSort::Direction direction);
        const char *c_str();
        size_t printTo(Print &p) const;
    };

    class CollectionSelector : public Printable
    {
    private:
        String buf, collId, allDes;
        FSUT fsut;
        JsonHelper jh;

        void setBuf()
        {
            buf.remove(0, buf.length());
            if (collId.length())
            {
                if (buf.length() == 0)
                    buf = collId;
                else
                    fsut.addMember(buf, collId, true, "}");
            }

            if (allDes.length())
            {
                if (buf.length() == 0)
                    buf = allDes;
                else
                    fsut.addMember(buf, allDes, true, "}");
            }
        }

    public:
        CollectionSelector() {}
        CollectionSelector(const String &collectionId, bool allDescendants)
        {
            CollectionSelector::collectionId(collectionId);
            CollectionSelector::allDescendants(allDescendants);
        }
        CollectionSelector &collectionId(const String &collectionId)
        {
            collId.remove(0, collId.length());
            jh.addObject(collId, "collectionId", jh.toString(collectionId), true);
            setBuf();
            return *this;
        }
        CollectionSelector &allDescendants(bool value)
        {
            allDes.remove(0, allDes.length());
            jh.addObject(allDes, "allDescendants", fsut.getBoolStr(value), true);
            setBuf();
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    };

    /**
     * A position in a query result set.
     */
    class Cursor : public Printable
    {
    private:
        String buf, field_str, direction_str;
        FSUT fsut;
        JsonHelper jh;
        void set();

    public:
        Cursor();
        /**
         * @param value The befor option value
         * If the position is just before or just after the given values, relative to the sort order defined by the query.
         */
        Cursor &before(bool value);
        /**
         * Add Value
         * @param value The value that represent a position, in the order they appear in the order by clause of a query.
         * Can contain fewer values than specified in the order by clause.
         */
        Cursor &addValue(Values::Value value);
        const char *c_str();
        size_t printTo(Print &p) const;
    };

    /**
     * A Firestore query.
     */
    class StructuredQuery : public Printable
    {
    private:
        String buf, sel, frm, frm_ar, where_str, ordby, ordby_ar, sta, ea, ofs, lim;
        FSUT fsut;
        JsonHelper jh;
        void set();

    public:
        StructuredQuery();
        /**
         * Optional sub-set of the fields to return.
         * This acts as a DocumentMask over the documents returned from a query.
         * When not set, assumes that the caller wants all fields returned.
         * @param projection The projection of document's fields to return.
         */
        StructuredQuery &select(Projection projection);
        /**
         * The collections to query.
         * @param collSelector A selection of a collection, such as messages as m1.
         * Call this multiple times for each collection.
         */
        StructuredQuery &from(CollectionSelector collSelector);

        /**
         * The filter to apply.
         * @param filter A filter.
         */
        StructuredQuery &where(Filter filter);

        /**
         * The order to apply to the query results.
         * @param orderBy An order on a field.
         * Call this multiple times for each order.
         */
        StructuredQuery &orderBy(Order orderBy);

        /**
         * A potential prefix of a position in the result set to start the query at.
         * @param startAt A position in a query result set.
         */
        StructuredQuery &startAt(Cursor startAt);

        /**
         * A potential prefix of a position in the result set to end the query at.
         * @param endAt A position in a query result set.
         */
        StructuredQuery &endAt(Cursor endAt);

        /**
         * The number of documents to skip before returning the first result.
         * @param value The number of documents to skip before returning the first result.
         */
        StructuredQuery &offset(int value);

        /**
         * The maximum number of results to return.
         * @param value The maximum number of results to return.
         */
        StructuredQuery &limit(int value);

        const char *c_str();
        size_t printTo(Print &p) const;
    };

    /**
     * A filter that merges multiple other filters using the given operator.
     */
    class CompositeFilter : public Printable
    {
    private:
        String buf, op_str, filter_str, filter_arr;
        FSUT fsut;
        JsonHelper jh;

    public:
        CompositeFilter();
        /**
         * The operator for combining multiple filters.
         * @param op The operator for combining multiple filters.
         */
        CompositeFilter &op(CompositFilterOperator::OPERATOR_TYPE filterOp);
        /**
         * Add filter.
         * @param filter The Filter.
         */
        CompositeFilter &addFilter(Filter filter);
        const char *c_str();
        size_t printTo(Print &p) const;
    };

    /**
     * A filter on a specific field.
     */
    class FieldFilter : public Printable
    {
    private:
        String buf;
        FSUT fsut;
        JsonHelper jh;

    public:
        FieldFilter();
        /**
         * The field to filter by.
         * @param field The field to filter by.
         */
        FieldFilter &field(FieldReference field);
        /**
         * The operator to filter by.
         * @param filterOp The operator to filter by.
         */
        FieldFilter &op(FieldFilterOperator::OPERATOR_TYPE filterOp);
        /**
         * The value to compare to.
         * @param value The value to compare to.
         */
        FieldFilter &value(Values::Value value);
        const char *c_str();
        size_t printTo(Print &p) const;
    };
    /**
     * A filter with a single operand.
     */
    class UnaryFilter : public Printable
    {

    private:
        String buf, op_str, field_str;
        FSUT fsut;
        JsonHelper jh;

        void set();

    public:
        UnaryFilter();
        /**
         * The unary operator to apply.
         * @param filterOp The unary operator to apply.
         *
         */
        UnaryFilter &op(UnaryFilterOperator::OPERATOR_TYPE filterOp);
        /**
         * The field to which to apply the operator.
         * @param field The field to which to apply the operator.
         *
         */
        UnaryFilter &field(FieldReference field);
        const char *c_str();
        size_t printTo(Print &p) const;
    };

    /**
     * A filter.
     */
    class Filter : public Printable
    {
    private:
        String buf;
        FSUT fsut;
        JsonHelper jh;

    public:
        Filter();
        /**
         * A composite filter.
         * @param compositeFilter A composite filter.
         */
        Filter(CompositeFilter compositeFilter);
        /**
         * A filter on a document field.
         * @param fieldFilter A filter on a document field.
         */
        Filter(FieldFilter fieldFilter);
        /**
         * A filter that takes exactly one argument.
         * @param unaryFilter A filter that takes exactly one argument.
         */
        Filter(UnaryFilter unaryFilter);
        const char *c_str();
        size_t printTo(Print &p) const;
    };

}

#endif

#endif
