/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H

#include <Arduino.h>
#include "./core/Options.h"
#include "./core/File/FileConfig.h"

#if defined(ENABLE_JWT) || defined(ENABLE_ESP_SSLCLIENT)
#include <ESP_SSLClient.h>
#endif

#if defined(ENABLE_JWT)
#include "./core/JWT/JWT.h"
#include "./core/JWT/JWT.cpp"
#endif
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
#include "messaging/MessagingInstance.h"
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

#if defined(ENABLE_RULESETS)
#if __has_include("rules/RuleSets.h")
#include "rules/RuleSets.h"
#endif
#if __has_include("rules/Releases.h")
#include "rules/Releases.h"
#endif
#if __has_include("rules/DataOptions.h")
#include "rules/DataOptions.h"
#endif
#endif

using namespace firebase_ns;

namespace firebase_ns
{
    class FirebaseClient : public AppBase
    {
    private:
        void configApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, firebase_core_auth_task_type task_type = firebase_core_auth_task_type_undefined)
        {
            app.deinit = false;
            app.aClient = &aClient;
            app.aclient_addr = reinterpret_cast<uint32_t>(&aClient);
#if defined(ENABLE_JWT)
            app.jwtProcessor()->setAppDebug(getAppDebug(app.aClient));
#endif
            if (app.refResult)
            {
                resultSetDebug(app.refResult, getAppDebug(app.aClient));
                resultSetEvent(app.refResult, getAppEvent(app.aClient));
                app.setRefResult(app.refResult, reinterpret_cast<uint32_t>(&(app.getRVec(app.aClient))));
            }

            app.addRemoveClientVecBase(app.aClient, reinterpret_cast<uint32_t>(&(app.cVec)), true);
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
        FirebaseClient() {}
        ~FirebaseClient() {}

        void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, uint32_t awaitMs = 0)
        {
            configApp(aClient, app, auth);

            if (app.auth_data.user_auth.auth_data_type == user_auth_data_legacy_token || app.auth_data.user_auth.auth_data_type == user_auth_data_no_token)
            {
#if defined(ENABLE_LEGACY_TOKEN)
                if (app.auth_data.user_auth.auth_data_type == user_auth_data_legacy_token)
                    app.auth_data.app_token.val[app_tk_ns::token] = app.auth_data.user_auth.legacy_token.val[legacy_tk_ns::token];
#endif
                app.auth_data.app_token.authenticated = true;

                resetTimer(app, false, FIREBASE_DEFAULT_TOKEN_TTL);
            }
            else if (app.auth_data.user_auth.auth_data_type == user_auth_data_id_token)
            {
#if defined(ENABLE_ID_TOKEN)
                app.auth_data.app_token.expire = app.expire;
                app.auth_data.app_token.val[app_tk_ns::token] = app.auth_data.user_auth.id_token.val[id_tk_ns::token];
                app.auth_data.app_token.val[app_tk_ns::refresh] = app.auth_data.user_auth.id_token.val[id_tk_ns::refresh];
                app.auth_data.app_token.authenticated = app.auth_data.user_auth.id_token.val[id_tk_ns::token].length() ? app.auth_data.user_auth.initialized : false;
                bool forceRefresh = app.auth_data.app_token.val[app_tk_ns::token].length() == 0 && app.auth_data.app_token.val[app_tk_ns::refresh].length();
                resetTimer(app, true, forceRefresh ? 0 : app.auth_data.user_auth.id_token.expire);
                if (forceRefresh)
                    app.expire = app.auth_data.user_auth.id_token.expire;
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
                uint32_t exp = 0;

#if defined(ENABLE_SERVICE_AUTH)
                if (app.auth_data.user_auth.auth_type == auth_sa_access_token || app.auth_data.user_auth.auth_type == auth_sa_custom_token)
                    exp = app.auth_data.user_auth.sa.expire;
#endif

#if defined(ENABLE_USER_AUTH)
                if (app.auth_data.user_auth.auth_type == auth_user_id_token)
                    exp = app.auth_data.user_auth.user.expire;
#endif
                resetTimer(app, true, 0, exp);
            }
            else
            {
                app.setEventResult(nullptr, "initialization failed", auth_event_error);
            }

            app.await(awaitMs);
        }

        void deinitializeApp(FirebaseApp &app) { app.deinit = true; }

        void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, uint32_t awaitMs = 0)
        {
            configApp(aClient, app, auth, firebase_core_auth_task_type_signup);
            resetTimer(app, true, 0);
            app.await(awaitMs);
        }

        void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, uint32_t awaitMs = 0)
        {
            configApp(aClient, app, auth, firebase_core_auth_task_type_reset_password);
            resetTimer(app, true, 0);
            app.await(awaitMs);
        }

        void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, uint32_t awaitMs = 0)
        {
            configApp(aClient, app, auth, firebase_core_auth_task_type_send_verify_email);
            resetTimer(app, true, 0);
            app.await(awaitMs);
        }

        void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, uint32_t awaitMs = 0)
        {
            configApp(aClient, app, auth, firebase_core_auth_task_type_delete_user);
            resetTimer(app, true, 0);
            app.await(awaitMs);
        }

        void printf(const char *format, ...)
        {
#if defined(FIREBASE_PRINTF_BUFFER)
            int size = FIREBASE_PRINTF_BUFFER;
#else
// Default buffer size for large JSON response.
#if defined(ARDUINO_ARCH_SAMD) || defined(ESP8266)
            int size = 1024;
#else
            int size = 4096;
#endif
#endif
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

/**
 * Get the user authentication/autorization credentials data.
 *
 * @param auth The user auth data (user_auth_data) which is the struct that holds the user sign-in credentials and tokens that obtained from the authentication/authorization classes via getAuth function.
 * @return user_auth_data.
 *
 */
template <typename T>
inline user_auth_data &getAuth(T &auth) { return auth.get(); }

/**
 * Initialize the FirebaseApp and wait.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which is the struct that holds the user sign-in credentials and tokens that obtained from the authentication/authorization classes via getAuth function.
 * @param timeoutMs Optional. The await timeout in milliseconds.
 * @param cb  Optional. The async result callback (AsyncResultCallback).
 */
inline void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, unsigned long timeoutMs = 0, AsyncResultCallback cb = NULL)
{
    app.setCallback(cb);
    Firebase.initializeApp(aClient, app, auth, timeoutMs);
}

