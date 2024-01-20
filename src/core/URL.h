
/**
 * Created January 11, 2024
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
#ifndef CORE_URL_H
#define CORE_URL_H

#include <Arduino.h>
#include <Client.h>
#include "./Config.h"
#include "StringHelper.h"
#include "Memory.h"

class URLHelper
{
public:
    /* Append a parameter to URL */
    bool addParam(String &url, PGM_P key, const String &val, bool &hasParam, bool allowEmptyValue = false)
    {
        if (!allowEmptyValue && val.length() == 0)
            return false;

        String _key(key);

        if (!hasParam && _key[0] == '&')
            _key[0] = '?';
        else if (hasParam && _key[0] == '?')
            _key[0] = '&';

        if (_key[0] != '?' && _key[0] != '&')
            url += !hasParam ? "?" : "&";

        if (_key[_key.length() - 1] != '=' && _key.indexOf('=') == -1)
            _key += "=";

        url += _key;
        url += val;
        hasParam = true;
        return true;
    }

    /* Append the comma separated tokens as URL parameters */
    void addParamsTokens(String &url, PGM_P key, String val, bool &hasParam)
    {
        if (val.length() == 0)
            return;

        Memory mem;

        StringHelper sh;

        char *p = reinterpret_cast<char *>(mem.alloc(val.length()));
        strcpy(p, val.c_str());
        char *pp = p;
        char *end = p;
        String tmp;
        while (pp != NULL)
        {
            sh.strsepImpl(&end, ",");
            if (strlen(pp) > 0)
            {
                tmp = pp;
                addParam(url, key, tmp, hasParam);
            }
            pp = end;
        }
        // release memory
        mem.release(&p);
    }

    /* Append the path to URL */
    void addPath(String &url, const String &path)
    {
        if (path.length() > 0)
        {
            if (path[0] != '/')
                url += "/";
        }
        else
            url += "/";

        url += path;
    }

    /* Append the string with google storage URL */
    void addGStorageURL(String &uri, const String &bucketID, const String &storagePath)
    {
        uri += "gs://";
        uri += bucketID;
        if (storagePath[0] != '/')
            uri += "/";
        uri += storagePath;
    }

    /* Append the string with cloudfunctions project host */
    void addFunctionsHost(String &uri, const String &locationId, const String &projectId,
                          const String &path, bool url)
    {
#if defined(ENABLE_FUNCTIONS)
        if (url)
            uri = "https://";
        uri += locationId;
        uri += "-";
        uri += projectId;
        uri += ".cloudfunctions.net";
        if (path.length() > 0)
        {
            uri += "/";
            uri += path;
        }
#endif
    }

    void addGAPIv1Path(String &uri)
    {
        uri += "/v1/projects/";
    }

    void addGAPIv1beta1Path(String &uri)
    {
        uri += "/v1beta1/projects/";
    }

    void host2Url(String &url, String &host)
    {
        url = "https://";
        url += host;
    }

    void hexchar(char c, char &hex1, char &hex2)
    {
        hex1 = c / 16;
        hex2 = c % 16;
        hex1 += hex1 < 10 ? '0' : 'A' - 10;
        hex2 += hex2 < 10 ? '0' : 'A' - 10;
    }

    String encode(const String &s)
    {
        String ret;
        ret.reserve(s.length() * 3 + 1);
        for (size_t i = 0, l = s.length(); i < l; i++)
        {
            char c = s[i];
            if ((c >= '0' && c <= '9') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
                c == '*' || c == '\'' || c == '(' || c == ')')
            {
                ret += c;
            }
            else
            {
                ret += '%';
                char d1, d2;
                hexchar(c, d1, d2);
                ret += d1;
                ret += d2;
            }
        }
        return ret;
    }
};

#endif