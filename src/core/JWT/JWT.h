/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_JWT_JWT_H
#define CORE_JWT_JWT_H

#include <Arduino.h>
#include "./core/Utils/Memory.h"
#include "./core/Auth/AuthConfig.h"
#include "./core/Utils/Base64.h"
#include "./core/Utils/URL.h"
#include "./core/Utils/JSON.h"
#include "./core/Error.h"
#include "./core/Core.h"
#include "./core/Utils/Timer.h"
#include "./core/Debug.h"

#if defined(ENABLE_JWT)

#if defined(ESP32)
#include <mbedtls/entropy.h>
#include <mbedtls/pk.h>
#include <mbedtls/ctr_drbg.h>
#endif

using namespace firebase_ns;

namespace firebase_ns
{
    struct jwt_token_data_t
    {
        String token, msg, pk;
        int err_code = 0;
        char *hash = nullptr;               // SHA256 size (256 bits or 32 bytes)
        unsigned char *signature = nullptr; // 256 bytes
    };

    class JWTClass
    {
        friend class FirebaseApp;
        friend class FirebaseClient;

    private:
        Memory mem;
        StringUtil sut;
        Base64Util but;
        URLUtil uut;
        String payload;
        JSONUtil json;
        jwt_token_data_t jwt_data;
        Timer err_timer;
        auth_data_t *auth_data = nullptr;
        app_log_t *debug_log = nullptr;
        bool processing = false;

        bool exit(bool ret)
        {
            processing = false;
            if (jwt_data.signature)
                mem.release(&jwt_data.signature);
            if (jwt_data.hash && !ret)
                mem.release(&jwt_data.hash);
            return ret;
        }

        bool begin(auth_data_t *auth_data);
        bool create();
        void sendErrCB(AsyncResultCallback cb, AsyncResult *aResult = nullptr);
        void sendErrResult(AsyncResult *refResult);
        void setAppDebug(app_log_t *debug_log);
        const char *token();
        bool ready();
        void clear();

    public:
        JWTClass();
        ~JWTClass();

        /**
         * Process the JWT token generation and returns the status.
         *
         * @return boolean of JWT processor result.
         */
        bool loop(auth_data_t *auth_data);
    };

    extern JWTClass JWT;
}
#endif
#endif