/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_USER_ACCOUNT_H
#define CORE_AUTH_USER_ACCOUNT_H

#include <Arduino.h>
#include "./core/Auth/UserAuth.h"

namespace firebase_ns
{
#if defined(ENABLE_USER_AUTH) || defined(ENABLE_ID_TOKEN)
    class UserAccount
    {
        friend class FirebaseApp;

    public:
        explicit UserAccount(const String &apiKey)
        {
            data.user.val[user_ns::api_key] = apiKey;
            setType(auth_user_id_token, user_auth_data_user_data);
        }

        ~UserAccount() { data.clear(); };

        UserAccount &email(const String &email)
        {
            data.user.val[user_ns::em] = email;
            return setType(auth_user_id_token, user_auth_data_user_data);
        }

        UserAccount &password(const String &password)
        {
            data.user.val[user_ns::psw] = password;
            return setType(auth_user_id_token, user_auth_data_user_data);
        }

        UserAccount &idToken(const String &idToken)
        {
            data.user.val[user_ns::id_token] = idToken;
            return setType(auth_user_id_token, user_auth_data_user_data);
        }

        void clear() { data.clear(); }

        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
        
        UserAccount &setType(auth_token_type auth_type, user_auth_data_type data_type)
        {
            data.auth_type = auth_type;
            data.auth_data_type = data_type;
            data.initialized = true;
            return *this;
        }
    };
#endif
}
#endif