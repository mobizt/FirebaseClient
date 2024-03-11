
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
#ifndef FIRESTORE_FILTER_CPP
#define FIRESTORE_FILTER_CPP

#include <Arduino.h>
#include "./Config.h"

#if defined(ENABLE_FIRESTORE) && defined(ENABLE_FIRESTORE_QUERY)

#include "./firestore/Query.h"

namespace FirestoreQuery
{

    StructuredQuery::StructuredQuery() {}

    StructuredQuery &StructuredQuery::select(Projection projection) { return setObject(buf[1], "select", projection.c_str(), false, true); }

    StructuredQuery &StructuredQuery::from(CollectionSelector collSelector)
    {
        owriter.addMapArrayMember(buf, bufSize, buf[2], "from", collSelector.c_str(), false);
        return *this;
    }

    StructuredQuery &StructuredQuery::where(Filter filter) { return setObject(buf[3], "where", filter.c_str(), false, true); }

    StructuredQuery &StructuredQuery::orderBy(Order orderBy)
    {
        owriter.addMapArrayMember(buf, bufSize, buf[4], "orderBy", orderBy.c_str(), false);
        return *this;
    }

    StructuredQuery &StructuredQuery::startAt(Cursor startAt) { return setObject(buf[5], "startAt", startAt.c_str(), false, true); }

    StructuredQuery &StructuredQuery::endAt(Cursor endAt) { return setObject(buf[6], "endAt", endAt.c_str(), false, true); }

    StructuredQuery &StructuredQuery::offset(int value) { return setObject(buf[7], "offset", String(value), false, true); }

    StructuredQuery &StructuredQuery::limit(int value) { return setObject(buf[8], "limit", String(value), false, true); }

    const char *StructuredQuery::c_str() { return buf[0].c_str(); }
    size_t StructuredQuery::printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void StructuredQuery::clear() { owriter.clearBuf(buf, bufSize); }

    StructuredQuery &StructuredQuery::setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
    {
        owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
        return *this;
    }

    CompositeFilter::CompositeFilter() {}

    CompositeFilter &CompositeFilter::op(CompositFilterOperator::OPERATOR_TYPE filterOp)
    {
        using namespace CompositFilterOperator;

        if (filterOp == AND)
            return setObject(buf[1], "op", "AND", true, true);
        else if (filterOp == OR)
            return setObject(buf[1], "op", "OR", true, true);
        return *this;
    }

    CompositeFilter &CompositeFilter::addFilter(Filter filter)
    {
        owriter.addMapArrayMember(buf, bufSize, buf[2], "filters", filter.c_str(), false);
        return *this;
    }
    const char *CompositeFilter::c_str() { return buf[0].c_str(); }
    size_t CompositeFilter::printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void CompositeFilter::clear() { owriter.clearBuf(buf, bufSize); }
    CompositeFilter &CompositeFilter::setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
    {
        owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
        return *this;
    }

    FieldFilter::FieldFilter() {}

    FieldFilter &FieldFilter::setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
    {
        owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
        return *this;
    }

    FieldFilter &FieldFilter::field(FieldReference field) { return setObject(buf[1], "field", field.c_str(), false, true); }

    FieldFilter &FieldFilter::op(FieldFilterOperator::OPERATOR_TYPE filterOp)
    {
        using namespace FieldFilterOperator;

        if (filterOp == LESS_THAN)
            return setObject(buf[2], "op", "LESS_THAN", true, true);
        else if (filterOp == LESS_THAN_OR_EQUAL)
            return setObject(buf[2], "op", "LESS_THAN_OR_EQUAL", true, true);
        else if (filterOp == GREATER_THAN)
            return setObject(buf[2], "op", "GREATER_THAN", true, true);
        else if (filterOp == GREATER_THAN_OR_EQUAL)
            return setObject(buf[2], "op", "GREATER_THAN_OR_EQUAL", true, true);
        else if (filterOp == EQUAL)
            return setObject(buf[2], "op", "EQUAL", true, true);
        else if (filterOp == NOT_EQUAL)
            return setObject(buf[2], "op", "NOT_EQUAL", true, true);
        else if (filterOp == ARRAY_CONTAINS)
            return setObject(buf[2], "op", "ARRAY_CONTAINS", true, true);
        else if (filterOp == IN)
            return setObject(buf[2], "op", "IN", true, true);
        else if (filterOp == ARRAY_CONTAINS_ANY)
            return setObject(buf[2], "op", "ARRAY_CONTAINS_ANY", true, true);
        else if (filterOp == NOT_IN)
            return setObject(buf[2], "op", "NOT_IN", true, true);
        return *this;
    }

