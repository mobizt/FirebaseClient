/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef DATABASE_DATABASE_FILTER_H
#define DATABASE_DATABASE_FILTER_H

#include <Arduino.h>
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
        String v = sut.numString(val);
        sut.printTo(uri, v.length(), "%slimitToFirst=%s", uri.length() ? "&" : "?", v.c_str());
        return *this;
    }

    template <typename T = int>
    DatabaseFilter &limitToLast(T val)
    {
        String v = sut.numString(val);
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