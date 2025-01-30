#ifndef CORE_AUTH_CUSTOM_AUTH_H
#define CORE_AUTH_CUSTOM_AUTH_H

#include <Arduino.h>
#include "./core/Auth/UserAuthData.h"
#include "./core/Auth/Parser/UserTokenFileParser.h"
#include "./core/Auth/Parser/SAParser.h"

namespace firebase_ns
{
#if defined(ENABLE_CUSTOM_AUTH)
    class CustomAuth
    {
        friend class FirebaseApp;

    public:
        explicit CustomAuth(TimeStatusCallback timeCb, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &scope = "", const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
            data.clear();
            data.sa.val[sa_ns::cm] = clientEmail;
            data.sa.val[sa_ns::pid] = projectId;
            data.sa.val[sa_ns::pk] = privateKey;
            data.sa.expire = expire;
            data.cust.val[cust_ns::api_key] = apiKey;
            data.cust.val[cust_ns::uid] = uid;
            data.cust.val[cust_ns::scope] = scope;
            data.cust.val[cust_ns::claims] = claims;
            data.initialized = isInitialized();
            data.auth_type = auth_sa_custom_token;
            data.auth_data_type = user_auth_data_custom_data;
            data.timestatus_cb = timeCb;
        };

        explicit CustomAuth(TimeStatusCallback timeCb, const file_config_data &safile, const String &apiKey, const String &uid, const String &scope = "", const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (safile.initialized)
            {
                data.file_data.copy(safile);
                data.sa.expire = expire;
                data.cust.val[cust_ns::api_key] = apiKey;
                data.cust.val[cust_ns::uid] = uid;
                data.cust.val[cust_ns::scope] = scope;
                data.cust.val[cust_ns::claims] = claims;
                data.timestatus_cb = timeCb;
            }
#endif
        }

        explicit CustomAuth(uint32_t ts, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &scope = "", const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
            CustomAuth(nullptr, apiKey, clientEmail, projectId, privateKey, uid, scope, claims, expire);
            data.ts = ts;
        };

        explicit CustomAuth(uint32_t ts, const file_config_data &safile, const String &apiKey, const String &uid, const String &scope = "", const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
#if defined(ENABLE_FS)
            CustomAuth(nullptr, safile, apiKey, uid, scope, claims, expire);
            data.ts = ts;
#endif
        }

        ~CustomAuth() { data.clear(); };

        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                bool ret = SAParser::parseSAFile(data.file_data.file, data);
                data.file_data.file.close();
                if (ret)
                {
                    data.initialized = isInitialized();
                    data.auth_type = auth_sa_custom_token;
                    data.auth_data_type = user_auth_data_custom_data;
                }
            }
#endif
            return data;
        }

        bool isInitialized() { return data.sa.val[sa_ns::pk].length() > 0 && data.sa.val[sa_ns::cm].length() > 0 && data.sa.val[sa_ns::pid].length() > 0 && data.cust.val[cust_ns::uid].length() > 0; }

    private:
        user_auth_data data;
    };
#endif
}
#endif