/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_DATA_AUTH_BASE_H
#define CORE_AUTH_DATA_AUTH_BASE_H

#include <Arduino.h>

#define FIREBASE_DEFAULT_TOKEN_TTL 3300

namespace firebase_ns
{
    typedef void (*TimeStatusCallback)(uint32_t &ts);

    enum firebase_core_auth_task_type
    {
        firebase_core_auth_task_type_undefined = -1,
        firebase_core_auth_task_type_authenticate,
        firebase_core_auth_task_type_refresh_token,
        firebase_core_auth_task_type_signup,
        firebase_core_auth_task_type_send_verify_email,
        firebase_core_auth_task_type_delete_user,
        firebase_core_auth_task_type_reset_password
    };

    enum firebase_auth_event_type
    {
        auth_event_error = -1,
        auth_event_uninitialized = 0,
        auth_event_initializing = 1,
        auth_event_signup = 2,
        auth_event_send_verify_email = 3,
        auth_event_delete_user = 4,
        auth_event_reset_password = 5,
        auth_event_token_signing = 6,
        auth_event_authenticating = 7,
        auth_event_auth_request_sent = 8,
        auth_event_auth_response_received = 9,
        auth_event_ready = 10,
        auth_event_deinitializing = 11,
        auth_event_deinitialized = 12
    };

    enum auth_token_type
    {
        auth_unknown_token,
        auth_user_id_token,        // Id token using email/password credentials
        auth_sa_access_token,      // access token using service account credentials
        auth_sa_custom_token,      // Id token with custom UID and claims using service account credentials
        auth_id_token,             // Id token
        auth_access_token,         // access token
        auth_custom_token,         // Id token with custom UID and claims
        auth_refresh_token,        // refresh token
        auth_user_signup2,         // signup
        auth_user_reset_password2, // reset password
    };

    enum user_auth_data_type
    {
        user_auth_data_undefined,
        user_auth_data_service_account,
        user_auth_data_custom_data,
        user_auth_data_user_data,
        user_auth_data_id_token,
        user_auth_data_access_token,
        user_auth_data_custom_token,
        user_auth_data_legacy_token,
        user_auth_data_no_token
    };

    enum firebase_token_type
    {
        token_type_no,
        token_type_legacy,
        token_type_access,
        token_type_id
    };
}
#endif