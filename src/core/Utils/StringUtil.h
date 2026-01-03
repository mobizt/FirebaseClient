/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_UTILS_STRING_UTILS_H
#define CORE_UTILS_STRING_UTILS_H

#include <Arduino.h>
#include <Client.h>
#include <type_traits>
#include <stdarg.h>

#if defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_MINIMA) || defined(ARDUINO_PORTENTA_C33)
#define FIREBASE_STRSEP strsepImpl
#define FIREBASE_USE_STRSEP_IMPL
#else
#define FIREBASE_STRSEP strsep
#endif

class StringUtil
{
public:
    StringUtil() {}

    ~StringUtil() {}

#if defined(FIREBASE_USE_STRSEP_IMPL)
    char *strsepImpl(char **stringp, const char *delim)
    {
        char *rv = *stringp;
        if (rv)
        {
            *stringp += strcspn(*stringp, delim);
            if (**stringp)
                *(*stringp)++ = '\0';
            else
                *stringp = 0;
        }
        return rv;
    }
#else
    char *strsepImpl(char **stringp, const char *delim) { return strsep(stringp, delim); }
#endif

    void parse(const String &str, const String &name, const String &delim, int &p1, int &p2)
    {
        p1 = str.indexOf(name, p1);
        if (p1 > -1)
        {
            p2 = str.indexOf(':', p1);
            if (p2 > -1 && str.substring(p1, p2) == name)
            {
                p1 = p2 + 1;
                while (str[p1] == ' ')
                {
                    p1++;
                }

                p2 = str.indexOf(delim, p2);
                if (p2 == -1)
                    p2 = str.length() - 1;

                return;
            }
        }
        p1 = -1;
        p2 = -1;
    }

    void trim(String &str)
    {
        int i = str.length() - 1;
        while (i >= 0 && (str[i] == ' ' || str[i] == '}' || str[i] == '\r' || str[i] == '\n'))
        {
            i--;
        }
        str.remove(i);
    }

    void addSp(String &buf) { buf += ' '; }

    // Some cores do not provide 64-bit integer to string conversion.
    template <typename T = uint64_t>
    auto numString(T val, bool negative = false) -> typename std::enable_if<(std::is_same<T, uint64_t>::value), String>::type
    {
        String v;
        char buffer[21];
        char *ndx = &buffer[sizeof(buffer) - 1];
        *ndx = '\0';
        do
        {
            *--ndx = val % 10 + '0';
            val = val / 10;
        } while (val != 0);

        if (negative)
            v += '-';
        v += ndx;
        return v;
    }

    template <typename T = int64_t>
    auto numString(T val) -> typename std::enable_if<(std::is_same<T, int64_t>::value), String>::type
    {
        uint64_t value = val < 0 ? -val : val;
        return numString(value, val < 0);
    }

    template <typename T = int>
    auto numString(T val) -> typename std::enable_if<(!std::is_same<T, uint64_t>::value && !std::is_same<T, int64_t>::value), String>::type
    {
        return String(val);
    }

    void clear(String &s) { s.remove(0, s.length()); }

    void printTo(String &buff, int size, const char *format, ...)
    {
        size += strlen(format) + 10;
        char s[size];
        va_list va;
        va_start(va, format);
        vsnprintf(s, size, format, va);
        va_end(va);
        buff += reinterpret_cast<const char *>(s);
    }
    void addParams(const String &src, String &dest)
    {
        dest += src;
        dest.replace(" ", "%20");
        dest.replace(",", "%2C");
    }
};
#endif