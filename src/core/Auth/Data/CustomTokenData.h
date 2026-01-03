/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_DATA_CUSTOM_TOKEN_DATA_H
#define CORE_AUTH_DATA_CUSTOM_TOKEN_DATA_H

#include <Arduino.h>
#include "./core/Auth/AuthBase.h"

namespace cust_tk_ns
{
    enum data_item_type_t
    {
        token, // id token
        max_type
    };
}

namespace firebase_ns
{
#if defined(ENABLE_CUSTOM_TOKEN)
    struct custom_token_data
    {
        String val[cust_tk_ns::max_type];
        size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;
        TimeStatusCallback timestatus_cb = NULL;

    private:
        StringUtil sut;

    public:
        custom_token_data() {}
        ~custom_token_data() { clear(); }
        void copy(const custom_token_data &rhs)
        {
            this->val[cust_tk_ns::token] = rhs.val[cust_tk_ns::token];
            this->expire = rhs.expire;
            this->timestatus_cb = rhs.timestatus_cb;
        }
        void clear()
        {
            sut.clear(val[cust_tk_ns::token]);
            expire = FIREBASE_DEFAULT_TOKEN_TTL;
            timestatus_cb = NULL;
        }
    };
#endif
}
#endif