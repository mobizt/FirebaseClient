/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FIRESTORE_QUERY_CPP
#define FIRESTORE_QUERY_CPP

#include <Arduino.h>

#if defined(ENABLE_FIRESTORE) && defined(ENABLE_FIRESTORE_QUERY)

#include "./firestore/Query.h"
namespace FirestoreQuery
{
    StructuredQuery::StructuredQuery() {}
    StructuredQuery &StructuredQuery::select(const Projection &value) { return wr.set<StructuredQuery &, Projection>(*this, value, buf, bufSize, 1, __func__); }
    StructuredQuery &StructuredQuery::from(const CollectionSelector &value) { return wr.append<StructuredQuery &, CollectionSelector>(*this, value, buf, bufSize, 2, __func__); }
    StructuredQuery &StructuredQuery::where(const Filter &value) { return wr.set<StructuredQuery &, Filter>(*this, value, buf, bufSize, 3, __func__); }
    StructuredQuery &StructuredQuery::orderBy(const Order &value) { return wr.append<StructuredQuery &, Order>(*this, value, buf, bufSize, 4, __func__); }
    StructuredQuery &StructuredQuery::startAt(const Cursor &value) { return wr.set<StructuredQuery &, Cursor>(*this, value, buf, bufSize, 5, __func__); }
    StructuredQuery &StructuredQuery::endAt(const Cursor &value) { return wr.set<StructuredQuery &, Cursor>(*this, value, buf, bufSize, 6, __func__); }
    StructuredQuery &StructuredQuery::offset(int value) { return wr.set<StructuredQuery &, int>(*this, value, buf, bufSize, 7, __func__); }
    StructuredQuery &StructuredQuery::limit(int value) { return wr.set<StructuredQuery &, int>(*this, value, buf, bufSize, 8, __func__); }

    CompositeFilter::CompositeFilter() {}
    CompositeFilter &CompositeFilter::op(CompositFilterOperator::OPERATOR_TYPE value) { return wr.set<CompositeFilter &, const char *>(*this, CompositFilterOperator::_OPERATOR_TYPE[value].text, buf, bufSize, 1, __func__); }
    CompositeFilter &CompositeFilter::filters(const Filter &value) { return wr.append<CompositeFilter &, Filter>(*this, value, buf, bufSize, 2, __func__); }
    CompositeFilter &CompositeFilter::addFilter(const Filter &value) { return filters(value); }

    FieldFilter::FieldFilter() {}
    FieldFilter &FieldFilter::field(const FieldReference &value) { return wr.set<FieldFilter &, FieldReference>(*this, value, buf, bufSize, 1, __func__); }
    FieldFilter &FieldFilter::op(FieldFilterOperator::OPERATOR_TYPE value) { return wr.set<FieldFilter &, const char *>(*this, FieldFilterOperator::_OPERATOR_TYPE[value].text, buf, bufSize, 2, __func__); }
    FieldFilter &FieldFilter::value(const Values::Value &value) { return wr.set<FieldFilter &, Values::Value>(*this, value, buf, bufSize, 3, __func__); }

    UnaryFilter::UnaryFilter() {}
    UnaryFilter &UnaryFilter::op(UnaryFilterOperator::OPERATOR_TYPE value) { return wr.set<UnaryFilter &, const char *>(*this, UnaryFilterOperator::_OPERATOR_TYPE[value].text, buf, bufSize, 1, __func__); }
    UnaryFilter &UnaryFilter::field(const FieldReference &value) { return wr.set<UnaryFilter &, FieldReference>(*this, value, buf, bufSize, 2, __func__); }

    Order::Order() {}
    Order::Order(const FieldReference &field, FilterSort::Direction direction) { Order::field(field).direction(direction); }
    Order &Order::field(const FieldReference &value) { return wr.set<Order &, FieldReference>(*this, value, buf, bufSize, 1, __func__); }
    Order &Order::direction(FilterSort::Direction value) { return wr.set<Order &, const char *>(*this, FilterSort::_Direction[value].text, buf, bufSize, 2, __func__); }

    Cursor::Cursor() { wr.set<Cursor &, const char *>(*this, "[]", buf, bufSize, 2, "values"); }
    Cursor &Cursor::before(bool value) { return wr.set<Cursor &, bool>(*this, value, buf, bufSize, 1, __func__); }
    Cursor &Cursor::values(const Values::Value &value) { return wr.append<Cursor &, Values::Value>(*this, value, buf, bufSize, 2, __func__); }
    Cursor &Cursor::addValue(const Values::Value &value) { return values(value); }

    Filter::Filter() {}
    Filter::Filter(const CompositeFilter &value) { wr.add<Filter &, CompositeFilter>(*this, value, buf, "compositeFilter"); }
    Filter::Filter(const FieldFilter &value) { wr.add<Filter &, FieldFilter>(*this, value, buf, "fieldFilter"); }
    Filter::Filter(const UnaryFilter &value) { wr.add<Filter &, UnaryFilter>(*this, value, buf, "unaryFilter"); }
}
#endif
#endif