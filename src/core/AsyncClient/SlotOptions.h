/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_CLIENT_SLOT_OPTIONS_H
#define CORE_ASYNC_CLIENT_SLOT_OPTIONS_H

#include "./core/Auth/Token/AppToken.h"

using namespace firebase_ns;

struct slot_options_t
{
public:
    bool auth_used = false, sse = false, async = false, sv = false, ota = false, no_etag = false, auth_param = false;
    app_token_t *app_token = nullptr;
    user_auth_data *user_auth = nullptr;
    slot_options_t() {}
    explicit slot_options_t(bool auth_used, bool sse, bool async, bool sv, bool ota, bool no_etag, bool auth_param = false)
    {
        this->auth_used = auth_used;
        this->sse = sse;
        this->async = async;
        this->sv = sv;
        this->ota = ota;
        this->no_etag = no_etag;
        this->auth_param = auth_param;
    }
};
#endif