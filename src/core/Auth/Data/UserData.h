/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_DATA_USER_DATA_H
#define CORE_AUTH_DATA_USER_DATA_H

#include <Arduino.h>
#include "./core/Auth/AuthBase.h"

namespace user_ns
{
    enum data_item_type_t
    {
        em,       // email
        psw,      // password
        api_key,  // API Key
                  // used with apiKey for current user verification and delete
        id_token, // Id token
        max_type
    };
}

namespace firebase_ns
{
    struct user_data
    {
        String val[user_ns::max_type];
        size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;

    private:
        StringUtil sut;

    public:
        user_data() {}
        ~user_data() { clear(); }
        void copy(const user_data &rhs)
        {
            for (size_t i = 0; i < user_ns::max_type; i++)
                this->val[i] = rhs.val[i];
            this->expire = rhs.expire;
        }
        void clear()
        {
            for (size_t i = 0; i < user_ns::max_type; i++)
                sut.clear(val[i]);
            expire = FIREBASE_DEFAULT_TOKEN_TTL;
        }
    };

}
#endif