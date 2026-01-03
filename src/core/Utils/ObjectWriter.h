/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_UTILS_OBJECT_WRITER_H
#define CORE_UTILS_OBJECT_WRITER_H

#include <Arduino.h>
#include "./core/Utils/JSON.h"

#define RESOURCE_PATH_BASE "<resource_path>"

class ObjectWriter
{
private:
    JSONUtil jut;
    StringUtil sut;

public:
    void addMember(String &buf, const String &v, bool isString, const String &token = "}}")
    {
        int p = buf.lastIndexOf(token);
        String str = buf.substring(0, p);
        str += ',';
        // Add to object
        if (token[0] == '}')
        {
            String tmp;
            if (isString)
                tmp += '"';
            tmp += v;
            if (isString)
                tmp += '"';
            str += tmp.substring(1, tmp.length() - 1);
        }
        // Add to array
        else
        {
            if (isString)
                str += '"';
            str += v;
            if (isString)
                str += '"';
        }
        str += token;
        buf = str;
    }

    void addObject(String &buf, const String &object, const String &token, bool clear = false)
    {
        if (clear)
            sut.clear(buf);
        if (object.length() > 0)
        {
            if (buf.length() == 0)
                buf = object;
            else
                addMember(buf, object, false, token);
        }
    }

    void addMapArrayMember(String *buf, size_t size, uint8_t index, const String &key, const String &memberValue, bool isString)
    {
        if (index < size)
        {
            if ((buf[index]).length() == 0)
            {
                String temp = "";
                jut.addArray(temp, memberValue, isString, true);
                jut.addObject(buf[index], key, temp, false, true);
            }
            else
                addMember(buf[index], memberValue, isString, "]}");

            getBuf(buf, size);
        }
    }

    void getBuf(String *buf, size_t size)
    {
        clear(buf[0]);
        for (size_t i = 1; i < size; i++)
            addObject(buf[0], buf[i], "}", i == 0);
    }

    void setObject(String *buf, size_t size, uint8_t index, const String &key, const String &value, bool isString, bool last)
    {
        if (index < size)
        {
            if (key.length())
            {
                clear(buf[index]);
                jut.addObject(buf[index], key, value, isString, last);
            }
            getBuf(buf, size);
        }
    }

    void clearBuf(String *buf, size_t size)
    {
        for (size_t i = 0; i < size; i++)
            clear(buf[i]);
    }

    void clear(String &buf) { sut.clear(buf); }

    const char *setPair(String &buf, const String &key, const String &value, bool isArrayValue = false)
    {
        sut.clear(buf);
        jut.addObject(buf, key, isArrayValue ? getArrayStr(value) : value, false, true);
        return buf.c_str();
    }
    void setBool(String &buf, bool value) { buf = getBoolStr(value); }

    String getBoolStr(bool value) { return value ? "true" : "false"; }

    String getArrayStr(const String &value)
    {
        String str = "[";
        str += value;
        str += ']';
        return str;
    }

    void setString(String &buf, const String &value)
    {
        buf = "\"";
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
            if (path[0] != '/')
                full_path += '/';
            full_path += path;
        }
        if (toString)
            full_path += '"';
        return full_path;
    }
};

class BufWriter
{
private:
    ObjectWriter owriter;
    JSONUtil jut;
    StringUtil sut;

    template <typename T>
    struct v_number
    {
        static bool const value = std::is_same<T, uint64_t>::value || std::is_same<T, int64_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value ||
                                  std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value ||
                                  std::is_same<T, double>::value || std::is_same<T, float>::value || std::is_same<T, int>::value;
    };

    template <typename T>
    struct v_sring
    {
        static bool const value = std::is_same<T, const char *>::value || std::is_same<T, std::string>::value || std::is_same<T, String>::value;
    };

    void setObject(String *buf, size_t bufSize, uint8_t index, const String &key, const String &value, bool isString, bool last)
    {
        owriter.setObject(buf, bufSize, index, key, value, isString, last);
    }

public:
    BufWriter() {}
    template <typename T1, typename T2>
    T1 add(T1 ret, bool value, String &buf, const String &name)
    {
        clear(buf);
        jut.addObject(buf, name, owriter.getBoolStr(value), false, true);
        return ret;
    }

    template <typename T1, typename T2>
    auto add(T1 ret, const T2 &value, String &buf, const String &name) -> typename std::enable_if<v_number<T2>::value, T1>::type
    {
        clear(buf);
        jut.addObject(buf, name, sut.numString(value), false, true);
        return ret;
    }

    template <typename T1, typename T2>
    auto add(T1 ret, const T2 &value, String &buf, const String &name) -> typename std::enable_if<v_sring<T2>::value, T1>::type
    {
        clear(buf);
        jut.addObject(buf, name, value, true, true);
        return ret;
    }

    template <typename T1, typename T2>
    auto add(T1 ret, const T2 &value, String &buf, const String &name) -> typename std::enable_if<(!v_sring<T2>::value && !v_number<T2>::value && !std::is_same<T2, bool>::value), T1>::type
    {
        clear(buf);
        jut.addObject(buf, name, value.c_str(), false, true);
        return ret;
    }

    template <typename T1, typename T2>
    T1 set(T1 ret, bool value, String *buf, size_t bufSize, uint8_t index, const String &name)
    {
        setObject(buf, bufSize, index, name, owriter.getBoolStr(value), false, true);
        return ret;
    }

