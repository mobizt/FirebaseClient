/**
 * Created February 10, 2024
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

class Filter
{
    friend class DatabaseOptions;
    friend class Database;
    friend class AsyncClientClass;

private:
    void copy(Filter &rhs)
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
    URLHelper uh;
    String uri;

public:
    Filter(){};
    ~Filter() { clear(); };

    Filter &orderBy(const String &val)
    {

        complete = true;
        String ob = "\"";
        ob += val;
        ob += "\"";
        uh.addParam(uri, "orderBy", ob, hasParams);
        return *this;
    }

    template <typename T = int>
    Filter &limitToFirst(T val)
    {
        uh.addParam(uri, "limitToFirst", String(val), hasParams);
        return *this;
    }

    template <typename T = int>
    Filter &limitToLast(T val)
    {
        uh.addParam(uri, "limitToLast", String(val), hasParams);
        return *this;
    }

    Filter &startAt(const String &val)
    {
        String sa = "\"";
        sa += val;
        sa += "\"";
        uh.addParam(uri, "startAt", sa, hasParams);
        return *this;
    }

    Filter &startAt(int val)
    {
        uh.addParam(uri, "startAt", String(val), hasParams);
        return *this;
    }

    Filter &endAt(const String &val)
    {
        String sa = "\"";
        sa += val;
        sa += "\"";
        uh.addParam(uri, "endAt", sa, hasParams);
        return *this;
    }

    Filter &endAt(int val)
    {
        uh.addParam(uri, "endAt", String(val), hasParams);
        return *this;
    }

    Filter &equalTo(const String &val)
    {
        String sa = "\"";
        sa += val;
        sa += "\"";
        uh.addParam(uri, "equalTo", sa, hasParams);
        return *this;
    }

    Filter &equalTo(int val)
    {
        uh.addParam(uri, "equalTo", String(val), hasParams);
        return *this;
    }

    Filter &clear()
    {
        uri.clear();
        complete = false;
        return *this;
    }
};

#endif

#endif