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
#ifndef FIRESTORE_OBJECT_WRITER_H
#define FIRESTORE_OBJECT_WRITER_H

#include <Arduino.h>
#include "./Config.h"

#include "./core/JSON.h"

#define RESOURCE_PATH_BASE FPSTR("<resource_path>")

class ObjectWriter
{
private:
    JSONUtil jut;

public:
    void addMember(String &buf, const String &v, const String &token = "}}")
    {
        int p = buf.lastIndexOf(token);
        String str = buf.substring(0, p);
        str += ',';
        // Add to object
        if (token[0] == '}')
        {
            String tmp = v;
            str += tmp.substring(1, tmp.length() - 1);
        }
        // Add to array
        else
            str += v;
        str += token;
        buf = str;
    }

    void addObject(String &buf, const String &object, const String &token, bool clear = false)
    {
        if (clear)
            buf.remove(0, buf.length());
        if (object.length() > 0)
        {
            if (buf.length() == 0)
                buf = object;
            else
                addMember(buf, object, token);
        }
    }

    void addMapArrayMember(String *buf, size_t size, String &buf_n, const String &key, const String &memberValue, bool isString)
    {
        if (buf_n.length() == 0)
        {
            String temp;
            jut.addArray(temp, memberValue, isString, true);
            jut.addObject(buf_n, key, temp, false, true);
        }
        else
            addMember(buf_n, isString ? jut.toString(memberValue) : memberValue, "]}");

        getBuf(buf, size);
    }

    void getBuf(String *buf, size_t size)
    {
        clear(buf[0]);
        for (size_t i = 1; i < size; i++)
            addObject(buf[0], buf[i], "}", i == 0);
    }

    void setObject(String *buf, size_t size, String &buf_n, const String &key, const String &value, bool isString, bool last)
    {
        if (key.length())
        {
            clear(buf_n);
            jut.addObject(buf_n, key, value, isString, last);
        }
        getBuf(buf, size);
    }

    void clearBuf(String *buf, size_t size)
    {
        for (size_t i = 0; i < size; i++)
            clear(buf[i]);
    }

    void clear(String &buf) { buf.remove(0, buf.length()); }

    const char *setPair(String &buf, const String &key, const String &value, bool isArrayValue = false)
    {
        buf.remove(0, buf.length());
        jut.addObject(buf, key, isArrayValue ? getArrayStr(value) : value, false, true);
        return buf.c_str();
    }
    void setBool(String &buf, bool value) { buf = getBoolStr(value); }

    String getBoolStr(bool value) { return value ? FPSTR("true") : FPSTR("false"); }

    String getArrayStr(const String &value)
    {
        String str = FPSTR("[");
        str += value;
        str += ']';
        return str;
    }

    void setString(String &buf, const String &value)
    {
        buf = FPSTR("\"");
        buf += value;
        buf += '"';
    }

    String makeResourcePath(const String &path, bool toString = false)
    {
        String full_path;
        if (toString)
            full_path += '"';
        full_path += RESOURCE_PATH_BASE;
        if (path.length())
        {
            if (path.length() && path[0] != '/')
                full_path += '/';
            full_path += path;
        }
        if (toString)
            full_path += '"';
        return full_path;
    }
};

#endif