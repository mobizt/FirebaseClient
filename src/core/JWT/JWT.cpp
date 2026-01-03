/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_JWT_JWT_CPP
#define CORE_JWT_JWT_CPP

#include <Arduino.h>
#include "JWT.h"

#if defined(ENABLE_JWT)

#if defined(ESP32)
#if defined(ESP_ARDUINO_VERSION)
#if ESP_ARDUINO_VERSION > ESP_ARDUINO_VERSION_VAL(3, 0, 0)
#define ESP32_CORE_V3_UP
#endif
#endif
#endif

#if defined(USE_EMBED_SSL_ENGINE) && !defined(CORE_MOCK)
extern "C"
{
#if defined(ESP8266)
    make_stack_thunk(br_rsa_i15_pkcs1_sign);
#endif
};
#endif

namespace firebase_ns
{

    JWTClass::JWTClass()
    {
        err_timer.feed(1);
#if defined(USE_EMBED_SSL_ENGINE)
        stack_thunk_add_ref();
#endif
    }

    JWTClass::~JWTClass()
    {
#if defined(USE_EMBED_SSL_ENGINE)
        stack_thunk_del_ref();
#endif
    }

    const char *JWTClass::token() { return jwt_data.token.c_str(); }

    void JWTClass::clear()
    {
        sut.clear(jwt_data.token);
        sut.clear(jwt_data.pk);
        sut.clear(payload);
        if (this->auth_data)
        {
            this->auth_data->user_auth.sa.step = jwt_step_begin;
            this->auth_data->user_auth.jwt_ts = 0;
            this->auth_data->user_auth.jwt_time_debug = false;
        }
        processing = false;
    }

    bool JWTClass::ready() { return this->auth_data && this->auth_data->user_auth.sa.step == jwt_step_ready; }

    bool JWTClass::loop(auth_data_t *auth_data)
    {
        if (auth_data && auth_data->user_auth.jwt_signing)
        {
            bool ret = begin(auth_data);
            if (ret)
                ret = create();
            if (!ret)
            {
                if (auth_data->refResult)
                    sendErrResult(auth_data->refResult);
                else
                    sendErrCB(auth_data->cb, nullptr);
            }
            return ret;
        }
        return false;
    }

    void JWTClass::sendErrCB(AsyncResultCallback cb, AsyncResult *aResult)
    {
        if (cb)
        {
            if (err_timer.remaining() == 0)
            {
                err_timer.feed(5);
                bool hasRes = aResult != nullptr;
                AsyncResult *ares = aResult;
                if (!hasRes)
                    ares = new AsyncResult();
                ares->error().setLastError(jwt_data.err_code, jwt_data.msg);
                firebase_bebug_callback(cb, *ares, __func__, __LINE__, __FILE__);
                if (!hasRes)
                {
                    delete ares;
                    ares = nullptr;
                }
            }
        }
    }

    void JWTClass::sendErrResult(AsyncResult *refResult)
    {
        if (refResult)
            refResult->error().setLastError(jwt_data.err_code, jwt_data.msg);
    }

    void JWTClass::setAppDebug(app_log_t *debug_log) { this->debug_log = debug_log; }

    bool JWTClass::begin(auth_data_t *auth_data)
    {
        if (processing || !auth_data)
            return false;

        if (auth_data->user_auth.sa.step == jwt_step_begin)
        {
            processing = true;
            this->auth_data = auth_data;
            this->auth_data->app_token.clear();
            this->auth_data->user_auth.jwt_ts = millis();
            this->auth_data->user_auth.sa.step = jwt_step_create_jwt;
            processing = false;
        }
        return true;
    }

