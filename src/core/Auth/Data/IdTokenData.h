/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_DATA_ID_TOKEN_DATA_H
#define CORE_AUTH_DATA_ID_TOKEN_DATA_H

#include <Arduino.h>
#include "./core/Auth/AuthBase.h"

namespace id_tk_ns
{
    enum data_item_type_t
    {
        token,   // id token
        refresh, // refresh token
        max_type
    };
}

namespace firebase_ns
{

#if defined(ENABLE_ID_TOKEN)
    struct id_token_data
    {
        String val[id_tk_ns::max_type];
        size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;

    private:
        StringUtil sut;

    public:
        id_token_data() {}
        ~id_token_data() { clear(); }
        void copy(const id_token_data &rhs)
        {
            for (size_t i = 0; i < id_tk_ns::max_type; i++)
                this->val[i] = rhs.val[i];
            this->expire = rhs.expire;
        }
        void clear()
        {
            for (size_t i = 0; i < id_tk_ns::max_type; i++)
                sut.clear(val[i]);
            expire = FIREBASE_DEFAULT_TOKEN_TTL;
        }
    };
#endif
}
#endif