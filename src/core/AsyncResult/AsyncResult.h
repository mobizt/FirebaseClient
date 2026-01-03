/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_RESULT_ASYNC_RESULT_H
#define CORE_ASYNC_RESULT_ASYNC_RESULT_H

#include "./core/Core.h"
#include "./core/AsyncResult/Value.h"
#include "./core/Error.h"
#include "./core/Utils/List.h"
#include "./core/Utils/Timer.h"
#include "./core/Utils/StringUtil.h"
#include "./core/AsyncResult/AppLog.h"
#include "./core/AsyncResult/RTDBResult.h"
#include "./core/AsyncResult/AppProgress.h"

#if defined(ENABLE_DATABASE)
#define PUBLIC_DATABASE_RESULT_BASE : public RealtimeDatabaseResult
#else
#define PUBLIC_DATABASE_RESULT_BASE
#endif

using namespace firebase_ns;

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

class AsyncResult PUBLIC_DATABASE_RESULT_BASE
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
    friend class async_data;
    friend class SlotManager;
    friend class FirebaseApp;

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
    private:
        StringUtil sut;

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
            sut.clear(downloadUrl);
        }
    };

private:
    StringUtil sut;
    uint32_t addr = 0, rvec_addr = 0;
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
            data_log.push_back(-2, "");
            val[ares_ns::data_payload] = data;
        }
#if defined(ENABLE_DATABASE)
        setRefPayload(&rtdbResult, &val[ares_ns::data_payload]);
#endif
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
        if (debug_log)
            debug_log->push_back(-1, msg);
    }
    void setUID(const String &uid = "")
    {
        if (uid.length())
            val[ares_ns::res_uid] = uid;
        else
        {
            val[ares_ns::res_uid] = "task_";
            String m = String(millis());
            // Roll over millis counter value (0-9999).
            if (m.length() > 4)
                m.remove(0, m.length() - 4);
            val[ares_ns::res_uid] += m;
        }
    }

    app_log_t *debug_log = nullptr;
    app_log_t *event_log = nullptr;
    // This required by appEvent() that returns the reference to the event_log object which is not yet initialized.
    app_log_t evt;
    app_log_t dbg;
    FirebaseError lastError;
    app_log_t data_log;
    uint32_t conn_ms = 0;

    bool _downloadProgress() { return download_data.download_progress.isProgress(false); }
    bool _uploadProgress() { return upload_data.upload_progress.isProgress(false); }
    void errorPopFront() { lastError.err.pop_front(); }

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
    const char *c_str()
    {
        data_log.read();
        return val[ares_ns::data_payload].c_str();
    }

    /**
     * Get the server response payload string.
     *
     * @return const String The payload string.
     */
    const String &payload()
    {
        data_log.read();
        return val[ares_ns::data_payload];
    }

    /**
     * Get the length of response payload string.
     *
     * @return The payload string length.
     */
    size_t length() const { return val[ares_ns::data_payload].length(); }

    /**
     * Get the path of the resource of the request.
     *
     * @return String The path of the resource of the request.
     */
    String path() const { return val[ares_ns::data_path].c_str(); }

    /**
     * Get the etag of the server response headers.
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
        static String buf;
        if (debug_log)
            return debug_log->message();
        return String();
    }

    /**
     * Clear the async result.
     */
    void clear()
    {
        for (size_t i = 0; i < ares_ns::max_type; i++)
            sut.clear(val[i]);

        lastError.reset();

        if (debug_log)
            debug_log->reset();

        if (event_log)
            event_log->reset();

        data_log.reset();
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
        data_log.read();
        static T o;
#if defined(ENABLE_DATABASE)
        if (std::is_same<T, RealtimeDatabaseResult>::value)
            return rtdbResult;
#endif
        return o;
    }

    /**
     * Get the number of bytes of available response payload.
     * @return int The number of bytes available.
     */
    int available()
    {
        if (data_log.isAvailable())
            return val[ares_ns::data_payload].length();
        return 0;
    }

    /**
     * Check if the AsyncResult data/information is available.
     *
     * @return bool Returns true if data/information is available.
     */
    bool isResult() { return data_log.isAvailable() || lastError.isError() || (debug_log && debug_log->isAvailable()) || (event_log && event_log->isAvailable()) || _downloadProgress() || _uploadProgress(); }

    /**
     * Get the reference of internal app event information.
     *
     * @return app_log_t The reference of internal app event.
     */
    app_log_t &eventLog()
    {
        if (event_log)
            return *event_log;
        return evt;
    }
    app_log_t &appEvent() { return eventLog(); }

    /**
     * Get the reference of internal app debug information.
     *
     * @return app_log_t The reference of internal app debug.
     */
    app_log_t &debugLog()
    {
        if (debug_log)
            return *debug_log;
        return dbg;
    }

    /**
     * Get the reference of internal app data log information.
     *
     * @return app_log_t The reference of internal app data log.
     */
    app_log_t &dataLog()
    {
        return data_log;
    }

    /**
     * Check if file/BLOB upload information is available.
     *
     * @return bool Returns true if upload information is available.
     */
    bool uploadProgress() { return upload_data.upload_progress.isProgress(true); }

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
    bool downloadProgress() { return download_data.download_progress.isProgress(true); }

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
    bool isDebug() { return debug_log && debug_log->isAvailable(); }

    /**
     * Check if the app event information is available.
     *
     * @return bool Returns true if app event information is available.
     */
    bool isEvent() { return event_log && event_log->isAvailable(); }

    /**
     * Get the reference of internal FirebaseError object.
     *
     * @return FirebaseError The internal FirebaseError object.
     */
    FirebaseError &error() { return lastError; }
};

typedef void (*AsyncResultCallback)(AsyncResult &aResult);

#endif