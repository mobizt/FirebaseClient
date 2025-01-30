#ifndef CORE_UTILS_URL_H
#define CORE_UTILS_URL_H

#include <Arduino.h>
#include <Client.h>
#include "./Config.h"
#include "./core/Utils/StringUtil.h"
#include "./core/Utils/Memory.h"

class URLUtil
{
public:
    /* Append a parameter to URL */
    bool addParam(String &url, const String &key, const String &val, bool &hasParam, bool allowEmptyValue = false)
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
    void addParamsTokens(String &url, const String &key, String val, bool &hasParam)
    {
        if (val.length() == 0)
            return;

        Memory mem;
        StringUtil sut;
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
    void addPath(String &url, const String &path)
    {
        if (path.length() > 0)
        {
            if (path[0] != '/')
                url += '/';
        }
        else
            url += '/';

        url += path;
    }
    /* Append the string with google storage URL */
    void addGStorageURL(String &uri, const String &bucketID, const String &storagePath)
    {
        uri += FPSTR("gs://");
        uri += bucketID;
        if (storagePath[0] != '/')
            uri += '/';
        uri += storagePath;
    }
    /* Append the string with cloudfunctions project host */
    void addFunctionsHost(String &uri, const String &locationId, const String &projectId, const String &path, bool url)
    {
#if defined(ENABLE_FUNCTIONS)
        if (url)
            uri = FPSTR("https://");
        uri += locationId;
        uri += '-';
        uri += projectId;
        uri += FPSTR(".cloudfunctions.net");
        if (path.length() > 0)
        {
            uri += '/';
            uri += path;
        }
#endif
    }
    void addGAPIv1Path(String &uri) { uri += FPSTR("/v1/projects/"); }
    void addGAPIv1beta1Path(String &uri) { uri += FPSTR("/v1beta1/projects/"); }
    void addGAPIv1beta2Path(String &uri) { uri += FPSTR("/v1beta2/projects/"); }
    void host2Url(String &url, const String &host)
    {
        url = FPSTR("https://");
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
        int p1 = url.indexOf("://");
        int p2 = -1;
        p1 = p1 > -1 ? p1 + 3 : 0;
        p2 = url.indexOf("/", p1);
        if (p2 == -1)
            p2 = url.length();

        if (ext)
            *ext = url.substring(p2, url.length());
        return url.substring(p1, p2);
    }

    String downloadURL(const String &bucketId, const String &object)
    {
        String url = FPSTR("https://firebasestorage.googleapis.com/v0/b/");
        url += bucketId;
        url += FPSTR("/o/");
        url += object;
        url += FPSTR("?alt=media&token=a82781ce-a115-442f-bac6-a52f7f63b3e8");
        return url.c_str();
    }

    void updateDownloadURL(String &url, const String &payload)
    {
        int p1 = payload.indexOf(FPSTR("\"downloadTokens\": \""));
        if (p1 > -1)
        {
            p1 += 19;
            int p2 = payload.indexOf("\"", p1);
            if (p2 > -1)
                url.replace(FPSTR("a82781ce-a115-442f-bac6-a52f7f63b3e8"), payload.substring(p1, p2));
        }
    }
};
#endif