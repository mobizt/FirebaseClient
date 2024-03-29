/**
 * Created March 11, 2024
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
#ifndef FIRESTORE_QUERY_H
#define FIRESTORE_QUERY_H

#include <Arduino.h>
#include "./Config.h"

#if defined(ENABLE_FIRESTORE) && defined(ENABLE_FIRESTORE_QUERY)

#include "./firestore/Values.h"

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
        JSONUtil jut;

    public:
        /**
         * @param fieldPath A reference to a field in a document.
         */
        FieldReference(const String &fieldPath)
        {
            jut.addObject(buf, "fieldPath", fieldPath, true, true);
        }

        const char *c_str() const { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    };

    /**
     * The projection of document's fields to return.
     */
    class Projection : public Printable
    {
    private:
        size_t bufSize = 2;
        String buf[2];
        ObjectWriter owriter;
        void addField(FieldReference &field) { owriter.addMapArrayMember(buf, bufSize, buf[1], FPSTR("fields"), field.c_str(), false); }

    public:
        /**
         * @param fieldPath A reference to a field in a document.
         */
        Projection(FieldReference field)
        {
            addField(field);
        }
        /**
         *  Add FieldReference to Projection
         * @param fieldPath A reference to a field in a document.
         */
        Projection &add(FieldReference field)
        {
            addField(field);
            return *this;
        }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * An order on a field.
     */
    class Order : public Printable
    {
    private:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;
        Order &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last);

    public:
        Order();
        Order(FieldReference field, FilterSort::Direction direction);
        Order &field(FieldReference field);
        Order &direction(FilterSort::Direction direction);
        const char *c_str();
        size_t printTo(Print &p) const;
        void clear();
    };

    class CollectionSelector : public Printable
    {
    private:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;
        JSONUtil jut;

        CollectionSelector &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        CollectionSelector() {}
        CollectionSelector(const String &collectionId, bool allDescendants)
        {
            CollectionSelector::collectionId(collectionId);
            CollectionSelector::allDescendants(allDescendants);
        }
        CollectionSelector &collectionId(const String &collectionId) { return setObject(buf[1], "collectionId", collectionId, true, true); }
        CollectionSelector &allDescendants(bool value) { return setObject(buf[2], "allDescendants", owriter.getBoolStr(value), false, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * A position in a query result set.
     */
    class Cursor : public Printable
    {
    private:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;
        JSONUtil jut;
        Cursor &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last);

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
        void clear();
    };

    /**
     * A Firestore query.
     */
    class StructuredQuery : public Printable
    {
    private:
        size_t bufSize = 9;
        String buf[9];
        ObjectWriter owriter;
        StructuredQuery &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last);

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
        void clear();
    };

    /**
     * A filter that merges multiple other filters using the given operator.
     */
    class CompositeFilter : public Printable
    {
    private:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        JSONUtil jut;
        CompositeFilter &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last);

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
        void clear();
    };

    /**
     * A filter on a specific field.
     */
    class FieldFilter : public Printable
    {
    private:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        JSONUtil jut;

        FieldFilter &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last);

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
        void clear();
    };
    /**
     * A filter with a single operand.
     */
    class UnaryFilter : public Printable
    {

    private:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;

        UnaryFilter &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last);

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
        void clear();
    };

    /**
     * A filter.
     */
    class Filter : public Printable
    {
    private:
        String buf;
        ObjectWriter owriter;
        JSONUtil jut;

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
        void clear();
    };

}

#endif

#endif
