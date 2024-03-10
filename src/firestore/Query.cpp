
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
#ifndef FIRESTORE_FILTER_CPP
#define FIRESTORE_FILTER_CPP

#include <Arduino.h>
#include "./Config.h"

#if defined(ENABLE_FIRESTORE) && defined(ENABLE_FIRESTORE_QUERY)

#include "./firestore/Query.h"

namespace FirestoreQuery
{

    StructuredQuery::StructuredQuery() {}

    StructuredQuery &StructuredQuery::select(Projection projection)
    {
        sel.remove(0, sel.length());
        jh.addObject(sel, "select", projection.c_str(), false, true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::from(CollectionSelector collSelector)
    {
        if (frm_ar.length() == 0)
            jh.addArray(frm_ar, collSelector.c_str(), false, true);
        else
            owriter.addMember(frm_ar, collSelector.c_str(), "]");
        frm.remove(0, frm.length());
        jh.addObject(frm, "from", frm_ar, false, true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::where(Filter filter)
    {
        where_str.remove(0, where_str.length());
        jh.addObject(where_str, "where", filter.c_str(), false, true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::orderBy(Order orderBy)
    {
        if (ordby_ar.length() == 0)
            jh.addArray(ordby_ar, orderBy.c_str(), false, true);
        else
            owriter.addMember(ordby_ar, orderBy.c_str(), "]");

        ordby.remove(0, ordby.length());
        jh.addObject(ordby, "orderBy", ordby_ar, false, true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::startAt(Cursor startAt)
    {
        sta.remove(0, sta.length());
        jh.addObject(sta, "startAt", startAt.c_str(), false, true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::endAt(Cursor endAt)
    {
        ea.remove(0, ea.length());
        jh.addObject(ea, "endAt", endAt.c_str(), false, true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::offset(int value)
    {
        ofs.remove(0, ofs.length());
        jh.addObject(ofs, "offset", String(value), false, true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::limit(int value)
    {
        lim.remove(0, lim.length());
        jh.addObject(lim, "limit", String(value), false, true);
        set();
        return *this;
    }

    const char *StructuredQuery::c_str() { return buf.c_str(); }
    size_t StructuredQuery::printTo(Print &p) const { return p.print(buf.c_str()); }
    void StructuredQuery::clear()
    {
        owriter.clear(buf);
        owriter.clear(sel);
        owriter.clear(frm);
        owriter.clear(frm_ar);
        owriter.clear(where_str);
        owriter.clear(ordby);
        owriter.clear(ordby_ar);
        owriter.clear(sta);
        owriter.clear(ea);
        owriter.clear(ofs);
        owriter.clear(lim);
    }

    void StructuredQuery::set()
    {
        buf.remove(0, buf.length());
        if (sel.length())
            buf = sel;

        owriter.addObject(buf, frm, "}");
        owriter.addObject(buf, where_str, "}");
        owriter.addObject(buf, ordby, "}");
        owriter.addObject(buf, sta, "}");
        owriter.addObject(buf, ea, "}");
        owriter.addObject(buf, ofs, "}");
        owriter.addObject(buf, lim, "}");
    }

    CompositeFilter::CompositeFilter() {}

    CompositeFilter &CompositeFilter::op(CompositFilterOperator::OPERATOR_TYPE filterOp)
    {
        using namespace CompositFilterOperator;

        op_str.remove(0, op_str.length());
        if (filterOp == AND)
            jh.addObject(op_str, "op", "AND", true, true);
        else if (filterOp == OR)
            jh.addObject(op_str, "op", "OR", true, true);
        return *this;
    }

    CompositeFilter &CompositeFilter::addFilter(Filter filter)
    {
        filter_str.remove(0, filter_str.length());
        if (filter_arr.length() == 0)
            jh.addArray(filter_arr, filter.c_str(), false, true);
        else
            owriter.addMember(filter_arr, filter.c_str(), "]");

        return *this;
    }
    const char *CompositeFilter::c_str() { return buf.c_str(); }
    size_t CompositeFilter::printTo(Print &p) const { return p.print(buf.c_str()); }
    void CompositeFilter::clear()
    {
        buf.remove(0, buf.length());
        filter_str.remove(0, filter_str.length());
        op_str.remove(0, op_str.length());
        filter_arr.remove(0, filter_arr.length());
    }

    FieldFilter::FieldFilter() {}

    FieldFilter &FieldFilter::field(FieldReference field)
    {
        jh.addObject(buf, "field", field.c_str(), false, true);
        return *this;
    }

    FieldFilter &FieldFilter::op(FieldFilterOperator::OPERATOR_TYPE filterOp)
    {
        using namespace FieldFilterOperator;

        if (filterOp == LESS_THAN)
            jh.addObject(buf, "op", "LESS_THAN", true, true);
        else if (filterOp == LESS_THAN_OR_EQUAL)
            jh.addObject(buf, "op", "LESS_THAN_OR_EQUAL", true, true);
        else if (filterOp == GREATER_THAN)
            jh.addObject(buf, "op", "GREATER_THAN", true, true);
        else if (filterOp == GREATER_THAN_OR_EQUAL)
            jh.addObject(buf, "op", "GREATER_THAN_OR_EQUAL", true, true);
        else if (filterOp == EQUAL)
            jh.addObject(buf, "op", "EQUAL", true, true);
        else if (filterOp == NOT_EQUAL)
            jh.addObject(buf, "op", "NOT_EQUAL", true, true);
        else if (filterOp == ARRAY_CONTAINS)
            jh.addObject(buf, "op", "ARRAY_CONTAINS", true, true);
        else if (filterOp == IN)
            jh.addObject(buf, "op", "IN", true, true);
        else if (filterOp == ARRAY_CONTAINS_ANY)
            jh.addObject(buf, "op", "ARRAY_CONTAINS_ANY", true, true);
        else if (filterOp == NOT_IN)
            jh.addObject(buf, "op", "NOT_IN", true, true);

        return *this;
    }

    FieldFilter &FieldFilter::value(Values::Value value)
    {
        jh.addObject(buf, "value", value.c_str(), false, true);
        return *this;
    }

    const char *FieldFilter::c_str() { return buf.c_str(); }
    size_t FieldFilter::printTo(Print &p) const { return p.print(buf.c_str()); }
    void FieldFilter::clear()
    {
        buf.remove(0, buf.length());
    }

    UnaryFilter::UnaryFilter() {}
    UnaryFilter &UnaryFilter::op(UnaryFilterOperator::OPERATOR_TYPE filterOp)
    {
        using namespace UnaryFilterOperator;

        op_str.remove(0, op_str.length());
        if (filterOp == IS_NAN)
            jh.addObject(op_str, "op", "IS_NAN", true, true);
        else if (filterOp == IS_NULL)
            jh.addObject(op_str, "op", "IS_NULL", true, true);
        else if (filterOp == IS_NOT_NAN)
            jh.addObject(op_str, "op", "IS_NOT_NAN", true, true);
        else if (filterOp == IS_NOT_NULL)
            jh.addObject(op_str, "op", "IS_NOT_NULL", true, true);

        set();
        return *this;
    }
    UnaryFilter &UnaryFilter::field(FieldReference field)
    {
        field_str.remove(0, field_str.length());
        jh.addObject(field_str, "field", field.c_str(), false, true);
        set();
        return *this;
    }
    const char *UnaryFilter::c_str() { return buf.c_str(); }
    size_t UnaryFilter::printTo(Print &p) const { return p.print(buf.c_str()); }
    void UnaryFilter::clear()
    {
        owriter.clear(buf);
        owriter.clear(op_str);
        owriter.clear(field_str);
    }
    void UnaryFilter::set()
    {
        owriter.addObject(buf, op_str, "}", true);
        owriter.addObject(buf, field_str, "}");
    }

    Order::Order(){};
    Order::Order(FieldReference field, FilterSort::Direction direction)
    {
        Order::field(field);
        Order::direction(direction);
    }
    Order &Order::field(FieldReference field)
    {
        field_str.remove(0, field_str.length());
        jh.addObject(field_str, "field", field.c_str(), false, true);
        set();
        return *this;
    }
    Order &Order::direction(FilterSort::Direction direction)
    {
        direction_str.remove(0, direction_str.length());
        if (direction == FilterSort::ASCENDING)
            jh.addObject(direction_str, "direction", "ASCENDING", true, true);
        else if (direction == FilterSort::DESCENDING)
            jh.addObject(direction_str, "direction", "DESCENDING", true, true);
        set();
        return *this;
    }

    const char *Order::c_str() { return buf.c_str(); }
    size_t Order::printTo(Print &p) const { return p.print(buf.c_str()); }
    void Order::clear()
    {
        owriter.clear(buf);
        owriter.clear(field_str);
        owriter.clear(direction_str);
    }
    void Order::set()
    {
        owriter.addObject(buf, field_str, "}", true);
        owriter.addObject(buf, direction_str, "}");
    }

    Cursor::Cursor()
    {
        jh.addArray(value_ar_str, "", false, true);
        value_str.remove(0, value_str.length());
        jh.addObject(value_str, "values", value_ar_str, false, true);
        value_ar_str.remove(0, value_ar_str.length());
        set();
    }

    Cursor &Cursor::before(bool value)
    {
        before_str.remove(0, before_str.length());
        jh.addObject(before_str, "before", owriter.getBoolStr(value), false, true);
        set();
        return *this;
    }

    Cursor &Cursor::addValue(Values::Value value)
    {
        if (value_ar_str.length() == 0)
            jh.addArray(value_ar_str, value.c_str(), false, true);
        else
            owriter.addMember(value_ar_str, value.c_str(), "]");

        value_str.remove(0, value_str.length());
        jh.addObject(value_str, "values", value_ar_str, false, true);

        set();

        return *this;
    }

    void Cursor::set()
    {
        buf.remove(0, buf.length());

        if (value_str.length())
            buf = value_str;

        owriter.addObject(buf, before_str, "}");
    }

    const char *Cursor::c_str() { return buf.c_str(); }
    size_t Cursor::printTo(Print &p) const { return p.print(buf.c_str()); }
    void Cursor::clear()
    {
        owriter.clear(buf);
        owriter.clear(before_str);
        owriter.clear(value_str);
        owriter.clear(value_ar_str);
    }

    Filter::Filter() {}
    Filter::Filter(CompositeFilter compositeFilter)
    {
        jh.addObject(buf, "compositeFilter", compositeFilter.c_str(), false, true);
    }
    Filter::Filter(FieldFilter fieldFilter)
    {
        jh.addObject(buf, "fieldFilter", fieldFilter.c_str(), false, true);
    }
    Filter::Filter(UnaryFilter unaryFilter)
    {
        jh.addObject(buf, "unaryFilter", unaryFilter.c_str(), false, true);
    }
    const char *Filter::c_str() { return buf.c_str(); }
    size_t Filter::printTo(Print &p) const { return p.print(buf.c_str()); }
    void Filter::clear()
    {
        buf.remove(0, buf.length());
    }

}

#endif

#endif
