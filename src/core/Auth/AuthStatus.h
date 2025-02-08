#ifndef CORE_AUTH_AUTH_STATUS_H
#define CORE_AUTH_AUTH_STATUS_H

#include <Arduino.h>
#include "./core/Auth/AuthBase.h"

namespace firebase_ns
{
    struct auth_status
    {
        friend class user_auth_data;
        friend class FirebaseApp;

    private:
        firebase_auth_event_type _event = auth_event_uninitialized;

        String authEventString(firebase_auth_event_type ev)
        {
            String evstr;
            switch (ev)
            {
            case auth_event_initializing:
                evstr = FPSTR("initializing");
                break;
            case auth_event_signup:
                evstr = FPSTR("sign up");
                break;
            case auth_event_send_verify_email:
                evstr = FPSTR("send verification email");
                break;
            case auth_event_delete_user:
                evstr = FPSTR("delete user");
                break;
            case auth_event_reset_password:
                evstr = FPSTR("reset password");
                break;
            case auth_event_token_signing:
                evstr = FPSTR("token signing");
                break;
            case auth_event_authenticating:
                evstr = FPSTR("authenticating");
                break;
            case auth_event_auth_request_sent:
                evstr = FPSTR("auth request sent");
                break;
            case auth_event_auth_response_received:
                evstr = FPSTR("auth response received");
                break;
            case auth_event_ready:
                evstr = FPSTR("ready");
                break;
            case auth_event_error:
                evstr = FPSTR("error");
                break;
            default:
                evstr = FPSTR("undefined");
                break;
            }
            return evstr;
        }

    public:
        firebase_auth_event_type event() const { return _event; }
    };
}
#endif