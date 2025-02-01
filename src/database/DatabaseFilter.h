/**
 * 2025-02-02
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 K. Suwatchai (Mobizt)
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
#ifndef DATABASE_DATABASE_FILTER_H
#define DATABASE_DATABASE_FILTER_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/Utils/StringUtil.h"

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
    bool complete = false;
    StringUtil sut;
    String uri;

public:
    DatabaseFilter() {}
    ~DatabaseFilter() { clear(); };

    DatabaseFilter &orderBy(const String &val)
    {
        complete = true;
        sut.printTo(uri, val.length(), "%sorderBy=\"%s\"", uri.length() ? "&" : "?", val.c_str());
        return *this;
    }

    template <typename T = int>
    DatabaseFilter &limitToFirst(T val)
    {
        String v = sut.num2Str(val);
        sut.printTo(uri, v.length(), "%slimitToFirst=%s", uri.length() ? "&" : "?", v.c_str());
        return *this;
    }

    template <typename T = int>
    DatabaseFilter &limitToLast(T val)
    {
        String v = sut.num2Str(val);
        sut.printTo(uri, v.length(), "%slimitToLast=%s", uri.length() ? "&" : "?", v.c_str());
        return *this;
    }

    DatabaseFilter &startAt(const String &val)
    {
        sut.printTo(uri, val.length(), "%sstartAt=\"%s\"", uri.length() ? "&" : "?", val.c_str());
        return *this;
    }

    DatabaseFilter &startAt(int val)
    {
        sut.printTo(uri, 30, "%sstartAt=%d", uri.length() ? "&" : "?", val);
        return *this;
    }

    DatabaseFilter &endAt(const String &val)
    {
        sut.printTo(uri, val.length(), "%sendAt=\"%s\"", uri.length() ? "&" : "?", val.c_str());
        return *this;
    }

    DatabaseFilter &endAt(int val)
    {
        sut.printTo(uri, 30, "%sendAt=%d", uri.length() ? "&" : "?", val);
        return *this;
    }

    DatabaseFilter &equalTo(const String &val)
    {
        sut.printTo(uri, val.length(), "%sequalTo=\"%s\"", uri.length() ? "&" : "?", val.c_str());
        return *this;
    }

    DatabaseFilter &equalTo(int val)
    {
        sut.printTo(uri, 30, "%sequalTo=%d", uri.length() ? "&" : "?", val);
        return *this;
    }

    DatabaseFilter &clear()
    {
        sut.clear(uri);
        complete = false;
        return *this;
    }
};
#endif
#endif