#ifndef CORE_DATABASE_FILTER_H
#define CORE_DATABASE_FILTER_H

#include <Arduino.h>
#include "Config.h"
#include "core/URL.h"


#if defined(ENABLE_DATABASE)

class Filter
{
    friend class DataOptions;
    friend class Database;
    friend class AsyncClient;

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