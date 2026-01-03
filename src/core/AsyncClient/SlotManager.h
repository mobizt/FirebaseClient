/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_CLIENT_SLOT_MANAGER_H
#define CORE_ASYNC_CLIENT_SLOT_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "./core/AsyncResult/RTDBResultImpl.h"
#include "./core/AsyncClient/SlotOptions.h"
#include "./core/AsyncClient/AsyncData.h"
#include "./core/Debug.h"

#if defined(ENABLE_DATABASE)
#define PUBLIC_DATABASE_RESULT_IMPL_BASE : public RTDBResultImpl
#else
#define PUBLIC_DATABASE_RESULT_IMPL_BASE
#endif

class SlotManager PUBLIC_DATABASE_RESULT_IMPL_BASE
{
    friend class AsyncClientClass;

private:
    app_log_t debug_log;
    app_log_t event_log;
    conn_handler conn;
    std::vector<uint32_t> sVec;
    Client *client = nullptr;
    Timer session_timer;
    uint32_t session_timeout_sec = 0;
    uint32_t result_addr = 0;
    AsyncResult *refResult = nullptr;
    AsyncResult aResult;
    FirebaseError lastErr;
    tcp_client_type client_type = tcpc_sync;
    std::vector<uint32_t> rVec; // AsyncResult vector
    String sse_events_filter;

public:
    SlotManager() {}

    bool isSSEFilter() { return sse_events_filter.length() > 0; }

    int availableSlot(slot_options_t &options)
    {
        int slot = -1;
        if (options.auth_used /* highest priority */ || !options.async)
            slot = 0;
        else
        {
            int sse_index = -1, auth_index = -1;
            for (size_t i = 0; i < sVec.size(); i++)
            {
                if (getData(i))
                {
                    if (getData(i)->auth_used)
                        auth_index = i;
                    else if (getData(i)->sse)
                        sse_index = i;
                }
            }

            if (auth_index > -1)
                slot = auth_index + 1;
            else if (sse_index > -1)
                slot = sse_index;

// Multiple SSE modes
#if defined(FIREBASE_ASYNC_QUEUE_LIMIT)

            if ((sse_index > -1 && options.sse) || sVec.size() >= (int)FIREBASE_ASYNC_QUEUE_LIMIT + 0)
                slot = -2;
#else
            if (sse_index > -1 && options.sse)
                slot = -2;
#endif

            if (slot >= (int)sVec.size())
                slot = -1;
        }

        return slot;
    }

    async_data *getData(uint8_t slot)
    {
        if (slot < sVec.size())
            return reinterpret_cast<async_data *>(sVec[slot]);
        return nullptr;
    }

    async_data *addSlot(int index = -1)
    {
        async_data *sData = new async_data();

        sData->aResult.debug_log = &debug_log;
        sData->aResult.event_log = &event_log;

        if (index > -1)
            sVec.insert(sVec.begin() + index, sData->addr);
        else
            sVec.push_back(sData->addr);

        return sData;
    }

    async_data *createSlot(slot_options_t &options)
    {
        int slot_index = availableSlot(options);
        // Only one SSE mode is allowed
        if (slot_index == -2)
            return nullptr;

        bool prev_async = getData(0) && getData(0)->async && conn.async;

        async_data *sData = addSlot(slot_index);
        sData->reset();

        // If new task is sync while current async task is running then stop it.
        // The stopped async task will be resumed later.
        if (prev_async && !options.auth_used && !options.async)
            sData->stop_current_async = true;

        return sData;
    }

    void removeSlot(uint8_t slot, bool sse = true)
    {
        async_data *sData = getData(slot);

        if (!sData)
            return;

        if (sData->sse && !sse)
            return;

        if (!sData->auth_used && sData->request.ota && sData->request.ul_dl_task_running_addr > 0)
        {
            bool *ul_dl_task_running = reinterpret_cast<bool *>(sData->request.ul_dl_task_running_addr);
            *ul_dl_task_running = false;
        }

#if defined(ENABLE_DATABASE)
        clearSSE(&sData->aResult.rtdbResult);
#endif
#if defined(ENABLE_FS)
        sData->request.closeFile();
#endif
        setLastError(sData);
        // data available from sync and asyn request except for sse
        returnResult(sData, true);
        reset(sData, sData->auth_used);
        if (!sData->auth_used)
            delete sData;
        sData = nullptr;
        sVec.erase(sVec.begin() + slot);
    }

