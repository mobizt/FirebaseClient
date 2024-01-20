/**
 *
 * Created November 10, 2023
 *
 * This work is a part of Firebase ESP Client library
 * Copyright (c) 2023 K. Suwatchai (Mobizt)
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 K. Suwatchai (Mobizt)
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
#ifndef VALUE_H
#define VALUE_H
#include <Arduino.h>
#include <string>
using namespace std;

enum database_data_type
{
    database_data_type_undefined = -1,
    database_data_type_null = 0,
    database_data_type_integer = 1,
    database_data_type_float = 2,
    database_data_type_double = 3,
    database_data_type_boolean = 4,
    database_data_type_string = 5,
    database_data_type_json = 6,
    database_data_type_array = 7
};

struct object_t
{
private:
    String buf;

public:
    object_t() {}
    object_t(const char *o) { buf = o; }
    const char *c_str() { return buf.c_str(); }
};

struct string_t
{
private:
    String buf;

public:
    string_t() {}
    string_t(const char *v)
    {
        buf = "\"";
        buf += v;
        buf += "\"";
    }
    const char *c_str() { return buf.c_str(); }
};

struct boolean_t
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
    boolean_t(bool v) { buf = v ? "true" : "false"; }
    const char *c_str() { return buf.c_str(); }
};

struct number_t
{
private:
    String buf;

public:
    number_t() {}
    template <typename T1 = int, typename T = int>
    number_t(T1 v, T d) { buf = String(v, d); }
    const char *c_str() { return buf.c_str(); }
};

class Value
{
public:
    Value() {}
    ~Value() {}

    template <typename T>
    struct is_string
    {
        static bool const value = is_same<T, const char *>::value || is_same<T, string>::value || is_same<T, String>::value;
    };

    template <typename T>
    struct is_number
    {
        static bool const value = is_same<T, uint64_t>::value || is_same<T, int64_t>::value || is_same<T, uint32_t>::value || is_same<T, int32_t>::value ||
                                  is_same<T, uint16_t>::value || is_same<T, int16_t>::value || is_same<T, uint8_t>::value || is_same<T, int8_t>::value ||
                                  is_same<T, double>::value || is_same<T, float>::value || is_same<T, bool>::value;
    };

    template <typename T = object_t>
    auto getVal(String &buf, T value) -> typename enable_if<is_same<T, object_t>::value || is_same<T, string_t>::value || is_same<T, boolean_t>::value || is_same<T, number_t>::value, void>::type
    {
        buf = value.c_str();
    }

    template <typename T = const char *>
    auto getVal(String &buf, T value) -> typename enable_if<(is_number<T>::value || is_string<T>::value) && !is_same<T, object_t>::value && !is_same<T, string_t>::value && !is_same<T, boolean_t>::value && !is_same<T, number_t>::value, void>::type
    {
        buf = "";
        if (is_string<T>::value)
            buf = '\"';
        buf += value;
        if (is_string<T>::value)
            buf += '\"';
    }

    template <typename T>
    auto to(const char *payload) -> typename enable_if<is_number<T>::value, T>::type
    {
        if (!useLength && strlen(payload) > 0)
        {
            if (getType(payload) == database_data_type_boolean)
                setBool(strcmp(payload, "true") == 0);
            else
            {
                setInt(payload);
                setFloat(payload);
            }
        }
        else
            setBool(strlen(payload));

        if (is_same<T, bool>::value)
            return iVal.int32 > 0;
        else if (is_same<T, int8_t>::value)
            return iVal.int8;
        else if (is_same<T, uint8_t>::value)
            return iVal.uint8;
        else if (is_same<T, int16_t>::value)
            return iVal.int16;
        else if (is_same<T, uint16_t>::value)
            return iVal.uint16;
        else if (is_same<T, int32_t>::value)
            return iVal.int32;
        else if (is_same<T, uint32_t>::value)
            return iVal.uint32;
        else if (is_same<T, int64_t>::value)
            return iVal.int64;
        else if (is_same<T, uint64_t>::value)
            return iVal.uint64;
        else if (is_same<T, float>::value)
            return fVal.f;
        else if (is_same<T, double>::value)
            return fVal.d;
        else
            return 0;
    }

    template <typename T>
    auto to(const char *payload) -> typename enable_if<is_string<T>::value, T>::type
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

    database_data_type getType(const char *payload)
    {
        if (strlen(payload) > 0)
        {
            size_t p1 = 0, p2 = strlen(payload) - 1;

            if (payload[p1] == '"')
                return database_data_type_string;
            else if (payload[p1] == '{')
                return database_data_type_json;
            else if (payload[p1] == '[')
                return database_data_type_array;
            // valid database response of none numeric except for ", { and [ character should be only true, false or null.
            else if (p2 > 0 && ((payload[p1] == 'f' && payload[p1 + 1] == 'a') || (payload[p1] == 't' && payload[p1 + 1] == 'r')))
                return database_data_type_boolean;
            else if (p2 > 0 && payload[p1] == 'n' && payload[p1 + 1] == 'u')
                return database_data_type_null;
            else
            {
                // response here should be numberic value
                double d = atof(payload);
                // find the dot and check its length to determine the type
                if (String(payload).indexOf('.') != -1)
                    return p2 <= 7 ? database_data_type_float : database_data_type_double;
                else
                    // no dot, determine the type from its value
                    return d > 0x7fffffff ? database_data_type_double : database_data_type_integer;
            }
        }

        return database_data_type_undefined;
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
    bool trim = false;
    bool useLength = false;

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