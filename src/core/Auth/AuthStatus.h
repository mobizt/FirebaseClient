/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

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
                evstr = "initializing";
                break;
            case auth_event_signup:
                evstr = "sign up";
                break;
            case auth_event_send_verify_email:
                evstr = "send verification email";
                break;
            case auth_event_delete_user:
                evstr = "delete user";
                break;
            case auth_event_reset_password:
                evstr = "reset password";
                break;
            case auth_event_token_signing:
                evstr = "token signing";
                break;
            case auth_event_authenticating:
                evstr = "authenticating";
                break;
            case auth_event_auth_request_sent:
                evstr = "auth request sent";
                break;
            case auth_event_auth_response_received:
                evstr = "auth response received";
                break;
            case auth_event_ready:
                evstr = "ready";
                break;
            case auth_event_error:
                evstr = "error";
                break;
            case auth_event_deinitializing:
                evstr = "deinitializing";
                break;
            case auth_event_deinitialized:
                evstr = "deinitialized";
                break;
            default:
                evstr = "undefined";
                break;
            }
            return evstr;
        }

    public:
        firebase_auth_event_type event() const { return _event; }
    };
}
#endif