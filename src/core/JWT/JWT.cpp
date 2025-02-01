#ifndef CORE_JWT_JWT_CPP
#define CORE_JWT_JWT_CPP

#include <Arduino.h>
#include "./Config.h"
#include "./core/Utils/Memory.h"
#include "./core/Auth/AuthConfig.h"
#include "./core/Utils/URL.h"
#include "./core/Utils/JSON.h"
#include "./core/Error.h"
#include "./core/Core.h"

#if defined(ENABLE_JWT)
#include "JWT.h"

#if defined(USE_EMBED_SSL_ENGINE) && !defined(CORE_MOCK)
extern "C"
{
#if defined(ESP8266)
    make_stack_thunk(br_rsa_i15_pkcs1_sign);
#endif
};
#endif

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
            cb(*ares);
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

void JWTClass::setAppDebug(app_debug_t *app_debug) { this->app_debug = app_debug; }

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
            if (app_debug && !auth_data->user_auth.jwt_time_debug)
            {
                auth_data->user_auth.jwt_time_debug = true;
                app_debug->setDebug(FPSTR("Getting current time..."));
                return exit(false);
            }
            auth_data->user_auth.timestatus_cb(now);
        }

        if (now < FIREBASE_DEFAULT_TS)
        {
            jwt_data.err_code = FIREBASE_ERROR_TIME_IS_NOT_SET_OR_INVALID;
            jwt_data.msg = FPSTR("JWT, time was not set or not valid");
            return exit(false);
        }

        // header
        // {"alg":"RS256","typ":"JWT"}

        size_t len = 0;
        jwt_data.token = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9";

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

        if (auth_data->user_auth.auth_type == auth_sa_access_token)
        {
            String s = "https://www.googleapis.com/auth/devstorage.full_control https://www.googleapis.com/auth/datastore https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/firebase.database https://www.googleapis.com/auth/cloud-platform https://www.googleapis.com/auth/iam";
            if (auth_data->user_auth.cust.val[cust_ns::scope].length() > 0)
            {
                char *p = reinterpret_cast<char *>(mem.alloc(auth_data->user_auth.cust.val[cust_ns::scope].length()));
                strcpy(p, auth_data->user_auth.cust.val[cust_ns::scope].c_str());
                char *pp = p;
                char *end = p;
                String tmp;
                int i = 0;
                while (pp != NULL)
                {
                    sut.strsepImpl(&end, ",");
                    if (strlen(pp) > 0)
                    {
                        tmp = pp;
                        s += ' ';
                        s += tmp;
                        i++;
                    }
                    pp = end;
                }
                // release memory
                mem.release(&p);
            }
            json.addObject(payload, "scope", s, true, true);
        }
        else if (auth_data->user_auth.auth_type == auth_sa_custom_token)
        {
            json.addObject(payload, "uid", auth_data->user_auth.cust.val[cust_ns::uid], true, auth_data->user_auth.cust.val[cust_ns::claims].length() <= 2);
            if (auth_data->user_auth.cust.val[cust_ns::claims].length() > 2)
                json.addObject(payload, "claims", auth_data->user_auth.cust.val[cust_ns::claims], true, true);
        }

        len = but.encodedLength(payload.length());
        char *buf = reinterpret_cast<char *>(mem.alloc(len));
        but.encodeUrl(mem, buf, reinterpret_cast<const unsigned char *>(payload.c_str()), payload.length());
        sut.clear(payload);
        jwt_data.token += '.';
        jwt_data.token += buf;
        mem.release(&buf);

        // create message digest from encoded header and payload
        br_sha256_context mc;
        br_sha256_init(&mc);
        br_sha256_update(&mc, jwt_data.token.c_str(), jwt_data.token.length());
        if (!jwt_data.hash)
            jwt_data.hash = reinterpret_cast<char *>(mem.alloc(32));
        br_sha256_out(&mc, jwt_data.hash);
        jwt_data.token += '.';
        auth_data->user_auth.sa.step = jwt_step_sign;
    }
    else if (auth_data->user_auth.sa.step == jwt_step_sign)
    {
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
            jwt_data.msg = FPSTR("JWT, private key parsing failed");
            auth_data->user_auth.sa.step = jwt_step_error;
            return exit(false);
        }

        if (!pk->isRSA())
        {
            delete pk;
            pk = nullptr;
            jwt_data.err_code = FIREBASE_ERROR_TOKEN_PARSE_PK;
            jwt_data.msg = FPSTR("JWT, invalid RSA private key");
            auth_data->user_auth.sa.step = jwt_step_error;
            return exit(false);
        }

        const br_rsa_private_key *br_rsa_key = pk->getRSA();

        // generate RSA signature from private key and message digest
        sys_idle();
        if (!jwt_data.signature)
            jwt_data.signature = reinterpret_cast<unsigned char *>(mem.alloc(256));
        int ret = br_rsa_i15_pkcs1_sign(BR_HASH_OID_SHA256, reinterpret_cast<const unsigned char *>(jwt_data.hash), br_sha256_SIZE, br_rsa_key, jwt_data.signature);
        sys_idle();

        if (jwt_data.hash)
            mem.release(&jwt_data.hash);

        size_t len = but.encodedLength(256);
        char *buf = reinterpret_cast<char *>(mem.alloc(len));
        but.encodeUrl(mem, buf, jwt_data.signature, 256);

        delete pk;
        pk = nullptr;

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
            jwt_data.msg = FPSTR("JWT, token signing failed");
            auth_data->user_auth.sa.step = jwt_step_error;
            return exit(false);
        }
    }

#endif
    return exit(true);
}
#endif
#endif