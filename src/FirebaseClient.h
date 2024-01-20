#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H
#include <Arduino.h>
#include "core/AuthConfig.h"
#include "core/FirebaseApp.h"
#include "database/Database.h"

class FirebaseClient
{
private:
    
public:
    FirebaseClient(){};
    ~FirebaseClient(){};

    void initializeApp(AsyncClient &aClient, FirebaseApp &app, user_auth_data &auth){}



};

FirebaseClient Firebase;

#endif