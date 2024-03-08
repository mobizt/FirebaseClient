/**
 * Created March 7, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *4
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
#ifndef ASYNC_RESULT_H
#define ASYNC_RESULT_H
#include "Value.h"
#include "./core/Error.h"
#include "./core/List.h"
#include "./core/Timer.h"
#include "./core/StringHelper.h"

#define FIREBASE_CHUNK_SIZE 2048
#define FIREBASE_BASE64_CHUNK_SIZE 1026
#define FIREBASE_SSE_TIMEOUT 40 * 1000

using namespace firebase;

namespace firebase
{
    struct app_event_t
    {
        friend class AuthRequest;

    private:
        String ev_msg;
        int ev_code = 0;

    public:
        String message() { return ev_msg; }
        int code() { return ev_code; }
        void setEvent(int code, const String &msg)
        {
            ev_code = code;
            ev_msg = msg;
        }
    };
#if defined(ENABLE_DATABASE)
    struct database_data_t
    {
        friend class AsyncResult;
        friend class AsyncClientClass;
        friend class Database;
        friend class Firesrore;
        friend class Messaging;
        friend class Functions;
        friend class Storage;
        friend class CloudStorage;

        enum event_resume_status_t
        {
            event_resume_status_undefined,
            event_resume_status_resuming,
            event_resume_status_finished
        };

    public:
        template <typename T>
        T to()
        {
            return vcon.to<T>(data());
        }
        bool isStream() const { return sse; }
        String name() const { return node_name.c_str(); }
        String ETag() const { return etag.c_str(); }
        String dataPath() const { return ref_payload ? ref_payload->substring(data_path_p1, data_path_p2).c_str() : ""; }
        String event() { return ref_payload ? ref_payload->substring(event_p1, event_p2).c_str() : ""; }
        String data()
        {
            if (data_p1 > 0)
                return ref_payload->substring(data_p1, data_p2).c_str();
            return ref_payload ? ref_payload->c_str() : "";
        }

        bool eventTimeout() { return sse && sse_timer.remaining() == 0; }

        database_data_type type() { return vcon.getType(data().c_str()); }
        
        void clearSSE()
        {
            data_path_p1 = 0;
            data_path_p2 = 0;
            event_p1 = 0;
            event_p2 = 0;
            data_p1 = 0;
            data_p2 = 0;
            sse = false;
            event_resume_status = event_resume_status_undefined;
        }
        void parseNodeName()
        {
            int p1 = 0, p2 = 0;
            sh.parse(*ref_payload, "\"name\"", "}", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                node_name = ref_payload->substring(p1 + 1, p2 - 1);
            }
        }
        void parseSSE()
        {
            clearSSE();
            int p1 = 0, p2 = 0;
            sh.parse(*ref_payload, "event", "\n", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                event_p1 = p1;
                event_p2 = p2;
                p1 = p2;
                setEventResumeStatus(event_resume_status_undefined);
                sse_timer.feed(String(event()).indexOf("cancel") > -1 || String(event()).indexOf("auth_revoked") > -1 ? 0 : FIREBASE_SSE_TIMEOUT);
                sse = true;
            }

            sh.parse(*ref_payload, "data", "\n", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                int p3 = p1, p4 = p2;
                if (ref_payload->substring(p1, p2) == "null")
                {
                    data_p1 = p1;
                    data_p2 = p2;
                    return;
                }

                sh.parse(*ref_payload, "\"path\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    data_path_p1 = p1 + 1;
                    data_path_p2 = p2 - 1;
                    p1 = p2;
                }

                sh.parse(*ref_payload, "\"data\"", "\n", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    if ((*ref_payload)[p2 - 1] == '}')
                        p2--;
                    data_p1 = p1;
                    data_p2 = p2;
                }

                if (data_p1 == 0)
                {
                    data_p1 = p3;
                    data_p2 = p4;
                }
            }
        }
        void setEventResumeStatus(event_resume_status_t status)
        {
            event_resume_status = status;
        }

        event_resume_status_t eventResumeStatus() const
        {
            return event_resume_status;
        }

        bool null_etag = false;
        String *ref_payload = nullptr;

    private:
        ValueConverter vcon;
        StringHelper sh;
        Timer sse_timer;
        bool sse = false;
        event_resume_status_t event_resume_status = event_resume_status_undefined;
        String node_name, etag;
        uint16_t data_path_p1 = 0, data_path_p2 = 0, event_p1 = 0, event_p2 = 0, data_p1 = 0, data_p2 = 0;
    };
#endif
}

class AsyncResult
{
    friend class AsyncClientClass;
    friend class FirebaseApp;
    friend class AuthRequest;
    friend class Database;
    friend class Firestore;
    friend class Messaging;
    friend class Functions;
    friend class Storage;
    friend class CloudStorage;
    friend class FirestoreDocuments;
    friend class async_data_item_t;

    struct download_data_t
    {
    public:
        size_t total = 0, downloaded = 0;
        bool progress_available = false;
        int progress = -1;
        void reset()
        {
            total = 0;
            downloaded = 0;
            progress_available = false;
            progress = -1;
        }
    };

    struct upload_data_t
    {
    public:
        size_t total = 0, uploaded = 0;
        bool progress_available = false;
        int progress = -1;
        void reset()
        {
            total = 0;
            uploaded = 0;
            progress_available = false;
            progress = -1;
        }
    };

private:
    uint32_t addr = 0;
    String result_uid, data_path, data_payload, res_etag, debug_info;
    bool debug_info_available = false;
    download_data_t download_data;
    upload_data_t upload_data;

    void setPayload(const String &data)
    {
        if (data.length())
        {
            data_available = true;
            data_payload = data;
        }
#if defined(ENABLE_DATABASE)
        database.ref_payload = &data_payload;
#endif
    }

    void setETag(const String &etag)
    {
        res_etag = etag;
    }

    void setPath(const String &path)
    {
        data_path = path;
    }

public:
    bool data_available = false, error_available = false;
    app_event_t app_event;
    FirebaseError lastError;

    void setDebug(const String &debug)
    {
        debug_info = debug;
        if (debug.length())
            debug_info_available = true;
    }

    AsyncResult()
    {
#if defined(ENABLE_DATABASE)
        database.ref_payload = &data_payload;
#endif
        addr = reinterpret_cast<uint32_t>(this);
        List vec;
        vec.addRemoveList(rVec, addr, true);
    };
    ~AsyncResult()
    {
        List vec;
        vec.addRemoveList(rVec, addr, false);
    };
    const char *c_str() { return data_payload.c_str(); }
    String payload() const { return data_payload.c_str(); }
    String path() const { return data_path.c_str(); }
    String etag() const { return res_etag.c_str(); }
    String uid() const { return result_uid.c_str(); }
    String debug() const { return debug_info.c_str(); }
    int available()
    {
        bool ret = data_available;
        data_available = false;
        return ret ? data_payload.length() : 0;
    }

    app_event_t appEvent() const { return app_event; }

    bool uploadProgress()
    {
        if (upload_data.uploaded > 0)
        {
            int progress = (float)upload_data.uploaded / upload_data.total * 100;
            if (upload_data.progress != progress && (progress == 0 || progress == 100 || upload_data.progress + 2 <= progress))
            {
                upload_data.progress = progress;
                return true;
            }
        }
        return false;
    }

    upload_data_t uploadInfo() const { return upload_data; }

    bool downloadProgress()
    {
        if (download_data.downloaded > 0)
        {
            int progress = (float)download_data.downloaded / download_data.total * 100;
            if (download_data.progress != progress && (progress == 0 || progress == 100 || download_data.progress + 2 <= progress))
            {
                download_data.progress = progress;
                return true;
            }
        }
        return false;
    }

    download_data_t downloadInfo() const { return download_data; }

    bool isError()
    {
        bool err = lastError.code() != 0 && lastError.code() != FIREBASE_ERROR_HTTP_CODE_OK;
        if (error_available)
        {
            error_available = false;
            return err;
        }
        return false;
    }

    bool isDebug()
    {
        bool dbg = debug_info.length() > 0;
        if (debug_info_available)
        {
            debug_info_available = false;
            return dbg;
        }
        return false;
    }

    FirebaseError error() const { return lastError; }
#if defined(ENABLE_DATABASE)
    database_data_t database;
#endif
};

typedef void (*AsyncResultCallback)(AsyncResult &aResult);

#endif
