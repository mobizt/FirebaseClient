/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_TOKEN_APP_TOKEN_H
#define CORE_AUTH_TOKEN_APP_TOKEN_H

#include <Arduino.h>
#include "./core/Auth/UserAuthData.h"

namespace firebase_ns
{
    struct app_token_t
    {
    public:
        String val[app_tk_ns::max_type];
        uint32_t expire = 0, auth_ts = 0;
        bool authenticated = false;
        auth_token_type auth_type = auth_unknown_token;
        user_auth_data_type auth_data_type = user_auth_data_undefined;
        void clear()
        {
            for (size_t i = 0; i < app_tk_ns::max_type; i++)
                val[i].remove(0, val[i].length());
            expire = FIREBASE_DEFAULT_TOKEN_TTL;
            authenticated = false;
            auth_type = auth_unknown_token;
            auth_data_type = user_auth_data_undefined;
        }
        app_token_t() {}
    };
}
#endif