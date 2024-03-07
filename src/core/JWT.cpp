/**
 * Created March 7, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef JWT_CPP
#define JWT_CPP

#include <Arduino.h>
#include "./Config.h"
#include "./core/Memory.h"
#include "./core/AuthConfig.h"
#include "./core/Base64.h"
#include "./core/URL.h"
#include "./core/JSON.h"
#include "./core/Error.h"

#if defined(ENABLE_JWT)

#include "JWT.h"

#if defined(USE_EMBED_SSL_ENGINE) && !defined(CORE_MOCK)
extern "C"
{
    make_stack_thunk(br_rsa_i15_pkcs1_sign);
};
#endif

JWTClass::JWTClass()
{
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
    jwt_data.token.remove(0, jwt_data.token.length());
    jwt_data.pk.remove(0, jwt_data.pk.length());
    payload.remove(0, payload.length());
    if (this->auth_data)
        this->auth_data->user_auth.sa.step = jwt_step_encode_header_payload;
    processing = false;
}

bool JWTClass::ready()
{
    return this->auth_data && this->auth_data->user_auth.sa.step == jwt_step_ready;
}

bool JWTClass::begin(auth_data_t *auth_data)
{
    if (processing || !auth_data)
        return false;
    processing = true;
    this->auth_data = auth_data;
    this->auth_data->app_token.clear();
    this->auth_data->user_auth.sa.step = jwt_step_encode_header_payload;
    return create();
}

bool JWTClass::create()
{

#if !defined(USE_LEGACY_TOKEN_ONLY) && !defined(FIREBASE_USE_LEGACY_TOKEN_ONLY)

    if (!auth_data)
        return exit(false);

    if (auth_data->user_auth.sa.step == jwt_step_encode_header_payload)
    {

        uint32_t now = 0;
        if (auth_data->user_auth.timestatus_cb)
            auth_data->user_auth.timestatus_cb(now);

        // header
        // {"alg":"RS256","typ":"JWT"}

        size_t len = 0;
        jwt_data.token = FPSTR("eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9");

        // payload
        // {"iss":"<email>","sub":"<email>","aud":"<audience>","iat":<timstamp>,"exp":<expire>,"scope":"<scope>"}
        // {"iss":"<email>","sub":"<email>","aud":"<audience>","iat":<timstamp>,"exp":<expire>,"uid":"<uid>","claims":"<claims>"}

        json.addObject(payload, json.toString("iss"), json.toString(auth_data->user_auth.sa.client_email));
        json.addObject(payload, json.toString("sub"), json.toString(auth_data->user_auth.sa.client_email));

        String t = FPSTR("https://");
        if (auth_data->user_auth.auth_type == auth_sa_custom_token)
        {
            jwt_add_gapis_host(t, "identitytoolkit");
            t += FPSTR("/google.identity.identitytoolkit.v1.IdentityToolkit");
        }
        else if (auth_data->user_auth.auth_type == auth_sa_access_token)
        {
            jwt_add_gapis_host(t, "oauth2");
            t += FPSTR("/token");
        }

        json.addObject(payload, json.toString("aud"), json.toString(t));
        t.remove(0, t.length());
        json.addObject(payload, json.toString("iat"), String(now));
        json.addObject(payload, json.toString("exp"), String((int)(now + 3600)));

        if (auth_data->user_auth.auth_type == auth_sa_access_token)
        {
            String buri;
            String host;
            jwt_add_gapis_host(host, "www");
            uh.host2Url(buri, host);
            buri += FPSTR("/auth/");

            String s = buri; // https://www.googleapis.com/auth/
            s += FPSTR("devstorage.full_control");
            jwt_add_sp(s);
            s += buri; // https://www.googleapis.com/auth/
            s += FPSTR("datastore");
            jwt_add_sp(s);
            s += buri; // https://www.googleapis.com/auth/
            s += FPSTR("userinfo.email");
            jwt_add_sp(s);
            s += buri; // https://www.googleapis.com/auth/
            s += FPSTR("firebase.database");
            jwt_add_sp(s);
            s += buri; // https://www.googleapis.com/auth/
            s += FPSTR("cloud-platform");
            jwt_add_sp(s);
            s += buri; // https://www.googleapis.com/auth/
            s += FPSTR("iam");
            buri.remove(0, buri.length());

            if (auth_data->user_auth.cust.scope.length() > 0)
            {
                char *p =reinterpret_cast<char *>(mem.alloc(auth_data->user_auth.cust.scope.length()));
                strcpy(p, auth_data->user_auth.cust.scope.c_str());
                char *pp = p;
                char *end = p;
                String tmp;
                int i = 0;
                while (pp != NULL)
                {
                    sh.strsepImpl(&end, ",");
                    if (strlen(pp) > 0)
                    {
                        tmp = pp;
                        jwt_add_sp(s);
                        s += tmp;
                        i++;
                    }
                    pp = end;
                }
                // release memory
                mem.release(&p);
            }

            json.addObject(payload, json.toString("scope"), json.toString(s), true);
        }
        else if (auth_data->user_auth.auth_type == auth_sa_custom_token)
        {
            json.addObject(payload, json.toString("uid"), json.toString(auth_data->user_auth.cust.uid), auth_data->user_auth.cust.claims.length() <= 2);

            if (auth_data->user_auth.cust.claims.length() > 2)
            {
                json.addObject(payload, json.toString("claims"), json.toString(auth_data->user_auth.cust.claims), true);
            }
        }

        len = bh.encodedLength(payload.length());
        char *buf = reinterpret_cast<char *>(mem.alloc(len));
        bh.encodeUrl(mem, buf, (unsigned char *)payload.c_str(), payload.length());
        payload.remove(0, payload.length());
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
        jwt_idle();
        // parse priv key
        if (jwt_data.pk.length() > 0)
            pk = new PrivateKey(jwt_data.pk.c_str());
        else if (auth_data->user_auth.sa.private_key.length() > 0)
            pk = new PrivateKey(auth_data->user_auth.sa.private_key.c_str());

        jwt_data.pk.remove(0, jwt_data.pk.length());

        if (!pk)
        {
            jwt_data.err_code = FIREBASE_ERROR_TOKEN_PARSE_PK;
            jwt_data.msg = (const char *)FPSTR("BearSSL, PrivateKey");
            auth_data->user_auth.sa.step = jwt_step_error;
            return exit(false);
        }

        if (!pk->isRSA())
        {
            delete pk;
            pk = nullptr;
            jwt_data.err_code = FIREBASE_ERROR_TOKEN_PARSE_PK;
            jwt_data.msg = (const char *)FPSTR("BearSSL, isRSA");
            auth_data->user_auth.sa.step = jwt_step_error;
            return exit(false);
        }

        const br_rsa_private_key *br_rsa_key = pk->getRSA();

        // generate RSA signature from private key and message digest
        jwt_idle();
        if (!jwt_data.signature)
            jwt_data.signature = reinterpret_cast<unsigned char *>(mem.alloc(256));
        int ret = br_rsa_i15_pkcs1_sign(BR_HASH_OID_SHA256, (const unsigned char *)jwt_data.hash, br_sha256_SIZE, br_rsa_key, jwt_data.signature);
        jwt_idle();

        if (jwt_data.hash)
            mem.release(&jwt_data.hash);

        size_t len = bh.encodedLength(256);
        char *buf = reinterpret_cast<char *>(mem.alloc(len));
        bh.encodeUrl(mem, buf, jwt_data.signature, 256);

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
            jwt_data.msg = (const char *)FPSTR("BearSSL, br_rsa_i15_pkcs1_sign");
            auth_data->user_auth.sa.step = jwt_step_error;
            return exit(false);
        }
    }

#endif

    return exit(true);
}

#endif

#endif