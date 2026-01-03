/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_DATA_SERVICE_DATA_H
#define CORE_AUTH_DATA_SERVICE_DATA_H

#include <Arduino.h>
#include "./core/File/FileConfig.h"
#include "./core/Auth/AuthBase.h"

namespace sa_ns
{
    enum data_item_type_t
    {
        cm,   // client email
        pid,  // project Id
        pk,   // private key
        pkid, // private key Id
        cid,  // client Id
        max_type
    };
}

namespace firebase_ns
{
    typedef void (*TimeStatusCallback)(uint32_t &ts);

    enum jwt_step
    {
        jwt_step_begin,
        jwt_step_create_jwt,
        jwt_step_sign,
        jwt_step_ready,
        jwt_step_error
    };

#if defined(ENABLE_SERVICE_AUTH) ||  defined(ENABLE_CUSTOM_AUTH)
    struct service_account
    {
        friend class SAParser;
        friend class user_auth_data;
        friend class CustomAuth;
        friend class ServiceAuth;
        friend class FirebaseApp;
        friend class JWTClass;
        friend class FirebaseClient;

    private:
        StringUtil sut;

    public:
        service_account() {}
        ~service_account() { clear(); }
        void copy(const service_account &rhs)
        {
            for (size_t i = 0; i < sa_ns::max_type; i++)
                this->val[i] = rhs.val[i];
            this->timestatus_cb = rhs.timestatus_cb;
            this->expire = rhs.expire;
            this->step = rhs.step;
        }

        void clear()
        {
            for (size_t i = 0; i < sa_ns::max_type; i++)
                sut.clear(val[i]);
            timestatus_cb = NULL;
            expire = FIREBASE_DEFAULT_TOKEN_TTL;
            step = jwt_step_begin;
        }

    protected:
        String val[sa_ns::max_type];
        jwt_step step = jwt_step_begin;
        TimeStatusCallback timestatus_cb = NULL;
        size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;
    };
#endif
}
#endif