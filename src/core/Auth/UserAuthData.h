/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_USER_AUTH_DATA_H
#define CORE_AUTH_USER_AUTH_DATA_H

#include <Arduino.h>
#include "./core/File/FileConfig.h"
#include "./core/Auth/Data/ServiceData.h"
#include "./core/Auth/Data/CustomData.h"
#include "./core/Auth/Data/UserData.h"
#include "./core/Auth/Data/AccessTokenData.h"
#include "./core/Auth/Data/IdTokenData.h"
#include "./core/Auth/Data/CustomTokenData.h"
#include "./core/Auth/Data/LegacyTokenData.h"
#include "./core/Auth/AuthStatus.h"

namespace app_tk_ns
{
    enum data_item_type_t
    {
        type,    // token type
        token,   // auth token
        refresh, // refresh token
        uid,     // UID
        pid,     // project Id
        max_type
    };
}

namespace firebase_ns
{
    struct user_auth_data
    {
        friend class SAParser;
        friend class UserAuth;
        friend class ServiceAuth;
        friend class CustomAuth;
        friend class IDToken;
        friend class AccessToken;
        friend class CustomToken;
        friend class JWTClass;
        friend class FirebaseApp;
        friend class UserTokenFileParser;
        friend class LegacyToken;
        friend class NoAuth;
        friend class UserAccount;
        friend class FirebaseClient;
        friend class RealtimeDatabase;

    public:
        user_auth_data() {}

        ~user_auth_data() { clear(); }

        user_auth_data &operator=(const user_auth_data &rhs)
        {
            copy(rhs);
            return *this;
        }

        void copy(const user_auth_data &rhs)
        {
            this->user.copy(rhs.user);
#if defined(ENABLE_SERVICE_AUTH) || defined(ENABLE_CUSTOM_AUTH)
            this->sa.copy(rhs.sa);
#endif
#if defined(ENABLE_CUSTOM_AUTH)
            this->cust.copy(rhs.cust);
#endif
#if defined(ENABLE_ID_TOKEN)
            this->id_token.copy(rhs.id_token);
#endif
#if defined(ENABLE_ACCESS_TOKEN)
            this->access_token.copy(rhs.access_token);
#endif
#if defined(ENABLE_CUSTOM_TOKEN)
            this->custom_token.copy(rhs.custom_token);
#endif
#if defined(ENABLE_LEGACY_TOKEN)
            this->legacy_token.copy(rhs.legacy_token);
#endif
            this->auth_type = rhs.auth_type;
            this->auth_data_type = rhs.auth_data_type;
            this->anonymous = rhs.anonymous;
            this->initialized = rhs.initialized;
            this->timestatus_cb = rhs.timestatus_cb;
            if (rhs.ts > this->ts)
                this->ts = rhs.ts;
            if (rhs.ms > this->ms)
                this->ms = rhs.ms;
        }

        void clear()
        {
            user.clear();
#if defined(ENABLE_SERVICE_AUTH) || defined(ENABLE_CUSTOM_AUTH)
            sa.clear();
#endif
#if defined(ENABLE_CUSTOM_AUTH)
            cust.clear();
#endif
#if defined(ENABLE_ID_TOKEN)
            id_token.clear();
#endif
#if defined(ENABLE_ACCESS_TOKEN)
            access_token.clear();
#endif
#if defined(ENABLE_CUSTOM_TOKEN)
            custom_token.clear();
#endif
#if defined(ENABLE_LEGACY_TOKEN)
            legacy_token.clear();
#endif
            timestatus_cb = NULL;
            anonymous = false;
            initialized = false;
            ts = 0;
        }

        user_auth_data_type getAuthDataType() { return auth_data_type; }
        auth_token_type getAuthTokenType() { return auth_type; }

    protected:
        user_data user;
#if defined(ENABLE_SERVICE_AUTH) || defined(ENABLE_CUSTOM_AUTH)
        service_account sa;
#endif
#if defined(ENABLE_CUSTOM_AUTH)
        custom_data cust;
#endif
#if defined(ENABLE_ID_TOKEN)
        id_token_data id_token;
#endif
#if defined(ENABLE_ACCESS_TOKEN)
        access_token_data access_token;
#endif
#if defined(ENABLE_CUSTOM_TOKEN)
        custom_token_data custom_token;
#endif
#if defined(ENABLE_LEGACY_TOKEN)
        legacy_token_data legacy_token;
#endif
        bool anonymous = false, initialized = false, jwt_signing = false, jwt_loop = false, jwt_time_debug = false;
        uint32_t jwt_ts = 0;
        auth_token_type auth_type = auth_unknown_token;
        user_auth_data_type auth_data_type = user_auth_data_undefined;
        firebase_core_auth_task_type task_type = firebase_core_auth_task_type_undefined;
        auth_status status;
        TimeStatusCallback timestatus_cb = NULL;
        uint32_t ts = 0, ms = 0;
        file_config_data file_data;
    };
}
#endif