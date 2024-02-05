/**
 * Created February 5, 2024
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
#ifndef CORE_ERROR_H
#define CORE_ERROR_H

#include <Arduino.h>

#define FIREBASE_ERROR_HTTP_CODE_UNDEFINED -1000

/// HTTP codes see RFC7231
#define FIREBASE_ERROR_HTTP_CODE_OK 200
#define FIREBASE_ERROR_HTTP_CODE_NON_AUTHORITATIVE_INFORMATION 203
#define FIREBASE_ERROR_HTTP_CODE_NO_CONTENT 204
#define FIREBASE_ERROR_HTTP_CODE_MOVED_PERMANENTLY 301
#define FIREBASE_ERROR_HTTP_CODE_FOUND 302
#define FIREBASE_ERROR_HTTP_CODE_USE_PROXY 305
#define FIREBASE_ERROR_HTTP_CODE_TEMPORARY_REDIRECT 307
#define FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT 308
#define FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST 400
#define FIREBASE_ERROR_HTTP_CODE_UNAUTHORIZED 401
#define FIREBASE_ERROR_HTTP_CODE_FORBIDDEN 403
#define FIREBASE_ERROR_HTTP_CODE_NOT_FOUND 404
#define FIREBASE_ERROR_HTTP_CODE_METHOD_NOT_ALLOWED 405
#define FIREBASE_ERROR_HTTP_CODE_NOT_ACCEPTABLE 406
#define FIREBASE_ERROR_HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED 407
#define FIREBASE_ERROR_HTTP_CODE_REQUEST_TIMEOUT 408
#define FIREBASE_ERROR_HTTP_CODE_LENGTH_REQUIRED 411
#define FIREBASE_ERROR_HTTP_CODE_PRECONDITION_FAILED 412
#define FIREBASE_ERROR_HTTP_CODE_PAYLOAD_TOO_LARGE 413
#define FIREBASE_ERROR_HTTP_CODE_URI_TOO_LONG 414
#define FIREBASE_ERROR_HTTP_CODE_MISDIRECTED_REQUEST 421
#define FIREBASE_ERROR_HTTP_CODE_UNPROCESSABLE_ENTITY 422
#define FIREBASE_ERROR_HTTP_CODE_TOO_MANY_REQUESTS 429
#define FIREBASE_ERROR_HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE 431
#define FIREBASE_ERROR_HTTP_CODE_INTERNAL_SERVER_ERROR 500
#define FIREBASE_ERROR_HTTP_CODE_NOT_IMPLEMENTED 501
#define FIREBASE_ERROR_HTTP_CODE_BAD_GATEWAY 502
#define FIREBASE_ERROR_HTTP_CODE_SERVICE_UNAVAILABLE 503
#define FIREBASE_ERROR_HTTP_CODE_GATEWAY_TIMEOUT 504
#define FIREBASE_ERROR_HTTP_CODE_HTTP_VERSION_NOT_SUPPORTED 505
#define FIREBASE_ERROR_HTTP_CODE_LOOP_DETECTED 508
#define FIREBASE_ERROR_HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED 511

/// TCP client errors
#define FIREBASE_ERROR_TCP_CONNECTION -1
#define FIREBASE_ERROR_TCP_SEND -2
#define FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT -3
#define FIREBASE_ERROR_TCP_DISCONNECTED -4
#define FIREBASE_ERROR_TCP_CLIENT_UNDEFINED -5

// Firebase error
#define FIREBASE_ERROR_NETWORK_DISCONNECTED -100
#define FIREBASE_ERROR_NETWORK_CONNECTION_CALLBACK -101
#define FIREBASE_ERROR_NETWORK_STATUS_CALLBACK -102
#define FIREBASE_ERROR_OPEN_FILE -103
#define FIREBASE_ERROR_FILE_READ -104
#define FIREBASE_ERROR_FILE_WRITE -105
#define FIREBASE_ERROR_UNAUTHENTICATE -106
#define FIREBASE_ERROR_SERVER_RESPONSE -107
#define FIREBASE_ERROR_PATH_NOT_EXIST -108
#define FIREBASE_ERROR_MAX_REDIRECT_REACHED -109
#define FIREBASE_ERROR_TOKEN_PARSE_PK -110
#define FIREBASE_ERROR_TOKEN_SIGN -111
#define FIREBASE_ERROR_FW_UPDATE_TOO_LOW_FREE_SKETCH_SPACE -112
#define FIREBASE_ERROR_FW_UPDATE_WRITE_FAILED -113
#define FIREBASE_ERROR_FW_UPDATE_END_FAILED -114
#define FIREBASE_ERROR_STREAM_TIMEDOUT -115
#define FIREBASE_ERROR_STREAM_AUTH_REVOKED -116
#define FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED -117
#define FIREBASE_ERROR_OPERATION_CANCELLED -118

#if !defined(FPSTR)
#define FPSTR
#endif

class FirebaseError
{
    friend class AsyncClient;
    friend class AuthRequest;
    friend class Database;

private:
    struct firebase_error_info_t
    {
        String message;
        int code = 0;
    };

    firebase_error_info_t err;

    void clearError()
    {
        err.message.remove(0, err.message.length());
        err.code = 0;
    }

    void setResponseError(const String &message, int code)
    {
        if (code == FIREBASE_ERROR_HTTP_CODE_PRECONDITION_FAILED)
            err.message = FPSTR("precondition failed (ETag does not match)");
        else if (code == FIREBASE_ERROR_HTTP_CODE_UNAUTHORIZED)
            err.message = FPSTR("unauthorized");
        else if (message.length())
            err.message = message;
        else
        {
            err.message = FPSTR("HTTP Status ");
            err.message += code;
        }
        err.code = code;
    }

    void setClientError(int code)
    {
        err.code = code;
        if (code < 0)
        {
            switch (code)
            {
            case FIREBASE_ERROR_TCP_CONNECTION:
                err.message = FPSTR("TCP connection failed");
                break;
            case FIREBASE_ERROR_TCP_SEND:
                err.message = FPSTR("TCP send failed");
                break;
            case FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT:
                err.message = FPSTR("TCP receive timed out");
                break;
            case FIREBASE_ERROR_TCP_DISCONNECTED:
                err.message = FPSTR("TCP disconnected");
                break;
            case FIREBASE_ERROR_TCP_CLIENT_UNDEFINED:
                err.message = FPSTR("TCP client was undefined");
                break;
            case FIREBASE_ERROR_NETWORK_DISCONNECTED:
                err.message = FPSTR("network disconnected");
                break;
            case FIREBASE_ERROR_NETWORK_CONNECTION_CALLBACK:
                err.message = FPSTR("network connection callback was undefined");
                break;
            case FIREBASE_ERROR_NETWORK_STATUS_CALLBACK:
                err.message = FPSTR("network status callback was undefined");
                break;
            case FIREBASE_ERROR_OPEN_FILE:
                err.message = FPSTR("error opening file");
                break;
            case FIREBASE_ERROR_FILE_READ:
                err.message = FPSTR("error reading file");
                break;
            case FIREBASE_ERROR_FILE_WRITE:
                err.message = FPSTR("error writing file");
                break;
            case FIREBASE_ERROR_UNAUTHENTICATE:
                err.message = FPSTR("unauthenticate");
                break;
            case FIREBASE_ERROR_SERVER_RESPONSE:
                err.message = FPSTR("server responses ");
                break;
            case FIREBASE_ERROR_PATH_NOT_EXIST:
                err.message = FPSTR("path does not exists");
                break;
            case FIREBASE_ERROR_MAX_REDIRECT_REACHED:
                err.message = FPSTR("maximum redirection reaches");
                break;
            case FIREBASE_ERROR_TOKEN_PARSE_PK:
                err.message = FPSTR("parse private key");
                break;
            case FIREBASE_ERROR_TOKEN_SIGN:
                err.message = FPSTR("sign JWT token");
                break;
            case FIREBASE_ERROR_FW_UPDATE_TOO_LOW_FREE_SKETCH_SPACE:
                err.message = FPSTR("too low sketch space");
                break;
            case FIREBASE_ERROR_FW_UPDATE_WRITE_FAILED:
                err.message = FPSTR("firmware write failed");
                break;
            case FIREBASE_ERROR_FW_UPDATE_END_FAILED:
                err.message = FPSTR("firmware end failed");
                break;
            case FIREBASE_ERROR_STREAM_TIMEDOUT:
                err.message = FPSTR("stream timed out");
                break;
            case FIREBASE_ERROR_STREAM_AUTH_REVOKED:
                err.message = FPSTR("auth revoked");
                break;
            case FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED:
                err.message = FPSTR("app was not assigned");
                break;
            case FIREBASE_ERROR_OPERATION_CANCELLED:
                err.message = FPSTR("operation was cancelled");
                break;
            default:
                err.message = FPSTR("undefined");
                break;
            }
        }
    }

public:
    FirebaseError(){};
    ~FirebaseError(){};
    String message() const { return err.message; }
    int code() const { return err.code; }
};

#endif