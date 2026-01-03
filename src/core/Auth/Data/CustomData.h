/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_DATA_CUSTOM_DATA_H
#define CORE_AUTH_DATA_CUSTOM_DATA_H

#include <Arduino.h>
#include "./core/Auth/AuthBase.h"

namespace cust_ns
{
    enum data_item_type_t
    {
        api_key, // API Key
        uid,     // UID
        claims,  // claims
        max_type
    };
}

namespace firebase_ns
{
#if defined(ENABLE_CUSTOM_AUTH)
    struct custom_data
    {
        // For custom claims
        String val[cust_ns::max_type];
        TimeStatusCallback timestatus_cb = NULL;

    private:
        StringUtil sut;

    public:
        custom_data() {}
        ~custom_data() { clear(); }
        void copy(const custom_data &rhs)
        {
            for (size_t i = 0; i < cust_ns::max_type; i++)
                this->val[i] = rhs.val[i];
            this->timestatus_cb = rhs.timestatus_cb;
        }

        void clear()
        {
            for (size_t i = 0; i < cust_ns::max_type; i++)
                sut.clear(val[i]);
            timestatus_cb = NULL;
        }
    };
#endif
}
#endif