/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_VALUE_H
#define CORE_VALUE_H

#include <Arduino.h>
#include <string>
#include "./core/Utils/StringUtil.h"

enum realtime_database_data_type
{
    realtime_database_data_type_undefined = -1,
    realtime_database_data_type_null = 0,
    realtime_database_data_type_integer = 1,
    realtime_database_data_type_float = 2,
    realtime_database_data_type_double = 3,
    realtime_database_data_type_boolean = 4,
    realtime_database_data_type_string = 5,
    realtime_database_data_type_json = 6,
    realtime_database_data_type_array = 7
};

struct boolean_t : public Printable
{
private:
    String buf;
    boolean_t &copy(bool rhs)
    {
        buf = rhs ? "true" : "false";
        return *this;
    }

public:
    boolean_t() {}
    explicit boolean_t(bool v) : buf(v ? "true" : "false") {}
    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const override { return p.print(buf.c_str()); }
};

struct number_t : public Printable
{
private:
    String buf;
    StringUtil sut;

public:
    number_t() {}
    template <typename T1 = int>
    explicit number_t(T1 v, int d) : buf(String(v, d)) {}
    template <typename T = int>
    explicit number_t(T o) : buf(sut.numString(o)) {}
    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const override { return p.print(buf.c_str()); }
};

struct string_t : public Printable
{
public:
    string_t() {}
    template <typename T = const char *>
    explicit string_t(T v)
    {
        aq(true);
        if (std::is_same<T, bool>::value)
            buf += v ? "true" : "false";
        else if (v_number<T>::value)
            buf += sut.numString(v);
        else
            buf += v;
        aq();
    }
    explicit string_t(number_t v)
    {
        aq(true);
        buf += v.c_str();
        aq();
    }
    explicit string_t(boolean_t v)
    {
        aq(true);
        buf += v.c_str();
        aq();
    }
    template <typename T>
    auto operator+=(const T &rval) -> typename std::enable_if<std::is_same<T, number_t>::value || std::is_same<T, boolean_t>::value, string_t &>::type
    {
        sap();
        buf += rval.c_str();
        aq();
        return *this;
    }

    template <typename T>
    auto operator+=(const T &rval) -> typename std::enable_if<!std::is_same<T, number_t>::value && !std::is_same<T, boolean_t>::value, string_t &>::type
    {
        sap();
        buf += rval;
        aq();
        return *this;
    }

    const char *c_str() const { return buf.c_str(); }
    size_t printTo(Print &p) const override { return p.print(buf.c_str()); }
    void clear() { sut.clear(buf); }

private:
    String buf;
    StringUtil sut;

    template <typename T>
    struct v_string
    {
        static bool const value = std::is_same<T, const char *>::value || std::is_same<T, std::string>::value || std::is_same<T, String>::value;
    };

    template <typename T>
    struct v_number
    {
        static bool const value = std::is_same<T, uint64_t>::value || std::is_same<T, int64_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value ||
                                  std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value ||
                                  std::is_same<T, double>::value || std::is_same<T, float>::value || std::is_same<T, int>::value;
    };
    void sap()
    {
        String temp;
        if (buf.length())
            temp = buf.substring(1, buf.length() - 1);
        aq(true);
        buf += temp;
    }
    void aq(bool clear = false)
    {
        if (clear)
            sut.clear(buf);
        buf += '"';
    }
};

struct object_t : public Printable
{
    friend class JsonWriter;

public:
    object_t() {}
    explicit object_t(const String &o) : buf(o) {}
    const char *c_str() const { return buf.c_str(); }
    template <typename T = const char *>
    explicit object_t(T o)
    {
        if (std::is_same<T, bool>::value)
            buf += o ? "true" : "false";
        else if (v_number<T>::value)
            buf += sut.numString(o);
        else
            buf += o;
    }
    explicit object_t(boolean_t o) : buf(o.c_str()) {}
    explicit object_t(number_t o) : buf(o.c_str()) {}
    explicit object_t(string_t o) : buf(o.c_str()) {}
    size_t printTo(Print &p) const override { return p.print(buf.c_str()); }
    void clear() { sut.clear(buf); }
    void initObject() { buf = "{}"; };
    void initArray() { buf = "[]"; };

private:
    String buf;
    StringUtil sut;

