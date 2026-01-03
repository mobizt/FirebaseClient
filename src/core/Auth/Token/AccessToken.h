/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_TOKEN_ACCESS_TOKEN_H
#define CORE_AUTH_ACCESS_TOKEN_H

#include <Arduino.h>
#include "./core/Auth/UserAuth.h"

namespace firebase_ns
{
#if defined(ENABLE_ACCESS_TOKEN)
    class AccessToken
    {
        friend class FirebaseApp;

    public:
        explicit AccessToken(const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL, const String &refresh = "", const String &client_id = "", const String &client_secret = "")
        {
            this->data.clear();
            this->data.access_token.val[access_tk_ns::token] = token;
            this->data.access_token.val[access_tk_ns::refresh] = refresh;
            this->data.access_token.val[access_tk_ns::cid] = client_id;
            this->data.access_token.val[access_tk_ns::csec] = client_secret;
            this->data.access_token.expire = expire;
            this->data.initialized = true;
            this->data.auth_type = auth_access_token;
            this->data.auth_data_type = user_auth_data_access_token;
        }

        explicit AccessToken(const file_config_data &tokenFile)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (tokenFile.initialized)
                data.file_data.copy(tokenFile);
#endif
        }

        void clear() { data.clear(); }
        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_access_token, data.file_data.file, data))
                {
                    data.initialized = true;
                    data.auth_type = auth_access_token;
                    data.auth_data_type = user_auth_data_access_token;
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
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_access_token, tokenFile.file, data);
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