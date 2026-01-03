/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_CONFIG_H
#define CORE_AUTH_CONFIG_H

#include <Arduino.h>
#include "./core/File/FileConfig.h"
#include "./core/Utils/StringUtil.h"
#include "./core/AsyncResult/AsyncResult.h"
#include "./core/Auth/UserAuth.h"
#include "./core/Auth/ServiceAuth.h"
#include "./core/Auth/CustomAuth.h"
#include "./core/Auth/Token/IdToken.h"
#include "./core/Auth/Token/AccessToken.h"
#include "./core/Auth/Token/CustomToken.h"
#include "./core/Auth/Token/LegacyToken.h"
#include "./core/Auth/NoAuth.h"
#include "./core/Auth/UserAccount.h"
#include "./core/Auth/Token/AppToken.h"

namespace firebase_ns
{
    struct auth_data_t
    {
        user_auth_data user_auth;
        app_token_t app_token;
        AsyncResultCallback cb;
        AsyncResult *refResult = nullptr;
        bool auto_renew = true;
        bool force_refresh = false;
    };
};
#endif