    FieldFilter &FieldFilter::value(Values::Value value) { return setObject(buf[3], "value", value.c_str(), false, true); }

    const char *FieldFilter::c_str() { return buf[0].c_str(); }
    size_t FieldFilter::printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void FieldFilter::clear() { owriter.clearBuf(buf, bufSize); }

    UnaryFilter::UnaryFilter() {}
    UnaryFilter &UnaryFilter::op(UnaryFilterOperator::OPERATOR_TYPE filterOp)
    {
        using namespace UnaryFilterOperator;

        if (filterOp == IS_NAN)
            return setObject(buf[1], "op", "IS_NAN", true, true);
        else if (filterOp == IS_NULL)
            return setObject(buf[1], "op", "IS_NULL", true, true);
        else if (filterOp == IS_NOT_NAN)
            return setObject(buf[1], "op", "IS_NOT_NAN", true, true);
        else if (filterOp == IS_NOT_NULL)
            return setObject(buf[1], "op", "IS_NOT_NULL", true, true);
        return *this;
    }
    UnaryFilter &UnaryFilter::field(FieldReference field) { return setObject(buf[2], "field", field.c_str(), false, true); }
    const char *UnaryFilter::c_str() { return buf[0].c_str(); }
    size_t UnaryFilter::printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void UnaryFilter::clear() { owriter.clearBuf(buf, bufSize); }
    UnaryFilter &UnaryFilter::setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
    {
        owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
        return *this;
    }

    Order &Order::setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
    {
        owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
        return *this;
    }

    Order::Order(){};
    Order::Order(FieldReference field, FilterSort::Direction direction)
    {
        Order::field(field);
        Order::direction(direction);
    }
    Order &Order::field(FieldReference field) { return setObject(buf[1], "field", field.c_str(), false, true); }
    Order &Order::direction(FilterSort::Direction direction)
    {
        if (direction == FilterSort::ASCENDING)
            return setObject(buf[2], "direction", "ASCENDING", true, true);
        else if (direction == FilterSort::DESCENDING)
            return setObject(buf[2], "direction", "DESCENDING", true, true);
        return *this;
    }

    const char *Order::c_str() { return buf[0].c_str(); }
    size_t Order::printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void Order::clear() { owriter.clearBuf(buf, bufSize); }

    Cursor::Cursor() { jh.addObject(buf[2], "values", "[]", false, true); }

    Cursor &Cursor::before(bool value) { return setObject(buf[1], "before", owriter.getBoolStr(value), false, true); }

    Cursor &Cursor::addValue(Values::Value value)
    {
        owriter.addMapArrayMember(buf, bufSize, buf[2], FPSTR("values"), value.c_str(), false);
        return *this;
    }

    Cursor &Cursor::setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
    {
        owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
        return *this;
    }

    const char *Cursor::c_str() { return buf[0].c_str(); }
    size_t Cursor::printTo(Print &p) const { return p.print(buf[0].c_str()); }
    void Cursor::clear() { owriter.clearBuf(buf, bufSize); }

    Filter::Filter() {}

    Filter::Filter(CompositeFilter compositeFilter)
    {
        clear();
        jh.addObject(buf, "compositeFilter", compositeFilter.c_str(), false, true);
    }

    Filter::Filter(FieldFilter fieldFilter)
    {
        clear();
        jh.addObject(buf, "fieldFilter", fieldFilter.c_str(), false, true);
    }

    Filter::Filter(UnaryFilter unaryFilter)
    {
        clear();
        jh.addObject(buf, "unaryFilter", unaryFilter.c_str(), false, true);
    }

    const char *Filter::c_str() { return buf.c_str(); }
    size_t Filter::printTo(Print &p) const { return p.print(buf.c_str()); }
    void Filter::clear() { buf.remove(0, buf.length()); }

}

#endif

#endif
