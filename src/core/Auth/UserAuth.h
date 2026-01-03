/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_USER_AUTH_H
#define CORE_AUTH_USER_AUTH_H

#include <Arduino.h>
#include "./core/Auth/UserAuthData.h"
#include "./core/Auth/Parser/UserTokenFileParser.h"

namespace firebase_ns
{
#if defined(ENABLE_USER_AUTH)
    class UserAuth
    {
        friend class FirebaseApp;

    public:
        explicit UserAuth(const String &api_key, const String &email, const String &password, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
            data.clear();
            data.user.val[user_ns::api_key] = api_key;
            data.user.val[user_ns::em] = email;
            data.user.val[user_ns::psw] = password;
            data.auth_type = auth_user_id_token;
            data.auth_data_type = user_auth_data_user_data;
            data.user.expire = expire;
            data.initialized = true;
        };

        explicit UserAuth(const file_config_data &userFile)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (userFile.initialized)
                data.file_data.copy(userFile);
#else
            (void)userFile;
#endif
        }

        ~UserAuth() { data.clear(); }

        void clear() { data.clear(); }

        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_user_data, data.file_data.file, data))
                    data.initialized = true;
                data.file_data.file.close();
            }
#endif
            return data;
        }

        bool isInitialized() { return data.initialized || data.file_data.initialized; }

        bool save(file_config_data &userFile)
        {
            bool ret = false;
#if defined(ENABLE_FS)
            if (userFile.initialized)
            {
                userFile.cb(userFile.file, userFile.filename.c_str(), file_mode_open_write);
                if (userFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_user_data, userFile.file, data);
                    userFile.file.close();
                }
            }
#else
            (void)userFile;
#endif
            return ret;
        }

    private:
        user_auth_data data;
    };

#endif
}
#endif