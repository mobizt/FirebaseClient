/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_DATA_LEGACY_TOKEN_DATA_H
#define CORE_AUTH_DATA_LEGACY_TOKEN_DATA_H

#include <Arduino.h>
#include "./core/Auth/AuthBase.h"

namespace legacy_tk_ns
{
    enum data_item_type_t
    {
        token, // token
        max_type
    };
}

namespace firebase_ns
{
#if defined(ENABLE_LEGACY_TOKEN)
        struct legacy_token_data
        {
            String val[legacy_tk_ns::max_type];

        private:
            StringUtil sut;

        public:
            legacy_token_data() {}
            ~legacy_token_data() { clear(); }
            void copy(const legacy_token_data &rhs) { this->val[legacy_tk_ns::token] = rhs.val[legacy_tk_ns::token]; }
            void clear() { sut.clear(val[legacy_tk_ns::token]); }
        };
#endif
}
#endif