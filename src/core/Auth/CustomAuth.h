/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

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
        explicit CustomAuth(TimeStatusCallback timeCb, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL) { init(timeCb, 0, apiKey, clientEmail, projectId, privateKey, uid, claims, expire); }

        explicit CustomAuth(TimeStatusCallback timeCb, const file_config_data &safile, const String &apiKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL) { init(timeCb, 0, safile, apiKey, uid, claims, expire); }

        explicit CustomAuth(const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL) { init(nullptr, 0, apiKey, clientEmail, projectId, privateKey, uid, claims, expire); }

        explicit CustomAuth(const file_config_data &safile, const String &apiKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL) { init(nullptr, 0, safile, apiKey, uid, claims, expire); }

        ~CustomAuth() { data.clear(); }

        void setTime(uint32_t ts)
        {
            data.ts = ts;
            data.ms = millis();
        }

        void setClaims(const String &claims) { data.cust.val[cust_ns::claims] = claims; }

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

        void init(TimeStatusCallback timeCb, int ts, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
            data.clear();
            data.sa.val[sa_ns::cm] = clientEmail;
            data.sa.val[sa_ns::pid] = projectId;
            data.sa.val[sa_ns::pk] = privateKey;
            data.sa.expire = expire;
            data.cust.val[cust_ns::api_key] = apiKey;
            data.cust.val[cust_ns::uid] = uid;
            data.cust.val[cust_ns::claims] = claims;
            data.initialized = isInitialized();
            data.auth_type = auth_sa_custom_token;
            data.auth_data_type = user_auth_data_custom_data;
            data.timestatus_cb = timeCb;
            setTime(ts);
        }

        void init(TimeStatusCallback timeCb, int ts, const file_config_data &safile, const String &apiKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (safile.initialized)
            {
                data.file_data.copy(safile);
                data.sa.expire = expire;
                data.cust.val[cust_ns::api_key] = apiKey;
                data.cust.val[cust_ns::uid] = uid;
                data.cust.val[cust_ns::claims] = claims;
                data.timestatus_cb = timeCb;
                setTime(ts);
            }
#endif
        }
    };
#endif
}
#endif