/**
 * Created February 6, 2024
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
#ifndef CORE_JWT_H
#define CORE_JWT_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/Memory.h"
#include "./core/AuthConfig.h"
#include "./core/Base64.h"
#include "./core/URL.h"
#include "./core/JSON.h"
#include "./core/AsyncClient/RequestHandler.h"
#include "./core/Error.h"

#if defined(ENABLE_JWT)

#if __has_include(<ESP_SSLClient.h>)
#include <ESP_SSLClient.h>
#else
#include "./core/SSLClient/ESP_SSLClient.h"
#endif

using namespace firebase;

namespace firebase
{
    class JWT
    {

    private:
        String jwt;
        StringHelper sh;
        Base64Helper bh;
        URLHelper uh;

    public:
        struct jwt_token_data
        {
            String token;
            int err_code = 0;
            String msg;
            String pk;
            size_t hashSize = 32; // SHA256 size (256 bits or 32 bytes)
            size_t signatureSize = 256;
            char *hash = nullptr;
            unsigned char *signature = nullptr;
            String encHeader;
            String encPayload;
            String encHeadPayload;
            String encSignature;
        };

        String token() { return jwt; }

        bool create(Memory &mem, jwt_token_data *jwt_data, user_auth_data &auth_data, time_t now)
        {

#if !defined(USE_LEGACY_TOKEN_ONLY) && !defined(FIREBASE_USE_LEGACY_TOKEN_ONLY)

            async_request_handler_t req;

            if (auth_data.sa.step == jwt_step_encode_header_payload)
            {

                jwt_data->token.clear();

                // header
                // {"alg":"RS256","typ":"JWT"}

                String header = FPSTR("{\"alg\":\"RS256\",\"typ\":\"JWT\"}");

                size_t len = bh.encodedLength(header.length());
                char *buf = reinterpret_cast<char *>(mem.alloc(len));
                bh.encodeUrl(mem, buf, (unsigned char *)header.c_str(), header.length());
                jwt_data->encHeader = buf;
                mem.release(&buf);
                jwt_data->encHeadPayload = jwt_data->encHeader;

                // payload
                // {"iss":"<email>","sub":"<email>","aud":"<audience>","iat":<timstamp>,"exp":<expire>,"scope":"<scope>"}
                // {"iss":"<email>","sub":"<email>","aud":"<audience>","iat":<timstamp>,"exp":<expire>,"uid":"<uid>","claims":"<claims>"}

                String payload;

                JsonHelper json;

                json.addObject(payload, json.toString("iss"), json.toString(auth_data.sa.client_email));
                json.addObject(payload, json.toString("sub"), json.toString(auth_data.sa.client_email));

                String t = FPSTR("https://");
                if (auth_data.auth_type == auth_sa_custom_token)
                {
                    req.addGAPIsHost(t, "identitytoolkit");
                    t += FPSTR("/google.identity.identitytoolkit.v1.IdentityToolkit");
                }
                else if (auth_data.auth_type == auth_sa_access_token)
                {
                    req.addGAPIsHost(t, "oauth2");
                    t += FPSTR("/token");
                }

                json.addObject(payload, json.toString("aud"), json.toString(t));
                json.addObject(payload, json.toString("iat"), String(now));
                json.addObject(payload, json.toString("exp"), String((int)(now + 3600)));

                if (auth_data.auth_type == auth_sa_access_token)
                {
                    String buri;
                    String host;
                    req.addGAPIsHost(host, "www");
                    uh.host2Url(buri, host);
                    buri += FPSTR("/auth/");

                    String s = buri; // https://www.googleapis.com/auth/
                    s += FPSTR("devstorage.full_control");
                    sh.addSp(s);
                    s += buri; // https://www.googleapis.com/auth/
                    s += FPSTR("datastore");
                    sh.addSp(s);
                    s += buri; // https://www.googleapis.com/auth/
                    s += FPSTR("userinfo.email");
                    sh.addSp(s);
                    s += buri; // https://www.googleapis.com/auth/
                    s += FPSTR("firebase.database");
                    sh.addSp(s);
                    s += buri; // https://www.googleapis.com/auth/
                    s += FPSTR("cloud-platform");
                    sh.addSp(s);
                    s += buri; // https://www.googleapis.com/auth/
                    s += FPSTR("iam");

                    if (auth_data.cust.scope.length() > 0)
                    {
                        char *p = reinterpret_cast<char *>(mem.alloc(auth_data.cust.scope.length()));
                        strcpy(p, auth_data.cust.scope.c_str());
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
                                sh.addSp(s);
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
                else if (auth_data.auth_type == auth_sa_custom_token)
                {
                    json.addObject(payload, json.toString("uid"), json.toString(auth_data.cust.uid), auth_data.cust.claims.length() <= 2);

                    if (auth_data.cust.claims.length() > 2)
                    {
                        json.addObject(payload, json.toString("claims"), json.toString(auth_data.cust.claims), true);
                    }
                }

                len = bh.encodedLength(payload.length());
                buf = reinterpret_cast<char *>(mem.alloc(len));
                bh.encodeUrl(mem, buf, (unsigned char *)payload.c_str(), payload.length());
                jwt_data->encPayload = buf;
                mem.release(&buf);

                jwt_data->encHeadPayload += '.';
                jwt_data->encHeadPayload += jwt_data->encPayload;

                jwt_data->encHeader.clear();
                jwt_data->encPayload.clear();

                // create message digest from encoded header and payload

                jwt_data->hash = reinterpret_cast<char *>(mem.alloc(jwt_data->hashSize));
                br_sha256_context mc;
                br_sha256_init(&mc);
                br_sha256_update(&mc, jwt_data->encHeadPayload.c_str(), jwt_data->encHeadPayload.length());
                br_sha256_out(&mc, jwt_data->hash);

                jwt_data->token = jwt_data->encHeadPayload;
                jwt_data->token += '.';
                jwt_data->encHeadPayload.clear();

                auth_data.sa.step = jwt_step_sign;
            }
            else if (auth_data.sa.step == jwt_step_sign)
            {

                // RSA private key
                PrivateKey *pk = nullptr;
                req.idle();
                // parse priv key
                if (jwt_data->pk.length() > 0)
                    pk = new PrivateKey(jwt_data->pk.c_str());
                else if (auth_data.sa.private_key.length() > 0)
                    pk = new PrivateKey(auth_data.sa.private_key.c_str());

                if (!pk)
                {
                    jwt_data->err_code = FIREBASE_ERROR_TOKEN_PARSE_PK;
                    jwt_data->msg = (const char *)FPSTR("BearSSL, PrivateKey");
                    auth_data.sa.step = jwt_step_error;
                    return false;
                }

                if (!pk->isRSA())
                {
                    delete pk;
                    pk = nullptr;
                    jwt_data->err_code = FIREBASE_ERROR_TOKEN_PARSE_PK;
                    jwt_data->msg = (const char *)FPSTR("BearSSL, isRSA");
                    auth_data.sa.step = jwt_step_error;
                    return false;
                }

                const br_rsa_private_key *br_rsa_key = pk->getRSA();

                // generate RSA signature from private key and message digest
                jwt_data->signature = new unsigned char[jwt_data->signatureSize];

                req.idle();
                int ret = br_rsa_i15_pkcs1_sign(BR_HASH_OID_SHA256, (const unsigned char *)jwt_data->hash,
                                                br_sha256_SIZE, br_rsa_key, jwt_data->signature);
                req.idle();
                mem.release(&jwt_data->hash);

                size_t len = bh.encodedLength(jwt_data->signatureSize);
                char *buf = reinterpret_cast<char *>(mem.alloc(len));
                bh.encodeUrl(mem, buf, jwt_data->signature, jwt_data->signatureSize);
                jwt_data->encSignature = buf;
                mem.release(&buf);
                mem.release(&jwt_data->signature);
                delete pk;
                pk = nullptr;

                // get the signed JWT
                if (ret > 0)
                {
                    jwt_data->token += jwt_data->encSignature;
                    jwt_data->pk.clear();
                    jwt_data->encSignature.clear();
                    auth_data.sa.step = jwt_step_ready;
                    jwt = jwt_data->token;
                    jwt_data->token.clear();
                }
                else
                {
                    jwt_data->err_code = FIREBASE_ERROR_TOKEN_SIGN;
                    jwt_data->msg = (const char *)FPSTR("BearSSL, br_rsa_i15_pkcs1_sign");
                    auth_data.sa.step = jwt_step_error;
                    return false;
                }
            }

#endif

            return true;
        }
    };

}

#endif

#endif