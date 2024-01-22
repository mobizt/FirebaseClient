#ifndef CORE_FIREBASE_APP_H
#define CORE_FIREBASE_APP_H
#include <Arduino.h>
#include "Config.h"
#include "core/AuthConfig.h"
#include "AsyncClient/AsyncClient.h"
#include "core/List.h"
#if defined(ENABLE_JWT)
#include "core/JWT.h";
#endif
#include "AuthRequest.h"

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
        uint32_t addr = 0;
        uint32_t ts = 0;
        JWT jwt;
        AsyncResult aResult;
        AuthRequest authReq;
        unsigned long last_error_ms = 0;

#if defined(ENABLE_JWT)

        JWT::jwt_token_data *jwt_data = nullptr;
        bool createJwt(user_auth_data &auth_data)
        {
            if (!jwt_data)
                jwt_data = new JWT::jwt_token_data();

            Memory mem;

            if (auth_data.timestatus_cb)
                auth_data.timestatus_cb(ts);

            if (!jwt.create(mem, jwt_data, auth_data, ts))
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

        bool parseItem(StringHelper &sh, const String &src, String &dest, const String &name, const String &delim, int &p1, int &p2)
        {
            sh.parse(aResult.payload(), name, delim, p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                dest = aResult.payload().substring(p1 + 1, p2 - 1);
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

            if (auth_data.user_auth.auth_type == auth_sa_custom_token ||
                auth_data.user_auth.auth_type == auth_sa_access_token ||
                auth_data.user_auth.auth_type == auth_access_token ||
                auth_data.user_auth.auth_type == auth_user_id_token ||
                auth_data.user_auth.auth_type == auth_custom_token)
            {
                String str;
                if (parseItem(sh, aResult.payload(), str, "\"error\"", "}", p1, p2))
                {
                    int code = 0;
                    str = "";
                    parseItem(sh, aResult.payload(), code, "\"code\"", ",", p1, p2);
                    parseItem(sh, aResult.payload(), str, "\"message\"", "}", p1, p2);
                    parseItem(sh, aResult.payload(), str, "\"error_description\"", "}", p1, p2);
                    authReq.setLastError(aResult, code, str);
                    return false;
                }
            }

            if (auth_data.user_auth.auth_type == auth_sa_custom_token || auth_data.user_auth.auth_type == auth_sa_access_token || auth_data.user_auth.auth_type == auth_access_token)
            {
                if (auth_data.user_auth.auth_type == auth_sa_custom_token)
                {
                    if (parseItem(sh, aResult.payload(), auth_data.app_token.token, "\"idToken\"", ",", p1, p2))
                    {
                        parseItem(sh, aResult.payload(), auth_data.app_token.refresh, "\"refreshToken\"", ",", p1, p2);
                        parseItem(sh, aResult.payload(), auth_data.app_token.expire, "\"expiresIn\"", "}", p1, p2);
                    }
                }
                else if (auth_data.user_auth.auth_type == auth_sa_access_token || auth_data.user_auth.auth_type == auth_access_token)
                {
                    if (parseItem(sh, aResult.payload(), auth_data.app_token.token, "\"access_token\"", ",", p1, p2))
                    {
                        parseItem(sh, aResult.payload(), auth_data.app_token.expire, "\"expires_in\"", ",", p1, p2);
                        parseItem(sh, aResult.payload(), auth_data.app_token.token_type, "\"token_type\"", "}", p1, p2);
                    }
                }
            }
            else if (auth_data.user_auth.auth_type == auth_user_id_token || auth_data.user_auth.auth_type == auth_custom_token)
            {

                if (auth_data.user_auth.auth_type == firebase_core_auth_task_type_delete_user || auth_data.user_auth.auth_type == firebase_core_auth_task_type_send_verify_email || auth_data.user_auth.auth_type == firebase_core_auth_task_type_reset_password)
                    return false;

                if (auth_data.user_auth.task_type == firebase_core_auth_task_type_signup)
                    auth_data.user_auth.anonymous = auth_data.user_auth.user.email.length() == 0 && auth_data.user_auth.user.password.length() == 0;

                if (auth_data.user_auth.auth_type == auth_custom_token)
                {
                    if (parseItem(sh, aResult.payload(), auth_data.app_token.token, "\"access_token\"", ",", p1, p2))
                        parseItem(sh, aResult.payload(), auth_data.app_token.token_type, "\"token_type\"", "}", p1, p2);
                }
                else
                {
                    parseItem(sh, aResult.payload(), auth_data.app_token.token, auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? "\"id_token\"" : "\"idToken\"", ",", p1, p2);
                }

                parseItem(sh, aResult.payload(), auth_data.app_token.refresh, auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? "\"refresh_token\"" : "\"refreshToken\"", ",", p1, p2);
                parseItem(sh, aResult.payload(), auth_data.app_token.expire, auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? "\"expires_in\"" : "\"expiresIn\"", ",", p1, p2);
                parseItem(sh, aResult.payload(), auth_data.app_token.uid, auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? "\"user_id\"" : "\"localId\"", "}", p1, p2);
            }

            return auth_data.app_token.token.length() > 0;
        }

        void setEvent(firebase_auth_event_type event)
        {
            if (auth_data.user_auth.status._event != event && event != auth_event_uninitialized)
                auth_data.user_auth.status._event = event;

            if (event == auth_event_error)
                last_error_ms = millis();
        }

        bool processAuth()
        {
            if (auth_data.user_auth.status._event == auth_event_uninitialized)
                auth_data.user_auth.status._event = auth_event_initializing;

            if (auth_data.user_auth.auth_type == auth_sa_access_token || auth_data.user_auth.auth_type == auth_sa_custom_token)
            {

                if (auth_data.user_auth.status._event == auth_event_re_authenticating || auth_data.user_auth.status._event == auth_event_initializing)
                {

                    if (auth_data.user_auth.timestatus_cb)
                        auth_data.user_auth.timestatus_cb(ts);

                    if (auth_data.user_auth.sa.step == jwt_step_begin)
                    {
                        auth_data.user_auth.sa.step = jwt_step_encode_header_payload;
                        if (!createJwt(auth_data.user_auth))
                        {
                            setEvent(auth_event_error);
                            return false;
                        }
                        setEvent(auth_event_token_signing);
                    }
                }
                else if (auth_data.user_auth.status._event == auth_event_token_signing)
                {
                    if (auth_data.user_auth.sa.step == jwt_step_sign)
                    {
                        if (!createJwt(auth_data.user_auth))
                            return false;

                        if (auth_data.user_auth.sa.step == jwt_step_ready)
                            setEvent(auth_event_authenticating);
                    }
                }
                else if (auth_data.user_auth.status._event == auth_event_authenticating)
                {
                    if (aClient)
                    {
                        authReq.asyncRequest(aClient, aResult, jwt);
                        setEvent(auth_event_auth_request_sent);
                    }

                    return false;
                }
                else if (auth_data.user_auth.status._event == auth_event_auth_request_sent)
                {
                    if (aResult.available())
                    {
                        setEvent(auth_event_auth_response_received);
                        if (parseToken())
                        {
                            auth_data.app_token.expire_ts = (ts > 0 ? ts : millis() / 1000) + auth_data.app_token.expire;
                            auth_data.user_auth.authenticated = true;
                            setEvent(auth_event_ready);
                        }
                    }
                }
            }

            return true;
        }
#endif
    public:
        FirebaseApp()
        {
            addr = reinterpret_cast<uint32_t>(this);
            List list;
            list.addRemoveList(firebase_app_list, addr, true);
        };
        ~FirebaseApp()
        {
            List list;
            list.addRemoveList(firebase_app_list, addr, false);
        };

        bool isInitialized()
        {
            return auth_data.user_auth.initialized;
        }

        bool ready()
        {
            return processAuth() && auth_data.user_auth.authenticated;
        }

        template <typename T>
        void getApp(T &app)
        {
            app.setAppToken(&auth_data.app_token);
        }

        String getToken()
        {
            return auth_data.app_token.token;
        }

        bool isAuthenticated()
        {
            return auth_data.user_auth.authenticated;
        }

        int ttl()
        {
            if (!isAuthenticated())
                return 0;

            if (auth_data.user_auth.timestatus_cb)
                auth_data.user_auth.timestatus_cb(ts);

            return ts < auth_data.app_token.expire_ts ? auth_data.app_token.expire_ts - ts : 0;
        }
    };

};

#endif