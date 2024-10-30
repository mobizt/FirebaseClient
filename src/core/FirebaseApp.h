/**
 * Created October 30, 2024
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
#include "./core/AsyncResult/ResultBase.h"
#include "./core/List.h"
#if defined(ENABLE_JWT)
#include "./core/JWT.h"
#endif
#include "./core/JSON.h"
#include "./core/Timer.h"
#include "./core/AppBase.h"

namespace firebase
{

#if defined(ENABLE_JWT)
    static JWTClass JWT;
#endif
    class FirebaseApp : public AppBase, public ResultBase
    {
        friend class RealtimeDatabase;
        friend class FirebaseClient;
        friend class Databases;
        friend class Documents;
        friend class CollectionGroups;
        friend class CloudFunctions;
        friend class Messaging;
        friend class Storage;
        friend class CloudStorage;

    private:
        uint16_t slot = 0;
        async_data_item_t *sData = nullptr;
        auth_data_t auth_data;
        AsyncClientClass *aClient = nullptr;
        uint32_t aclient_addr = 0, app_addr = 0, ref_ts = 0;
        std::vector<uint32_t> aVec; // FirebaseApp vector
        std::vector<uint32_t> cVec; // AsyncClient vector
        AsyncResultCallback resultCb = NULL;
        AsyncResult *refResult = nullptr;
        uint32_t ref_result_addr = 0;
        Timer req_timer, auth_timer, err_timer, app_ready_timer;
        bool deinit = false;
        bool processing = false;
        bool ul_dl_task_running = false;
        uint32_t expire = FIREBASE_DEFAULT_TOKEN_TTL;
        JSONUtil json;
        String extras, subdomain, host;
        slot_options_t sop;
        String uid;
#if defined(ENABLE_JWT)
        JWTClass *jwtClass = nullptr;
#endif

        void setLastError(AsyncResult *aResult, int code, const String &message)
        {
            setLastErrorBase(aResult, code, message);
        }

        bool parseItem(StringUtil &sut, const String &src, String &dest, const String &name, const String &delim, int &p1, int &p2)
        {
            sut.parse(src, name, delim, p1, p2);
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
        bool parseItem(StringUtil &sut, const String &src, T &dest, const String &name, const String &delim, int &p1, int &p2)
        {
            sut.parse(src, name, delim, p1, p2);
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
            StringUtil sut;
            int p1 = 0, p2 = 0;
            auth_data.app_token.clear();
            String token, refresh;

            if (payload.indexOf("\"error\"") > -1)
            {
                String str;
                if (parseItem(sut, payload, str, "\"error\"", auth_data.user_auth.auth_type == auth_user_id_token || auth_data.user_auth.auth_type == auth_sa_custom_token ? "}" : ",", p1, p2))
                {
                    int code = 0;
                    str = "";
                    p1 = 0;
                    p2 = 0;
                    parseItem(sut, payload, code, "\"code\"", ",", p1, p2);
                    parseItem(sut, payload, str, "\"message\"", ",", p1, p2);
                    parseItem(sut, payload, str, "\"error_description\"", "}", p1, p2);
                    if (str[str.length() - 1] == '"')
                        str[str.length() - 1] = '\0';
                    setLastError(sData ? &sData->aResult : nullptr, code, str);
                }
            }
            else if (payload.indexOf("\"idToken\"") > -1)
            {
                parseItem(sut, payload, auth_data.app_token.val[app_tk_ns::uid], "\"localId\"", ",", p1, p2);
                p1 = 0;
                p2 = 0;
                sut.trim(auth_data.app_token.val[app_tk_ns::uid]);
                if (parseItem(sut, payload, token, "\"idToken\"", ",", p1, p2))
                {
                    sut.trim(token);
                    parseItem(sut, payload, refresh, "\"refreshToken\"", ",", p1, p2);
                    sut.trim(refresh);
                    parseItem(sut, payload, auth_data.app_token.expire, "\"expiresIn\"", "}", p1, p2);
                }
            }
            else if (payload.indexOf("\"id_token\"") > -1)
            {
                parseItem(sut, payload, auth_data.app_token.expire, "\"expires_in\"", ",", p1, p2);
                parseItem(sut, payload, refresh, "\"refresh_token\"", ",", p1, p2);
                parseItem(sut, payload, token, "\"id_token\"", ",", p1, p2);
                parseItem(sut, payload, auth_data.app_token.val[app_tk_ns::uid], "\"user_id\"", ",", p1, p2);
                sut.trim(refresh);
                sut.trim(token);
                sut.trim(auth_data.app_token.val[app_tk_ns::uid]);
            }
            else if (payload.indexOf("\"access_token\"") > -1)
            {
                if (parseItem(sut, payload, token, "\"access_token\"", ",", p1, p2))
                {
                    parseItem(sut, payload, auth_data.app_token.expire, "\"expires_in\"", ",", p1, p2);
                    parseItem(sut, payload, auth_data.app_token.val[app_tk_ns::type], "\"token_type\"", "}", p1, p2);
                }
            }

            if (token.length() > 0 && token[token.length() - 1] == '"')
                token.remove(token.length() - 1, 1);

            if (refresh.length() > 0 && refresh[refresh.length() - 1] == '"')
                refresh.remove(refresh.length() - 1, 1);

            auth_data.app_token.val[app_tk_ns::token] = token;
            auth_data.app_token.val[app_tk_ns::refresh] = refresh;
#if defined(ENABLE_SERVICE_AUTH)
            auth_data.app_token.val[app_tk_ns::pid] = auth_data.user_auth.sa.val[sa_ns::pid];
#endif
            return token.length() > 0;
        }

        AsyncClientClass *getClient()
        {
            List v;
            return v.existed(cVec, aclient_addr) ? aClient : nullptr;
        }

        void setEvent(firebase_auth_event_type event)
        {
            if (auth_data.user_auth.status._event == event)
                return;

            auth_data.user_auth.status._event = event;

            if (event == auth_event_initializing || event == auth_event_authenticating)
                processing = true;

            if (event == auth_event_error)
            {
                err_timer.feed(5);
                auth_timer.stop();
            }

            setEventResult(sData ? &sData->aResult : getRefResult(), auth_data.user_auth.status.authEventString(auth_data.user_auth.status._event), auth_data.user_auth.status._event);

            if (event == auth_event_error || event == auth_event_ready)
            {
                processing = false;
                clearLastError(sData ? &sData->aResult : nullptr);
                if (getClient())
                    stop(aClient);
            }
        }

        void clearLastError(AsyncResult *aResult)
        {
            clearLastErrorBase(aResult);
        }

        void setEventResult(AsyncResult *aResult, const String &msg, int code)
        {
            // If aResult was not initiated, create and send temporary result to callback
            bool isRes = aResult != nullptr;
            AsyncResult *ares = aResult;

            // Set uid from user defined async result.
            if (getRefResult())
                uid = refResult->uid();

            if (!isRes)
            {
                ares = new AsyncResult();
                resultSetEvent(ares, getAppEvent(aClient));
                resultSetDebug(ares, getAppDebug(aClient));

                // Store the uid;
                if (uid.length() == 0)
                    uid = ares->uid();
                else
                    setResultUID(ares, uid);
            }

            if (!getRefResult())
                resetEvent(*getAppEvent(aClient));

            setEventBase(*getAppEvent(aClient), code, msg);

            if (resultCb)
                resultCb(*ares);

            if (!isRes)
            {
                delete ares;
                ares = nullptr;
            }
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
            if (aClient)
                sData = createSlotBase(aClient, soption);
        }

        AsyncResult *getRefResult()
        {
            List v;
            return aClient && v.existed(getRVec(aClient), ref_result_addr) ? refResult : nullptr;
        }

        void setRefResult(AsyncResult *refResult, uint32_t rvec_addr)
        {
            this->refResult = refResult;
            ref_result_addr = reinterpret_cast<uint32_t>(refResult);
            setRVec(this->refResult, rvec_addr);
            if (rvec_addr > 0)
            {
                std::vector<uint32_t> *rVec = reinterpret_cast<std::vector<uint32_t> *>(rvec_addr);
                List v;
                v.addRemoveList(*rVec, ref_result_addr, true);
            }
        }

        void newRequest(AsyncClientClass *aClient, slot_options_t &soption, const String &subdomain, const String &extras, AsyncResultCallback resultCb, const String &uid = "")
        {
            if (!aClient)
                return;

            sys_idle();

            if (sData)
            {
                addGAPIsHost(host, subdomain.c_str());
                newRequestBase(aClient, sData, host, extras, "", async_request_handler_t::http_post, soption, uid);

                addContentTypeHeader(sData->request.val[req_hndlr_ns::header], "application/json");
                setContentLengthBase(aClient, sData, sData->request.val[req_hndlr_ns::payload].length());
                req_timer.feed(FIREBASE_TCP_READ_TIMEOUT_SEC);
                slot = slotCountBase(aClient) - 1;

                setDebugBase(*getAppDebug(aClient), FPSTR("Connecting to server..."));

                if (resultCb)
                    resultCb(sData->aResult);
            }
        }

        void process(AsyncClientClass *aClient, AsyncResult *aResult, AsyncResultCallback resultCb)
        {
            if (!aClient)
                return;

            processBase(aClient, true);
            handleRemoveBase(aClient);
        }

        // Stop client and remove slot
        void stop(AsyncClientClass *aClient)
        {
            if (!aClient)
                return;

            stopAsync(aClient, sData);

            if (sData)
            {
                removeSlotBase(aClient, slot, false);
                if (sData)
                    delete sData;
                sData = nullptr;
            }

            sData = nullptr;
        }

        bool processAuth()
        {
            sys_idle();

            if (!getClient())
                return false;

            // Prevent authentication task from running when upload/download task is running.
            if (ul_dl_task_running)
                return true;

            // Deinitialize
            if (deinit && auth_data.user_auth.status._event == auth_event_uninitialized)
            {
                if (auth_data.user_auth.initialized)
                {
                    stop(aClient);
                    deinitializeApp();
                    auth_timer.stop();
                }
                return false;
            }

            updateDebug(*getAppDebug(aClient));
            updateEvent(*getAppEvent(aClient));

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

            if (auth_data.user_auth.jwt_signing && auth_data.user_auth.jwt_ts == 0)
            {
#if defined(ENABLE_JWT)
                if (err_timer.remaining() == 0)
                {
                    err_timer.feed(3);
                    jwtProcessor()->jwt_data.err_code = FIREBASE_ERROR_JWT_CREATION_REQUIRED;
                    jwtProcessor()->jwt_data.msg = "JWT process has not begun";
                    if (getRefResult())
                        jwtProcessor()->sendErrResult(auth_data.refResult);
                    else
                        jwtProcessor()->sendErrCB(auth_data.cb, nullptr);
                }
#endif
                return false;
            }

            if (auth_data.user_auth.status._event == auth_event_uninitialized && err_timer.remaining() > 0)
                return false;

            if (auth_data.user_auth.auth_type == auth_access_token ||
                auth_data.user_auth.auth_type == auth_sa_access_token ||
                ((auth_data.user_auth.auth_type == auth_custom_token || auth_data.user_auth.auth_type == auth_sa_custom_token) &&
                 auth_data.app_token.val[app_tk_ns::refresh].length() == 0))
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
                    if (auth_data.user_auth.sa.step == jwt_step_begin)
                    {
                        if (getClient())
                            stop(aClient);

                        if (auth_data.user_auth.status._event != auth_event_token_signing)
                            setEvent(auth_event_token_signing);

                        auth_data.user_auth.jwt_signing = true;

                        jwtProcessor()->begin(&auth_data);
                    }
                    else if (auth_data.user_auth.sa.step == jwt_step_sign || auth_data.user_auth.sa.step == jwt_step_ready)
                    {
                        if (jwtProcessor()->ready())
                        {
                            auth_data.user_auth.jwt_signing = false;
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

                    // Remove all slots except sse in case ServiceAuth and CustomAuth to free up memory.
                    if (getClient())
                    {
                        for (size_t i = slotCountBase(aClient) - 1; i == 0; i--)
                            removeSlotBase(aClient, i, false);

                        createSlot(aClient, sop);
                    }

                    if (auth_data.user_auth.auth_type == auth_sa_access_token)
                    {
#if defined(ENABLE_SERVICE_AUTH)
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "grant_type", "urn:ietf:params:oauth:grant-type:jwt-bearer", true);
#if defined(ENABLE_JWT)
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "assertion", jwtProcessor()->token(), true, true);
                        jwtProcessor()->clear();
#endif
#endif
                    }
                    else if (auth_data.user_auth.auth_type == auth_sa_custom_token || auth_data.user_auth.auth_type == auth_custom_token)
                    {
#if defined(ENABLE_CUSTOM_AUTH) && defined(ENABLE_JWT)
                        if (auth_data.user_auth.auth_type == auth_sa_custom_token)
                        {
                            json.addObject(sData->request.val[req_hndlr_ns::payload], "token", jwtProcessor()->token(), true);
                            jwtProcessor()->clear();
                        }
#endif
#if defined(ENABLE_CUSTOM_TOKEN)
                        if (auth_data.user_auth.auth_type == auth_custom_token)
                            json.addObject(sData->request.val[req_hndlr_ns::payload], "token", auth_data.user_auth.custom_token.val[cust_tk_ns::token], true);
#endif

                        json.addObject(sData->request.val[req_hndlr_ns::payload], "returnSecureToken", "true", false, true);
                    }
                    else if (auth_data.user_auth.auth_type == auth_access_token && auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token)
                    {
#if defined(ENABLE_ACCESS_TOKEN)
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "client_id", auth_data.user_auth.access_token.val[access_tk_ns::cid], true);
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "client_secret", auth_data.user_auth.access_token.val[access_tk_ns::csec], true);
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "grant_type", "refresh_token", true);
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "refresh_token", auth_data.user_auth.access_token.val[access_tk_ns::refresh], true, true);
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
                            extras += auth_data.user_auth.cust.val[cust_ns::api_key];
#endif
                        }
                        else
                            extras += auth_data.user_auth.user.val[user_ns::api_key];
                    }

                    if (getClient())
                        newRequest(aClient, sop, subdomain, extras, resultCb, uid);

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

                if ((auth_data.user_auth.task_type == firebase_core_auth_task_type_undefined || auth_data.user_auth.auth_type == auth_sa_custom_token) && auth_data.app_token.val[app_tk_ns::refresh].length())
                    auth_data.user_auth.task_type = firebase_core_auth_task_type_refresh_token;

                subdomain = auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? FPSTR("securetoken") : FPSTR("identitytoolkit");

                if (auth_data.user_auth.status._event == auth_event_authenticating)
                {
                    sop.async = true;
                    sop.auth_used = true;

                    if (getClient())
                        createSlot(aClient, sop);

                    if (auth_data.user_auth.task_type == firebase_core_auth_task_type_reset_password || auth_data.user_auth.task_type == firebase_core_auth_task_type_send_verify_email)
                    {
                        if (auth_data.user_auth.task_type == firebase_core_auth_task_type_send_verify_email)
                        {
                            json.addObject(sData->request.val[req_hndlr_ns::payload], "requestType", "VERIFY_EMAIL", true);
                            json.addObject(sData->request.val[req_hndlr_ns::payload], "idToken", auth_data.user_auth.user.val[user_ns::id_token].length() > 0 ? auth_data.user_auth.user.val[user_ns::id_token] : auth_data.app_token.val[app_tk_ns::token], true, true);
                        }
                        else if (auth_data.user_auth.task_type == firebase_core_auth_task_type_reset_password)
                        {
                            json.addObject(sData->request.val[req_hndlr_ns::payload], "requestType", "PASSWORD_RESET", true);
                            json.addObject(sData->request.val[req_hndlr_ns::payload], "email", auth_data.user_auth.user.val[user_ns::em], true, true);
                        }

                        extras = FPSTR("/v1/accounts:sendOobCode?key=");
                    }
                    else if (auth_data.user_auth.task_type == firebase_core_auth_task_type_delete_user)
                    {
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "idToken", auth_data.user_auth.user.val[user_ns::id_token].length() ? auth_data.user_auth.user.val[user_ns::id_token] : auth_data.app_token.val[app_tk_ns::token], true, true);
                        extras = FPSTR("/v1/accounts:delete?key=");
                    }
                    else if (auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token)
                    {
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "grantType", "refresh_token", true);
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "refreshToken", auth_data.app_token.val[app_tk_ns::refresh], true, true);
                        extras = FPSTR("/v1/token?key=");
                    }
                    else
                    {
                        if (auth_data.user_auth.user.val[user_ns::em].length() && auth_data.user_auth.user.val[user_ns::psw].length())
                        {
                            json.addObject(sData->request.val[req_hndlr_ns::payload], "email", auth_data.user_auth.user.val[user_ns::em], true);
                            json.addObject(sData->request.val[req_hndlr_ns::payload], "password", auth_data.user_auth.user.val[user_ns::psw], true);
                        }
                        json.addObject(sData->request.val[req_hndlr_ns::payload], "returnSecureToken", "true", false, true);
                        extras = auth_data.user_auth.task_type == firebase_core_auth_task_type_signup ? FPSTR("/v1/accounts:signUp?key=") : FPSTR("/v1/accounts:signInWithPassword?key=");
                    }

                    if (auth_data.user_auth.auth_type == auth_sa_custom_token)
                    {
#if defined(ENABLE_CUSTOM_AUTH)
                        extras += auth_data.user_auth.cust.val[cust_ns::api_key];
#endif
                    }
                    else
                        extras += auth_data.user_auth.user.val[user_ns::api_key];

                    if (getClient())
                        newRequest(aClient, sop, subdomain, extras, resultCb, uid);

                    extras.remove(0, extras.length());
                    host.remove(0, host.length());
                    setEvent(auth_event_auth_request_sent);
                    return true;
                }
            }

            if (auth_data.user_auth.status._event == auth_event_auth_request_sent || auth_data.user_auth.status._event == auth_event_auth_response_received)
            {
                // token_ready = false;
                sys_idle();

                if (sData && ((sData->response.payloadLen > 0 && sData->aResult.error().code() != 0) || req_timer.remaining() == 0))
                {
                    // In case of googleapis returns http status code >= 400 or request is timed out.
                    // Note that, only status line was read in case http status code >= 400
                    setEvent(auth_event_error);
                    return false;
                }

                if (sData && sData->response.auth_data_available)
                {
                    if (auth_data.user_auth.status._event != auth_event_auth_response_received)
                    {
                        setEvent(auth_event_auth_response_received);
                        if (getRefResult())
                            return false;
                    }

                    if (auth_data.user_auth.task_type == firebase_core_auth_task_type_delete_user || auth_data.user_auth.task_type == firebase_core_auth_task_type_send_verify_email || auth_data.user_auth.task_type == firebase_core_auth_task_type_reset_password)
                    {
                        auth_data.app_token.authenticated = auth_data.user_auth.task_type != firebase_core_auth_task_type_delete_user && auth_data.user_auth.task_type != firebase_core_auth_task_type_reset_password;
                        auth_data.app_token.auth_type = auth_data.user_auth.auth_type;
                        auth_data.app_token.auth_data_type = auth_data.user_auth.auth_data_type;
                        auth_data.app_token.expire = FIREBASE_DEFAULT_TOKEN_TTL;
                        auth_timer.feed(FIREBASE_DEFAULT_TOKEN_TTL);
                        setEvent(auth_event_ready);
                        return true;
                    }

                    if (auth_data.user_auth.task_type == firebase_core_auth_task_type_signup)
                        auth_data.user_auth.anonymous = auth_data.user_auth.user.val[user_ns::em].length() == 0 && auth_data.user_auth.user.val[user_ns::psw].length() == 0;

                    if (parseToken(sData->response.val[res_hndlr_ns::payload].c_str()))
                    {
                        sData->response.val[res_hndlr_ns::payload].remove(0, sData->response.val[res_hndlr_ns::payload].length());
                        auth_timer.feed(expire && expire < auth_data.app_token.expire ? expire : auth_data.app_token.expire - 2 * 60);
                        auth_data.app_token.authenticated = true;
                        auth_data.app_token.auth_ts = millis();
                        if (getClient())
                            setAuthTsBase(aClient, auth_data.app_token.auth_ts);
                        auth_data.app_token.auth_type = auth_data.user_auth.auth_type;
                        auth_data.app_token.auth_data_type = auth_data.user_auth.auth_data_type;
                        setEvent(auth_event_ready);
                        app_ready_timer.feed(1);
                    }
                    else
                    {
                        setEvent(auth_event_error);
                    }
                }
            }

            // Defer the ready status to allow the remaining information to be read or printed inside the waiting loop.
            if (auth_data.user_auth.status._event == auth_event_ready && auth_data.app_token.authenticated && app_ready_timer.remaining() > 0)
                return false;

            return true;
        }

        void deinitializeApp()
        {
            auth_data.app_token.clear();
            auth_data.user_auth.clear();
        }

#if defined(ENABLE_JWT)
        JWTClass *jwtProcessor() { return jwtClass ? jwtClass : &JWT; }
#endif

    public:
        FirebaseApp()
        {
            app_addr = reinterpret_cast<uint32_t>(this);
            List v;
            v.addRemoveList(aVec, app_addr, true);
        };

        ~FirebaseApp()
        {
            if (sData)
                delete sData;
            sData = nullptr;
            List v;
            v.addRemoveList(aVec, app_addr, false);
        };

        /**
         * Get the app initialization status.
         *
         * @return bool Return true if initialized.
         */
        bool isInitialized() const { return auth_data.user_auth.initialized; }

        /**
         * The authentication/authorization handler.
         */
        void loop()
        {
            auth_data.user_auth.jwt_loop = true;
            processAuth();
            auth_data.user_auth.jwt_loop = false;
        }

        /**
         * Get the authentication/autorization process status.
         *
         * @return bool Return true if the auth process was finished. Returns false if isExpired() returns true.
         */
        bool ready() { return processAuth() && auth_data.app_token.authenticated; }

        /**
         * Appy the authentication/authorization credentials to the Firebase services app.
         *
         * @param app The Firebase services calss object e.g. RealtimeDatabase, Storage, Messaging, CloudStorage and CloudFunctions.
         */
        template <typename T>
        void getApp(T &app) { setAppBase(app, app_addr, &auth_data.app_token, reinterpret_cast<uint32_t>(&aVec), reinterpret_cast<uint32_t>(&ul_dl_task_running)); }

        /**
         * Get the auth token.
         *
         * @return String of auth tokens based on the authentication/authoeization e.g. ID token and access token.
         */
        String getToken() const { return auth_data.app_token.val[app_tk_ns::token]; }

        /**
         * Get the refresh token.
         *
         * @return String of refresh token after user sign in or authorization using ID token.
         */
        String getRefreshToken() const { return auth_data.app_token.val[app_tk_ns::refresh]; }

        /**
         * Get unique identifier.
         *
         * @return String of unique identifier after user sign in or authorization using ID token.
         */
        String getUid() const { return auth_data.app_token.val[app_tk_ns::uid]; }

        /**
         * Get the authentication status since app initialized.
         *
         * @return bool Return true once authenticated since initialized. It will reset when app re-initialized and user management task was executed.
         */
        bool isAuthenticated() const { return auth_data.app_token.authenticated; }

        /**
         * Get the auth token expiration status.
         *
         * @return bool Return true if auth token was expired upon the expire period setup.
         */
        bool isExpired() { return auth_timer.remaining() == 0; }

        /**
         * Get the remaining time to live of token until expired.
         *
         * @return integer value of ttl.
         */
        unsigned long ttl() { return auth_timer.remaining(); }

        /**
         * Set the async result callback function.
         *
         * @param cb The async result callback function (AsyncResultCallback).
         */
        void setCallback(AsyncResultCallback cb)
        {
            this->resultCb = cb;
            auth_data.cb = cb;
        }

        /**
         * Set the async result class object.
         *
         * @param aResult The async result class object (AsyncResult).
         */
        void setAsyncResult(AsyncResult &aResult)
        {
            refResult = &aResult;
            auth_data.refResult = &aResult;
        }

        /**
         * Set the UID for authentication task.
         *
         * @param uid The unique identifier for the authentication task.
         *
         * The UID will be applied when used with the async result callback only.
         * The async result object set via initializeApp and FirebaseApp::setAsyncResult will not take effect.
         */
        void setUID(const String &uid) { this->uid = uid; }

#if defined(ENABLE_JWT)
        /**
         * Set the JWT token processor object.
         *
         * This function should be executed before calling initializeApp.
         *
         * @param jwtClass The JWT token processor object.
         *
         */
        void setJWTProcessor(JWTClass &jwtClass) { this->jwtClass = &jwtClass; }
#endif

        /**
         * Get the pointer to the internal auth data.
         *
         * @return auth_data_t* The pointer to internal auth data.
         */
        auth_data_t *getAuth() { return &auth_data; }
    };
};

#endif