/**
 * Initialize the FirebaseApp with callback.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which is the struct that holds the user sign-in credentials and tokens that obtained from the authentication/authorization classes via getAuth function.
 * @param cb The async result callback (AsyncResultCallback).
 * @param uid The user specified UID of async result (optional).
 */
inline void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
{
    app.setUID(uid);
    initializeApp(aClient, app, auth, 0, cb);
}

/**
 * Initialize the FirebaseApp without callback.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which is the struct that holds the user sign-in credentials and tokens that obtained from the authentication/authorization classes via getAuth function.
 * @param aResult The async result (AsyncResult).
 */
inline void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
{
    app.setAsyncResult(aResult);
    Firebase.initializeApp(aClient, app, auth);
}

/**
 * Deinitialize the FirebaseApp.
 *
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 */
inline void deinitializeApp(FirebaseApp &app) { Firebase.deinitializeApp(app); }

/**
 * Signup a new user.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param timeoutMs Optional. The await timeout in milliseconds.
 * @param cb  Optional. The async result callback (AsyncResultCallback).
 */
inline void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, unsigned long timeoutMs = 0, AsyncResultCallback cb = NULL)
{
    app.setCallback(cb);
    Firebase.signup(aClient, app, auth, timeoutMs);
}

/**
 * Signup a new user with callback.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param cb The async result callback (AsyncResultCallback).
 * @param uid The user specified UID of async result (optional).
 */
inline void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
{
    app.setUID(uid);
    app.setCallback(cb);
    Firebase.signup(aClient, app, auth);
}

/**
 * Signup a new user without callback (obsoleted).
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param aResult The async result (AsyncResult).
 */
inline void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
{
    Serial.println("Warning. The AsyncResult is not needed any more when calling the signup.");
    app.setAsyncResult(aResult);
    Firebase.signup(aClient, app, auth);
}

/**
 * Reset the user password.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param timeoutMs Optional. The await timeout in milliseconds.
 * @param cb  Optional. The async result callback (AsyncResultCallback).
 */
inline void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, unsigned long timeoutMs = 0, AsyncResultCallback cb = NULL)
{
    app.setCallback(cb);
    Firebase.resetPassword(aClient, app, auth, timeoutMs);
}

/**
 * Reset the user password with callback.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param cb The async result callback (AsyncResultCallback).
 * @param uid The user specified UID of async result (optional).
 */
inline void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
{
    app.setUID(uid);
    app.setCallback(cb);
    Firebase.resetPassword(aClient, app, auth);
}

/**
 * Reset the user password without callback (obsoleted).
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param aResult The async result (AsyncResult).
 */
inline void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
{
    app.setAsyncResult(aResult);
    Firebase.resetPassword(aClient, app, auth);
}

/**
 * Send the user verification link to email.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param timeoutMs Optional. The await timeout in milliseconds.
 * @param cb  Optional. The async result callback (AsyncResultCallback).
 */
inline void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, unsigned long timeoutMs = 0, AsyncResultCallback cb = NULL)
{
    app.setCallback(cb);
    Firebase.verify(aClient, app, auth, timeoutMs);
}

/**
 * Send the user verification link to email with callback.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param cb The async result callback (AsyncResultCallback).
 * @param uid The user specified UID of async result (optional).
 */
inline void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
{
    app.setUID(uid);
    app.setCallback(cb);
    Firebase.verify(aClient, app, auth);
}

/**
 * Send the user verification link to email without callback (obsoleted).
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param aResult The async result (AsyncResult).
 */
inline void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
{
    app.setAsyncResult(aResult);
    Firebase.verify(aClient, app, auth);
}

/**
 * Delete a user.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param timeoutMs Optional. The await timeout in milliseconds.
 * @param cb  Optional. The async result callback (AsyncResultCallback).
 */
inline void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, unsigned long timeoutMs = 0, AsyncResultCallback cb = NULL)
{
    app.setCallback(cb);
    Firebase.deleteUser(aClient, app, auth, timeoutMs);
}

/**
 * Delete a user with callback.
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param cb The async result callback (AsyncResultCallback).
 * @param uid The user specified UID of async result (optional).
 */
inline void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
{
    app.setUID(uid);
    app.setCallback(cb);
    Firebase.deleteUser(aClient, app, auth);
}

/**
 * Delete a user without callback (obsoleted).
 *
 * @param aClient  The async client to work for authentication/authorization task.
 * @param app The FirebaseApp class object to handle authentication/authorization task.
 * @param auth The user auth data (user_auth_data) which holds the user credentials from USerAccount class.
 * @param aResult The async result (AsyncResult).
 */
inline void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
{
    app.setAsyncResult(aResult);
    Firebase.deleteUser(aClient, app, auth);
}

#endif