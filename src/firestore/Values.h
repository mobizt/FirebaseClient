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
#ifndef FIRESTORE_VALUES_H
#define FIRESTORE_VALUES_H

#include <Arduino.h>
#include "./Config.h"

#if defined(ENABLE_FIRESTORE)

#include "./firestore/ObjectWriter.h"

enum firestore_const_key_type
{
    firestore_const_key_nullValue,
    firestore_const_key_booleanValue,
    firestore_const_key_integerValue,
    firestore_const_key_doubleValue,
    firestore_const_key_timestampValue,
    firestore_const_key_stringValue,
    firestore_const_key_bytesValue,
    firestore_const_key_referenceValue,
    firestore_const_key_geoPointValue,
    firestore_const_key_arrayValue,
    firestore_const_key_mapValue,
    firestore_const_key_maxType
};

struct firebase_firestore_const_key_t
{
    char text[15];
};

const struct firebase_firestore_const_key_t firestore_const_key[firestore_const_key_maxType] PROGMEM = {
    "nullValue",
    "booleanValue",
    "integerValue",
    "doubleValue",
    "timestampValue",
    "stringValue",
    "bytesValue",
    "referenceValue",
    "geoPointValue",
    "arrayValue",
    "mapValue"};

/**
 * A message that can hold any of the supported value types.
 */
namespace Values
{

    class NullValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;

    public:
        /**
         * A null value.
         */
        NullValue() { buf = FPSTR("null"); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_nullValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class StringValue : public Printable
    {

    private:
        String buf, str;
        ObjectWriter owriter;

    public:
        /**
         * A string value.
         *  @param value The string vakue
         */
        StringValue(const String &value) { owriter.setString(buf, value); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_stringValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class BooleanValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;

    public:
        /**
         * A boolean value.
         *  @param value The boolean value
         */
        BooleanValue(bool value) { owriter.setBool(buf, value); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_booleanValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class IntegerValue : public Printable
    {

    private:
        String buf, str;
        ObjectWriter owriter;

    public:
        /**
         * A integer value.
         *  @param value The integer value
         */
        IntegerValue(int value) { buf = StringValue(String(value)).c_str(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_integerValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class DoubleValue : public Printable
    {

    private:
        String buf, str;
        ObjectWriter owriter;

    public:
        /**
         * A double value.
         *  @param value The double value
         */
        DoubleValue(double value) { buf = String(value); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_doubleValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class TimestampValue : public Printable
    {

    private:
        String buf, str;
        ObjectWriter owriter;

    public:
        /**
         * A timestamp value.
         * Precise only to microseconds. When stored, any additional precision is rounded down.
         * @param value The timestamp value string
         */
        TimestampValue(const String &value) { buf = StringValue(value).c_str(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_timestampValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class BytesValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;

    public:
        /**
         * A bytes value.
         * Must not exceed 1 MiB - 89 bytes. Only the first 1,500 bytes are considered by queries.
         * A base64-encoded string.
         * @param value The bytes value string
         */
        BytesValue(const String &value) { buf = StringValue(value).c_str(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_bytesValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class ReferenceValue : public Printable
    {

    private:
        String buf, str;
        ObjectWriter owriter;

    public:
        /**
         * A reference to a document.
         * @param value The resource name of document
         */
        ReferenceValue(const String &value) { buf = StringValue(value).c_str(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_referenceValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class GeoPointValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        JsonHelper jh;

    public:
        /**
         * A geo point value representing a point on the surface of Earth.
         * @param lat The latitude
         * @param lng The longitude
         */
        GeoPointValue(double lat, double lng)
        {
            jh.addObject(buf, FPSTR("latitude"), String(lat));
            jh.addObject(buf, FPSTR("longitude"), String(lng), true);
        }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_geoPointValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    class ArrayValue : public Printable
    {

    private:
        String buf, str;
        ObjectWriter owriter;
        bool flags[11];

        template <typename T>
        bool isExisted(T value)
        {
            String tmp = value.val();
            for (size_t i = 0; i <= firestore_const_key_mapValue; i++)
            {
                if (tmp.indexOf(firestore_const_key[i].text) > -1)
                {
                    if (flags[i])
                        return true;
                    flags[i] = 1;
                }
            }

            return false;
        }
        template <typename T>
        void set(T value)
        {
            if (isExisted(value))
                return;
            owriter.setPair(buf, FPSTR("values"), value.val(), true);
        }

    public:
        /**
         * An array value.
         * Cannot directly contain another array value, though can contain an map which contains another array.
         * @param value The object except for array value
         */
        template <typename T>
        ArrayValue(T value)
        {
            memset(flags, 0, 11);
            set(value);
        }
        /**
         * Add object to array value
         * @param value The resource name of document
         */
        template <typename T>
        ArrayValue &add(T value)
        {
            if (!isExisted(value))
            {
                if (buf.length() == 0)
                    set(value);
                else
                    owriter.addMember(buf, value.val(), true, "]}");
            }
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_arrayValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };

    struct MAP
    {
    private:
        String buf;
        ObjectWriter owriter;

    public:
        template <typename T>
        MAP(const String &key, T value, bool val) { owriter.setPair(buf, key, val ? value.val() : value.c_str()); }
        const char *c_str() { return buf.c_str(); }
    };

    /**
     * A map value.
     */
    class MapValue : public Printable
    {

    private:
        String buf, str;
        ObjectWriter owriter;
        template <typename T>
        void set(const String &key, T value) { owriter.setPair(buf, FPSTR("fields"), MAP(key, value, true).c_str()); }

    public:
        MapValue() {}
        /**
         * A map value.
         * @param value The map value
         */
        template <typename T>
        MapValue(const String &key, T value) { set(key, value); }
        template <typename T>
        MapValue &add(const String &key, T value)
        {
            if (buf.length() == 0)
                set(key, value);
            else
                owriter.addMember(buf, MAP(key, value, true).c_str(), true);
            return *this;
        }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return owriter.setPair(str, firestore_const_key[firestore_const_key_mapValue].text, buf); }
        size_t printTo(Print &p) const { return p.print(str.c_str()); }
    };
    /**
     * A message that can hold any of the supported value types.
     */
    class Value : public Printable
    {
    private:
        String buf;

    public:
        Value() {}
        /**
         * @param value The object value
         */
        template <typename T>
        Value(T value) { buf = value.val(); }
        const char *c_str() { return buf.c_str(); }
        const char *val() { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
    };
};

#endif
#endif
