/**
 * Created March 8, 2024
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
#ifndef CORE_FIREBASE_APP_H
#define CORE_FIREBASE_APP_H
#include <Arduino.h>
#include "./Config.h"
#include "./core/AuthConfig.h"
#include "./core/AsyncClient/AsyncClient.h"
#include "./core/List.h"
#if defined(ENABLE_JWT)
#include "./core/JWT.h"
#endif
#include "./core/Timer.h"

#define FIREBASE_DEFAULT_TS 1618971013

namespace firebase
{

    static JWTClass JWT;

    class FirebaseApp
    {
        friend class FirebaseClient;
        friend class Database;
        friend class Firestore;
        friend class Functions;
        friend class Messaging;
        friend class Storage;
        friend class CloudStorage;

    private:
        Timer req_timer;
        uint16_t slot = 0;
        async_data_item_t *sData = nullptr;
        auth_data_t auth_data;
        AsyncClientClass *aClient = nullptr;
        uint32_t aclient_addr = 0;
        uint32_t app_addr = 0;
        uint32_t ref_ts = 0;
        AsyncResultCallback resultCb = NULL;
        Timer auth_timer, err_timer;
        List vec;
        bool processing = false;
        uint32_t expire = 3600;
        JsonHelper json;
        String extras;
        String subdomain;
        String host;
        slot_options_t sop;

#if defined(ENABLE_JWT)

        void setLastError(AsyncResult *aResult, int code, const String &message)
        {
            if (aResult)
                aResult->lastError.setLastError(code, message);
        }

#endif

        bool parseItem(StringHelper &sh, const String &src, String &dest, const String &name, const String &delim, int &p1, int &p2)
        {
            sh.parse(src, name, delim, p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                if (src[p1] == '"')
                    p1++;
                if (src[p2] == '"')
                    p2--;
                dest = src.substring(p1, p2);
                p1 = p2;
                return true;
            }
            p1 = 0;
            p2 = 0;
            return false;
        }

        template <typename T = int>
        bool parseItem(StringHelper &sh, const String &src, T &dest, const String &name, const String &delim, int &p1, int &p2)
        {
            sh.parse(src, name, delim, p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                if (src[p1] == '"')
                    p1++;
                if (src[p2] == '"')
                    p2--;
                dest = atoi(src.substring(p1, p2).c_str());
                p1 = p2;
                return true;
            }
            p1 = 0;
            p2 = 0;
            return false;
        }

        bool parseToken(const String &payload)
        {
            StringHelper sh;
            int p1 = 0, p2 = 0;
            auth_data.app_token.clear();
            String token, refresh;

            if (payload.indexOf("\"error\"") > -1)
            {
                String str;
                if (parseItem(sh, payload, str, "\"error\"", auth_data.user_auth.auth_type == auth_user_id_token || auth_data.user_auth.auth_type == auth_sa_custom_token ? "}" : ",", p1, p2))
                {
                    int code = 0;
                    str = "";
                    p1 = 0;
                    p2 = 0;
                    parseItem(sh, payload, code, "\"code\"", ",", p1, p2);
                    parseItem(sh, payload, str, "\"message\"", ",", p1, p2);
                    parseItem(sh, payload, str, "\"error_description\"", "}", p1, p2);
                    if (str[str.length() - 1] == '"')
                        str[str.length() - 1] = '\0';
                    setLastError(sData ? &sData->aResult : nullptr, code, str);
                }
            }
            else if (payload.indexOf("\"idToken\"") > -1)
            {
                parseItem(sh, payload, auth_data.app_token.uid, "\"localId\"", ",", p1, p2);
                p1 = 0;
                p2 = 0;
                sh.trim(auth_data.app_token.uid);
                if (parseItem(sh, payload, token, "\"idToken\"", ",", p1, p2))
                {
                    sh.trim(token);
                    parseItem(sh, payload, refresh, "\"refreshToken\"", ",", p1, p2);
                    sh.trim(refresh);
                    parseItem(sh, payload, auth_data.app_token.expire, "\"expiresIn\"", "}", p1, p2);
                }
            }
            else if (payload.indexOf("\"id_token\"") > -1)
            {
                parseItem(sh, payload, auth_data.app_token.expire, "\"expires_in\"", ",", p1, p2);
                parseItem(sh, payload, refresh, "\"refresh_token\"", ",", p1, p2);
                parseItem(sh, payload, token, "\"id_token\"", ",", p1, p2);
                parseItem(sh, payload, auth_data.app_token.uid, "\"user_id\"", ",", p1, p2);
                sh.trim(refresh);
                sh.trim(token);
                sh.trim(auth_data.app_token.uid);
            }
            else if (payload.indexOf("\"access_token\"") > -1)
            {
                if (parseItem(sh, payload, token, "\"access_token\"", ",", p1, p2))
                {
                    parseItem(sh, payload, auth_data.app_token.expire, "\"expires_in\"", ",", p1, p2);
                    parseItem(sh, payload, auth_data.app_token.token_type, "\"token_type\"", "}", p1, p2);
                }
            }
            auth_data.app_token.token = token;
            auth_data.app_token.refresh = refresh;
            auth_data.app_token.project_id = auth_data.user_auth.sa.project_id;
            return token.length() > 0;
        }

        void setEvent(firebase_auth_event_type event)
        {
            auth_data.user_auth.status._event = event;

            if (event == auth_event_initializing || event == auth_event_authenticating)
                processing = true;

            if (event == auth_event_error)
            {
                err_timer.feed(5);
                auth_timer.stop();
            }

            setEventResult(sData ? &sData->aResult : nullptr, auth_data.user_auth.status.authEventString(auth_data.user_auth.status._event), auth_data.user_auth.status._event);

            if (resultCb && sData)
                resultCb(sData->aResult);

            if (event == auth_event_error || event == auth_event_ready)
            {
                processing = false;
                stop(aClient);
                event = auth_event_uninitialized;
                clearLastError(sData ? &sData->aResult : nullptr);
                remove(aClient);
            }
        }

        void remove(AsyncClientClass *aClient)
        {
            if (!aClient)
                return;
            aClient->handleRemove();
        }

        void clearLastError(AsyncResult *aResult)
        {
            if (aResult)
                aResult->lastError.setLastError(0, "");
        }

        void setEventResult(AsyncResult *aResult, const String &msg, int code)
        {
            if (aResult)
                aResult->app_event.setEvent(code, msg);
        }

        void addGAPIsHost(String &str, PGM_P sub)
        {
            str += sub;
            if (str[str.length() - 1] != '.')
                str += ".";
            str += FPSTR("googleapis.com");
        }

        void addContentTypeHeader(String &header, PGM_P v)
        {
            header += FPSTR("Content-Type: ");
            header += v;
            header += FPSTR("\r\n");
        }

        void createSlot(AsyncClientClass *aClient, slot_options_t &soption)
        {
            sData = aClient->createSlot(soption);
        }

        void newRequest(AsyncClientClass *aClient, slot_options_t &soption, const String &subdomain, const String &extras, AsyncResultCallback resultCb, const String &uid = "")
        {
            if (!aClient)
                return;

            if (sData)
            {
                addGAPIsHost(host, subdomain.c_str());
                aClient->newRequest(sData, host, extras, "", async_request_handler_t::http_post, soption, uid);

                addContentTypeHeader(sData->request.header, "application/json");
                aClient->setContentLength(sData, sData->request.payload.length());
                req_timer.feed(FIREBASE_TCP_READ_TIMEOUT_SEC);
                slot = aClient->slotCount() - 1;
                sData->aResult.setDebug(FPSTR("Connecting to server..."));
                if (resultCb)
                    resultCb(sData->aResult);
            }
        }

        void process(AsyncClientClass *aClient, AsyncResult *aResult, AsyncResultCallback resultCb)
        {
            if (!aClient)
                return;

            aClient->process(true);
            aClient->handleRemove();

            if (resultCb && aResult && aResult->error().code() != 0 && aResult->error_available)
            {
                aResult->data_available = false;
                resultCb(*aResult);
            }
        }

        void stop(AsyncClientClass *aClient)
        {
            if (!aClient)
                return;
            aClient->stop(sData);
            if (sData)
            {
                delete sData;
            }
            sData = nullptr;
        }

        bool processAuth()
        {

            sys_idle();

            process(aClient, sData ? &sData->aResult : nullptr, resultCb);

            if (!isExpired())
                return true;

            if (!processing)
            {
                if ((auth_data.user_auth.auth_type == auth_access_token || auth_data.user_auth.auth_type == auth_custom_token) && isExpired())
                {
                    processing = true;
                    auth_data.user_auth.task_type = firebase_core_auth_task_type_refresh_token;
                    setEvent(auth_event_uninitialized);
                }
                else if ((auth_data.user_auth.status._event == auth_event_error || auth_data.user_auth.status._event == auth_event_ready) && (auth_data.app_token.expire == 0 || (auth_data.app_token.expire > 0 && isExpired())))
                {
                    processing = true;
                    setEvent(auth_event_uninitialized);
                }
            }

            if (auth_data.user_auth.status._event == auth_event_uninitialized && err_timer.remaining() > 0)
                return false;

            if (auth_data.user_auth.auth_type == auth_access_token ||
                auth_data.user_auth.auth_type == auth_sa_access_token ||
                ((auth_data.user_auth.auth_type == auth_custom_token || auth_data.user_auth.auth_type == auth_sa_custom_token) &&
                 auth_data.app_token.refresh.length() == 0))
            {

                if (auth_data.user_auth.status._event == auth_event_uninitialized)
                {
                    if (auth_data.user_auth.auth_type == auth_custom_token || (auth_data.user_auth.auth_type == auth_access_token && auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token))
                        setEvent(auth_event_authenticating);
                    else
                        setEvent(auth_event_initializing);
                }

                if (auth_data.user_auth.status._event == auth_event_initializing || auth_data.user_auth.status._event == auth_event_token_signing)
                {
                    sys_idle();
#if defined(ENABLE_JWT)
                    if (auth_data.user_auth.sa.step == jwt_step_begin || auth_data.user_auth.sa.step == jwt_step_encode_header_payload)
                    {
                        auth_data.user_auth.sa.step = jwt_step_sign;
                        stop(aClient);
                        setEvent(auth_event_token_signing);
                        auth_data.user_auth.jwt_signing = true;
                    }
                    else if (auth_data.user_auth.sa.step == jwt_step_sign || auth_data.user_auth.sa.step == jwt_step_ready)
                    {
                        if (JWT.ready())
                        {
                            setEvent(auth_event_authenticating);
                            auth_data.user_auth.sa.step = jwt_step_begin;
                        }
                    }
#endif
                }
                else if (auth_data.user_auth.status._event == auth_event_authenticating)
                {

                    subdomain = auth_data.user_auth.auth_type == auth_sa_access_token || auth_data.user_auth.auth_type == auth_access_token ? FPSTR("oauth2") : FPSTR("identitytoolkit");
                    sop.async = true;
                    sop.auth_used = true;

                    createSlot(aClient, sop);

                    if (auth_data.user_auth.auth_type == auth_sa_access_token)
                    {
#if defined(ENABLE_SERVICE_AUTH)
                        json.addObject(sData->request.payload, json.toString("grant_type"), json.toString("urn:ietf:params:oauth:grant-type:jwt-bearer"));
                        json.addObject(sData->request.payload, json.toString("assertion"), json.toString(JWT.token()), true);
                        JWT.clear();
#endif
                    }
                    else if (auth_data.user_auth.auth_type == auth_sa_custom_token || auth_data.user_auth.auth_type == auth_custom_token)
                    {
#if defined(ENABLE_CUSTOM_AUTH)
                        if (auth_data.user_auth.auth_type == auth_sa_custom_token)
                        {
                            json.addObject(sData->request.payload, json.toString("token"), json.toString(JWT.token()));
                            JWT.clear();
                        }
#endif
#if defined(ENABLE_CUSTOM_TOKEN)
                        if (auth_data.user_auth.auth_type == auth_custom_token)
                            json.addObject(sData->request.payload, json.toString("token"), json.toString(auth_data.user_auth.custom_token.token));
#endif

                        json.addObject(sData->request.payload, json.toString("returnSecureToken"), "true", true);
                    }
                    else if (auth_data.user_auth.auth_type == auth_access_token && auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token)
                    {
#if defined(ENABLE_ACCESS_TOKEN)
                        json.addObject(sData->request.payload, json.toString("client_id"), json.toString(auth_data.user_auth.access_token.client_id));
                        json.addObject(sData->request.payload, json.toString("client_secret"), json.toString(auth_data.user_auth.access_token.client_secret));
                        json.addObject(sData->request.payload, json.toString("grant_type"), json.toString("refresh_token"));
                        json.addObject(sData->request.payload, json.toString("refresh_token"), json.toString(auth_data.user_auth.access_token.refresh), true);
#endif
                    }

                    if (auth_data.user_auth.auth_type == auth_sa_access_token || auth_data.user_auth.auth_type == auth_access_token)
                        extras = FPSTR("/token");
                    else
                    {
                        extras = FPSTR("/v1/accounts:signInWithCustomToken?key=");
                        if (auth_data.user_auth.auth_type == auth_sa_custom_token)
                        {
#if defined(ENABLE_CUSTOM_AUTH)
                            extras += auth_data.user_auth.cust.api_key;
#endif
                        }
                        else
                            extras += auth_data.user_auth.user.api_key;
                    }

                    newRequest(aClient, sop, subdomain, extras, resultCb);
                    extras.remove(0, extras.length());
                    host.remove(0, host.length());
                    setEvent(auth_event_auth_request_sent);
                }
            }
            else
            {
                // user/pass auth
                if (auth_data.user_auth.status._event == auth_event_uninitialized)
                    setEvent(auth_event_authenticating);

                if ((auth_data.user_auth.task_type == firebase_core_auth_task_type_undefined || auth_data.user_auth.auth_type == auth_sa_custom_token) && auth_data.app_token.refresh.length())
                    auth_data.user_auth.task_type = firebase_core_auth_task_type_refresh_token;

                subdomain = auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? FPSTR("securetoken") : FPSTR("identitytoolkit");

                if (auth_data.user_auth.status._event == auth_event_authenticating)
                {
                    sop.async = true;
                    sop.auth_used = true;
                    createSlot(aClient, sop);

                    if (auth_data.user_auth.task_type == firebase_core_auth_task_type_reset_password || auth_data.user_auth.task_type == firebase_core_auth_task_type_send_verify_email)
                    {
                        if (auth_data.user_auth.task_type == firebase_core_auth_task_type_send_verify_email)
                        {
                            json.addObject(sData->request.payload, json.toString("requestType"), json.toString("VERIFY_EMAIL"));
                            json.addObject(sData->request.payload, json.toString("idToken"), json.toString(auth_data.user_auth.user.id_token.length() > 0 ? auth_data.user_auth.user.id_token : auth_data.app_token.token), true);
                        }
                        else if (auth_data.user_auth.task_type == firebase_core_auth_task_type_reset_password)
                        {
                            json.addObject(sData->request.payload, json.toString("requestType"), json.toString("PASSWORD_RESET"));
                            json.addObject(sData->request.payload, json.toString("email"), json.toString(auth_data.user_auth.user.email), true);
                        }

                        extras = FPSTR("/v1/accounts:sendOobCode?key=");
                    }
                    else if (auth_data.user_auth.task_type == firebase_core_auth_task_type_delete_user)
                    {
                        json.addObject(sData->request.payload, json.toString("idToken"), json.toString(auth_data.user_auth.user.id_token.length() ? auth_data.user_auth.user.id_token : auth_data.app_token.token), true);
                        extras = FPSTR("/v1/accounts:delete?key=");
                    }
                    else if (auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token)
                    {
                        json.addObject(sData->request.payload, json.toString("grantType"), json.toString("refresh_token"));
                        json.addObject(sData->request.payload, json.toString("refreshToken"), json.toString(auth_data.app_token.refresh), true);
                        extras = FPSTR("/v1/token?key=");
                    }
                    else
                    {
                        if (auth_data.user_auth.user.email.length() && auth_data.user_auth.user.password.length())
                        {
                            json.addObject(sData->request.payload, json.toString("email"), json.toString(auth_data.user_auth.user.email));
                            json.addObject(sData->request.payload, json.toString("password"), json.toString(auth_data.user_auth.user.password));
                        }
                        json.addObject(sData->request.payload, json.toString("returnSecureToken"), "true", true);
                        extras = auth_data.user_auth.task_type == firebase_core_auth_task_type_signup ? FPSTR("/v1/accounts:signUp?key=") : FPSTR("/v1/accounts:signInWithPassword?key=");
                    }

                    if (auth_data.user_auth.auth_type == auth_sa_custom_token)
                    {
#if defined(ENABLE_CUSTOM_AUTH)
                        extras += auth_data.user_auth.cust.api_key;
#endif
                    }
                    else
                        extras += auth_data.user_auth.user.api_key;

                    newRequest(aClient, sop, subdomain, extras, resultCb);
                    extras.remove(0, extras.length());
                    host.remove(0, host.length());
                    setEvent(auth_event_auth_request_sent);
                    return true;
                }
            }

            if (auth_data.user_auth.status._event == auth_event_auth_request_sent)
            {
                if (sData && (sData->aResult.error().code() != 0 || req_timer.remaining() == 0))
                {
                    setEvent(auth_event_error);
                    return false;
                }

                if (sData && sData->response.auth_data_available)
                {

                    setEvent(auth_event_auth_response_received);

                    if (auth_data.user_auth.task_type == firebase_core_auth_task_type_delete_user || auth_data.user_auth.task_type == firebase_core_auth_task_type_send_verify_email || auth_data.user_auth.task_type == firebase_core_auth_task_type_reset_password)
                    {
                        auth_data.app_token.authenticated = auth_data.user_auth.task_type != firebase_core_auth_task_type_delete_user && auth_data.user_auth.task_type != firebase_core_auth_task_type_reset_password;
                        auth_data.app_token.auth_type = auth_data.user_auth.auth_type;
                        auth_data.app_token.auth_data_type = auth_data.user_auth.auth_data_type;
                        auth_data.app_token.expire = 3600;
                        auth_timer.feed(3600);
                        setEvent(auth_event_ready);
                        return true;
                    }

                    if (auth_data.user_auth.task_type == firebase_core_auth_task_type_signup)
                        auth_data.user_auth.anonymous = auth_data.user_auth.user.email.length() == 0 && auth_data.user_auth.user.password.length() == 0;

                    if (parseToken(sData->response.payload.c_str()))
                    {
                        sData->response.payload.remove(0, sData->response.payload.length());
                        auth_timer.feed(expire && expire < auth_data.app_token.expire ? expire : auth_data.app_token.expire - 2 * 60);
                        auth_data.app_token.authenticated = true;
                        auth_data.app_token.auth_type = auth_data.user_auth.auth_type;
                        auth_data.app_token.auth_data_type = auth_data.user_auth.auth_data_type;
                        setEvent(auth_event_ready);
                    }
                    else
                    {
                        setEvent(auth_event_error);
                    }
                }
            }

            return true;
        }

    public:
        FirebaseApp()
        {
            app_addr = reinterpret_cast<uint32_t>(this);
            vec.addRemoveList(aVec, app_addr, true);
        };
        ~FirebaseApp() { vec.addRemoveList(aVec, app_addr, false); };

        bool isInitialized() const { return auth_data.user_auth.initialized; }

        void loop()
        {
            auth_data.user_auth.jwt_loop = true;
            processAuth();
            auth_data.user_auth.jwt_loop = false;
        }

        bool isJWT() { return auth_data.user_auth.jwt_signing; }

        bool ready() { return processAuth() && auth_data.app_token.authenticated; }

        template <typename T>
        void getApp(T &app) { app.setApp(app_addr, &auth_data.app_token); }

        String getToken() const { return auth_data.app_token.token; }

        String getRefreshToken() const { return auth_data.app_token.refresh; }

        String getUid() const { return auth_data.app_token.uid; }

        bool isAuthenticated() const { return auth_data.app_token.authenticated; }

        bool isExpired() { return auth_timer.remaining() == 0; }

        unsigned long ttl() { return auth_timer.remaining(); }

        void setCallback(AsyncResultCallback cb) { this->resultCb = cb; }

        auth_data_t *getAuth() { return &auth_data; }
    };
};

#endif