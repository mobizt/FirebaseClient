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
#ifndef FIRESTORE_OBJECT_WRITER_H
#define FIRESTORE_OBJECT_WRITER_H

#include <Arduino.h>
#include "./Config.h"

#if defined(ENABLE_FIRESTORE)

#include "./core/JSON.h"

#define FIRESTORE_RESOURCE_PATH_BASE FPSTR("<resource_path>")

class ObjectWriter
{
private:
    JsonHelper jh;

public:
    void addMember(String &buf, const String &v, bool isObjValue, const String &token = "}}")
    {
        int p = buf.lastIndexOf(token);
        String str = buf.substring(0, p);
        str += ',';
        if (isObjValue)
        {
            String tmp = v;
            str += tmp.substring(1, tmp.length() - 1);
        }
        else
            str += v;
        str += token;
        buf = str;
    }
    const char *setPair(String &buf, const String &key, const String &value, bool isArrayValue = false)
    {
        buf.remove(0, buf.length());
        jh.addObject(buf, key, isArrayValue ? getArrayStr(value) : value, true);
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

    String getDocPath(const String &document)
    {
        String doc_path = FIRESTORE_RESOURCE_PATH_BASE;
        if (document.length())
        {
            if (document.length() && document[0] != '/')
                doc_path += '/';
            doc_path += document;
        }
        return doc_path;
    }
};

#endif

#endif