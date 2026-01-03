/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FIRESTORE_QUERY_H
#define FIRESTORE_QUERY_H

#include <Arduino.h>

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

        const struct firebase_ns::key_str_30 _OPERATOR_TYPE[OPERATOR_TYPE::OR + 1] PROGMEM = {"OPERATOR_UNSPECIFIED", "AND", "OR"};
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

        const struct firebase_ns::key_str_30 _OPERATOR_TYPE[OPERATOR_TYPE::IS_NOT_NULL + 1] PROGMEM = {"OPERATOR_UNSPECIFIED", "IS_NAN", "IS_NULL", "IS_NOT_NAN", "IS_NOT_NULL"};

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

        const struct firebase_ns::key_str_30 _OPERATOR_TYPE[OPERATOR_TYPE::NOT_IN + 1] PROGMEM = {"OPERATOR_UNSPECIFIED", "LESS_THAN", "LESS_THAN_OR_EQUAL", "GREATER_THAN", "GREATER_THAN_OR_EQUAL", "EQUAL", "NOT_EQUAL", "ARRAY_CONTAINS", "IN", "ARRAY_CONTAINS_ANY", "NOT_IN"};

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
        const struct firebase_ns::key_str_30 _Direction[Direction::DESCENDING + 1] PROGMEM = {"DIRECTION_UNSPECIFIED", "ASCENDING", "DESCENDING"};
    }

    /**
     * A reference to a field in a document, ex: stats.operations.
     */
    class FieldReference : public BaseO1
    {
    public:
        // A reference to a field in a document.
        explicit FieldReference(const String &value) { fieldPath(value); }

        // A reference to a field in a document.
        FieldReference &fieldPath(const String &value) { return wr.add<FieldReference &, String>(*this, value, buf, __func__); }
    };

    /**
     * The projection of document's fields to return.
     */
    class Projection : public BaseO2
    {
    public:
        // A reference to a field in a document.
        explicit Projection(const FieldReference &value) { Projection::fields(value); }

        // This value represents the item to add to an array.
        // A reference to a field in a document.
        Projection &fields(const FieldReference &value) { return wr.append<Projection &, FieldReference>(*this, value, buf, bufSize, 1, __func__); }

        // Obsoleted, use fields instead.
        Projection &add(const FieldReference &value) { return fields(value); }
    };

    /**
     * An order on a field.
     */
    class Order : public BaseO4
    {
    private:
        ObjectWriter owriter;

    public:
        Order();
        explicit Order(const FieldReference &field, FilterSort::Direction direction);

        // The field to order by.
        Order &field(const FieldReference &value);

        // The direction to order by. Defaults to ASCENDING.
        Order &direction(FilterSort::Direction value);
    };

    /**
     * A selection of a collection, such as messages as m1.
     */
    class CollectionSelector : public BaseO4
    {
    public:
        CollectionSelector() {}
        explicit CollectionSelector(const String &collectionId, bool allDescendants) { CollectionSelector::collectionId(collectionId).allDescendants(allDescendants); }

        // The collection ID. When set, selects only collections with this ID.
        CollectionSelector &collectionId(const String &value) { return wr.set<CollectionSelector &, String>(*this, value, buf, bufSize, 1, __func__); }

        // When false, selects only collections that are immediate children of the parent specified in the containing RunQueryRequest. When true, selects all descendant collections.
        CollectionSelector &allDescendants(bool value) { return wr.set<CollectionSelector &, bool>(*this, value, buf, bufSize, 2, __func__); }
    };

    /**
     * A position in a query result set.
     */
    class Cursor : public BaseO4
    {
    public:
        Cursor();

        // The befor option value
        // If the position is just before or just after the given values, relative to the sort order defined by the query.
        Cursor &before(bool value);

        // This value represents the item to add to an array.
        // The value that represent a position, in the order they appear in the order by clause of a query.
        // Can contain fewer values than specified in the order by clause.
        Cursor &values(const Values::Value &value);

        // Obsoleted, use values instead.
        Cursor &addValue(const Values::Value &value);
    };

    /**
     * A Firestore query.
     */
    class StructuredQuery : public BaseO12
    {
    public:
        StructuredQuery();

        // Optional sub-set of the fields to return.
        // This acts as a DocumentMask over the documents returned from a query. When not set, assumes that the caller wants all fields returned.
        StructuredQuery &select(const Projection &value);

        // This value represents the item to add to an array.
        // The collections to query.
        StructuredQuery &from(const CollectionSelector &value);

        // The filter to apply.
        StructuredQuery &where(const Filter &value);

        // This value represents the item to add to an array.
        // The order to apply to the query results.
        // Firestore allows callers to provide a full ordering, a partial ordering, or no ordering at all. In all cases, Firestore guarantees a stable ordering through the following rules:
        //
        // The orderBy is required to reference all fields used with an inequality filter.
        // All fields that are required to be in the orderBy but are not already present are appended in lexicographical ordering of the field name.
        // If an order on __name__ is not specified, it is appended by default.
        // Fields are appended with the same sort direction as the last order specified, or 'ASCENDING' if no order was specified. For example:
        //
        // ORDER BY a becomes ORDER BY a ASC, __name__ ASC
        // ORDER BY a DESC becomes ORDER BY a DESC, __name__ DESC
        // WHERE a > 1 becomes WHERE a > 1 ORDER BY a ASC, __name__ ASC
        // WHERE __name__ > ... AND a > 1 becomes WHERE __name__ > ... AND a > 1 ORDER BY a ASC, __name__ ASC
        StructuredQuery &orderBy(const Order &value);

        // A potential prefix of a position in the result set to start the query at.
        // The ordering of the result set is based on the ORDER BY clause of the original query.
        // SELECT * FROM k WHERE a = 1 AND b > 2 ORDER BY b ASC, __name__ ASC;
        StructuredQuery &startAt(const Cursor &value);

        // A potential prefix of a position in the result set to end the query at.
        // This is similar to START_AT but with it controlling the end position rather than the start position.
        StructuredQuery &endAt(const Cursor &endAt);

        // The number of documents to skip before returning the first result.
        // This applies after the constraints specified by the WHERE, START AT, & END AT but before the LIMIT clause.
        StructuredQuery &offset(int value);

        // The maximum number of results to return.
        // Applies after all other constraints.
        StructuredQuery &limit(int value);
    };

    /**
     * A filter that merges multiple other filters using the given operator.
     */
    class CompositeFilter : public BaseO4
    {
    public:
        CompositeFilter();
        // The operator for combining multiple filters.
        CompositeFilter &op(CompositFilterOperator::OPERATOR_TYPE value);

        // This value represents the item to add to an array.
        //  The Filter.
        CompositeFilter &filters(const Filter &filter);

        // Obsoleted, use fields instead.
        CompositeFilter &addFilter(const Filter &filter);
    };

    /**
     * A filter on a specific field.
     */
    class FieldFilter : public BaseO4
    {
    public:
        FieldFilter();

        // The field to filter by.
        FieldFilter &field(const FieldReference &value);

        // The operator to filter by.
        FieldFilter &op(FieldFilterOperator::OPERATOR_TYPE value);

        // The value to compare to.
        FieldFilter &value(const Values::Value &value);
    };
    /**
     * A filter with a single operand.
     */
    class UnaryFilter : public BaseO4
    {
    public:
        UnaryFilter();

        // The unary operator to apply.
        UnaryFilter &op(UnaryFilterOperator::OPERATOR_TYPE value);

        // The field to which to apply the operator.
        UnaryFilter &field(const FieldReference &value);
    };

    /**
     * A filter.
     */
    class Filter : public BaseO1
    {
    public:
        Filter();

        // A composite filter.
        explicit Filter(const CompositeFilter &value);

        // A filter on a document field.
        explicit Filter(const FieldFilter &fieldFilter);

        // A filter that takes exactly one argument.
        explicit Filter(const UnaryFilter &unaryFilter);
    };

}
#endif
#endif