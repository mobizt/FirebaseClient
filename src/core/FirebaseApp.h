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
                auth_data.status._err.string = jwt_data->msg;
                auth_data.status._err.code = jwt_data->err_code;
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

        void parseToken()
        {
            StringHelper sh;
            int p1 = 0, p2 = 0;

            if (auth_data.user_auth.auth_type == auth_sa_custom_token ||
                auth_data.user_auth.auth_type == auth_sa_access_token ||
                auth_data.user_auth.auth_type == auth_access_token ||
                auth_data.user_auth.auth_type == auth_user_id_token ||
                auth_data.user_auth.auth_type == auth_custom_token)
            {

                sh.parse(aResult.payload(), "\"error\"", "}", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    p1 = p2;
                    sh.parse(aResult.payload(), "\"code\"", ",", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.user_auth.status._err.code = atoi(aResult.payload().substring(p1, p2).c_str());
                        p1 = p2;
                    }

                    sh.parse(aResult.payload(), "\"message\"", ",", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.user_auth.status._err.string = aResult.payload().substring(p1 + 1, p2 - 1);
                        p1 = p2;
                    }

                    sh.parse(aResult.payload(), "\"error_description\"", ",", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.user_auth.status._err.string = aResult.payload().substring(p1 + 1, p2 - 1);
                        p1 = p2;
                    }
                }
            }

            p1 = 0;
            p2 = 0;

            if (auth_data.user_auth.auth_type == auth_sa_custom_token || auth_data.user_auth.auth_type == auth_sa_access_token || auth_data.user_auth.auth_type == auth_access_token)
            {
                if (auth_data.user_auth.auth_type == auth_sa_custom_token)
                {
                    sh.parse(aResult.payload(), "\"idToken\"", ",", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.app_token.token = aResult.payload().substring(p1 + 1, p2 - 1);
                        p1 = p2;
                        auth_data.user_auth.authenticated = true;
                        setEvent(auth_event_ready);
                        Serial.println(auth_data.app_token.token);
                    }
                    sh.parse(aResult.payload(), "\"refreshToken\"", ",", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.app_token.token_type = aResult.payload().substring(p1 + 1, p2 - 1);
                        p1 = p2;
                    }
                    sh.parse(aResult.payload(), "\"expiresIn\"", "}", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.app_token.expire = atoi(aResult.payload().substring(p1, p2).c_str());
                        p1 = p2;
                    }
                }
                else if (auth_data.user_auth.auth_type == auth_sa_access_token || auth_data.user_auth.auth_type == auth_access_token)
                {
                    sh.parse(aResult.payload(), "\"access_token\"", ",", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.app_token.token = aResult.payload().substring(p1 + 1, p2 - 1);
                        p1 = p2;
                        auth_data.user_auth.authenticated = true;
                        setEvent(auth_event_ready);
                    }
                    sh.parse(aResult.payload(), "\"expires_in\"", ",", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.app_token.expire = atoi(aResult.payload().substring(p1, p2).c_str());
                        p1 = p2;
                    }
                    sh.parse(aResult.payload(), "\"token_type\"", "}", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.app_token.token_type = aResult.payload().substring(p1 + 1, p2 - 1);
                        p1 = p2;
                    }
                }
            }
            else if (auth_data.user_auth.auth_type == auth_user_id_token || auth_data.user_auth.auth_type == auth_custom_token)
            {

                if (auth_data.user_auth.auth_type == firebase_core_auth_task_type_delete_user || auth_data.user_auth.auth_type == firebase_core_auth_task_type_send_verify_email || auth_data.user_auth.auth_type == firebase_core_auth_task_type_reset_password)
                    return;

                if (auth_data.user_auth.task_type == firebase_core_auth_task_type_signup)
                    auth_data.user_auth.anonymous = auth_data.user_auth.user.email.length() == 0 && auth_data.user_auth.user.password.length() == 0;

                if (auth_data.user_auth.auth_type == auth_custom_token)
                {
                    sh.parse(aResult.payload(), "\"access_token\"", ",", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.app_token.token = aResult.payload().substring(p1 + 1, p2 - 1);
                        p1 = p2;
                        auth_data.user_auth.authenticated = true;
                        setEvent(auth_event_ready);
                    }
                    sh.parse(aResult.payload(), "\"token_type\"", "}", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.app_token.token_type = aResult.payload().substring(p1 + 1, p2 - 1);
                        p1 = p2;
                    }
                }
                else
                {
                    sh.parse(aResult.payload(), auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? "\"id_token\"" : "\"idToken\"", ",", p1, p2);
                    if (p1 > -1 && p2 > -1)
                    {
                        auth_data.app_token.token = aResult.payload().substring(p1 + 1, p2 - 1);
                        p1 = p2;
                        auth_data.user_auth.authenticated = true;
                        setEvent(auth_event_ready);
                    }
                }

                sh.parse(aResult.payload(), auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? "\"refresh_token\"" : "\"refreshToken\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.app_token.token = aResult.payload().substring(p1 + 1, p2 - 1);
                    p1 = p2;
                }

                sh.parse(aResult.payload(), auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? "\"expires_in\"" : "\"expiresIn\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.app_token.expire = atoi(aResult.payload().substring(p1, p2).c_str());
                    p1 = p2;
                }

                sh.parse(aResult.payload(), auth_data.user_auth.task_type == firebase_core_auth_task_type_refresh_token ? "\"user_id\"" : "\"localId\"", "}", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.app_token.uid = aResult.payload().substring(p1 + 1, p2 - 1);
                    p1 = p2;
                }
            }
        }

        void setEvent(firebase_auth_event_type event)
        {
            if (auth_data.user_auth.status._event != event && event != auth_event_uninitialized)
            {
                auth_data.user_auth.status._event = event;
            }

            if (event == auth_event_error)
                last_error_ms = millis();

            auth_data.user_auth.status._err.code = 0;
            auth_data.user_auth.status._err.string = "";
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
                        return true;
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

                        return true;
                    }
                }
                else if (auth_data.user_auth.status._event == auth_event_authenticating)
                {
                    if (aClient)
                    {
                        AuthRequest req;
                        req.asyncRequest(aClient, aResult, jwt);
                        setEvent(auth_event_auth_request_sent);
                        return true;
                    }

                    return false;
                }
                else if (auth_data.user_auth.status._event == auth_event_auth_request_sent)
                {
                    if (aResult.available())
                    {
                        setEvent(auth_event_auth_response_received);
                        parseToken();
                    }
                }
            }

            return auth_data.user_auth.authenticated;
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
    };

};

#endif