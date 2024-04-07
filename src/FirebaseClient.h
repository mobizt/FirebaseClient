/**
 * Created April 7, 2024
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

#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H

#if defined(FIREBASE_CLIENT_VERSION)
#undef FIREBASE_CLIENT_VERSION
#endif

#define FIREBASE_CLIENT_VERSION "1.0.11"

#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./core/AsyncClient/AsyncClient.h"

#if defined(ENABLE_DATABASE)
#if __has_include("database/RealtimeDatabase.h")
#include "database/RealtimeDatabase.h"
#endif
#endif

#if defined(ENABLE_FIRESTORE)
#if __has_include("firestore/Databases.h")
#include "firestore/Databases.h"
#endif
#if __has_include("firestore/Documents.h")
#include "firestore/Documents.h"
#endif
#if __has_include("firestore/CollectionGroups.h")
#include "firestore/CollectionGroups.h"
#endif
#endif

#if defined(ENABLE_MESSAGING)
#if __has_include("messaging/Messaging.h")
#include "messaging/Messaging.h"
#endif
#endif

#if defined(ENABLE_STORAGE)
#if __has_include("storage/Storage.h")
#include "storage/Storage.h"
#endif
#endif

#if defined(ENABLE_CLOUD_STORAGE)
#if __has_include("cloud_storage/CloudStorage.h")
#include "cloud_storage/CloudStorage.h"
#endif
#endif

#if defined(ENABLE_FUNCTIONS)
#if __has_include("functions/Functions.h")
#include "functions/Functions.h"
#endif
#if __has_include("functions/DataOptions.h")
#include "functions/DataOptions.h"
#endif
#if __has_include("functions/Policy.h")
#include "functions/Policy.h"
#endif
#endif

using namespace firebase;

namespace firebase
{
    class FirebaseClient
    {
    private:
        void configApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, firebase_core_auth_task_type task_type = firebase_core_auth_task_type_undefined)
        {
            app.aClient = &aClient;
            app.aclient_addr = reinterpret_cast<uint32_t>(&aClient);

            app.aClient->addRemoveClientVec(reinterpret_cast<uint32_t>(&(app.cVec)), true);
            app.auth_data.user_auth.copy(auth);

            app.auth_data.app_token.clear();
            app.auth_data.app_token.auth_type = app.auth_data.user_auth.auth_type;
            app.auth_data.app_token.auth_data_type = app.auth_data.user_auth.auth_data_type;

            app.auth_data.app_token.authenticated = false;
            app.auth_data.user_auth.task_type = task_type;
        }

        void resetTimer(FirebaseApp &app, bool start, int interval = 0, int exp = -1)
        {
            app.err_timer.feed(0);
            app.auth_timer.stop();
            app.auth_timer.setInterval(interval);
            app.expire = exp == -1 ? interval : exp;
            if (start)
                app.auth_timer.start();
        }

    public:
        FirebaseClient(){};
        ~FirebaseClient(){};

        void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            configApp(aClient, app, auth);

            if (app.auth_data.user_auth.auth_data_type == user_auth_data_legacy_token || app.auth_data.user_auth.auth_data_type == user_auth_data_no_token)
            {
#if defined(ENABLE_LEGACY_TOKEN)
                if (app.auth_data.user_auth.auth_data_type == user_auth_data_legacy_token)
                    app.auth_data.app_token.val[app_tk_ns::token] = app.auth_data.user_auth.legacy_token.val[legacy_tk_ns::token];
#endif
                app.auth_data.app_token.authenticated = true;

                resetTimer(app, false, 3600);
            }
            else if (app.auth_data.user_auth.auth_data_type == user_auth_data_id_token)
            {
#if defined(ENABLE_ID_TOKEN)

                app.auth_data.app_token.expire = app.expire;
                app.auth_data.user_auth.user.val[user_ns::api_key] = app.auth_data.user_auth.user.val[user_ns::api_key];
                app.auth_data.app_token.val[app_tk_ns::token] = app.auth_data.user_auth.id_token.val[id_tk_ns::token];
                app.auth_data.app_token.val[app_tk_ns::refresh] = app.auth_data.user_auth.id_token.val[id_tk_ns::refresh];
                app.auth_data.app_token.authenticated = app.auth_data.user_auth.id_token.val[id_tk_ns::token].length() ? app.auth_data.user_auth.initialized : false;
                resetTimer(app, true, app.auth_data.user_auth.id_token.expire);
#endif
            }
            else if (app.auth_data.user_auth.auth_type == auth_access_token || app.auth_data.user_auth.auth_type == auth_custom_token)
            {
                if (app.auth_data.user_auth.auth_type == auth_access_token)
                {
#if defined(ENABLE_ACCESS_TOKEN)

                    app.auth_data.app_token.expire = app.auth_data.user_auth.access_token.expire;

                    if (app.auth_data.user_auth.access_token.val[access_tk_ns::token].length())
                    {
                        app.auth_data.app_token.val[app_tk_ns::token] = app.auth_data.user_auth.access_token.val[access_tk_ns::token];
                        app.auth_data.app_token.val[app_tk_ns::refresh] = app.auth_data.user_auth.access_token.val[access_tk_ns::refresh];
                        app.auth_data.app_token.authenticated = app.auth_data.user_auth.initialized;
                    }
                    else
                    {
                        app.auth_data.app_token.val[app_tk_ns::refresh] = app.auth_data.user_auth.access_token.val[access_tk_ns::refresh];
                        app.auth_data.app_token.authenticated = false;
                    }

                    resetTimer(app, true, app.auth_data.user_auth.access_token.expire);

#endif
                }
                else if (app.auth_data.user_auth.auth_type == auth_custom_token)
                {
#if defined(ENABLE_CUSTOM_TOKEN)
                    app.auth_data.app_token.expire = app.auth_data.user_auth.custom_token.expire;

                    int token_part = 0;
                    for (size_t i = 0; i < app.auth_data.user_auth.custom_token.val[cust_tk_ns::token].length(); i++)
                        if (app.auth_data.user_auth.custom_token.val[cust_tk_ns::token][i] == '.')
                            token_part++;

                    if (token_part == 2)
                        app.auth_data.app_token.val[app_tk_ns::token] = app.auth_data.user_auth.custom_token.val[cust_tk_ns::token];
                    else // not a valid custom token, treat as a refresh token
                        app.auth_data.app_token.val[app_tk_ns::refresh] = app.auth_data.user_auth.custom_token.val[cust_tk_ns::token];

                    app.auth_data.app_token.authenticated = false;

                    resetTimer(app, true, 0, app.auth_data.user_auth.custom_token.expire);
#endif
                }
            }
            else if (app.auth_data.user_auth.auth_type == auth_sa_access_token || app.auth_data.user_auth.auth_type == auth_sa_custom_token || app.auth_data.user_auth.auth_type == auth_user_id_token)
            {
                app.auth_data.app_token.authenticated = false;
                uint32_t exp = app.auth_data.user_auth.auth_type == auth_user_id_token ? app.auth_data.user_auth.user.expire : app.auth_data.user_auth.sa.expire;
                resetTimer(app, true, 0, exp);
            }
        }

        void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            configApp(aClient, app, auth, firebase_core_auth_task_type_signup);
            resetTimer(app, true, 0);
        }

        void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            configApp(aClient, app, auth, firebase_core_auth_task_type_reset_password);
            resetTimer(app, true, 0);
        }

        void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            configApp(aClient, app, auth, firebase_core_auth_task_type_send_verify_email);
            resetTimer(app, true, 0);
        }

        void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            configApp(aClient, app, auth, firebase_core_auth_task_type_delete_user);
            resetTimer(app, true, 0);
        }

        void printf(const char *format, ...)
        {
            int size = 2048;
            char s[size];
            va_list va;
            va_start(va, format);
            vsnprintf(s, size, format, va);
            va_end(va);
#if defined(FIREBASE_PRINTF_PORT)
            FIREBASE_PRINTF_PORT.print(s);
#else
            Serial.print(s);
#endif
        }
    };

}

static FirebaseClient Firebase;

template <typename T>
static user_auth_data &getAuth(T &auth) { return auth.get(); }
static void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.initializeApp(aClient, app, auth); }
template <typename T = const char *>
static void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.signup(aClient, app, auth); }
template <typename T = const char *>
static void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.resetPassword(aClient, app, auth); }
template <typename T = const char *>
static void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.verify(aClient, app, auth); }
template <typename T = const char *>
static void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.deleteUser(aClient, app, auth); }

#endif