    bool JWTClass::create()
    {
#if !defined(USE_LEGACY_TOKEN_ONLY) && !defined(FIREBASE_USE_LEGACY_TOKEN_ONLY)

        if (!auth_data)
            return exit(false);

        if (auth_data->user_auth.sa.step == jwt_step_create_jwt)
        {
            uint32_t now = auth_data->user_auth.ts;
            if (auth_data->user_auth.timestatus_cb)
            {
                if (debug_log && !auth_data->user_auth.jwt_time_debug)
                {
                    auth_data->user_auth.jwt_time_debug = true;
                    debug_log->push_back(-1, "Getting current time...");
                    return exit(false);
                }
                auth_data->user_auth.timestatus_cb(now);
            }
            else
            {
                // Current time calculation based on the counter since the time was set.
                uint32_t diff = millis() > auth_data->user_auth.ms ? millis() - auth_data->user_auth.ms : 0;
                now += diff / 1000;
                auth_data->user_auth.ts = now;
                auth_data->user_auth.ms = millis();
            }

            if (now < FIREBASE_DEFAULT_TS)
            {
                jwt_data.err_code = FIREBASE_ERROR_TIME_IS_NOT_SET_OR_INVALID;
                jwt_data.msg = "JWT, time was not set or not valid. Use FirebaseApp::setTime to set.";
                return exit(false);
            }

            // header
            // {"alg":"RS256","typ":"JWT"}

            size_t len = 0;
            jwt_data.token = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9"; // base64 encoded string of JWT header

            // payload
            // {"iss":"<email>","sub":"<email>","aud":"<audience>","iat":<timstamp>,"exp":<expire>,"scope":"<scope>"}
            // {"iss":"<email>","sub":"<email>","aud":"<audience>","iat":<timstamp>,"exp":<expire>,"uid":"<uid>","claims":"<claims>"}

            json.addObject(payload, "iss", auth_data->user_auth.sa.val[sa_ns::cm], true);
            json.addObject(payload, "sub", auth_data->user_auth.sa.val[sa_ns::cm], true);

            if (auth_data->user_auth.auth_type == auth_sa_custom_token)
                json.addObject(payload, "aud", "https://identitytoolkit.googleapis.com/google.identity.identitytoolkit.v1.IdentityToolkit", true);
            else if (auth_data->user_auth.auth_type == auth_sa_access_token)
                json.addObject(payload, "aud", "https://oauth2.googleapis.com/token", true);

            json.addObject(payload, "iat", String(now), false);
            json.addObject(payload, "exp", String((int)(now + 3600)), false);

#if defined(ENABLE_SERVICE_AUTH)
            if (auth_data->user_auth.auth_type == auth_sa_access_token)
                json.addObject(payload, "scope", "https://www.googleapis.com/auth/devstorage.full_control https://www.googleapis.com/auth/datastore https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/firebase.database https://www.googleapis.com/auth/cloud-platform https://www.googleapis.com/auth/iam", true, true);
#endif

#if defined(ENABLE_CUSTOM_AUTH)
            if (auth_data->user_auth.auth_type == auth_sa_custom_token)
            {
                json.addObject(payload, "uid", auth_data->user_auth.cust.val[cust_ns::uid], true, auth_data->user_auth.cust.val[cust_ns::claims].length() <= 2);
                if (auth_data->user_auth.cust.val[cust_ns::claims].length() > 2)
                    json.addObject(payload, "claims", auth_data->user_auth.cust.val[cust_ns::claims], false, true);
            }
#endif

            len = but.encodedLength(payload.length());
            char *buf = reinterpret_cast<char *>(mem.alloc(len));
            but.encodeUrl(mem, buf, reinterpret_cast<const unsigned char *>(payload.c_str()), payload.length());
            sut.clear(payload);
            jwt_data.token += '.';
            jwt_data.token += buf;
            mem.release(&buf);

#if defined(ESP32)

            if (!jwt_data.hash)
                jwt_data.hash = reinterpret_cast<char *>(mem.alloc(32));

            int ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (const unsigned char *)jwt_data.token.c_str(), jwt_data.token.length(), (uint8_t *)jwt_data.hash);
            if (ret != 0)
            {
                jwt_data.err_code = FIREBASE_ERROR_TOKEN_PARSE_PK;
                jwt_data.msg = "JWT, can't create message digest";
                auth_data->user_auth.sa.step = jwt_step_error;
                return exit(false);
            }
            jwt_data.token += '.';
            auth_data->user_auth.sa.step = jwt_step_sign;
#else

            // create message digest from encoded header and payload
            br_sha256_context mc;
            br_sha256_init(&mc);
            br_sha256_update(&mc, jwt_data.token.c_str(), jwt_data.token.length());
            if (!jwt_data.hash)
                jwt_data.hash = reinterpret_cast<char *>(mem.alloc(32));
            br_sha256_out(&mc, jwt_data.hash);
            jwt_data.token += '.';
            auth_data->user_auth.sa.step = jwt_step_sign;
#endif
        }
        else if (auth_data->user_auth.sa.step == jwt_step_sign)
        {
            int ret = 0;
#if defined(ESP32)

            mbedtls_pk_context *pk_ctx = new mbedtls_pk_context();
            mbedtls_pk_init(pk_ctx);

// parse priv key
#if defined(ESP32_CORE_V3_UP)
            if (jwt_data.pk.length() > 0)
                ret = mbedtls_pk_parse_key(pk_ctx, (const unsigned char *)jwt_data.pk.c_str(), jwt_data.pk.length() + 1, 0, 0, 0, 0);
            else if (auth_data->user_auth.sa.val[sa_ns::pk].length() > 0)
                ret = mbedtls_pk_parse_key(pk_ctx, (const unsigned char *)auth_data->user_auth.sa.val[sa_ns::pk].c_str(), auth_data->user_auth.sa.val[sa_ns::pk].length() + 1, 0, 0, 0, 0);
#else
            if (jwt_data.pk.length() > 0)
                ret = mbedtls_pk_parse_key(pk_ctx, (const unsigned char *)jwt_data.pk.c_str(), jwt_data.pk.length() + 1, NULL, 0);
            else if (auth_data->user_auth.sa.val[sa_ns::pk].length() > 0)
                ret = mbedtls_pk_parse_key(pk_ctx, (const unsigned char *)auth_data->user_auth.sa.val[sa_ns::pk].c_str(), auth_data->user_auth.sa.val[sa_ns::pk].length() + 1, NULL, 0);

#endif
            if (ret != 0)
            {
                jwt_data.err_code = FIREBASE_ERROR_TOKEN_PARSE_PK;
                jwt_data.msg = "JWT, private key parsing failed";
                auth_data->user_auth.sa.step = jwt_step_error;
                return exit(false);
            }

            // generate RSA signature from private key and message digest
            if (!jwt_data.signature)
                jwt_data.signature = reinterpret_cast<unsigned char *>(mem.alloc(256));
            size_t sigLen = 0;
            mbedtls_entropy_context *entropy_ctx = new mbedtls_entropy_context();
            mbedtls_ctr_drbg_context *ctr_drbg_ctx = new mbedtls_ctr_drbg_context();
            mbedtls_entropy_init(entropy_ctx);
            mbedtls_ctr_drbg_init(ctr_drbg_ctx);
            mbedtls_ctr_drbg_seed(ctr_drbg_ctx, mbedtls_entropy_func, entropy_ctx, NULL, 0);
#if defined(ESP32_CORE_V3_UP)
            ret = mbedtls_pk_sign(pk_ctx, MBEDTLS_MD_SHA256, (const unsigned char *)jwt_data.hash, 32, jwt_data.signature, 256, &sigLen, mbedtls_ctr_drbg_random, ctr_drbg_ctx);
#else
            ret = mbedtls_pk_sign(pk_ctx, MBEDTLS_MD_SHA256, (const unsigned char *)jwt_data.hash, 32, jwt_data.signature, &sigLen, mbedtls_ctr_drbg_random, ctr_drbg_ctx);
#endif
            if (ret != 0)
            {
                jwt_data.err_code = FIREBASE_ERROR_TOKEN_SIGN;
                jwt_data.msg = "JWT, private key signing failed";
                auth_data->user_auth.sa.step = jwt_step_error;
                return exit(false);
            }

            ret = 1;
            mbedtls_pk_free(pk_ctx);
            mbedtls_entropy_free(entropy_ctx);
            mbedtls_ctr_drbg_free(ctr_drbg_ctx);
            delete pk_ctx;
            delete entropy_ctx;
            delete ctr_drbg_ctx;

#else
            // RSA private key
            PrivateKey *pk = nullptr;
            sys_idle();

            // parse priv key
            if (jwt_data.pk.length() > 0)
                pk = new PrivateKey(jwt_data.pk.c_str());
            else if (auth_data->user_auth.sa.val[sa_ns::pk].length() > 0)
                pk = new PrivateKey(auth_data->user_auth.sa.val[sa_ns::pk].c_str());

            sut.clear(jwt_data.pk);

            if (!pk)
            {
                jwt_data.err_code = FIREBASE_ERROR_TOKEN_PARSE_PK;
                jwt_data.msg = "JWT, private key parsing failed";
                auth_data->user_auth.sa.step = jwt_step_error;
                return exit(false);
            }

            if (!pk->isRSA())
            {
                delete pk;
                pk = nullptr;
                jwt_data.err_code = FIREBASE_ERROR_TOKEN_PARSE_PK;
                jwt_data.msg = "JWT, invalid RSA private key";
                auth_data->user_auth.sa.step = jwt_step_error;
                return exit(false);
            }

            const br_rsa_private_key *br_rsa_key = pk->getRSA();

            // generate RSA signature from private key and message digest
            sys_idle();
            if (!jwt_data.signature)
                jwt_data.signature = reinterpret_cast<unsigned char *>(mem.alloc(256));
            ret = br_rsa_i15_pkcs1_sign(BR_HASH_OID_SHA256, reinterpret_cast<const unsigned char *>(jwt_data.hash), br_sha256_SIZE, br_rsa_key, jwt_data.signature);
            sys_idle();

            delete pk;
            pk = nullptr;

#endif

            if (jwt_data.hash)
                mem.release(&jwt_data.hash);

            size_t len = but.encodedLength(256);
            char *buf = reinterpret_cast<char *>(mem.alloc(len));
            but.encodeUrl(mem, buf, jwt_data.signature, 256);

            // get the signed JWT
            if (ret > 0)
            {
                jwt_data.token += buf;
                mem.release(&buf);
                auth_data->user_auth.sa.step = jwt_step_ready;
            }
            else
            {
                mem.release(&buf);
                jwt_data.err_code = FIREBASE_ERROR_TOKEN_SIGN;
                jwt_data.msg = "JWT, token signing failed";
                auth_data->user_auth.sa.step = jwt_step_error;
                return exit(false);
            }
        }

#endif
        return exit(true);
    }

    JWTClass JWT;
}

#endif
#endif