/**
 * Created February 6, 2024
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
#ifndef CORE_JSON_H
#define CORE_JSON_H

#include <Arduino.h>
#include <Client.h>
#include "./Config.h"
#include "./core/Memory.h"
#include "./core/StringHelper.h"

class JsonHelper
{

private:
public:
    JsonHelper(){};
    ~JsonHelper(){};

    void addObject(String &buf, const String &name, const String &value, bool last = false)
    {
        if (buf.length() == 0)
            buf += '{';
        else
            buf += ',';
        buf += name;
        buf += ':';
        buf += value;
        if (last)
            buf += '}';
    }

    void addArray(String &buf, const String &value, bool last = false)
    {
        if (buf.length() == 0)
            buf += '[';
        else
            buf += ',';
        buf += value;
        if (last)
            buf += ']';
    }
    String toString(const String &value)
    {
        String buf;
        buf += '"';
        buf += value;
        buf += '"';
        return buf;
    }

};

class JsonWriter
{

private:
public:
    JsonWriter(){};
    ~JsonWriter(){};

    void create(const String &path, object_t &o, const String &value)
    {
        StringHelper sh;
        char *p = new char[path.length() + 1];
        memset(p, 0, path.length() + 1);
        strcpy(p, path.c_str());
        char *pp = p;
        char *end = p;
        String tmp;
        int i = 0;
        o = "{";
        while (pp != NULL)
        {
            sh.strsepImpl(&end, "/");
            if (strlen(pp) > 0)
            {
                tmp = pp;
                if (i > 0)
                    o += '{';
                o += '"';
                o += tmp;
                o += '"';
                o += ':';
                i++;
            }
            pp = end;
        }

        delete p;

        o += value;
        for (int j = 0; j < i; j++)
            o += '}';
    }

    void create(object_t &o, const String &path, object_t &value)
    {
        create(path, o, value.c_str());
    }

    void join(object_t &o, int nunArgs, ...)
    {
        bool arr = false;
        if (strcmp(o.c_str(), "[]") == 0)
            arr = true;
        o = "";
        o += !arr ? '{' : '[';

        va_list ap;
        va_start(ap, nunArgs);
        object_t p = va_arg(ap, object_t);

        o += !arr ? p.c_str()[0] == '{' || p.c_str()[0] == '[' ? p.substring(1, p.length() - 1) : p : p;
        for (int i = 2; i <= nunArgs; i++)
        {
            o += ',';
            p = va_arg(ap, object_t);
            o += !arr ? p.c_str()[0] == '{' || p.c_str()[0] == '[' ? p.substring(1, p.length() - 1) : p : p;
        }
        va_end(ap);
        o += !arr ? '}' : ']';
    }
};

#endif