    template <typename T>
    struct v_string
    {
        static bool const value = std::is_same<T, const char *>::value || std::is_same<T, std::string>::value || std::is_same<T, String>::value;
    };

    template <typename T>
    struct v_number
    {
        static bool const value = std::is_same<T, uint64_t>::value || std::is_same<T, int64_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value ||
                                  std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value ||
                                  std::is_same<T, double>::value || std::is_same<T, float>::value || std::is_same<T, int>::value;
    };
    explicit operator bool() const { return buf.length() > 0; }

    template <typename T = const char *>
    auto operator=(const T &rval) -> typename std::enable_if<!std::is_same<T, object_t>::value && !std::is_same<T, string_t>::value && !std::is_same<T, number_t>::value && !std::is_same<T, boolean_t>::value, object_t &>::type
    {
        buf = rval;
        return *this;
    }

    template <typename T = String>
    auto operator+=(const T &rval) -> typename std::enable_if<!std::is_same<T, object_t>::value && !std::is_same<T, string_t>::value && !std::is_same<T, number_t>::value && !std::is_same<T, boolean_t>::value, object_t &>::type
    {
        buf += rval;
        return *this;
    }

    template <typename T>
    auto operator+=(const T &rval) -> typename std::enable_if<std::is_same<T, object_t>::value || std::is_same<T, string_t>::value || std::is_same<T, number_t>::value || std::is_same<T, boolean_t>::value, object_t &>::type
    {
        buf += rval.c_str();
        return *this;
    }

    size_t length() const { return buf.length(); }
    String substring(unsigned int beginIndex, unsigned int endIndex) const { return buf.substring(beginIndex, endIndex); }
};

class ValueConverter
{
public:
    ValueConverter() {}
    ~ValueConverter() {}

    template <typename T>
    struct v_string
    {
        static bool const value = std::is_same<T, const char *>::value || std::is_same<T, std::string>::value || std::is_same<T, String>::value;
    };

    template <typename T>
    struct v_number
    {
        static bool const value = std::is_same<T, uint64_t>::value || std::is_same<T, int64_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value ||
                                  std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value ||
                                  std::is_same<T, double>::value || std::is_same<T, float>::value || std::is_same<T, int>::value;
    };

    template <typename T = object_t>
    auto getVal(String &buf, T value) -> typename std::enable_if<std::is_same<T, object_t>::value || std::is_same<T, string_t>::value || std::is_same<T, boolean_t>::value || std::is_same<T, number_t>::value, void>::type
    {
        buf = value.c_str();
    }

    template <typename T = const char *>
    auto getVal(String &buf, T value) -> typename std::enable_if<(v_number<T>::value || v_string<T>::value || std::is_same<T, bool>::value) && !std::is_same<T, object_t>::value && !std::is_same<T, string_t>::value && !std::is_same<T, boolean_t>::value && !std::is_same<T, number_t>::value, void>::type
    {
        sut.clear(buf);
        if (std::is_same<T, bool>::value)
            buf = value ? "true" : "false";
        else
        {
            if (v_string<T>::value)
                buf += '\"';

            buf += sut.numString(value);

            if (v_string<T>::value)
                buf += '\"';
        }
    }

