/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_UTILS_URL_H
#define CORE_UTILS_URL_H

#include <Arduino.h>
#include <Client.h>
#include "./core/Utils/StringUtil.h"
#include "./core/Utils/Memory.h"

class URLUtil
{
public:
    /* Append a parameter to URL */
    bool addParam(String &url, const String &key, const String &val, bool &hasParam, bool allowEmptyValue = false, bool isEncode = false)
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
        url += isEncode ? encode(val) : val;
        hasParam = true;
        return true;
    }

    /* Append the comma separated tokens as URL parameters */
    void addParamsTokens(String &url, const String &key, String val, bool &hasParam)
    {
        if (val.length() == 0)
            return;
        Memory mem;
        char *p = reinterpret_cast<char *>(mem.alloc(val.length()));
        strcpy(p, val.c_str());
        char *pp = p;
        char *end = p;
        String tmp;
        while (pp != NULL)
        {
            sut.strsepImpl(&end, ",");
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
    void addPath(String &url, const String &path) { sut.printTo(url, path.length(), "%s%s", path.length() == 0 || (path.length() && path[0] != '/') ? "/" : "", path); }

    /* Append the string with google storage URL */
    void addGStorageURL(String &uri, const String &bucketID, const String &storagePath) { sut.printTo(uri, 500, "gs://%s%s%s", bucketID.c_str(), storagePath[0] != '/' ? "/" : "", storagePath.c_str()); }
#if defined(ENABLE_FUNCTIONS)
    /* Append the string with cloudfunctions project host */
    void addFunctionsHost(String &uri, const String &locationId, const String &projectId, const String &path, bool url)
    {
        if (url)
            sut.clear(uri);
        sut.printTo(uri, 500, "%s%s-%s.cloudfunctions.net%s%s", url ? "https://" : "", locationId.c_str(), projectId.c_str(), path.length() ? "/" : "", path.length() ? path.c_str() : "");
    }
#endif
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

    void relocate(String &header, const String &host, const String &ext)
    {
        int p1 = header.indexOf(" ");
        if (p1 > -1)
        {
            String part1, part2;
            part1 = header.substring(0, p1);
            int p2 = header.indexOf(" ", p1 + 1);
            part2 = header.substring(p2, header.length());
            header = part1 + ext + part2;
            p1 = header.indexOf("Host: ");
            part1 = header.substring(0, p1 + 6);
            p2 = header.indexOf("\r\n", p1);
            part2 = header.substring(p2, header.length());
            header = part1 + host + part2;
        }
    }

    String getHost(const String &url, String *ext = nullptr)
    {
        int start = url.length() && url[0] == '/' ? 1 : (url.indexOf("://") == -1 ? 0 : url.indexOf("://") + 3);
        int end = url.indexOf("/", start) == -1 ? url.length() : url.indexOf("/", start);
        if (ext)
            *ext = url.substring(end, url.length());
        return url.substring(start, end);
    }

    String downloadURL(const String &bucketId, const String &object)
    {
        String url;
        sut.printTo(url, 500, "https://firebasestorage.googleapis.com/v0/b/%s/o/%s?alt=media&token=a82781ce-a115-442f-bac6-a52f7f63b3e8", encode(bucketId).c_str(), encode(object).c_str());
        return url.c_str();
    }

    void updateDownloadURL(String &url, const String &payload)
    {
        int p1 = payload.indexOf("\"downloadTokens\": \"");
        if (p1 > -1)
        {
            p1 += 19;
            int p2 = payload.indexOf("\"", p1);
            if (p2 > -1)
                url.replace("a82781ce-a115-442f-bac6-a52f7f63b3e8", payload.substring(p1, p2));
        }
    }

    void addEncUrl(String &buff, const String &prefix, const String &param) { sut.printTo(buff, encode(param).length() + prefix.length(), "%s%s", prefix.c_str(), encode(param).c_str()); }
    void addUrl(String &buff, const String &param) { sut.printTo(buff, param.length(), "%s%s", param.length() ? (buff.length() ? "&" : "?") : "", param.length() ? param.c_str() : ""); }

private:
    StringUtil sut;
};
#endif