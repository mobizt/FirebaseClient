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
        }
    };

}

FirebaseClient Firebase;

template <typename T>
static user_auth_data &getAuth(T &auth) { return auth.get(); }

#endif