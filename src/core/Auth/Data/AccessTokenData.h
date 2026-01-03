/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_DATA_ACCESS_TOKEN_DATA_H
#define CORE_AUTH_DATA_ACCESS_TOKEN_DATA_H

#include <Arduino.h>
#include "./core/Auth/AuthBase.h"

namespace access_tk_ns
{
    enum data_item_type_t
    {
        token,   // access token
        refresh, // refresh token
        cid,     // client id
        csec,    // client secret
        max_type
    };
}

namespace firebase_ns
{
#if defined(ENABLE_ACCESS_TOKEN)
    struct access_token_data
    {
        String val[access_tk_ns::max_type];
        size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;
        TimeStatusCallback timestatus_cb = NULL;

    private:
        StringUtil sut;

    public:
        access_token_data() {}
        ~access_token_data() { clear(); }
        void copy(const access_token_data &rhs)
        {
            for (size_t i = 0; i < access_tk_ns::max_type; i++)
                this->val[i] = rhs.val[i];
            this->expire = rhs.expire;
            this->timestatus_cb = rhs.timestatus_cb;
        }
        void clear()
        {
            for (size_t i = 0; i < access_tk_ns::max_type; i++)
                sut.clear(val[i]);
            expire = FIREBASE_DEFAULT_TOKEN_TTL;
            timestatus_cb = NULL;
        }
    };
#endif
}
#endif