    template <typename T>
    auto to(const char *payload) -> typename std::enable_if<v_number<T>::value || std::is_same<T, bool>::value, T>::type
    {
        if (strlen(payload) > 0)
        {
            if (getType(payload) == realtime_database_data_type_boolean)
                setBool(strcmp(payload, "true") == 0);
            else
            {
                setInt(payload);
                setFloat(payload);
            }
        }
        else
            setBool(strlen(payload));

        if (std::is_same<T, int>::value)
            return iVal.int32;
        else if (std::is_same<T, bool>::value)
            return iVal.int32 > 0;
        else if (std::is_same<T, int8_t>::value)
            return iVal.int8;
        else if (std::is_same<T, uint8_t>::value)
            return iVal.uint8;
        else if (std::is_same<T, int16_t>::value)
            return iVal.int16;
        else if (std::is_same<T, uint16_t>::value)
            return iVal.uint16;
        else if (std::is_same<T, int32_t>::value)
            return iVal.int32;
        else if (std::is_same<T, uint32_t>::value)
            return iVal.uint32;
        else if (std::is_same<T, int64_t>::value)
            return iVal.int64;
        else if (std::is_same<T, uint64_t>::value)
            return iVal.uint64;
        else if (std::is_same<T, float>::value)
            return fVal.f;
        else if (std::is_same<T, double>::value)
            return fVal.d;
        else
            return 0;
    }

    template <typename T>
    auto to(const char *payload) -> typename std::enable_if<v_string<T>::value, T>::type
    {
        if (payload && payload[0] == '"' && payload[strlen(payload) - 1] == '"')
        {
            buf = payload + 1;
            buf[buf.length() - 1] = 0;
        }
        else
            buf = payload;

        return buf.c_str();
    }

    realtime_database_data_type getType(const char *payload)
    {
        if (strlen(payload) > 0)
        {
            size_t p1 = 0, p2 = strlen(payload) - 1;

            if (payload[p1] == '"')
                return realtime_database_data_type_string;
            else if (payload[p1] == '{')
                return realtime_database_data_type_json;
            else if (payload[p1] == '[')
                return realtime_database_data_type_array;
            // valid database response of none numeric except for ", { and [ character should be only true, false or null.
            else if (p2 > 0 && ((payload[p1] == 'f' && payload[p1 + 1] == 'a') || (payload[p1] == 't' && payload[p1 + 1] == 'r')))
                return realtime_database_data_type_boolean;
            else if (p2 > 0 && payload[p1] == 'n' && payload[p1 + 1] == 'u')
                return realtime_database_data_type_null;
            else
            {
                // response here should be numberic value
                // find the dot and check its length to determine the type
                if (String(payload).indexOf('.') != -1)
                    return p2 <= 7 ? realtime_database_data_type_float : realtime_database_data_type_double;
                else
                    // no dot, determine the type from its value
                    return atof(payload) > 0x7fffffff ? realtime_database_data_type_double : realtime_database_data_type_integer;
            }
        }

        return realtime_database_data_type_undefined;
    }

private:
    union IVal
    {
        uint64_t uint64;
        int64_t int64;
        uint32_t uint32;
        int32_t int32;
        int16_t int16;
        uint16_t uint16;
        int8_t int8;
        uint8_t uint8;
    };

    struct FVal
    {
        double d = 0;
        float f = 0;
        void setd(double v)
        {
            d = v;
            f = static_cast<float>(v);
        }

        void setf(float v)
        {
            f = v;
            d = static_cast<double>(v);
        }
    };

    String buf;
    StringUtil sut;
    bool trim = false;
    IVal iVal = {0};
    FVal fVal;

    void setBool(bool value)
    {
        if (value)
        {
            iVal = {1};
            fVal.setd(1);
        }
        else
        {
            iVal = {0};
            fVal.setd(0);
        }
    }

    void setInt(const char *value)
    {
        if (strlen(value) > 0)
        {
            char *pEnd;
            value[0] == '-' ? iVal.int64 = strtoll(value, &pEnd, 10) : iVal.uint64 = strtoull(value, &pEnd, 10);
        }
        else
            iVal = {0};
    }

    void setFloat(const char *value)
    {
        if (strlen(value) > 0)
        {
            char *pEnd;
            fVal.setd(strtod(value, &pEnd));
        }
        else
            fVal.setd(0);
    }
};
#endif