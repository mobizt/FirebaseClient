#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H
#include <Arduino.h>
#include "core/FirebaseApp.h"
#include "database/Database.h"

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
            app.auth_data.user_auth.copy(auth);

            if (app.auth_data.user_auth.auth_data_type == user_auth_data_legacy_token || app.auth_data.user_auth.auth_data_type == user_auth_data_no_token)
            {
                app.timer.stop();
#if defined(ENABLE_LEGACY_TOKEN)
                if (app.auth_data.user_auth.auth_data_type == user_auth_data_legacy_token)
                    app.auth_data.app_token.token = app.auth_data.user_auth.legacy_token.token;
#endif
                app.auth_data.app_token.expire = 0;
                app.auth_data.app_token.refresh = "";
                app.auth_data.app_token.authenticated = true;
                app.timer.setInterval(3600);
            }
            else if (app.auth_data.user_auth.auth_data_type == user_auth_data_id_token)
            {
#if defined(ENABLE_ID_TOKEN)
                app.timer.stop();
                app.auth_data.user_auth.user.api_key = app.auth_data.user_auth.user.api_key;
                app.auth_data.app_token.token = app.auth_data.user_auth.id_token.token;
                app.auth_data.app_token.refresh = app.auth_data.user_auth.id_token.refresh;
                app.auth_data.app_token.expire = app.auth_data.user_auth.id_token.expire;
                app.auth_data.app_token.authenticated = app.auth_data.user_auth.initialized;
                app.timer.setInterval(app.auth_data.app_token.expire);
                app.timer.start();
#endif
            }
            else if (app.auth_data.user_auth.auth_type == auth_access_token || app.auth_data.user_auth.auth_type == auth_custom_token)
            {
                if (app.auth_data.user_auth.auth_type == auth_access_token)
                {
#if defined(ENABLE_ACCESS_TOKEN)
                    app.timer.stop();
                    app.auth_data.app_token.token = app.auth_data.user_auth.access_token.token;
                    app.auth_data.app_token.expire = app.auth_data.user_auth.access_token.expire;
                    app.auth_data.app_token.refresh = app.auth_data.user_auth.access_token.refresh;
                    app.auth_data.app_token.authenticated = app.auth_data.user_auth.initialized;
                    app.timer.setInterval(app.auth_data.user_auth.access_token.expire);
                    app.timer.start();
#endif
                }
                else if (app.auth_data.user_auth.auth_type == auth_custom_token)
                {
#if defined(ENABLE_CUSTOM_TOKEN)
                    app.timer.stop();
                    app.auth_data.app_token.token = app.auth_data.user_auth.custom_token.token;
                    app.auth_data.app_token.expire = app.auth_data.user_auth.custom_token.expire;
                    app.auth_data.app_token.refresh = app.auth_data.user_auth.custom_token.refresh;
                    app.auth_data.app_token.authenticated = app.auth_data.user_auth.initialized;
                    app.timer.setInterval(app.auth_data.user_auth.custom_token.expire);
                    app.timer.start();
#endif
                }
            }
            else if (app.auth_data.user_auth.auth_type == auth_sa_access_token || app.auth_data.user_auth.auth_type == auth_sa_custom_token || app.auth_data.user_auth.auth_type == auth_user_id_token)
            {
                app.auth_data.app_token.authenticated = false;
                app.timer.stop();
                app.timer.setInterval(0);
                app.timer.start();
            }
        }
    };

}

FirebaseClient Firebase;

template <typename T>
static user_auth_data &getAuth(T &auth) { return auth.get(); }

#endif