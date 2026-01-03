/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FIRESTORE_VALUES_H
#define FIRESTORE_VALUES_H

#include <Arduino.h>
#include "./core/Utils/ObjectWriter.h"

#if defined(ENABLE_FIRESTORE)
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
        StringUtil sut;
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_nullValue].text, buf); }

    public:
        /**
         * A null value.
         */
        NullValue() : buf("null") { getVal(); }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    class StringValue : public Printable
    {
    private:
        StringUtil sut;
        String buf, str;
        ObjectWriter owriter;
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_stringValue].text, buf); }

    public:
        /**
         * A string value.
         *  @param value The string value.
         */
        explicit StringValue(const String &value)
        {
            owriter.setString(buf, value);
            getVal();
        }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    class BooleanValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        StringUtil sut;
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_booleanValue].text, buf); }

    public:
        /**
         * A boolean value.
         *  @param value The boolean value.
         */
        explicit BooleanValue(bool value)
        {
            owriter.setBool(buf, value);
            getVal();
        }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    class IntegerValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        StringUtil sut;
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_integerValue].text, buf); }

    public:
        /**
         * A 64-bit signed integer value.
         *  @param value The 64-bit signed integer value.
         */
        explicit IntegerValue(int64_t value) : buf(StringValue(sut.numString(value)).c_str()) { getVal(); }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    class DoubleValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        StringUtil sut;
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_doubleValue].text, buf); }

    public:
        /**
         * [OBSOLETED] A double value.
         *  @param value The double value.
         */
        explicit DoubleValue(double value) : buf(String(value)) { getVal(); }

        /**
         * A double value.
         *  @param value The `number_t` represents the double value.
         */
        explicit DoubleValue(const number_t &value) : buf(value.c_str()) { getVal(); }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    class TimestampValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        StringUtil sut;
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_timestampValue].text, buf); }

    public:
        /**
         * A timestamp value.
         * Precise only to microseconds. When stored, any additional precision is rounded down.
         * @param value The timestamp value string.
         */
        explicit TimestampValue(const String &value) : buf(StringValue(value).c_str()) { getVal(); }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    class BytesValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        StringUtil sut;
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_bytesValue].text, buf); }

    public:
        /**
         * A bytes value.
         * Must not exceed 1 MiB - 89 bytes. Only the first 1,500 bytes are considered by queries.
         * A base64-encoded string.
         * @param value The bytes value string.
         */
        explicit BytesValue(const String &value) : buf(StringValue(value).c_str()) { getVal(); }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    class ReferenceValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        StringUtil sut;
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_referenceValue].text, buf); }

    public:
        /**
         * A reference to a document.
         * @param value The resource name of document.
         */
        explicit ReferenceValue(const String &value) : buf(StringValue(value).c_str()) { getVal(); }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    class GeoPointValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        JSONUtil jut;
        StringUtil sut;
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_geoPointValue].text, buf); }

    public:
        /**
         * [OBSOLETED} A GeoPointValue representing a point on the surface of Earth.
         * @param lat The latitude.
         * @param lng The longitude.
         */
        explicit GeoPointValue(double lat, double lng)
        {
            jut.addObject(buf, "latitude", String(lat), false);
            jut.addObject(buf, "longitude", String(lng), false, true);
            getVal();
        }

        /**
         * A geo point value representing a point on the surface of Earth.
         * @param lat The number_t represents the latitude value.
         * @param lng The number_t represents the longitude value.
         */
        explicit GeoPointValue(const number_t &lat, const number_t &lng)
        {
            jut.addObject(buf, "latitude", lat.c_str(), false);
            jut.addObject(buf, "longitude", lng.c_str(), false, true);
            getVal();
        }

        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    class ArrayValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        StringUtil sut;
        bool flags[11];

        template <typename T>
        void set(T value)
        {
            owriter.setPair(buf, "values", value.val(), true);
        }

        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_arrayValue].text, buf); }

    public:
        /**
         * An array value.
         * Cannot directly contain another array value, though can contain an map which contains another array.
         * @param value The object except for array value.
         */
        template <typename T>
        explicit ArrayValue(T value)
        {
            memset(flags, 0, 11);
            set(value);
            getVal();
        }

        /**
         * Add object to array value
         * @param value The resource name of document
         */
        template <typename T>
        ArrayValue &add(T value)
        {
            if (buf.length() == 0)
                set(value);
            else
                owriter.addMember(buf, value.val(), false, "]}");
            getVal();
            return *this;
        }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };

    struct MAP
    {
    private:
        String buf;
        ObjectWriter owriter;
        StringUtil sut;

    public:
        template <typename T>
        explicit MAP(const String &key, T value, bool val) { owriter.setPair(buf, key, val ? value.val() : value.c_str()); }
        const char *c_str() const { return buf.c_str(); }
        void clear() { sut.clear(buf); }
    };

    /**
     * A map value.
     */
    class MapValue : public Printable
    {
    private:
        String buf, str;
        ObjectWriter owriter;
        StringUtil sut;
        template <typename T>
        void set(const String &key, T value) { owriter.setPair(buf, "fields", MAP(key, value, true).c_str()); }
        const char *getVal() { return owriter.setPair(str, firestore_const_key[firestore_const_key_mapValue].text, buf); }

    public:
        MapValue() {}

        /**
         * A map value.
         * @param key The key.
         * @param value The value.
         */
        template <typename T>
        explicit MapValue(const String &key, T value)
        {
            set(key, value);
            getVal();
        }
        template <typename T>
        MapValue &add(const String &key, T value)
        {
            if (buf.length() == 0)
                set(key, value);
            else
                owriter.addMember(buf, MAP(key, value, true).c_str(), false);
            getVal();
            return *this;
        }
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return getVal(); }
        size_t printTo(Print &p) const override { return p.print(str.c_str()); }
        void clear()
        {
            sut.clear(buf);
            sut.clear(str);
        }
    };
    /**
     * A message that can hold any of the supported value types.
     */
    class Value : public Printable
    {
    private:
        String buf;
        StringUtil sut;

    public:
        Value() {}

        /**
         * @param value The object value
         */
        template <typename T>
        explicit Value(T value) : buf(value.val()) {}
        const char *c_str() const { return buf.c_str(); }
        const char *val() { return buf.c_str(); }
        size_t printTo(Print &p) const override { return p.print(buf.c_str()); }
        void clear() { sut.clear(buf); }
    };
};
#endif
#endif