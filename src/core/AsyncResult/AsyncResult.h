/**
 * Created June 12, 2024
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
#ifndef ASYNC_RESULT_H
#define ASYNC_RESULT_H

#include "./core/Core.h"
#include "Value.h"
#include "./core/Error.h"
#include "./core/List.h"
#include "./core/Timer.h"
#include "./core/StringUtil.h"
#include "./core/AsyncResult/AppEvent.h"
#include "./core/AsyncResult/AppDebug.h"
#include "./core/AsyncResult/ResultBase.h"
#include "./core/AsyncResult/AppData.h"
#include "./core/AsyncResult/AppProgress.h"

using namespace firebase;

namespace ares_ns
{
    enum data_item_type_t
    {
        res_uid,
        res_etag,
        data_path,
        data_payload,
        max_type
    };
}

class AsyncResult : public ResultBase, RealtimeDatabaseResult
{
    friend class AsyncClientClass;
    friend class AppBase;
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
        bool ota = false;
        int progress = -1;
        app_progress_t download_progress;
        void reset()
        {
            total = 0;
            downloaded = 0;
            download_progress.reset();
            progress = -1;
            ota = false;
        }
    };

    struct upload_data_t
    {
    public:
        size_t total = 0, uploaded = 0;
        app_progress_t upload_progress;
        int progress = -1;
        String downloadUrl;
        void reset()
        {
            total = 0;
            uploaded = 0;
            upload_progress.reset();
            progress = -1;
            downloadUrl.remove(0, downloadUrl.length());
        }
    };

private:
    uint32_t addr = 0;
    uint32_t rvec_addr = 0;
    String val[ares_ns::max_type];
    download_data_t download_data;
    upload_data_t upload_data;
#if defined(ENABLE_DATABASE)
    RealtimeDatabaseResult rtdbResult;
#endif

    void setPayload(const String &data)
    {
        if (data.length())
        {
            app_data.setData();
            val[ares_ns::data_payload] = data;
        }
#if defined(ENABLE_DATABASE)
        setRefPayload(&rtdbResult, &val[ares_ns::data_payload]);
#endif
    }

    void updateData()
    {
        app_data.update();
    }

    void setETag(const String &etag) { val[ares_ns::res_etag] = etag; }
    void setPath(const String &path) { val[ares_ns::data_path] = path; }

    bool setDownloadProgress()
    {
        if (download_data.downloaded > 0)
        {
            int progress = (float)download_data.downloaded / download_data.total * 100;
            if (download_data.progress != progress && (progress == 0 || progress == 100 || download_data.progress + 2 <= progress))
            {
                download_data.download_progress.setProgress(progress);
                download_data.progress = progress;
                return true;
            }
        }
        return false;
    }

    bool setUploadProgress()
    {
        if (upload_data.uploaded > 0)
        {
            int progress = (float)upload_data.uploaded / upload_data.total * 100;
            if (upload_data.progress != progress && (progress == 0 || progress == 100 || upload_data.progress + 2 <= progress))
            {
                upload_data.upload_progress.setProgress(progress);
                upload_data.progress = progress;
                return true;
            }
        }
        return false;
    }

    void setDebug(const String &msg)
    {
        if (app_debug)
            setDebugBase(*app_debug, msg);
    }

    void setUID(const String &uid = "")
    {
        if (uid.length())
            val[ares_ns::res_uid] = uid;
        else
        {
            val[ares_ns::res_uid] = FPSTR("task_");
            String ms = String(millis());
            // Roll over millis counter value (0-9999).
            if (ms.length() > 4)
                ms.remove(0, ms.length() - 4);
            val[ares_ns::res_uid] += ms;
        }
    }

    app_debug_t *app_debug = nullptr;
    app_event_t *app_event = nullptr;

    // This required by appEvent() that returns the reference to the app_event object which is not yet initialized.
    app_event_t ev;
    FirebaseError lastError;
    app_data_t app_data;

    uint32_t conn_ms = 0;

public:
    AsyncResult()
    {
#if defined(ENABLE_DATABASE)
        setRefPayload(&rtdbResult, &val[ares_ns::data_payload]);
#endif
        addr = reinterpret_cast<uint32_t>(this);
        setUID();
    };

    ~AsyncResult()
    {
        if (rvec_addr > 0)
        {
            std::vector<uint32_t> *rVec = reinterpret_cast<std::vector<uint32_t> *>(rvec_addr);
            List vec;
            addr = reinterpret_cast<uint32_t>(this);
            vec.addRemoveList(*rVec, addr, false);
        }
    };

    /**
     * Get the pointer to the internal response payload string buffer.
     *
     * @return const char * The pointer to internal response payload string.
     */
    const char *c_str() { return val[ares_ns::data_payload].c_str(); }

    /**
     * Get the copy of server response payload string.
     *
     * @return String The copy of payload string.
     */
    String payload() const { return val[ares_ns::data_payload].c_str(); }

    /**
     * Get the path of the resource of the request.
     *
     * @return String The path of the resource of the request.
     */
    String path() const { return val[ares_ns::data_path].c_str(); }

    /**
     * Get the Etag of the server response headers.
     *
     * @return String The ETag of response header.
     */
    String etag() const { return val[ares_ns::res_etag].c_str(); }

    /**
     * Get the unique identifier of async task.
     *
     * @return String The UID of async task.
     */
    String uid() const { return val[ares_ns::res_uid].c_str(); }

    /**
     * Get the debug information.
     *
     * @return String The debug information.
     */
    String debug()
    {
        if (app_debug)
            return app_debug->message();
        return "";
    }

    /**
     * Clear the async result.
     */
    void clear()
    {
        for (size_t i = 0; i < ares_ns::max_type; i++)
            val[i].remove(0, val[i].length());

        lastError.reset();

        if (app_debug)
            resetDebug(*app_debug);

        if (app_event)
            resetEvent(*app_event);

        app_data.reset();
        download_data.reset();
        upload_data.reset();
#if defined(ENABLE_DATABASE)
        clearSSE(&rtdbResult);
#endif
    }

    /**
     * Get the reference to the internal RealtimeDatabaseResult object.
     *
     * @return RealtimeDatabaseResult The reference to the internal RealtimeDatabaseResult object.
     */
    template <typename T>
    T &to()
    {
        static T o;
        if (std::is_same<T, RealtimeDatabaseResult>::value)
            return rtdbResult;
        return o;
    }

    /**
     * Get the number of bytes of available response payload.
     * @return int The number of bytes available.
     */
    int available()
    {
        if (app_data.isData())
            return val[ares_ns::data_payload].length();
        return 0;
    }

    /**
     * Get the reference of internal app event information.
     *
     * @return app_event_t The reference of internal app event.
     */
    app_event_t &appEvent()
    {
        if (app_event)
            return *app_event;
        return ev;
    }

    /**
     * Check if file/BLOB upload information is available.
     *
     * @return bool Returns true if upload information is available.
     */
    bool uploadProgress() { return upload_data.upload_progress.isProgress(); }

    /**
     * Get the file/BLOB upload information.
     *
     * @return upload_data_t The file/BLOB upload information.
     */
    upload_data_t uploadInfo() const { return upload_data; }

    /**
     * Check if the file/BLOB download information is availablle.
     *
     * @return bool The file/BLOB download status.
     */
    bool downloadProgress() { return download_data.download_progress.isProgress(); }

    /**
     * Get the file/BLOB download information.
     *
     * @return upload_data_t The file/BLOB download information.
     */
    download_data_t downloadInfo() const { return download_data; }

    /**
     * Check if the result is from OTA download task.
     *
     * @return bool Returns true if the result is from OTA download task.
     */
    bool isOTA() const { return download_data.ota; }

    /**
     * Check if the error occurred in async task.
     *
     * @return bool Returns true if error occurred.
     */
    bool isError() { return lastError.isError() && lastError.code() != 0 && lastError.code() != FIREBASE_ERROR_HTTP_CODE_OK; }

    /**
     * Check if the debug information in available.
     *
     * @return bool Returns true if debug information in available.
     */
    bool isDebug()
    {
        if (app_debug)
            return isDebugBase(*app_debug);
        return false;
    }

    /**
     * Check if the app event information in available.
     *
     * @return bool Returns true if app event information in available.
     */
    bool isEvent()
    {
        if (app_event)
            return isEventBase(*app_event);
        return false;
    }

    /**
     * Get the reference of internal FirebaseError object.
     *
     * @return FirebaseError The internal FirebaseError object.
     */
    FirebaseError &error() { return lastError; }
};

typedef void (*AsyncResultCallback)(AsyncResult &aResult);

#endif