/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_TOKEN_CUSTOM_TOKEN_H
#define CORE_AUTH_TOKEN_CUSTOM_TOKEN_H

#include <Arduino.h>
#include "./core/Auth/UserAuth.h"

namespace firebase_ns
{
#if defined(ENABLE_CUSTOM_TOKEN)
    class CustomToken
    {
        friend class FirebaseApp;

    public:
        explicit CustomToken(const String &api_key, const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
            this->data.clear();
            this->data.custom_token.val[cust_tk_ns::token] = token;
            this->data.user.val[user_ns::api_key] = api_key;
            this->data.custom_token.expire = expire;
            this->data.initialized = data.custom_token.val[cust_tk_ns::token].length() > 0 && data.user.val[user_ns::api_key].length() > 0;
            this->data.auth_type = auth_custom_token;
            this->data.auth_data_type = user_auth_data_custom_token;
        }

        explicit CustomToken(const file_config_data &tokenFile)
        {
            data.clear();
            if (tokenFile.initialized)
            {
#if defined(ENABLE_FS)
                data.file_data.copy(tokenFile);
#endif
            }
        }

        void clear() { data.clear(); }
        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_custom_token, data.file_data.file, data))
                {
                    data.initialized = data.custom_token.val[cust_tk_ns::token].length() > 0 && data.user.val[user_ns::api_key].length() > 0;
                    data.auth_type = auth_custom_token;
                    data.auth_data_type = user_auth_data_custom_token;
                }
                data.file_data.file.close();
            }
#endif
            return data;
        }

        bool isInitialized() { return data.initialized || data.file_data.initialized; }

         bool save(file_config_data &tokenFile)
        {
            bool ret = false;
#if defined(ENABLE_FS)
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_write);
                if (tokenFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_custom_token, tokenFile.file, data);
                    tokenFile.file.close();
                }
            }
#endif
            return ret;
        }

    private:
        user_auth_data data;
    };
#endif
}
#endif