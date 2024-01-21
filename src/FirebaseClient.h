#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H
#include <Arduino.h>
#include "core/AuthConfig.h"
#include "AsyncClient/AsyncClient.h"
#include "database/Database.h"

class FirebaseClient
{
private:
    vector<uint32_t> appList;

    void addApp(uint32_t addr)
    {
        for (size_t i = 0; i < appList.size(); i++)
        {
            if (appList[i] == addr)
                return;
        }

        appList.push_back(addr);
    }

public:
    FirebaseClient(){};
    ~FirebaseClient(){};

    // template <typename T= Database>
    void initializeApp(AsyncClient &aClient, Database &app, user_auth_data &auth)
    {
        addApp(reinterpret_cast<uint32_t>(&app.auth_token));
    }
};

FirebaseClient Firebase;

template <typename T>
static user_auth_data &getAuth(T &auth) { return auth.get(); }

#endif