    bool sseFilter(async_data *sData)
    {
#if defined(ENABLE_DATABASE)
        String event = sData->aResult.rtdbResult.event();
        return (sse_events_filter.length() == 0 ||
                (sData->response.flags.http_response && sse_events_filter.indexOf("get") > -1 && event.indexOf("put") > -1) ||
                (!sData->response.flags.http_response && sse_events_filter.indexOf("put") > -1 && event.indexOf("put") > -1) ||
                (sse_events_filter.indexOf("patch") > -1 && event.indexOf("patch") > -1) ||
                (sse_events_filter.indexOf("keep-alive") > -1 && event.indexOf("keep-alive") > -1) ||
                (sse_events_filter.indexOf("cancel") > -1 && event.indexOf("cancel") > -1) ||
                (sse_events_filter.indexOf("auth_revoked") > -1 && event.indexOf("auth_revoked") > -1));
#else
        return false;
#endif
    }

    void reset(async_data *sData, bool disconnect)
    {
        if (disconnect)
            stop();

        sData->response.httpCode = 0;
        sData->error.code = 0;
        sData->response.flags.reset();
        sData->state = astate_undefined;
        sData->return_type = ret_undefined;
        sData->response.val[resns::etag].remove(0, sData->response.val[resns::etag].length());
        sData->aResult.download_data.reset();
        sData->aResult.upload_data.reset();
        sData->response.clearHeader(!sData->auth_used);
    }

    AsyncResult *getResult(async_data *sData)
    {
        List vec;
        return vec.existed(rVec, sData->ref_result_addr) ? sData->refResult : nullptr;
    }

    void returnResult(async_data *sData, bool setData)
    {
        bool error_notify_timeout = false;
        if (sData->err_timer.remaining() == 0)
        {
            sData->err_timer.feed(5);

            error_notify_timeout = sData->aResult.lastError.isError();
            sData->aResult.lastError.reset();
        }

        bool sseTimeout = false;
#if defined(ENABLE_DATABASE)
        sseTimeout = sData->sse && sData->aResult.rtdbResult.eventTimeout();
#endif
        bool download_status = sData->download && sData->aResult.setDownloadProgress();
        bool upload_status = sData->upload && sData->upload_progress_enabled && sData->aResult.setUploadProgress();

        if (sData->upload && sData->aResult.upload_data.progress == 100)
        {
            // Defer the complete upload progress when the payload does not set yet.
            if (sData->aResult.val[ares_ns::data_payload].length() == 0)
                upload_status = false;
        }

#if defined(ENABLE_FS)
        // Close file when downloading complete.
        if (sData->download && sData->aResult.download_data.progress == 100 && sData->request.file_data.file_status == file_config_data::file_status_opened)
            sData->request.closeFile();
#endif

        if (getResult(sData))
        {
            if (sseTimeout || setData || error_notify_timeout || download_status || upload_status)
            {
                sData->refResult->upload_data.reset();

                *sData->refResult = sData->aResult;

                if (setData)
                    sData->refResult->setPayload(sData->aResult.val[ares_ns::data_payload]);

                if (sData->aResult.download_data.downloaded == 0 || sData->aResult.upload_data.uploaded == 0)
                {
                    sData->refResult->setETag(sData->aResult.val[ares_ns::res_etag]);
                    sData->refResult->setPath(sData->aResult.val[ares_ns::data_path]);
                }
            }
        }

        if (sData->cb && (sseTimeout || setData || error_notify_timeout || download_status || upload_status))
        {
            if (!sData->auth_used)
                firebase_bebug_callback(sData->cb, sData->aResult, __func__, __LINE__, __FILE__);
        }

        if (getResult(sData))
        {
            // In case external async result was set, when download completed,
            // we need to set the download status again because the internal async result was deleted.
            if (sData->aResult.download_data.progress == 100)
                sData->refResult->download_data.download_progress.setProgress(sData->aResult.download_data.progress);

            // In case external async result was set, when upload complete,
            // we need to reset the internal async result upload status to prevent redundant complete messages.
            if (sData->aResult.upload_data.progress == 100 && upload_status)
                sData->aResult.upload_data.reset();
        }
    }

