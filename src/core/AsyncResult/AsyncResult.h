/**
 * Created March 13, 2024
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

#include "./core/Core.h"
#include "Value.h"
#include "./core/Error.h"
#include "./core/List.h"
#include "./core/Timer.h"
#include "./core/StringUtil.h"

#define FIREBASE_CHUNK_SIZE 2048
#define FIREBASE_BASE64_CHUNK_SIZE 1026
#define FIREBASE_SSE_TIMEOUT 40 * 1000

using namespace firebase;

namespace ares_ns
{
    enum data_item_type_t
    {
        res_uid,
        res_etag,
        data_path,
        data_payload,
        debug_info,
        max_type
    };
}

namespace firebase
{
    struct app_event_t
    {

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
    struct RealtimeDatabaseResult
    {
        friend class AsyncResult;
        friend class AsyncClientClass;

        enum event_resume_status_t
        {
            event_resume_status_undefined,
            event_resume_status_resuming,
            event_resume_status_finished
        };

    public:
        template <typename T>
        T to() { return vcon.to<T>(data().c_str()); }
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

        realtime_database_data_type type() { return vcon.getType(data().c_str()); }

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
            sut.parse(*ref_payload, "\"name\"", "}", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                node_name = ref_payload->substring(p1 + 1, p2 - 1);
            }
        }
        void parseSSE()
        {
            clearSSE();
            int p1 = 0, p2 = 0;
            sut.parse(*ref_payload, "event", "\n", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                event_p1 = p1;
                event_p2 = p2;
                p1 = p2;
                setEventResumeStatus(event_resume_status_undefined);
                sse_timer.feed(String(event()).indexOf("cancel") > -1 || String(event()).indexOf("auth_revoked") > -1 ? 0 : FIREBASE_SSE_TIMEOUT);
                sse = true;
            }

            sut.parse(*ref_payload, "data", "\n", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                int p3 = p1, p4 = p2;
                if (ref_payload->substring(p1, p2) == "null")
                {
                    data_p1 = p1;
                    data_p2 = p2;
                    return;
                }

                sut.parse(*ref_payload, "\"path\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    data_path_p1 = p1 + 1;
                    data_path_p2 = p2 - 1;
                    p1 = p2;
                }

                sut.parse(*ref_payload, "\"data\"", "\n", p1, p2);
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
        void setEventResumeStatus(event_resume_status_t status) { event_resume_status = status; }

        event_resume_status_t eventResumeStatus() const { return event_resume_status; }

        bool null_etag = false;
        String *ref_payload = nullptr;

    private:
        ValueConverter vcon;
        StringUtil sut;
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
    friend class RealtimeDatabase;
    friend class Messaging;
    friend class Functions;
    friend class Storage;
    friend class CloudStorage;
    friend class FirestoreBase;
    friend class FirestoreDocuments;
    friend class async_data_item_t;

    struct download_data_t
    {
    public:
        size_t total = 0, downloaded = 0;
        bool progress_available = false, ota = false;
        int progress = -1;
        void reset()
        {
            total = 0;
            downloaded = 0;
            progress_available = false;
            progress = -1;
            ota = false;
        }
    };

    struct upload_data_t
    {
    public:
        size_t total = 0, uploaded = 0;
        bool progress_available = false;
        int progress = -1;
        String downloadUrl;
        void reset()
        {
            total = 0;
            uploaded = 0;
            progress_available = false;
            progress = -1;
            downloadUrl.remove(0, downloadUrl.length());
        }
    };

private:
    uint32_t addr = 0;
    uint32_t rvec_addr = 0;
    String val[ares_ns::max_type];
    bool debug_info_available = false;
    uint32_t debug_ms = 0, last_debug_ms = 0;
    download_data_t download_data;
    upload_data_t upload_data;
#if defined(ENABLE_DATABASE)
    RealtimeDatabaseResult rtdbResult;
#endif

    void setPayload(const String &data)
    {
        if (data.length())
        {
            data_available = true;
            val[ares_ns::data_payload] = data;
        }
#if defined(ENABLE_DATABASE)
        rtdbResult.ref_payload = &val[ares_ns::data_payload];
#endif
    }

    void setETag(const String &etag) { val[ares_ns::res_etag] = etag; }
    void setPath(const String &path) { val[ares_ns::data_path] = path; }

    bool setDownloadProgress()
    {
        download_data.progress_available = false;
        if (download_data.downloaded > 0)
        {
            int progress = (float)download_data.downloaded / download_data.total * 100;
            if (download_data.progress != progress && (progress == 0 || progress == 100 || download_data.progress + 2 <= progress))
            {
                download_data.progress_available = true;
                download_data.progress = progress;
                return true;
            }
        }
        return false;
    }

    bool setUploadProgress()
    {
        upload_data.progress_available = false;
        if (upload_data.uploaded > 0)
        {
            int progress = (float)upload_data.uploaded / upload_data.total * 100;
            if (upload_data.progress != progress && (progress == 0 || progress == 100 || upload_data.progress + 2 <= progress))
            {
                upload_data.progress_available = true;
                upload_data.progress = progress;
                return true;
            }
        }
        return false;
    }

public:
    bool data_available = false, error_available = false;
    app_event_t app_event;
    FirebaseError lastError;

    void setDebug(const String &debug)
    {
        // Keeping old message in case unread.
        debug_ms = millis();
        if (debug_info_available && val[ares_ns::debug_info].length() < 200)
        {
            if (val[ares_ns::debug_info].indexOf(debug) == -1)
            {
                val[ares_ns::debug_info] += " >> ";
                val[ares_ns::debug_info] += debug;
            }
        }
        else
            val[ares_ns::debug_info] = debug;
        if (debug.length())
            debug_info_available = true;
    }

    AsyncResult()
    {
#if defined(ENABLE_DATABASE)
        rtdbResult.ref_payload = &val[ares_ns::data_payload];
#endif
        addr = reinterpret_cast<uint32_t>(this);
        val[ares_ns::res_uid] = FPSTR("task_");
        val[ares_ns::res_uid] += String(millis());
    };
    ~AsyncResult()
    {
        if (rvec_addr > 0)
        {
            std::vector<uint32_t> *rVec = reinterpret_cast<std::vector<uint32_t> *>(rvec_addr);
            if (rVec)
            {
                List vec;
                addr = reinterpret_cast<uint32_t>(this);
                vec.addRemoveList(*rVec, addr, false);
            }
        }
    };
    const char *c_str() { return val[ares_ns::data_payload].c_str(); }
    String payload() const { return val[ares_ns::data_payload].c_str(); }
    String path() const { return val[ares_ns::data_path].c_str(); }
    String etag() const { return val[ares_ns::res_etag].c_str(); }
    String uid() const { return val[ares_ns::res_uid].c_str(); }
    String debug()
    {
        last_debug_ms = millis();
        return val[ares_ns::debug_info].c_str();
    }
    void clear()
    {
        for (size_t i = 0; i < ares_ns::max_type; i++)
            val[i].remove(0, val[i].length());
        debug_info_available = false;
        lastError.setLastError(0, "");
        app_event.setEvent(0, "");
        data_available = false;
        download_data.reset();
        upload_data.reset();
#if defined(ENABLE_DATABASE)
        rtdbResult.clearSSE();
#endif
    }
    template <typename T>
    T &to()
    {
        static T o;
        if (std::is_same<T, RealtimeDatabaseResult>::value)
            return rtdbResult;
        return o;
    }
    int available()
    {
        bool ret = data_available;
        data_available = false;
        return ret ? val[ares_ns::data_payload].length() : 0;
    }

    app_event_t appEvent() const { return app_event; }

    bool uploadProgress()
    {
        if (!upload_data.progress_available)
            setUploadProgress();
        return upload_data.progress_available;
    }

    upload_data_t uploadInfo() const { return upload_data; }

    bool downloadProgress()
    {
        if (!download_data.progress_available)
            setDownloadProgress();
        return download_data.progress_available;
    }

    download_data_t downloadInfo() const { return download_data; }

    bool isOTA() { return download_data.ota; }

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
        bool dbg = val[ares_ns::debug_info].length() > 0;
        if (debug_info_available && last_debug_ms < debug_ms && debug_ms > 0)
        {
            debug_info_available = false;
            return dbg;
        }
        return false;
    }

    FirebaseError error() const { return lastError; }
};

typedef void (*AsyncResultCallback)(AsyncResult &aResult);

#endif