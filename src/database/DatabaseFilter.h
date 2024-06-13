/**
 * Created March 13, 2024
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
#ifndef DATABASE_FILTER_H
#define DATABASE_FILTER_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/URL.h"

#if defined(ENABLE_DATABASE)

class DatabaseFilter
{
    friend class DatabaseOptions;
    friend class RealtimeDatabase;
    friend class AsyncClientClass;

private:
    void copy(const DatabaseFilter &rhs)
    {
        this->uri = rhs.uri;
        this->complete = rhs.complete;
    }
    void toString(String &buf)
    {
        String v = "\"";
        v += buf;
        buf = v;
        buf += '"';
    }
    bool hasParams = true;
    bool complete = false;
    URLUtil uut;
    String uri;

public:
    DatabaseFilter() {}
    ~DatabaseFilter() { clear(); };

    DatabaseFilter &orderBy(const String &val)
    {
        complete = true;
        String ob = "\"";
        ob += val;
        ob += "\"";
        uut.addParam(uri, FPSTR("orderBy"), ob, hasParams);
        return *this;
    }

    template <typename T = int>
    DatabaseFilter &limitToFirst(T val)
    {
        uut.addParam(uri, FPSTR("limitToFirst"), String(val), hasParams);
        return *this;
    }

    template <typename T = int>
    DatabaseFilter &limitToLast(T val)
    {
        uut.addParam(uri, FPSTR("limitToLast"), String(val), hasParams);
        return *this;
    }

    DatabaseFilter &startAt(const String &val)
    {
        String sa = "\"";
        sa += val;
        sa += "\"";
        uut.addParam(uri, FPSTR("startAt"), sa, hasParams);
        return *this;
    }

    DatabaseFilter &startAt(int val)
    {
        uut.addParam(uri, FPSTR("startAt"), String(val), hasParams);
        return *this;
    }

    DatabaseFilter &endAt(const String &val)
    {
        String sa = "\"";
        sa += val;
        sa += "\"";
        uut.addParam(uri, FPSTR("endAt"), sa, hasParams);
        return *this;
    }

    DatabaseFilter &endAt(int val)
    {
        uut.addParam(uri, FPSTR("endAt"), String(val), hasParams);
        return *this;
    }

    DatabaseFilter &equalTo(const String &val)
    {
        String sa = "\"";
        sa += val;
        sa += "\"";
        uut.addParam(uri, FPSTR("equalTo"), sa, hasParams);
        return *this;
    }

    DatabaseFilter &equalTo(int val)
    {
        uut.addParam(uri, FPSTR("equalTo"), String(val), hasParams);
        return *this;
    }

    DatabaseFilter &clear()
    {
        uri.remove(0, uri.length());
        complete = false;
        return *this;
    }
};

#endif

#endif