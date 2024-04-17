
/**
 * Created April 3, 2024
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
#ifndef FIRESTORE_FILTER_CPP
#define FIRESTORE_FILTER_CPP

#include <Arduino.h>
#include "./Config.h"

#if defined(ENABLE_FIRESTORE) && defined(ENABLE_FIRESTORE_QUERY)

#include "./firestore/Query.h"

namespace FirestoreQuery
{

    StructuredQuery::StructuredQuery() {}
    StructuredQuery &StructuredQuery::select(const Projection &value) { return wr.set<StructuredQuery &, Projection>(*this, value, buf, bufSize, 1, FPSTR(__func__)); }
    StructuredQuery &StructuredQuery::from(const CollectionSelector &value) { return wr.append<StructuredQuery &, CollectionSelector>(*this, value, buf, bufSize, 2, FPSTR(__func__)); }
    StructuredQuery &StructuredQuery::where(const Filter &value) { return wr.set<StructuredQuery &, Filter>(*this, value, buf, bufSize, 3, FPSTR(__func__)); }
    StructuredQuery &StructuredQuery::orderBy(const Order &value) { return wr.append<StructuredQuery &, Order>(*this, value, buf, bufSize, 4, FPSTR(__func__)); }
    StructuredQuery &StructuredQuery::startAt(const Cursor &value) { return wr.set<StructuredQuery &, Cursor>(*this, value, buf, bufSize, 5, FPSTR(__func__)); }
    StructuredQuery &StructuredQuery::endAt(const Cursor &value) { return wr.set<StructuredQuery &, Cursor>(*this, value, buf, bufSize, 6, FPSTR(__func__)); }
    StructuredQuery &StructuredQuery::offset(int value) { return wr.set<StructuredQuery &, int>(*this, value, buf, bufSize, 7, FPSTR(__func__)); }
    StructuredQuery &StructuredQuery::limit(int value) { return wr.set<StructuredQuery &, int>(*this, value, buf, bufSize, 8, FPSTR(__func__)); }

    CompositeFilter::CompositeFilter() {}
    CompositeFilter &CompositeFilter::op(CompositFilterOperator::OPERATOR_TYPE value) { return wr.set<CompositeFilter &, const char *>(*this, CompositFilterOperator::_OPERATOR_TYPE[value].text, buf, bufSize, 1, FPSTR(__func__)); }
    CompositeFilter &CompositeFilter::filters(const Filter &value) { return wr.append<CompositeFilter &, Filter>(*this, value, buf, bufSize, 2, FPSTR(__func__)); }
    CompositeFilter &CompositeFilter::addFilter(const Filter &value) { return filters(value); }

    FieldFilter::FieldFilter() {}
    FieldFilter &FieldFilter::field(const FieldReference &value) { return wr.set<FieldFilter &, FieldReference>(*this, value, buf, bufSize, 1, FPSTR(__func__)); }
    FieldFilter &FieldFilter::op(FieldFilterOperator::OPERATOR_TYPE value) { return wr.set<FieldFilter &, const char *>(*this, FieldFilterOperator::_OPERATOR_TYPE[value].text, buf, bufSize, 2, FPSTR(__func__)); }
    FieldFilter &FieldFilter::value(const Values::Value &value) { return wr.set<FieldFilter &, Values::Value>(*this, value, buf, bufSize, 3, FPSTR(__func__)); }

    UnaryFilter::UnaryFilter() {}
    UnaryFilter &UnaryFilter::op(UnaryFilterOperator::OPERATOR_TYPE value) { return wr.set<UnaryFilter &, const char *>(*this, UnaryFilterOperator::_OPERATOR_TYPE[value].text, buf, bufSize, 1, FPSTR(__func__)); }
    UnaryFilter &UnaryFilter::field(const FieldReference &value) { return wr.set<UnaryFilter &, FieldReference>(*this, value, buf, bufSize, 2, FPSTR(__func__)); }

    Order::Order() {}
    Order::Order(const FieldReference &field, FilterSort::Direction direction) { Order::field(field).direction(direction); }
    Order &Order::field(const FieldReference &value) { return wr.set<Order &, FieldReference>(*this, value, buf, bufSize, 1, FPSTR(__func__)); }
    Order &Order::direction(FilterSort::Direction value) { return wr.set<Order &, const char *>(*this, FilterSort::_Direction[value].text, buf, bufSize, 2, FPSTR(__func__)); }

    Cursor::Cursor() { wr.set<Cursor &, const char *>(*this, "[]", buf, bufSize, 2, "values"); }
    Cursor &Cursor::before(bool value) { return wr.set<Cursor &, bool>(*this, value, buf, bufSize, 1, FPSTR(__func__)); }
    Cursor &Cursor::values(const Values::Value &value) { return wr.append<Cursor &, Values::Value>(*this, value, buf, bufSize, 2, FPSTR(__func__)); }
    Cursor &Cursor::addValue(const Values::Value &value) { return values(value); }

    Filter::Filter() {}
    Filter::Filter(const CompositeFilter &value) { wr.add<Filter &, CompositeFilter>(*this, value, buf, "compositeFilter"); }
    Filter::Filter(const FieldFilter &value) { wr.add<Filter &, FieldFilter>(*this, value, buf, "fieldFilter"); }
    Filter::Filter(const UnaryFilter &value) { wr.add<Filter &, UnaryFilter>(*this, value, buf, "unaryFilter"); }

}

#endif

#endif