    template <typename T1, typename T2>
    auto set(T1 ret, const T2 &value, String *buf, size_t bufSize, uint8_t index, const String &name) -> typename std::enable_if<v_number<T2>::value, T1>::type
    {
        setObject(buf, bufSize, index, name, sut.numString(value), false, true);
        return ret;
    }

    template <typename T1, typename T2>
    auto set(T1 ret, const T2 &value, String *buf, size_t bufSize, uint8_t index, const String &name) -> typename std::enable_if<v_sring<T2>::value, T1>::type
    {
        setObject(buf, bufSize, index, name, value, true, true);
        return ret;
    }

    template <typename T1, typename T2>
    auto set(T1 ret, const T2 &value, String *buf, size_t bufSize, uint8_t index, const String &name) -> typename std::enable_if<(!v_sring<T2>::value && !v_number<T2>::value && !std::is_same<T2, bool>::value), T1>::type
    {
        setObject(buf, bufSize, index, name, value.c_str(), false, true);
        return ret;
    }

    template <typename T1, typename T2>
    T1 append(T1 ret, bool value, String *buf, size_t bufSize, uint8_t index, const String &name)
    {
        owriter.addMapArrayMember(buf, bufSize, index, name, owriter.getBoolStr(value), false);
        return ret;
    }

    template <typename T1, typename T2>
    auto append(T1 ret, const T2 &value, String *buf, size_t bufSize, uint8_t index, const String &name) -> typename std::enable_if<v_number<T2>::value, T1>::type
    {
        owriter.addMapArrayMember(buf, bufSize, index, name, sut.numString(value), false);
        return ret;
    }

    template <typename T1, typename T2>
    auto append(T1 ret, const T2 &value, String *buf, size_t bufSize, uint8_t index, const String &name) -> typename std::enable_if<v_sring<T2>::value, T1>::type
    {
        owriter.addMapArrayMember(buf, bufSize, index, name, value, true);
        return ret;
    }

    template <typename T1, typename T2>
    auto append(T1 ret, const T2 &value, String *buf, size_t bufSize, uint8_t index, const String &name) -> typename std::enable_if<(!v_sring<T2>::value && !v_number<T2>::value && !std::is_same<T2, bool>::value), T1>::type
    {
        owriter.addMapArrayMember(buf, bufSize, index, name, value.c_str(), false);
        return ret;
    }
    void clear(String &buf) { sut.clear(buf); }
    void clear(String *buf, size_t bufSize) { owriter.clearBuf(buf, bufSize); }
};

class BaseObjects : public Printable
{

protected:
    size_t bufferSize = 0;
    String *buffers = nullptr;
    BufWriter wr;

public:
    BaseObjects() {}
    ~BaseObjects() { clear(); }
    void init(String *buffers, size_t size)
    {
        this->buffers = buffers;
        this->bufferSize = size;
    }
    const char *c_str() const { return buffers[0].c_str(); }
    size_t printTo(Print &p) const override { return p.print(buffers[0].c_str()); }
    void clear() { wr.clear(buffers, bufferSize); }
    void setContent(const String &content)
    {
        clear();
        buffers[0] = content;
    }
};

class BaseO1 : public Printable
{

protected:
    String buf;
    BufWriter wr;
    StringUtil sut;

public:
    BaseO1() {}
    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const override { return p.print(buf.c_str()); }
    void clear() { sut.clear(buf); }
    void setContent(const String &content)
    {
        clear();
        buf = content;
    }
};

class BaseO2 : public BaseObjects
{

protected:
    static const size_t bufSize = 2;
    String buf[bufSize];

public:
    BaseO2() { init(buf, bufSize); }
};

class BaseO4 : public BaseObjects
{
protected:
    static const size_t bufSize = 4;
    String buf[bufSize];

public:
    BaseO4() { init(buf, bufSize); }
};

class BaseO6 : public BaseObjects
{

protected:
    static const size_t bufSize = 6;
    String buf[bufSize];

public:
    BaseO6() { init(buf, bufSize); }
};

class BaseO8 : public BaseObjects
{
protected:
    static const size_t bufSize = 8;
    String buf[bufSize];

public:
    BaseO8() { init(buf, bufSize); }
};

class BaseO10 : public BaseObjects
{
protected:
    static const size_t bufSize = 10;
    String buf[bufSize];

public:
    BaseO10() { init(buf, bufSize); }
};

class BaseO12 : public BaseObjects
{
protected:
    static const size_t bufSize = 12;
    String buf[bufSize];

public:
    BaseO12() { init(buf, bufSize); }
};

class BaseO16 : public BaseObjects
{
protected:
    static const size_t bufSize = 16;
    String buf[bufSize];

public:
    BaseO16() { init(buf, bufSize); }
};

class BaseO26 : public BaseObjects
{
protected:
    static const size_t bufSize = 26;
    String buf[bufSize];

public:
    BaseO26() { init(buf, bufSize); }
};

namespace firebase_ns
{
    struct key_str_10
    {
        char text[10];
    };

    struct key_str_20
    {
        char text[20];
    };

    struct key_str_30
    {
        char text[30];
    };

    struct key_str_40
    {
        char text[40];
    };

    struct key_str_50
    {
        char text[50];
    };

    struct key_str_60
    {
        char text[60];
    };

    class UnityRange
    {
    public:
        UnityRange() {}

        float val(float value)
        {
            if (value > 1)
                value = 1;
            else if (value < 0)
                value = 0;
            return value;
        }
    };
}
#endif