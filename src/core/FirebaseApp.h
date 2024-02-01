/**
 * Created January 31, 2024
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
#include "./core/AuthRequest.h"
#include "./core/Timer.h"

#define FIREBASE_DEFAULT_TS 1618971013

namespace firebase
{

    class FirebaseApp
    {
        friend class FirebaseClient;
        friend class Database;

    private:
        auth_data_t auth_data;
        AsyncClient *aClient = nullptr;
        uint32_t aclient_addr = 0;
        uint32_t app_addr = 0;
        uint32_t ref_ts = 0;
        AsyncResultCallback resultCb = NULL;
        AsyncResult aResult;
        AuthRequest authReq;
        Timer timer;
        List list;
        unsigned long last_error_ms = 0;
        unsigned long last_auth_ms = 0;
        bool processing = false;
        uint32_t expire = 3600;

#if defined(ENABLE_JWT)
        JWT jwt;
        JWT::jwt_token_data *jwt_data = nullptr;
        bool createJwt(user_auth_data &auth_data)
        {
            if (!jwt_data)
                jwt_data = new JWT::jwt_token_data();

            Memory mem;

            if (auth_data.timestatus_cb)
                auth_data.timestatus_cb(ref_ts);

            if (!jwt.create(mem, jwt_data, auth_data, ref_ts))
            {
                authReq.setLastError(aResult, jwt_data->err_code, jwt_data->msg);
                clearJWT();
                return false;
            }

            return true;
        }

        void clearJWT()
        {
            if (jwt_data)
            {
                jwt_data->token.clear();
                jwt_data->msg.clear();
                jwt_data->pk.clear();
                jwt_data->encHeader.clear();
                jwt_data->encPayload.clear();
                jwt_data->encHeadPayload.clear();
                jwt_data->encSignature.clear();
                delete jwt_data;
            }
            jwt_data = nullptr;
        }

#endif

        bool parseItem(StringHelper &sh, const String &src, String &dest, const String &name, const String &delim, int &p1, int &p2)
        {
            sh.parse(aResult.payload(), name, delim, p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                if (aResult.payload()[p1] == '"')
                    p1++;
                if (aResult.payload()[p2] == '"')
                    p2--;
                dest = aResult.payload().substring(p1, p2);
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
            sh.parse(aResult.payload(), name, delim, p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                if (aResult.payload()[p1] == '"')
                    p1++;
                if (aResult.payload()[p2] == '"')
                    p2--;
                dest = atoi(aResult.payload().substring(p1, p2).c_str());
                p1 = p2;
                return true;
            }
            p1 = 0;
            p2 = 0;
            return false;
        }

        bool parseToken()
        {
            StringHelper sh;
            int p1 = 0, p2 = 0;
            auth_data.app_token.clear();

            if (aResult.payload().indexOf("\"error\"") > -1)
            {
                String str;
                if (parseItem(sh, aResult.payload(), str, "\"error\"", auth_data.user_auth.auth_type == auth_user_id_token || auth_data.user_auth.auth_type == auth_sa_custom_token ? "}" : ",", p1, p2))
                {
                    int code = 0;
                    str = "";
                    p1 = 0;
                    p2 = 0;
                    parseItem(sh, aResult.payload(), code, "\"code\"", ",", p1, p2);
                    parseItem(sh, aResult.payload(), str, "\"message\"", ",", p1, p2);
                    parseItem(sh, aResult.payload(), str, "\"error_description\"", "}", p1, p2);
                    if (str[str.length() - 1] == '"')
                        str[str.length() - 1] = '\0';
                    authReq.setLastError(aResult, code, str);
                }
            }
            else if (aResult.payload().indexOf("\"idToken\"") > -1)
            {
                parseItem(sh, aResult.payload(), auth_data.app_token.uid, "\"localId\"", ",", p1, p2);
                p1 = 0;
                p2 = 0;
                sh.trim(auth_data.app_token.uid);
                if (parseItem(sh, aResult.payload(), auth_data.app_token.token, "\"idToken\"", ",", p1, p2))
                {
                    sh.trim(auth_data.app_token.token);
                    parseItem(sh, aResult.payload(), auth_data.app_token.refresh, "\"refreshToken\"", ",", p1, p2);
                    sh.trim(auth_data.app_token.refresh);
                    parseItem(sh, aResult.payload(), auth_data.app_token.expire, "\"expiresIn\"", "}", p1, p2);
                }
            }
            else if (aResult.payload().indexOf("\"id_token\"") > -1)
            {
                parseItem(sh, aResult.payload(), auth_data.app_token.expire, "\"expires_in\"", ",", p1, p2);
                parseItem(sh, aResult.payload(), auth_data.app_token.refresh, "\"refresh_token\"", ",", p1, p2);
                parseItem(sh, aResult.payload(), auth_data.app_token.token, "\"id_token\"", ",", p1, p2);
                parseItem(sh, aResult.payload(), auth_data.app_token.uid, "\"user_id\"", ",", p1, p2);
                sh.trim(auth_data.app_token.refresh);
                sh.trim(auth_data.app_token.token);
                sh.trim(auth_data.app_token.uid);

            }
            else if (aResult.payload().indexOf("\"access_token\"") > -1)
            {
                if (parseItem(sh, aResult.payload(), auth_data.app_token.token, "\"access_token\"", ",", p1, p2))
                {
                    parseItem(sh, aResult.payload(), auth_data.app_token.expire, "\"expires_in\"", ",", p1, p2);
                    parseItem(sh, aResult.payload(), auth_data.app_token.token_type, "\"token_type\"", "}", p1, p2);
                }
            }

            return auth_data.app_token.token.length() > 0;
        }

        void setEvent(firebase_auth_event_type event)
        {
            auth_data.user_auth.status._event = event;

            if (event == auth_event_initializing || event == auth_event_authenticating)
                processing = true;

            if (event == auth_event_error)
            {
                last_error_ms = millis();
                timer.stop();
            }

            if (event == auth_event_ready)
                last_auth_ms = millis();

            authReq.setEventResult(aResult, auth_data.user_auth.status.authEventString(auth_data.user_auth.status._event), auth_data.user_auth.status._event);

            if (resultCb)
                resultCb(aResult);

            if (event == auth_event_error || event == auth_event_ready)
            {
                processing = false;
                authReq.stop(aClient);
                event = auth_event_uninitialized;
                authReq.clearLastError(aResult);
                authReq.remove(aClient);
            }
        }

        void getTime()
        {
            if (auth_data.user_auth.timestatus_cb)
                auth_data.user_auth.timestatus_cb(ref_ts);
        }

        bool processAuth()
        {
            async_request_handler_t req;
            req.idle();

            authReq.process(aClient, aResult, resultCb);

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

            if (auth_data.user_auth.status._event == auth_event_uninitialized && millis() - last_error_ms < 5000)
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

                if (auth_data.user_auth.status._event == auth_event_initializing)
                {
                    getTime();
#if defined(ENABLE_JWT)
                    if (auth_data.user_auth.sa.step == jwt_step_begin)
                    {
                        clearJWT();
                        auth_data.user_auth.sa.step = jwt_step_encode_header_payload;
                        if (!createJwt(auth_data.user_auth))
                        {
                            setEvent(auth_event_error);
                            return false;
                        }
                        setEvent(auth_event_token_signing);
                    }
#endif
                }
                else if (auth_data.user_auth.status._event == auth_event_token_signing)
                {
#if defined(ENABLE_JWT)
                    if (auth_data.user_auth.sa.step == jwt_step_sign)
                    {
                        if (createJwt(auth_data.user_auth) && auth_data.user_auth.sa.step == jwt_step_ready)
                            setEvent(auth_event_authenticating);

                        auth_data.user_auth.sa.step = jwt_step_begin;
                    }
#endif
                }
                else if (auth_data.user_auth.status._event == auth_event_authenticating)
                {
                    String payload;
                    String extras;
                    JSON json;
                    String subdomain = auth_data.user_auth.auth_type == auth_sa_access_token || auth_data.user_auth.auth_type == auth_access_token ? FPSTR("oauth2") : FPSTR("identitytoolkit");

                    if (auth_data.user_auth.auth_type == auth_sa_access_token)
                    {
#if defined(ENABLE_SERVICE_AUTH)
                        json.addObject(payload, json.toString("grant_type"), json.toString("urn:ietf:params:oauth:grant-type:jwt-bearer"));
                        json.addObject(payload, json.toString("assertion"), json.toString(jwt.token()), true);
                        clearJWT();
#endif
                    }
                    else if (auth_data.user_auth.auth_type == auth_sa_custom_token || auth_data.user_auth.auth_type == auth_custom_token)
                    {
#if defined(ENABLE_CUSTOM_AUTH)
                        if (auth_data.user_auth.auth_type == auth_sa_custom_token)
                        {
                            json.addObject(payload, json.toString("token"), json.toString(jwt.token()));
                            clearJWT();
                        }
#endif
#if defined(ENABLE_CUSTOM_TOKEN)
                        if (auth_data.user_auth.auth_type == auth_custom_token)
                            json.addObject(payload, json.toString("token"), json.toString(auth_data.user_auth.custom_token.token));
#endif

                        json.addObject(payload, json.toString("returnSecureToken"), "true", true);
                    }
                    else if (auth_data.user_auth.auth_type == auth_access_token && auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token)
                    {
#if defined(ENABLE_ACCESS_TOKEN)
                        json.addObject(payload, json.toString("client_id"), json.toString(auth_data.user_auth.access_token.client_id));
                        json.addObject(payload, json.toString("client_secret"), json.toString(auth_data.user_auth.access_token.client_secret));
                        json.addObject(payload, json.toString("grant_type"), json.toString("refresh_token"));
                        json.addObject(payload, json.toString("refresh_token"), json.toString(auth_data.user_auth.access_token.refresh), true);
#endif
                    }

                    String api_key;

                    if (auth_data.user_auth.auth_type == auth_sa_custom_token)
                    {
#if defined(ENABLE_CUSTOM_AUTH)
                        api_key = auth_data.user_auth.cust.api_key;
#endif
                    }
                    else
                        api_key = auth_data.user_auth.user.api_key;

                    if (auth_data.user_auth.auth_type == auth_sa_access_token || auth_data.user_auth.auth_type == auth_access_token)
                        extras = FPSTR("/token");
                    else
                    {
                        extras = FPSTR("/v1/accounts:signInWithCustomToken?key=");
                        extras += api_key;
                    }

                    authReq.asyncRequest(aClient, subdomain, extras, payload, aResult, "");
                    payload.remove(0, payload.length());
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

                String payload;
                String extras;
                JSON json;
                String subdomain = auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? FPSTR("securetoken") : FPSTR("identitytoolkit");

                if (auth_data.user_auth.status._event == auth_event_authenticating)
                {
                    if (auth_data.user_auth.task_type == firebase_core_auth_task_type_reset_password || auth_data.user_auth.task_type == firebase_core_auth_task_type_send_verify_email)
                    {
                        if (auth_data.user_auth.task_type == firebase_core_auth_task_type_send_verify_email)
                        {
                            json.addObject(payload, json.toString("requestType"), json.toString("VERIFY_EMAIL"));
                            json.addObject(payload, json.toString("idToken"), json.toString(auth_data.user_auth.user.id_token.length() > 0 ? auth_data.user_auth.user.id_token : auth_data.app_token.token), true);
                        }
                        else if (auth_data.user_auth.task_type == firebase_core_auth_task_type_reset_password)
                        {
                            json.addObject(payload, json.toString("requestType"), json.toString("PASSWORD_RESET"));
                            json.addObject(payload, json.toString("email"), json.toString(auth_data.user_auth.user.email), true);
                        }

                        extras = FPSTR("/v1/accounts:sendOobCode?key=");
                    }
                    else if (auth_data.user_auth.task_type == firebase_core_auth_task_type_delete_user)
                    {
                        json.addObject(payload, json.toString("idToken"), json.toString(auth_data.user_auth.user.id_token.length() ? auth_data.user_auth.user.id_token : auth_data.app_token.token), true);
                        extras = FPSTR("/v1/accounts:delete?key=");
                    }
                    else if (auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token)
                    {
                        json.addObject(payload, json.toString("grantType"), json.toString("refresh_token"));
                        json.addObject(payload, json.toString("refreshToken"), json.toString(auth_data.app_token.refresh), true);
                        extras = FPSTR("/v1/token?key=");
                    }
                    else
                    {
                        if (auth_data.user_auth.user.email.length() && auth_data.user_auth.user.password.length())
                        {
                            json.addObject(payload, json.toString("email"), json.toString(auth_data.user_auth.user.email));
                            json.addObject(payload, json.toString("password"), json.toString(auth_data.user_auth.user.password));
                        }
                        json.addObject(payload, json.toString("returnSecureToken"), "true", true);
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

                    authReq.asyncRequest(aClient, subdomain, extras, payload, aResult, "");
                    payload.remove(0, payload.length());
                    setEvent(auth_event_auth_request_sent);
                    return true;
                }
            }

            if (auth_data.user_auth.status._event == auth_event_auth_request_sent)
            {

                if (aResult.error().code() != 0 || millis() - authReq.request_sent_ms > FIREBASE_TCP_READ_TIMEOUT)
                {
                    setEvent(auth_event_error);
                    return false;
                }

                if (aResult.available())
                {

                    setEvent(auth_event_auth_response_received);

                    if (auth_data.user_auth.task_type == firebase_core_auth_task_type_delete_user || auth_data.user_auth.task_type == firebase_core_auth_task_type_send_verify_email || auth_data.user_auth.task_type == firebase_core_auth_task_type_reset_password)
                    {
                        auth_data.app_token.authenticated = auth_data.user_auth.task_type != firebase_core_auth_task_type_delete_user && auth_data.user_auth.task_type != firebase_core_auth_task_type_reset_password;
                        auth_data.app_token.auth_type = auth_data.user_auth.auth_type;
                        auth_data.app_token.auth_data_type = auth_data.user_auth.auth_data_type;
                        auth_data.app_token.expire = 3600;
                        timer.setInterval(3600);
                        timer.start();
                        setEvent(auth_event_ready);
                        return true;
                    }

                    if (auth_data.user_auth.task_type == firebase_core_auth_task_type_signup)
                        auth_data.user_auth.anonymous = auth_data.user_auth.user.email.length() == 0 && auth_data.user_auth.user.password.length() == 0;

                    if (parseToken())
                    {
                        timer.setInterval(expire && expire < auth_data.app_token.expire ? expire : auth_data.app_token.expire - 2 * 60);
                        timer.start();
                        auth_data.app_token.authenticated = true;
                        auth_data.app_token.auth_type = auth_data.user_auth.auth_type;
                        auth_data.app_token.auth_data_type = auth_data.user_auth.auth_data_type;
                        last_auth_ms = millis();
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
            list.addRemoveList(firebase_app_list, app_addr, true);
        };
        ~FirebaseApp()
        {
            list.addRemoveList(firebase_app_list, app_addr, false);
        };

        bool isInitialized()
        {
            return auth_data.user_auth.initialized;
        }

        void loop()
        {
            processAuth();
        }

        bool ready()
        {
            return processAuth() && auth_data.app_token.authenticated;
        }

        template <typename T>
        void getApp(T &app)
        {
            app.setApp(app_addr, &auth_data.app_token);
        }

        String getToken()
        {
            return auth_data.app_token.token;
        }

        String getRefreshToken()
        {
            return auth_data.app_token.refresh;
        }

        String getUid()
        {
            return auth_data.app_token.uid;
        }

        bool isAuthenticated()
        {
            return auth_data.app_token.authenticated;
        }

        bool isExpired()
        {
            return timer.remaining() == 0;
        }

        unsigned long ttl()
        {
            return timer.remaining();
        }

        void setCallback(AsyncResultCallback cb)
        {
            this->resultCb = cb;
        }

        AsyncResult getResult()
        {
            return aResult;
        }
    };
};

#endif