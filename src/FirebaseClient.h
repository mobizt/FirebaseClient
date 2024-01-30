#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H
#include <Arduino.h>
#include "core/FirebaseApp.h"
#include "database/Database.h"

#if defined(FIREBASE_CLIENT_VERSION)
#undef FIREBASE_CLIENT_VERSION
#endif

#define FIREBASE_CLIENT_VERSION "0.0.1"

using namespace firebase;

namespace firebase
{
    class FirebaseClient
    {
    private:
    public:
        FirebaseClient(){};
        ~FirebaseClient(){};

        void initializeApp(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            app.aClient = &aClient;
            app.aclient_addr = reinterpret_cast<uint32_t>(&aClient);
            app.auth_data.user_auth.copy(auth);

            app.auth_data.app_token.clear();
            app.auth_data.app_token.auth_type = app.auth_data.user_auth.auth_type;
            app.auth_data.app_token.auth_data_type = app.auth_data.user_auth.auth_data_type;

            if (app.auth_data.user_auth.auth_data_type == user_auth_data_legacy_token || app.auth_data.user_auth.auth_data_type == user_auth_data_no_token)
            {
                app.timer.stop();
                app.expire = 3600;
#if defined(ENABLE_LEGACY_TOKEN)
                if (app.auth_data.user_auth.auth_data_type == user_auth_data_legacy_token)
                    app.auth_data.app_token.token = app.auth_data.user_auth.legacy_token.token;
#endif
                app.auth_data.app_token.authenticated = true;

                app.timer.setInterval(3600);
            }
            else if (app.auth_data.user_auth.auth_data_type == user_auth_data_id_token)
            {
#if defined(ENABLE_ID_TOKEN)

                app.timer.stop();
                app.expire = app.auth_data.user_auth.id_token.expire;
                app.auth_data.app_token.expire = app.expire;
                app.auth_data.user_auth.user.api_key = app.auth_data.user_auth.user.api_key;
                app.auth_data.app_token.token = app.auth_data.user_auth.id_token.token;
                app.auth_data.app_token.refresh = app.auth_data.user_auth.id_token.refresh;
                app.auth_data.app_token.authenticated = app.auth_data.user_auth.id_token.token.length() ? app.auth_data.user_auth.initialized : false;
                app.timer.setInterval(app.expire);
                app.timer.start();
#endif
            }
            else if (app.auth_data.user_auth.auth_type == auth_access_token || app.auth_data.user_auth.auth_type == auth_custom_token)
            {
                if (app.auth_data.user_auth.auth_type == auth_access_token)
                {
#if defined(ENABLE_ACCESS_TOKEN)
                    app.timer.stop();
                    app.expire = app.auth_data.user_auth.access_token.expire;
                    app.auth_data.app_token.expire = app.expire;

                    if (app.auth_data.user_auth.access_token.token.length())
                    {
                        app.auth_data.app_token.token = app.auth_data.user_auth.access_token.token;
                        app.auth_data.app_token.refresh = app.auth_data.user_auth.access_token.refresh;
                        app.auth_data.app_token.authenticated = app.auth_data.user_auth.initialized;
                    }
                    else
                    {
                        app.auth_data.app_token.refresh = app.auth_data.user_auth.access_token.refresh;
                        app.auth_data.app_token.authenticated = false;
                    }

                    app.timer.setInterval(app.expire);
                    app.timer.start();
#endif
                }
                else if (app.auth_data.user_auth.auth_type == auth_custom_token)
                {
#if defined(ENABLE_CUSTOM_TOKEN)
                    app.timer.stop();
                    app.expire = app.auth_data.user_auth.custom_token.expire;
                    app.auth_data.app_token.expire = app.expire;

                    int token_part = 0;
                    for (size_t i = 0; i < app.auth_data.user_auth.custom_token.token.length(); i++)
                        if (app.auth_data.user_auth.custom_token.token[i] == '.')
                            token_part++;

                    if (token_part == 3)
                        app.auth_data.app_token.token = app.auth_data.user_auth.custom_token.token;
                    else // not a valid custom token, treat as a refresh token
                        app.auth_data.app_token.refresh = app.auth_data.user_auth.custom_token.token;

                    app.auth_data.app_token.authenticated = false;
                    app.timer.setInterval(0);
#endif
                }
            }
            else if (app.auth_data.user_auth.auth_type == auth_sa_access_token || app.auth_data.user_auth.auth_type == auth_sa_custom_token || app.auth_data.user_auth.auth_type == auth_user_id_token)
            {
                app.auth_data.app_token.authenticated = false;
                app.expire = app.auth_data.user_auth.auth_type == auth_user_id_token ? app.auth_data.user_auth.user.expire : app.auth_data.user_auth.sa.expire;

                app.timer.stop();
                app.timer.setInterval(0);
                app.timer.start();
            }
        }

        void signup(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            // TO DO
        }

        void resetPassword(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            // TO DO
        }

        void verify(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            // TO DO
        }

        void deleteUser(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth)
        {
            // TO DO
        }
    };

}

FirebaseClient Firebase;

template <typename T>
static user_auth_data &getAuth(T &auth) { return auth.get(); }
static void initializeApp(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.initializeApp(aClient, app, auth); }
template <typename T = const char *>
static void signup(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.signup(aClient, app, auth); }
template <typename T = const char *>
static void resetPassword(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.resetPassword(aClient, app, auth); }
template <typename T = const char *>
static void verify(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.verify(aClient, app, auth); }
template <typename T = const char *>
static void deleteUser(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth) { Firebase.deleteUser(aClient, app, auth); }

#endif