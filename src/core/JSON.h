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
    int prek(object_t &obj, const String &path)
    {
        StringHelper sh;
        char *p = new char[path.length() + 1];
        memset(p, 0, path.length() + 1);
        strcpy(p, path.c_str());
        char *pp = p;
        char *end = p;
        String tmp;
        int i = 0;
        obj = "{";
        while (pp != NULL)
        {
            sh.strsepImpl(&end, "/");
            if (strlen(pp) > 0)
            {
                tmp = pp;
                if (i > 0)
                    obj += '{';
                obj += '"';
                obj += tmp;
                obj += '"';
                obj += ':';
                i++;
            }
            pp = end;
        }

        delete p;
        return i;
    }
    void ek(object_t &obj, int i)
    {
        for (int j = 0; j < i; j++)
            obj += '}';
    }

public:
    JsonWriter(){};
    ~JsonWriter(){};

    template <typename T>
    auto create(object_t &obj, const String &path, T value) -> typename std::enable_if<!is_same<T, object_t>::value && !is_same<T, string_t>::value && !is_same<T, number_t>::value && !is_same<T, boolean_t>::value, void>::type
    {
        int i = prek(obj, path);
        if (ValueConverter::is_string<T>::value)
            obj += "\"";
        obj += value;
        if (ValueConverter::is_string<T>::value)
            obj += "\"";
        ek(obj, i);
    }

    template <typename T>
    auto create(object_t &obj, const String &path, T value) -> typename std::enable_if<is_same<T, object_t>::value || is_same<T, string_t>::value || is_same<T, number_t>::value || is_same<T, boolean_t>::value, void>::type
    {
        int i = prek(obj, path);
        obj += value.c_str();
        ek(obj, i);
    }

    void join(object_t &obj, int nunArgs, ...)
    {
        bool arr = strcmp(obj.c_str(), "[]") == 0;
        obj = "";
        obj += !arr ? '{' : '[';
        va_list ap;
        va_start(ap, nunArgs);
        object_t p = va_arg(ap, object_t);
        if (p)
            obj += !arr ? p.c_str()[0] == '{' || p.c_str()[0] == '[' ? p.substring(1, p.length() - 1) : p : p;
        for (int i = 2; i <= nunArgs; i++)
        {
            obj += ',';
            p = va_arg(ap, object_t);
            if (p)
                obj += !arr ? p.c_str()[0] == '{' || p.c_str()[0] == '[' ? p.substring(1, p.length() - 1) : p : p;
        }
        va_end(ap);
        obj += !arr ? '}' : ']';
    }
};

#endif