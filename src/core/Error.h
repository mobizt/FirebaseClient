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

// Firebase error
#define FIREBASE_ERROR_OPEN_FILE -103
#define FIREBASE_ERROR_FILE_READ -104
#define FIREBASE_ERROR_FILE_WRITE -105
#define FIREBASE_ERROR_UNAUTHENTICATE -106
#define FIREBASE_ERROR_TOKEN_PARSE_PK -110
#define FIREBASE_ERROR_TOKEN_SIGN -111
#define FIREBASE_ERROR_FW_UPDATE_TOO_LOW_FREE_SKETCH_SPACE -112
#define FIREBASE_ERROR_FW_UPDATE_WRITE_FAILED -113
#define FIREBASE_ERROR_FW_UPDATE_END_FAILED -114
#define FIREBASE_ERROR_STREAM_TIMEOUT -115
#define FIREBASE_ERROR_STREAM_AUTH_REVOKED -116
#define FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED -117
#define FIREBASE_ERROR_OPERATION_CANCELLED -118
#define FIREBASE_ERROR_TIME_IS_NOT_SET_OR_INVALID -119
#define FIREBASE_ERROR_JWT_CREATION_REQUIRED -120
#define FIREBASE_ERROR_JWT_CREATION_TIMEDOUT -121
#define FIREBASE_ERROR_INVALID_DATABASE_SECRET -122
#define FIREBASE_ERROR_FW_UPDATE_OTA_STORAGE_CLASS_OBJECT_UNINITIALIZE -123
#define FIREBASE_ERROR_INVALID_DATABASE_URL -124
#define FIREBASE_ERROR_INVALID_HOST -125

#include "./core/AsyncResult/AppLog.h"

class FirebaseError
{
    friend class AsyncClientClass;
    friend class RealtimeDatabase;
    friend class FirestoreBase;
    friend class FirestoreDocuments;
    friend class FirestoreDatabase;
    friend class Messaging;
    friend class CloudFunctions;
    friend class Storage;
    friend class CloudStorage;
    friend class FirebaseApp;
    friend class SlotManager;
    friend class AsyncResult;
    friend class ResultBase;

private:
    app_log_t err;
    void clearError() { err.reset(); }
    void setResponseError(const String &message, int code)
    {
        if (code == FIREBASE_ERROR_HTTP_CODE_PRECONDITION_FAILED)
            err.push_back(code, "precondition failed (ETag does not match)");
        else if (code == FIREBASE_ERROR_HTTP_CODE_UNAUTHORIZED)
            err.push_back(code, "unauthorized");
        else if (message.length())
            err.push_back(code, message);
        else
        {
            String buf = "HTTP Status ";
            buf += code;
            err.push_back(code, buf);
        }
    }
    void setClientError(int code)
    {
        if (code < 0)
        {
            switch (code)
            {
            case FIREBASE_ERROR_TCP_CONNECTION:
                err.push_back(code, "TCP connection failed");
                break;
            case FIREBASE_ERROR_TCP_SEND:
                err.push_back(code, "TCP send failed");
                break;
            case FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT:
                err.push_back(code, "TCP receive timed out");
                break;
            case FIREBASE_ERROR_TCP_DISCONNECTED:
                err.push_back(code, "TCP disconnected");
                break;
            case FIREBASE_ERROR_OPEN_FILE:
                err.push_back(code, "error opening file");
                break;
            case FIREBASE_ERROR_FILE_READ:
                err.push_back(code, "error reading file");
                break;
            case FIREBASE_ERROR_FILE_WRITE:
                err.push_back(code, "error writing file");
                break;
            case FIREBASE_ERROR_UNAUTHENTICATE:
                err.push_back(code, "unauthenticate");
                break;
            case FIREBASE_ERROR_TOKEN_PARSE_PK:
                err.push_back(code, "parse private key");
                break;
            case FIREBASE_ERROR_TOKEN_SIGN:
                err.push_back(code, "sign JWT token");
                break;
            case FIREBASE_ERROR_FW_UPDATE_TOO_LOW_FREE_SKETCH_SPACE:
                err.push_back(code, "too low sketch space");
                break;
            case FIREBASE_ERROR_FW_UPDATE_OTA_STORAGE_CLASS_OBJECT_UNINITIALIZE:
                err.push_back(code, "OTA Storage was not set");
                break;
            case FIREBASE_ERROR_FW_UPDATE_WRITE_FAILED:
                err.push_back(code, "firmware write failed");
                break;
            case FIREBASE_ERROR_FW_UPDATE_END_FAILED:
                err.push_back(code, "firmware end failed");
                break;
            case FIREBASE_ERROR_STREAM_TIMEOUT:
                err.push_back(code, "stream connection timed out");
                break;
            case FIREBASE_ERROR_STREAM_AUTH_REVOKED:
                err.push_back(code, "auth revoked");
                break;
            case FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED:
                err.push_back(code, "app was not assigned");
                break;
            case FIREBASE_ERROR_OPERATION_CANCELLED:
                err.push_back(code, "operation was cancelled");
                break;
            case FIREBASE_ERROR_TIME_IS_NOT_SET_OR_INVALID:
                err.push_back(code, "time was not set or not valid");
                break;
            case FIREBASE_ERROR_INVALID_DATABASE_SECRET:
                err.push_back(code, "invalid database secret");
                break;
            case FIREBASE_ERROR_INVALID_DATABASE_URL:
                err.push_back(code, "invalid database URL, please use RealtimeDatabase::url to set");
                break;
            case FIREBASE_ERROR_INVALID_HOST:
                err.push_back(code, "invalid host");
                break;
            default:
                err.push_back(code, "undefined");
                break;
            }
        }
    }

public:
    FirebaseError() {}
    ~FirebaseError() {}
    String message() { return err.message(); }
    int code() { return err.code(); }
    void setLastError(int code, const String &msg) { err.push_back(code, msg); }
    void reset() { err.reset(); }
    bool isError() { return err.isAvailable(); }
    void errorPopFront() { err.pop_front(); }
};
#endif