
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
        jh.addObject(sel, "select", projection.c_str(), true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::from(CollectionSelector collSelector)
    {
        if (frm_ar.length() == 0)
            jh.addArray(frm_ar, collSelector.c_str(), true);
        else
            owriter.addMember(frm_ar, collSelector.c_str(), true, "]");
        frm.remove(0, frm.length());
        jh.addObject(frm, "from", frm_ar, true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::where(Filter filter)
    {
        where_str.remove(0, where_str.length());
        jh.addObject(where_str, "where", filter.c_str(), true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::orderBy(Order orderBy)
    {
        if (ordby_ar.length() == 0)
            jh.addArray(ordby_ar, orderBy.c_str(), true);
        else
            owriter.addMember(ordby_ar, orderBy.c_str(), true, "]");

        ordby.remove(0, ordby.length());
        jh.addObject(ordby, "orderBy", ordby_ar, true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::startAt(Cursor startAt)
    {
        sta.remove(0, sta.length());
        jh.addObject(sta, "startAt", startAt.c_str(), true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::endAt(Cursor endAt)
    {
        ea.remove(0, ea.length());
        jh.addObject(ea, "endAt", endAt.c_str(), true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::offset(int value)
    {
        ofs.remove(0, ofs.length());
        jh.addObject(ofs, "offset", String(value), true);
        set();
        return *this;
    }

    StructuredQuery &StructuredQuery::limit(int value)
    {
        lim.remove(0, lim.length());
        jh.addObject(lim, "limit", String(value), true);
        set();
        return *this;
    }

    const char *StructuredQuery::c_str() { return buf.c_str(); }
    size_t StructuredQuery::printTo(Print &p) const { return p.print(buf.c_str()); }

    void StructuredQuery::set()
    {
        buf.remove(0, buf.length());
        if (sel.length())
            buf = sel;

        if (frm.length())
        {
            if (buf.length() == 0)
                buf = frm;
            else
                owriter.addMember(buf, frm, true, "}");
        }

        if (where_str.length())
        {
            if (buf.length() == 0)
                buf = where_str;
            else
                owriter.addMember(buf, where_str, true, "}");
        }

        if (ordby.length())
        {
            if (buf.length() == 0)
                buf = ordby;
            else
                owriter.addMember(buf, ordby, true, "}");
        }

        if (sta.length())
        {
            if (buf.length() == 0)
                buf = sta;
            else
                owriter.addMember(buf, sta, true, "}");
        }

        if (ea.length())
        {
            if (buf.length() == 0)
                buf = ea;
            else
                owriter.addMember(buf, ea, true, "}");
        }

        if (ofs.length())
        {
            if (buf.length() == 0)
                buf = ofs;
            else
                owriter.addMember(buf, ofs, true, "}");
        }

        if (lim.length())
        {
            if (buf.length() == 0)
                buf = lim;
            else
                owriter.addMember(buf, lim, true, "}");
        }
    }

    CompositeFilter::CompositeFilter() {}

    CompositeFilter &CompositeFilter::op(CompositFilterOperator::OPERATOR_TYPE filterOp)
    {
        using namespace CompositFilterOperator;

        op_str.remove(0, op_str.length());
        if (filterOp == AND)
            jh.addObject(op_str, "op", jh.toString("AND"), true);
        else if (filterOp == OR)
            jh.addObject(op_str, "op", jh.toString("OR"), true);
        return *this;
    }

    CompositeFilter &CompositeFilter::addFilter(Filter filter)
    {
        filter_str.remove(0, filter_str.length());
        if (filter_arr.length() == 0)
            jh.addArray(filter_arr, filter.c_str(), true);
        else
            owriter.addMember(filter_arr, filter.c_str(), false, "]");

        return *this;
    }
    const char *CompositeFilter::c_str() { return buf.c_str(); }
    size_t CompositeFilter::printTo(Print &p) const { return p.print(buf.c_str()); }

    FieldFilter::FieldFilter() {}

    FieldFilter &FieldFilter::field(FieldReference field)
    {
        jh.addObject(buf, "field", field.c_str(), true);
        return *this;
    }

    FieldFilter &FieldFilter::op(FieldFilterOperator::OPERATOR_TYPE filterOp)
    {
        using namespace FieldFilterOperator;

        if (filterOp == LESS_THAN)
            jh.addObject(buf, "op", jh.toString("LESS_THAN"), true);
        else if (filterOp == LESS_THAN_OR_EQUAL)
            jh.addObject(buf, "op", jh.toString("LESS_THAN_OR_EQUAL"), true);
        else if (filterOp == GREATER_THAN)
            jh.addObject(buf, "op", jh.toString("GREATER_THAN"), true);
        else if (filterOp == GREATER_THAN_OR_EQUAL)
            jh.addObject(buf, "op", jh.toString("GREATER_THAN_OR_EQUAL"), true);
        else if (filterOp == EQUAL)
            jh.addObject(buf, "op", jh.toString("EQUAL"), true);
        else if (filterOp == NOT_EQUAL)
            jh.addObject(buf, "op", jh.toString("NOT_EQUAL"), true);
        else if (filterOp == ARRAY_CONTAINS)
            jh.addObject(buf, "op", jh.toString("ARRAY_CONTAINS"), true);
        else if (filterOp == IN)
            jh.addObject(buf, "op", jh.toString("IN"), true);
        else if (filterOp == ARRAY_CONTAINS_ANY)
            jh.addObject(buf, "op", jh.toString("ARRAY_CONTAINS_ANY"), true);
        else if (filterOp == NOT_IN)
            jh.addObject(buf, "op", jh.toString("NOT_IN"), true);

        return *this;
    }

    FieldFilter &FieldFilter::value(Values::Value value)
    {
        jh.addObject(buf, "value", value.c_str(), true);
        return *this;
    }

    const char *FieldFilter::c_str() { return buf.c_str(); }
    size_t FieldFilter::printTo(Print &p) const { return p.print(buf.c_str()); }

    UnaryFilter::UnaryFilter() {}
    UnaryFilter &UnaryFilter::op(UnaryFilterOperator::OPERATOR_TYPE filterOp)
    {
        using namespace UnaryFilterOperator;

        op_str.remove(0, op_str.length());
        if (filterOp == IS_NAN)
            jh.addObject(op_str, "op", jh.toString("IS_NAN"), true);
        else if (filterOp == IS_NULL)
            jh.addObject(op_str, "op", jh.toString("IS_NULL"), true);
        else if (filterOp == IS_NOT_NAN)
            jh.addObject(op_str, "op", jh.toString("IS_NOT_NAN"), true);
        else if (filterOp == IS_NOT_NULL)
            jh.addObject(op_str, "op", jh.toString("IS_NOT_NULL"), true);

        set();
        return *this;
    }
    UnaryFilter &UnaryFilter::field(FieldReference field)
    {
        field_str.remove(0, field_str.length());
        jh.addObject(field_str, "field", field.c_str(), true);
        set();
        return *this;
    }
    const char *UnaryFilter::c_str() { return buf.c_str(); }
    size_t UnaryFilter::printTo(Print &p) const { return p.print(buf.c_str()); }
    void UnaryFilter::set()
    {
        buf.remove(0, buf.length());

        if (op_str.length())
        {
            if (buf.length() == 0)
                buf = op_str;
            else
                owriter.addMember(buf, op_str, true, "}");
        }

        if (field_str.length())
        {
            if (buf.length() == 0)
                buf = field_str;
            else
                owriter.addMember(buf, field_str, true, "}");
        }
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
        jh.addObject(field_str, "field", field.c_str(), true);
        set();
        return *this;
    }
    Order &Order::direction(FilterSort::Direction direction)
    {
        direction_str.remove(0, direction_str.length());
        if (direction == FilterSort::ASCENDING)
            jh.addObject(direction_str, "direction", jh.toString("ASCENDING"), true);
        else if (direction == FilterSort::DESCENDING)
            jh.addObject(direction_str, "direction", jh.toString("DESCENDING"), true);
        set();
        return *this;
    }

    const char *Order::c_str() { return buf.c_str(); }
    size_t Order::printTo(Print &p) const { return p.print(buf.c_str()); }
    void Order::set()
    {
        buf.remove(0, buf.length());
        if (field_str.length())
        {
            if (buf.length() == 0)
                buf = field_str;
            else
                owriter.addMember(buf, field_str, true, "}");
        }

        if (direction_str.length())
        {
            if (buf.length() == 0)
                buf = direction_str;
            else
                owriter.addMember(buf, direction_str, true, "}");
        }
    }

    Cursor::Cursor() {}

    Cursor &Cursor::before(bool value)
    {
        return *this;
    }

    Cursor &Cursor::addValue(Values::Value value)
    {
        return *this;
    }

    const char *Cursor::c_str() { return buf.c_str(); }
    size_t Cursor::printTo(Print &p) const { return p.print(buf.c_str()); }

    Filter::Filter() {}
    Filter::Filter(CompositeFilter compositeFilter)
    {
        jh.addObject(buf, "compositeFilter", compositeFilter.c_str(), true);
    }
    Filter::Filter(FieldFilter fieldFilter)
    {
        jh.addObject(buf, "fieldFilter", fieldFilter.c_str(), true);
    }
    Filter::Filter(UnaryFilter unaryFilter)
    {
        jh.addObject(buf, "unaryFilter", unaryFilter.c_str(), true);
    }
    const char *Filter::c_str() { return buf.c_str(); }
    size_t Filter::printTo(Print &p) const { return p.print(buf.c_str()); }

}

#endif

#endif