    void newCon(async_data *sData, const char *host, uint16_t port)
    {
        conn.newConn(client_type, client, &debug_log);
        sData->request.setClient(client_type, client);
        sData->response.setClient(client_type, client);

        if ((!sData->sse && session_timeout_sec >= FIREBASE_SESSION_TIMEOUT_SEC && session_timer.remaining() == 0) || sData->stop_current_async ||
            (conn.sse && !sData->sse) || (!conn.sse && sData->sse) || (sData->auth_used && sData->state == astate_undefined) ||
            strcmp(conn.host.c_str(), host) != 0 || conn.port != port)
        {
            sData->stop_current_async = false;
            stop();
            getResult()->clear();
        }

        // Required for sync task.
        if (!sData->async)
        {
            sData->aResult.lastError.clearError();
            lastErr.clearError();
        }
    }

    void stop()
    {
        if (conn.isConnected())
            debug_log.push_back(-1, "Terminating the server connection...");
        conn.stop();
    }

    AsyncResult *getResult()
    {
        List vec;
        return vec.existed(rVec, result_addr) ? refResult : &aResult;
    }

    void setAsyncError(async_data *sData, async_state state, int code, bool toRemove, bool toCloseFile)
    {
        if (!sData)
            return;

        sData->error.state = state;
        sData->error.code = code;

        if (toRemove)
            sData->to_remove = toRemove;

#if defined(ENABLE_FS)
        if (toCloseFile)
            sData->request.closeFile();
#endif
        (void)toCloseFile;
        setLastError(sData);
    }

    template <typename T>
    void setClientError(T &request, int code)
    {
        AsyncResult *aRes = request.aResult;

        if (!aRes)
            aRes = new AsyncResult();

        aRes->lastError.setClientError(code);
        firebase_bebug_callback(request.cb, *aRes, __func__, __LINE__, __FILE__);

        if (!request.aResult)
        {
            delete aRes;
            aRes = nullptr;
        }
    }

    void setLastError(async_data *sData)
    {
        if (sData && sData->error.code < 0)
        {
            sData->aResult.lastError.setClientError(sData->error.code);
            lastErr.setClientError(sData->error.code);
            sData->aResult.data_log.reset();
        }
        else if (sData && sData->response.httpCode > 0 && sData->response.httpCode >= FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST)
        {
            sData->aResult.lastError.setResponseError(sData->response.val[resns::payload], sData->response.httpCode);
            lastErr.setResponseError(sData->response.val[resns::payload], sData->response.httpCode);
            sData->aResult.data_log.reset();
        }
    }

    function_return_type connErrorHandler(async_data *sData, async_state state)
    {
        // TCP connection error.
        setAsyncError(sData, state, FIREBASE_ERROR_TCP_CONNECTION, !sData->sse, false);
        return ret_failure;
    }

    function_return_type connect(async_data *sData, const char *host, uint16_t port)
    {
        sData->aResult.lastError.clearError();
        lastErr.clearError();
        sData->aResult.data_log.reset();

        if ((sData->auth_used || sData->sse) && (millis() - sData->aResult.conn_ms < FIREBASE_RECONNECTION_TIMEOUT_MSEC) && sData->aResult.conn_ms > 0)
            return ret_continue;

        sData->aResult.conn_ms = millis();
        debug_log.reset();

        if (!conn.isConnected() && !sData->auth_used) // This info is already shown in auth task
            debug_log.push_back(-1, "Connecting to server...");

        sData->return_type = conn.connect(host, port);

        if (conn.isConnected() && !sData->sse && session_timeout_sec >= FIREBASE_SESSION_TIMEOUT_SEC)
            session_timer.feed(session_timeout_sec);

        return sData->return_type;
    }
};
#endif