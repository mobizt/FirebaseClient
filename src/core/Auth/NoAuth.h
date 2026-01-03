/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_NO_AUTH_H
#define CORE_AUTH_NO_AUTH_H

#include <Arduino.h>
#include "./core/Auth/UserAuth.h"

namespace firebase_ns
{
    class NoAuth
    {
        friend class FirebaseApp;

    public:
        NoAuth()
        {
            this->data.clear();
            this->data.initialized = true;
            this->data.auth_type = auth_unknown_token;
            this->data.auth_data_type = user_auth_data_no_token;
        }
        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
    };
}
#endif