/**
 * 2025-01-26
 *
 * For MCU build target (CORE_ARDUINO_XXXX), see Options.h.
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 K. Suwatchai (Mobizt)
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
#ifndef ASYNC_CLIENT_H
#define ASYNC_CLIENT_H
#include <vector>
#include "./core/AsyncClient/ConnectionHandler.h"
#include "./core/AsyncClient/RequestHandler.h"
#include "./core/AsyncClient/ResponseHandler.h"
#include "./core/Memory.h"
#include "./core/FileConfig.h"
#include "./core/Error.h"
#include "./core/OTA.h"
#include "./core/AsyncResult/AsyncResult.h"
#include "./core/AsyncResult/ResultBase.h"
#include "./core/AsyncResult/RTDBResultBase.h"
#include "./core/AuthConfig.h"
#include "./core/List.h"
#include "./core/Core.h"
#include "./core/URL.h"
#include "./core/StringUtil.h"

#if defined(ENABLE_ASYNC_TCP_CLIENT)
#include "./core/AsyncTCPConfig.h"
#endif

using namespace firebase_ns;

enum async_state
{
    astate_undefined,
    astate_send_header,
    astate_send_payload,
    astate_read_response,
    astate_complete
};

struct async_data
{
    friend class FirebaseApp;

public:
    struct async_error_t
    {
        async_state state = astate_undefined;
        int code = 0;
    };

    async_state state = astate_undefined;
    function_return_type return_type = ret_undefined;
    req_handler request;
    res_handler response;
    async_error_t error;
    bool to_remove = false, auth_used = false, complete = false, async = false, sse = false, path_not_existed = false;
    bool download = false, upload_progress_enabled = false, upload = false;
    uint32_t auth_ts = 0, addr = 0, ref_result_addr = 0;
    AsyncResult aResult;
    AsyncResult *refResult = nullptr;
    AsyncResultCallback cb = NULL;
    Timer err_timer;

    async_data()
    {
        addr = reinterpret_cast<uint32_t>(this);
        err_timer.feed(0);
    }

    void setRefResult(AsyncResult *refResult, uint32_t rvec_addr)
    {
        this->refResult = refResult;
        ref_result_addr = reinterpret_cast<uint32_t>(refResult);
        this->refResult->rvec_addr = rvec_addr;
        if (rvec_addr > 0)
        {
            std::vector<uint32_t> *rVec = reinterpret_cast<std::vector<uint32_t> *>(rvec_addr);
            List vec;
            vec.addRemoveList(*rVec, ref_result_addr, true);
        }
    }

    void reset()
    {
        state = astate_undefined;
        return_type = ret_undefined;
        request.clear();
        response.clear();
        error.code = 0;
        error.state = astate_undefined;
        to_remove = false;
        auth_used = false;
        complete = false;
        async = false;
        sse = false;
        path_not_existed = false;
        cb = NULL;
        err_timer.reset();
    }
};

struct slot_options_t
{
public:
    bool auth_used = false, sse = false, async = false, sv = false, ota = false, no_etag = false, auth_param = false;
    app_token_t *app_token = nullptr;
    slot_options_t() {}
    explicit slot_options_t(bool auth_used, bool sse, bool async, bool sv, bool ota, bool no_etag, bool auth_param = false)
    {
        this->auth_used = auth_used;
        this->sse = sse;
        this->async = async;
        this->sv = sv;
        this->ota = ota;
        this->no_etag = no_etag;
        this->auth_param = auth_param;
    }
};

class AsyncClientClass : public ResultBase, RTDBResultBase
{
    friend class AppBase;
    friend class RealtimeDatabase;
    friend class Databases;
    friend class Documents;
    friend class CollectionGroups;
    friend class CloudFunctions;
    friend class Messaging;
    friend class Storage;
    friend class CloudStorage;
    friend class FirestoreBase;

private:
    StringUtil sut;
    conn_handler conn;
    app_debug_t app_debug;
    app_event_t app_event;
    FirebaseError lastErr;
    String header, reqEtag, resETag, sse_events_filter;
    AsyncResult *refResult = nullptr;
    AsyncResult aResult;
    uint32_t addr = 0, auth_ts = 0, cvec_addr = 0, result_addr = 0, sync_send_timeout_sec = 0, sync_read_timeout_sec = 0, session_timeout_sec = 0;
    Timer session_timer;
    Client *client = nullptr;
#if defined(ENABLE_ASYNC_TCP_CLIENT)
    AsyncTCPConfig *atcp_config = nullptr;
#else
    void *atcp_config = nullptr;
#endif
    tcp_client_type client_type = tcpc_sync;
    bool sse = false;
    String host;
    uint16_t port;
    std::vector<uint32_t> sVec;
    Memory mem;
    Base64Util b64ut;
    OTAUtil otaut;
    bool inProcess = false, inStopAsync = false;

    void newCon(async_data *sData, const char *host, uint16_t port)
    {
        conn.newConn(client_type, client, atcp_config, &app_debug);
        sData->request.setClient(client_type, client, atcp_config);
        sData->response.setClient(client_type, client, atcp_config);

        if ((!sData->sse && session_timeout_sec >= FIREBASE_SESSION_TIMEOUT_SEC && session_timer.remaining() == 0) || (sse && !sData->sse) || (!sse && sData->sse) || (sData->auth_used && sData->state == astate_undefined) ||
            strcmp(this->host.c_str(), host) != 0 || this->port != port)
        {
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

    function_return_type networkConnect(async_data *sData)
    {
        function_return_type ret = conn.netConnect();
        if (ret == ret_failure)
        {
            // TCP (network) disconnected error.
            setAsyncError(sData ? sData : nullptr, sData ? sData->state : astate_undefined, FIREBASE_ERROR_TCP_DISCONNECTED, sData ? !sData->sse : true, false);
            return ret_failure;
        }

        return ret;
    }

    function_return_type sendHeader(async_data *sData, const char *data)
    {
        return send(sData, reinterpret_cast<const uint8_t *>(data), data ? strlen(data) : 0, data ? strlen(data) : 0, astate_send_header);
    }

    function_return_type sendHeader(async_data *sData, const uint8_t *data, size_t len)
    {
        return send(sData, data, len, len, astate_send_header);
    }

    function_return_type sendBuff(async_data *sData, async_state state = astate_send_payload)
    {
        function_return_type ret = ret_continue;

        size_t totalLen = sData->request.file_data.file_size;
        bool fileopen = sData->request.payloadIndex == 0;

        if (sData->upload)
            sData->upload_progress_enabled = true;

        if (fileopen)
        {
#if defined(ENABLE_FS)
            if (sData->request.file_data.filename.length() > 0)
            {
                if (sData->request.file_data.file_status == file_config_data::file_status_closed)
                {
                    if (!sData->request.openFile(file_mode_open_read))
                    {
                        // File open error.
                        setAsyncError(sData, state, FIREBASE_ERROR_OPEN_FILE, !sData->sse, true);
                        return ret_failure;
                    }
                }
            }
#endif
            if (sData->request.base64)
            {
                ret = send(sData, reinterpret_cast<const uint8_t *>("\""), 1, totalLen, astate_send_payload);
                if (ret != ret_continue)
                    return ret;
            }
        }

        uint8_t *buf = nullptr;
#if defined(ENABLE_FS)
        if (sData->request.file_data.filename.length() > 0 ? sData->request.file_data.file.available() : sData->request.file_data.data_pos < sData->request.file_data.data_size)
#else
        if (sData->request.file_data.data_pos < sData->request.file_data.data_size)
#endif
        {
            int toSend = 0;
            if (sData->request.base64)
            {

                toSend = FIREBASE_BASE64_CHUNK_SIZE;

#if defined(ENABLE_FS)
                if (sData->request.file_data.filename.length() > 0)
                {
                    if (sData->request.file_data.file.available() < toSend)
                        toSend = sData->request.file_data.file.available();
                }
#endif
                if (sData->request.file_data.data && sData->request.file_data.data_size)
                {
                    if ((int)(sData->request.file_data.data_size - sData->request.file_data.data_pos) < toSend)
                        toSend = sData->request.file_data.data_size - sData->request.file_data.data_pos;
                }

                buf = reinterpret_cast<uint8_t *>(mem.alloc(toSend));
#if defined(ENABLE_FS)
                if (sData->request.file_data.filename.length() > 0)
                {
                    toSend = sData->request.file_data.file.read(buf, toSend);
                    if (toSend == 0)
                    {
                        // File read error.
                        setAsyncError(sData, state, FIREBASE_ERROR_FILE_READ, !sData->sse, true);
                        ret = ret_failure;
                        goto exit;
                    }
                }
#endif
                if (sData->request.file_data.data && sData->request.file_data.data_size)
                {
                    memcpy(buf, sData->request.file_data.data + sData->request.file_data.data_pos, toSend);
                    sData->request.file_data.data_pos += toSend;
                }

                uint8_t *temp = reinterpret_cast<uint8_t *>(b64ut.encodeToChars(mem, buf, toSend));
                mem.release(&buf);
                toSend = strlen(reinterpret_cast<char *>(temp));
                buf = temp;
            }
            else
            {
#if defined(ENABLE_CLOUD_STORAGE)
                if (sData->request.file_data.resumable.isEnabled() && sData->request.file_data.resumable.isUpload())
                    toSend = sData->request.file_data.resumable.getChunkSize(totalLen, sData->request.payloadIndex, sData->request.file_data.data_pos);
                else
#endif
                    toSend = totalLen - sData->request.file_data.data_pos < FIREBASE_CHUNK_SIZE ? totalLen - sData->request.file_data.data_pos : FIREBASE_CHUNK_SIZE;

                buf = reinterpret_cast<uint8_t *>(mem.alloc(toSend));

#if defined(ENABLE_FS)
                if (sData->request.file_data.filename.length() > 0)
                {
                    toSend = sData->request.file_data.file.read(buf, toSend);
                    if (toSend == 0)
                    {
                        // File read error.
                        setAsyncError(sData, state, FIREBASE_ERROR_FILE_READ, !sData->sse, true);
                        ret = ret_failure;
                        goto exit;
                    }
                }
#endif
                if (sData->request.file_data.data && sData->request.file_data.data_size)
                {
                    memcpy(buf, sData->request.file_data.data + sData->request.file_data.data_pos, toSend);
                }

                sData->request.file_data.data_pos += toSend;
            }

            ret = send(sData, buf, toSend, totalLen, astate_send_payload);
        }
        else if (sData->request.base64)
            ret = send(sData, reinterpret_cast<const uint8_t *>("\""), 1, totalLen, astate_send_payload);

#if defined(ENABLE_FS)
    exit:
#endif

        if (buf)
            mem.release(&buf);

        return ret;
    }

    function_return_type send(async_data *sData, const char *data, async_state state = astate_send_payload)
    {
        return send(sData, reinterpret_cast<const uint8_t *>(data), data ? strlen(data) : 0, data ? strlen(data) : 0, state);
    }

    function_return_type send(async_data *sData, const uint8_t *data, size_t len, size_t size, async_state state = astate_send_payload)
    {
        sData->state = state;

        if (data && len && this->client)
        {
            uint16_t toSend = len - sData->request.dataIndex > FIREBASE_CHUNK_SIZE ? FIREBASE_CHUNK_SIZE : len - sData->request.dataIndex;

            size_t sent = sData->request.tcpWrite(data + sData->request.dataIndex, toSend);
            sys_idle();

            if (sent == toSend)
            {
                sData->request.dataIndex += toSend;
                sData->request.payloadIndex += toSend;

                if (sData->upload && sData->upload_progress_enabled && sData->request.file_data.file_size)
                {
                    sData->aResult.upload_data.total = sData->request.file_data.file_size > size ? sData->request.file_data.file_size : size;
                    sData->aResult.upload_data.uploaded = sData->request.payloadIndex;
                    returnResult(sData, false);
                }

                if (sData->request.dataIndex == len)
                    sData->request.dataIndex = 0;

#if defined(ENABLE_CLOUD_STORAGE)

                if (sData->request.file_data.resumable.isEnabled() && sData->request.file_data.resumable.isUpload())
                {
                    int ret = sData->request.file_data.resumable.isComplete(size, sData->request.payloadIndex);
                    if (ret == 2)
                    {
                        sData->state = astate_read_response;
                        sData->return_type = ret_complete;
                        sData->request.dataIndex = 0;
                        sData->request.payloadIndex = 0;
                        sData->response.clear();
                        return sData->return_type;
                    }
                    else if (ret == 1)
                    {
                        sData->return_type = ret_continue;
                        return sData->return_type;
                    }
                }
                else if (sData->request.payloadIndex < size)
                {
                    sData->return_type = ret_continue;
                    return sData->return_type;
                }

#else
                if (sData->request.payloadIndex < size)
                {
                    sData->return_type = ret_continue;
                    return sData->return_type;
                }
#endif
            }
        }

        sData->return_type = sData->request.payloadIndex == size && size > 0 ? ret_complete : ret_failure;

        // TCP write error.
        if (sData->return_type == ret_failure)
            setAsyncError(sData, state, FIREBASE_ERROR_TCP_SEND, !sData->sse, false);

        sData->request.payloadIndex = 0;
        sData->request.dataIndex = 0;
        sData->request.file_data.data_pos = 0;

        if (sData->return_type == ret_complete)
        {
            if (state == astate_send_header)
            {
                // No payload when Content-Length is 0, or not set.
                if (sData->request.val[reqns::header].indexOf("Content-Length: 0\r\n") > -1 || sData->request.val[reqns::header].indexOf("Content-Length") == -1)
                    sData->state = astate_read_response;
                else
                    sData->state = astate_send_payload;
            }
            else if (state == astate_send_payload)
                sData->state = astate_read_response;

#if defined(ENABLE_CLOUD_STORAGE)
            if (sData->upload)
            {
                if (sData->request.file_data.resumable.isEnabled())
                    sData->request.file_data.resumable.updateState(sData->request.payloadIndex);
            }
#endif

            if (sData->state != astate_read_response)
                sData->response.clear();
        }

        return sData->return_type;
    }

    function_return_type send(async_data *sData)
    {
        function_return_type ret = networkConnect(sData);

        if (ret != ret_complete)
            return ret;

        ret = ret_continue;

        if (sData->state == astate_undefined || sData->state == astate_send_header)
        {
            newCon(sData, sData->request.getHost(true, &sData->response.val[resns::location]).c_str(), sData->request.port);

            if ((client_type == tcpc_sync && !conn.isConnected()) || client_type == tcpc_async)
            {
                ret = connect(sData, sData->request.getHost(true, &sData->response.val[resns::location]).c_str(), sData->request.port);

                // allow non-blocking async tcp connection
                if (ret == ret_continue)
                    return ret;

                if (ret != ret_complete)
                    return connErrorHandler(sData, sData->state);

                sse = sData->sse;
                sData->auth_ts = auth_ts;
            }

            if (sData->upload)
                sData->upload_progress_enabled = false;

            if (sData->request.app_token && sData->request.app_token->auth_data_type != user_auth_data_no_token)
            {
                if (sData->request.app_token->val[app_tk_ns::token].length() == 0)
                {
                    // Missing auth token error.
                    setAsyncError(sData, sData->state, FIREBASE_ERROR_UNAUTHENTICATE, !sData->sse, false);
                    return ret_failure;
                }

                header = sData->request.val[reqns::header];
                header.replace(FIREBASE_AUTH_PLACEHOLDER, sData->request.app_token->val[app_tk_ns::token]);
                ret = sendHeader(sData, header.c_str());
                sut.clear(header);
                return ret;
            }
            return sendHeader(sData, sData->request.val[reqns::header].c_str());
        }
        else if (sData->state == astate_send_payload)
        {
            if (sData->upload)
                sData->upload_progress_enabled = true;

            if (sData->request.method == reqns::http_get || sData->request.method == reqns::http_delete)
                sData->state = astate_read_response;
            else
            {
                if (sData->request.val[reqns::payload].length())
                    ret = send(sData, sData->request.val[reqns::payload].c_str());
                else if (sData->upload)
                {
                    if (sData->request.data && sData->request.dataLen)
                        ret = send(sData, sData->request.data, sData->request.dataLen, sData->request.dataLen);
                    else
                        ret = sendBuff(sData);
                }
            }
        }
        return ret;
    }

    function_return_type receive(async_data *sData)
    {
        function_return_type ret = networkConnect(sData);

        if (ret != ret_complete)
            return ret;

        // HTTP error is allowed in case non-auth task to get its response.
        if (!readResponse(sData))
        {
            // HTTP or TCP read error.
            setAsyncError(sData, sData->state, sData->response.httpCode > 0 ? sData->response.httpCode : FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT, !sData->sse, false);
            return ret_failure;
        }

        if (sData->response.httpCode == 0)
            return ret_continue;

#if defined(ENABLE_CLOUD_STORAGE)

        if (sData->request.file_data.resumable.isEnabled() && sData->request.file_data.resumable.getLocation().length() && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining)
        {
            String ext;
            String _host = sData->request.getHost(false, &sData->response.val[resns::location], &ext);

            if (connect(sData, _host.c_str(), sData->request.port) > ret_failure)
            {
                sut.clear(sData->request.val[reqns::payload]);
                sData->request.file_data.resumable.getHeader(sData->request.val[reqns::header], _host, ext);
                sData->state = astate_send_header;
                sData->request.file_data.resumable.setHeaderState();
                return ret_continue;
            }

            return connErrorHandler(sData, sData->state);
        }

#else
        if (sData->response.val[resns::location].length() && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining)
        {
            String ext;
            String _host = sData->request.getHost(false, &sData->response.val[resns::location], &ext);
            conn.stop();

            if (conn.connect(_host.c_str(), sData->request.port) > ret_failure)
            {
                URLUtil uut;
                uut.relocate(sData->request.val[reqns::header], _host, ext);
                sut.clear(sData->request.val[reqns::payload]);
                sData->state = astate_send_header;
                return ret_continue;
            }

            return connErrorHandler(sData, sData->state);
        }

#endif

        if (!sData->sse && sData->response.httpCode > 0 && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining)
        {
            sData->state = astate_undefined;
            return ret_complete;
        }

        return ret_continue;
    }

    function_return_type connErrorHandler(async_data *sData, async_state state)
    {
        // TCP connection error.
        setAsyncError(sData, state, FIREBASE_ERROR_TCP_CONNECTION, !sData->sse, false);
        return ret_failure;
    }

    void setAsyncError(async_data *sData, async_state state, int code, bool toRemove, bool toCloseFile)
    {
        if (!sData)
            return;

        sData->error.state = state;
        sData->error.code = code;

        if (toRemove)
            sData->to_remove = toRemove;

        if (toCloseFile)
            sData->request.closeFile();

        setLastError(sData);
    }

    template <typename T>
    void setClientError(T &request, int code)
    {
        AsyncResult *aResult = request.aResult;

        if (!aResult)
            aResult = new AsyncResult();

        aResult->lastError.setClientError(code);

        if (request.cb)
            request.cb(*aResult);

        if (!request.aResult)
        {
            delete aResult;
            aResult = nullptr;
        }
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

        sData->aResult.app_debug = &app_debug;
        sData->aResult.app_event = &app_event;

        if (index > -1)
            sVec.insert(sVec.begin() + index, sData->addr);
        else
            sVec.push_back(sData->addr);

        return sData;
    }

    AsyncResult *getResult(async_data *sData)
    {
        List vec;
        return vec.existed(rVec, sData->ref_result_addr) ? sData->refResult : nullptr;
    }

    AsyncResult *getResult()
    {
        List vec;
        return vec.existed(rVec, result_addr) ? refResult : &aResult;
    }

    void returnResult(async_data *sData, bool setData)
    {
        bool error_notify_timeout = false;
        if (sData->err_timer.remaining() == 0)
        {
            sData->err_timer.feed(5);
            error_notify_timeout = true;
        }

        bool sseTimeout = true;
#if defined(ENABLE_DATABASE)
        sseTimeout = sData->sse && sData->aResult.rtdbResult.eventTimeout();
#endif
        bool download_status = sData->download && sData->aResult.setDownloadProgress();
        bool upload_status = sData->upload && sData->upload_progress_enabled && sData->aResult.setUploadProgress();

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
                sData->cb(sData->aResult);
        }

        if (getResult(sData))
        {
            // In case external async result was set, when download completed,
            // we need to set the download status again because the internal async result was deleted.
            if (sData->aResult.download_data.progress == 100)
                sData->refResult->download_data.download_progress.setProgress(sData->aResult.download_data.progress);

            // In case external async result was set, when upload complete,
            // we need to reset the internal async result upload status to prevent redundant complete messages.
            if (sData->aResult.upload_data.progress == 100)
                sData->aResult.upload_data.reset();
        }
    }

    void setLastError(async_data *sData)
    {
        if (sData && sData->error.code < 0)
        {
            sData->aResult.lastError.setClientError(sData->error.code);
            lastErr.setClientError(sData->error.code);
            clearAppData(sData->aResult.app_data);

            // Required for sync task.
            if (!sData->async)
            {
                sData->aResult.lastError.isError();
                lastErr.isError();
            }
        }
        else if (sData && sData->response.httpCode > 0 && sData->response.httpCode >= FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST)
        {
            sData->aResult.lastError.setResponseError(sData->response.val[resns::payload], sData->response.httpCode);
            lastErr.setResponseError(sData->response.val[resns::payload], sData->response.httpCode);
            clearAppData(sData->aResult.app_data);

            // Required for sync task.
            if (!sData->async)
            {
                sData->aResult.lastError.isError();
                lastErr.isError();
            }
        }
    }

    void clear(String &str) { sut.clear(str); }

    bool readResponse(async_data *sData)
    {
        if (!client || !sData)
            return false;

        if (sData->response.tcpAvailable() > 0)
        {
            // status line or data?
            if (!sData->response.readStatusLine())
            {
                // remaining headers to read?
                if (sData->response.flags.header_remaining)
                    readHeader(sData);
                // read payload
                else if (sData->response.flags.payload_remaining || sData->response.flags.sse)
                {
                    if (!readPayload(sData))
                        return false;

                    String *payload = &sData->response.val[resns::payload];

                    if (sData->response.flags.sse || !sData->response.flags.payload_remaining)
                    {
                        if (!sData->auth_used)
                        {
                            if (!sData->response.flags.sse)
                                sData->aResult.setPayload(*payload);

                            if (sData->aResult.download_data.total > 0)
                                clearAppData(sData->aResult.app_data);

#if defined(ENABLE_DATABASE)

                            if (sData->request.method == reqns::http_post)
                                parseNodeName(&sData->aResult.rtdbResult);

                            // Data available from sse event
                            if (sData->response.flags.sse && payload->length())
                            {

                                if (payload->indexOf("event: ") > -1 && payload->indexOf("data: ") > -1 && payload->indexOf("\n") > -1)
                                {
                                    // Prevent sse timeout due to large sse Stream playload
                                    feedSSETimer(&sData->aResult.rtdbResult);

                                    if ((*payload)[payload->length() - 1] == '\n' && sData->response.tcpAvailable() == 0)
                                    {
                                        setRefPayload(&sData->aResult.rtdbResult, payload);
                                        parseSSE(&sData->aResult.rtdbResult);

                                        // Event filtering.
                                        if (sseFilter(sData))
                                        {
                                            // save payload to slot result
                                            sData->aResult.setPayload(*payload);
                                            clear(*payload);
                                            sData->response.flags.payload_available = true;
                                            returnResult(sData, true);
                                        }
                                        else
                                            clear(*payload);

                                        sData->response.flags.http_response = false;
                                    }
                                }
                            }
#endif
                        }
                    }
                }
            }
        }

        return true;
    }

    void readHeader(async_data *sData)
    {
        if (sData->response.flags.header_remaining)
        {
            if (sData->response.readHeader(sData->sse, !sData->auth_used, sData->upload))
            {
#if defined(ENABLE_CLOUD_STORAGE)
                if (sData->upload)
                    sData->response.parseRespHeader(sData->request.file_data.resumable.getLocationRef(), "Location");
#else
                sData->response.parseRespHeader(sData->response.val[resns::location], "Location");
#endif
                resETag = sData->response.val[resns::etag];
                sData->aResult.val[ares_ns::res_etag] = sData->response.val[resns::etag];
                sData->aResult.val[ares_ns::data_path] = sData->request.val[reqns::path];

#if defined(ENABLE_DATABASE)
                setNullETagOption(&sData->aResult.rtdbResult, sData->response.val[resns::etag].indexOf("null_etag") > -1);
#endif

#if defined(ENABLE_CLOUD_STORAGE)
                if (sData->upload && sData->request.file_data.resumable.isEnabled())
                {
                    sData->request.file_data.resumable.setHeaderState();
                    if (sData->response.flags.uploadRange)
                        sData->request.file_data.resumable.updateRange();
                }
#endif
                if (sData->request.method == reqns::http_delete && sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
                    setDebugBase(app_debug, FPSTR("Delete operation complete"));
            }
        }
    }

    bool readPayload(async_data *sData)
    {
        uint8_t *buf = nullptr;

        if (sData->response.flags.payload_remaining)
        {
            sData->response.feedTimer(!sData->async && sync_read_timeout_sec > 0 ? sync_read_timeout_sec : -1);

            // the next chunk data is the payload
            if (sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
            {
                if (sData->response.flags.chunks)
                {
                    // Use temporary String buffer for decodeChunks
                    String temp;
                    int res = sData->response.decodeChunks(&temp);
                    if (temp.length())
                    {
                        reserveString(sData);
                        sData->response.val[resns::payload] += temp;
                    }

                    if (res == -1)
                        sData->response.flags.payload_remaining = false;
                }
                else
                {
                    if (sData->download)
                    {
                        if (sData->response.payloadLen)
                        {
                            if (sData->response.payloadRead == 0)
                            {
                                if (sData->request.ota)
                                {
#if defined(FIREBASE_OTA_STORAGE)
                                    otaut.setOTAStorage(sData->request.ota_storage_addr);
#endif

                                    otaut.prepareDownloadOTA(sData->response.payloadLen, sData->request.base64, sData->request.ota_error);
                                    if (sData->request.ota_error != 0)
                                    {
                                        // OTA error.
                                        setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                        return false;
                                    }
                                }
#if defined(ENABLE_FS)
                                else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                                {
                                    sData->request.closeFile();

                                    if (!sData->request.openFile(file_mode_open_write))
                                    {
                                        // File open error.
                                        setAsyncError(sData, astate_read_response, FIREBASE_ERROR_OPEN_FILE, !sData->sse, true);
                                        return false;
                                    }
                                }
#endif
                                else
                                    sData->request.file_data.outB.init(sData->request.file_data.data, sData->request.file_data.data_size);
                            }

                            int toRead = 0, read = 0;
                            uint8_t ofs = 0;

                            buf = asyncBase64Buffer(sData, mem, read, toRead);

                            if (sData->response.toFillLen)
                                return true;

                            if (!buf)
                            {
                                ofs = sData->request.base64 && sData->response.payloadRead == 0 ? 1 : 0;
                                toRead = (int)(sData->response.payloadLen - sData->response.payloadRead) > FIREBASE_CHUNK_SIZE + ofs ? FIREBASE_CHUNK_SIZE + ofs : sData->response.payloadLen - sData->response.payloadRead;
                                buf = reinterpret_cast<uint8_t *>(mem.alloc(toRead));
                                read = sData->response.tcpRead(buf, toRead);
                            }

                            if (read > 0)
                            {
                                if (sData->request.base64 && read < toRead)
                                {
                                    sData->response.toFillIndex += read;
                                    sData->response.toFillLen = toRead - read;
                                    sData->response.toFill = reinterpret_cast<uint8_t *>(mem.alloc(toRead));
                                    memcpy(sData->response.toFill, buf, read);
                                    goto exit;
                                }

                                sData->response.payloadRead += read;
                                if (sData->request.base64)
                                {
                                    otaut.getPad(buf + ofs, read, sData->request.b64Pad);
                                    if (sData->request.ota)
                                    {
                                        otaut.decodeBase64OTA(mem, &b64ut, reinterpret_cast<const char *>(buf), read - ofs, sData->request.ota_error);
                                        if (sData->request.ota_error != 0)
                                        {
                                            // OTA error.
                                            setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                            goto exit;
                                        }

                                        if (sData->request.b64Pad > -1)
                                        {
                                            otaut.endDownloadOTA(b64ut, sData->request.b64Pad, sData->request.ota_error);
                                            if (sData->request.ota_error != 0)
                                            {
                                                // OTA error.
                                                setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                                goto exit;
                                            }
                                        }
                                    }
#if defined(ENABLE_FS)
                                    else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                                    {

                                        if (!b64ut.decodeToFile(mem, sData->request.file_data.file, reinterpret_cast<const char *>(buf + ofs)))
                                        {
                                            // File write error.
                                            setAsyncError(sData, astate_read_response, FIREBASE_ERROR_FILE_WRITE, !sData->sse, true);
                                            goto exit;
                                        }
                                    }
#endif
                                    else
                                        b64ut.decodeToBlob(mem, &sData->request.file_data.outB, reinterpret_cast<const char *>(buf + ofs));
                                }
                                else
                                {
                                    if (sData->request.ota)
                                    {
                                        b64ut.updateWrite(buf, read);

                                        if (sData->response.payloadRead == sData->response.payloadLen)
                                        {
                                            otaut.endDownloadOTA(b64ut, 0, sData->request.ota_error);
                                            if (sData->request.ota_error != 0)
                                            {
                                                // OTA error.
                                                setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                                goto exit;
                                            }
                                        }
                                    }
#if defined(ENABLE_FS)
                                    else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                                    {
                                        int write = sData->request.file_data.file.write(buf, read);
                                        if (write < read)
                                        {
                                            // File write error.
                                            setAsyncError(sData, astate_read_response, FIREBASE_ERROR_FILE_WRITE, !sData->sse, true);
                                            goto exit;
                                        }
                                    }
#endif
                                    else
                                        sData->request.file_data.outB.write(buf, read);
                                }
                            }
                        }

                        if (sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_OK)
                        {
                            sData->aResult.download_data.total = sData->response.payloadLen;
                            sData->aResult.download_data.downloaded = sData->response.payloadRead;
                            returnResult(sData, false);
                        }
                    }
                    else
                    {
                        // Use temporary String buffer for readLine
                        String temp;
                        size_t len = sData->response.readLine(&temp);
                        sData->response.payloadRead += len;
                        reserveString(sData);
                        sData->response.val[resns::payload] += temp;
                    }
                }
            }
        }
    exit:

        if (buf)
            mem.release(&buf);

        if (sData->response.payloadLen > 0 && sData->response.payloadRead >= sData->response.payloadLen && sData->response.tcpAvailable() == 0)
        {
            // Async payload and header data collision workaround from session reusage.
            if (!sData->response.flags.chunks && sData->response.payloadRead > sData->response.payloadLen)
            {
                sData->response.val[resns::header] = sData->response.val[resns::payload].substring(sData->response.payloadRead - sData->response.payloadLen);
                sData->response.val[resns::payload].remove(0, sData->response.payloadLen);
                sData->return_type = ret_continue;
                sData->state = astate_read_response;
                sData->response.flags.header_remaining = true;
            }

            if (sData->upload)
            {
                URLUtil uut;
                uut.updateDownloadURL(sData->aResult.upload_data.downloadUrl, sData->response.val[resns::payload]);
            }

            if (sData->response.flags.chunks && sData->auth_used)
                stop();

            // HTTP server error.
            if (sData->response.httpCode >= FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST)
            {
                setAsyncError(sData, sData->state, sData->response.httpCode, !sData->sse, true);
                sData->return_type = ret_failure;
                returnResult(sData, false);
            }

            if (sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_OK && sData->download)
            {
                sData->aResult.download_data.total = sData->response.payloadLen;
                sData->aResult.download_data.downloaded = sData->response.payloadRead;
                returnResult(sData, false);
            }

            // Don't reset the payload remaining flag in case of chunked.
            // It should set in readPayload when decodeChunks returns -1.
            if (!sData->response.flags.chunks)
                sData->response.flags.payload_remaining = false;

            sData->request.closeFile();

            if (sData->auth_used)
                sData->response.auth_data_available = true;
        }

        return sData->error.code == 0;
    }

    bool sseFilter(async_data *sData)
    {
        String event = sData->aResult.rtdbResult.event();
        return (sse_events_filter.length() == 0 ||
                (sData->response.flags.http_response && sse_events_filter.indexOf("get") > -1 && event.indexOf("put") > -1) ||
                (!sData->response.flags.http_response && sse_events_filter.indexOf("put") > -1 && event.indexOf("put") > -1) ||
                (sse_events_filter.indexOf("patch") > -1 && event.indexOf("patch") > -1) ||
                (sse_events_filter.indexOf("keep-alive") > -1 && event.indexOf("keep-alive") > -1) ||
                (sse_events_filter.indexOf("cancel") > -1 && event.indexOf("cancel") > -1) ||
                (sse_events_filter.indexOf("auth_revoked") > -1 && event.indexOf("auth_revoked") > -1));
    }

    void reserveString(async_data *sData)
    {
        // String memory reservation is needed to hadle large data in external memory.
#if defined(ENABLE_PSRAM) && ((defined(ESP8266) && defined(MMU_EXTERNAL_HEAP)) || (defined(ESP32) && defined(BOARD_HAS_PSRAM)))
        String old = sData->response.val[resns::payload];
        sData->response.val[resns::payload].remove(0, sData->response.val[resns::payload].length());
        sData->response.val[resns::payload].reserve(sData->response.payloadRead + 1);
        sData->response.val[resns::payload] = old;
#endif
    }

    // non-block memory buffer for collecting the multiple of 4 data prepared for base64 decoding
    uint8_t *asyncBase64Buffer(async_data *sData, Memory &mem, int &toRead, int &read)
    {
        uint8_t *buf = nullptr;

        if (sData->request.base64)
        {
            if (sData->response.toFill && sData->response.toFillLen)
            {
                int currentRead = sData->response.tcpRead(sData->response.toFill + sData->response.toFillIndex, sData->response.toFillLen);
                if (currentRead == sData->response.toFillLen)
                {
                    buf = reinterpret_cast<uint8_t *>(mem.alloc(sData->response.toFillIndex + sData->response.toFillLen));
                    memcpy(buf, sData->response.toFill, sData->response.toFillIndex + sData->response.toFillLen);
                    mem.release(&sData->response.toFill);
                    read = sData->response.toFillLen + sData->response.toFillIndex;
                    toRead = read;
                    sData->response.toFillIndex = 0;
                    sData->response.toFillLen = 0;
                }
                else
                {
                    sData->response.toFillIndex += currentRead;
                    sData->response.toFillLen -= currentRead;
                }
            }
        }

        return buf;
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
        clear(sData->response.val[resns::etag]);
        sData->aResult.download_data.reset();
        sData->aResult.upload_data.reset();
        sData->response.clear(!sData->auth_used);
    }

    function_return_type connect(async_data *sData, const char *host, uint16_t port)
    {
        sData->aResult.lastError.clearError();
        lastErr.clearError();
        clearAppData(sData->aResult.app_data);

        if ((sData->auth_used || sData->sse) && (millis() - sData->aResult.conn_ms < FIREBASE_RECONNECTION_TIMEOUT_MSEC) && sData->aResult.conn_ms > 0)
            return ret_continue;

        sData->aResult.conn_ms = millis();
        resetDebug(app_debug);

        if (!conn.isConnected() && !sData->auth_used) // This info is already shown in auth task
            setDebugBase(app_debug, FPSTR("Connecting to server..."));

        sData->return_type = conn.connect(host, port);

        this->host = host;
        this->port = port;

        if (conn.isConnected() && session_timeout_sec >= FIREBASE_SESSION_TIMEOUT_SEC)
            session_timer.feed(session_timeout_sec);

        return sData->return_type;
    }

    int sMan(slot_options_t &options)
    {
        int slot = -1;
        if (options.auth_used)
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
            if ((sse_index > -1 && options.sse) || sVec.size() >= FIREBASE_ASYNC_QUEUE_LIMIT)
                slot = -2;

            if (slot >= (int)sVec.size())
                slot = -1;
        }

        return slot;
    }

    uint8_t slotCount() const { return sVec.size(); }

    bool processLocked()
    {
        if (inProcess)
            return true;
        inProcess = true;
        return false;
    }

#if defined(ENABLE_DATABASE)
    void handleEventTimeout(async_data *sData)
    {
        if (sData->sse && sData->aResult.rtdbResult.eventTimeout())
        {
            if (eventResumeStatus(&sData->aResult.rtdbResult) == event_resume_status_undefined)
            {
                // In case Stream timed out,
                // we have to clear app data (available), reset the last error and set the Stream time out error.
                sData->aResult.lastError.reset();
                clearAppData(sData->aResult.app_data);
                setEventResumeStatus(&sData->aResult.rtdbResult, event_resume_status_resuming);
                // Stream timed out error.
                setAsyncError(sData, sData->state, FIREBASE_ERROR_STREAM_TIMEOUT, false, false);
                returnResult(sData, false);
                reset(sData, true);
            }
        }
    }
#endif

    bool handleSendTimeout(async_data *sData)
    {
        if (sData->request.send_timer.remaining() == 0)
        {
            // TCP write error.
            setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_SEND, !sData->sse, false);
            sData->return_type = ret_failure;
            // This requires by WiFiSSLClient before stating a new connection in case session was reused.
            reset(sData, true);
            return true;
        }
        return false;
    }

    bool handleReadTimeout(async_data *sData)
    {
        if (!sData->sse && sData->response.read_timer.remaining() == 0)
        {
            // TCP read error.
            setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT, !sData->sse, false);
            sData->return_type = ret_failure;
            // This requires by WiFiSSLClient before stating a new connection in case session was reused.
            reset(sData, true);
            return true;
        }
        return false;
    }

    void handleProcessFailure(async_data *sData)
    {
        if (sData->return_type == ret_failure)
        {
            if (sData->async)
                returnResult(sData, false);
            reset(sData, true);
        }
    }

    void stopAsyncImpl(bool all = false, const String &uid = "")
    {
        if (inStopAsync)
            return;

        inStopAsync = true;
        size_t size = slotCount();
        if (size)
        {
            for (int i = size - 1; i >= 0; i--)
            {
                sys_idle();
                async_data *sData = getData(i);
                if (sData && sData->async && !sData->auth_used && !sData->to_remove)
                {
                    // Reset the app data to reset clear the available status when the task was canceled.
                    sData->aResult.reset(sData->aResult.app_data);
                    if (getResult(sData))
                        getResult(sData)->reset(getResult(sData)->app_data);

                    if (uid.length())
                    {
                        if (strcmp(sData->aResult.uid().c_str(), uid.c_str()) == 0)
                            sData->to_remove = true;
                    }
                    else
                    {
                        sData->to_remove = true;
                        if (!all)
                            break;
                    }
                }
            }
        }

        inStopAsync = false;
    }

    void stop()
    {
        if (conn.isConnected())
            setDebugBase(app_debug, FPSTR("Terminating the server connection..."));

        conn.stop();
        clear(host);
        this->port = 0;
    }

    async_data *createSlot(slot_options_t &options)
    {
        if (!options.auth_used)
            sut.clear(sse_events_filter);

        int slot_index = sMan(options);
        // Only one SSE mode is allowed
        if (slot_index == -2)
            return nullptr;
        async_data *sData = addSlot(slot_index);
        sData->reset();
        return sData;
    }

    void newRequest(async_data *sData, const String &url, const String &path, const String &extras, reqns::http_request_method method, const slot_options_t &options, const String &uid)
    {
        sData->async = options.async;
        sData->request.val[reqns::url] = url;
        sData->request.val[reqns::path] = path;
        sData->request.method = method;
        sData->sse = options.sse;
        sData->request.val[reqns::etag] = reqEtag;

        clear(reqEtag);
        sData->aResult.setUID(uid);

        clear(sData->request.val[reqns::header]);
        sData->request.addRequestHeaderFirst(method);
        if (path.length() == 0)
            sData->request.val[reqns::header] += '/';
        else if (path[0] != '/')
            sData->request.val[reqns::header] += '/';
        sData->request.val[reqns::header] += path;
        sData->request.val[reqns::header] += extras;
        sData->request.addRequestHeaderLast();
        sData->request.addHostHeader(sData->request.getHost(true, &sData->response.val[resns::location]).c_str());

        sData->auth_used = options.auth_used;

        if (!options.auth_used)
        {
            sData->request.app_token = options.app_token;
            if (options.app_token && !options.auth_param && (options.app_token->auth_type > auth_unknown_token && options.app_token->auth_type < auth_refresh_token))
            {
                sData->request.addAuthHeaderFirst(options.app_token->auth_type);
                sData->request.val[reqns::header] += FIREBASE_AUTH_PLACEHOLDER;
                sData->request.addNewLine();
            }

            sData->request.addConnectionHeader(true);

            if (!options.sv && !options.no_etag && method != reqns::http_patch && extras.indexOf("orderBy") == -1)
            {
                sData->request.val[reqns::header] += FPSTR("X-Firebase-ETag: true");
                sData->request.addNewLine();
            }

            if (sData->request.val[reqns::etag].length() > 0 && (method == reqns::http_put || method == reqns::http_delete))
            {
                sData->request.val[reqns::header] += FPSTR("if-match: ");
                sData->request.val[reqns::header] += sData->request.val[reqns::etag];
                sData->request.addNewLine();
            }

            if (options.sse)
            {
                sData->request.val[reqns::header] += FPSTR("Accept: text/event-stream");
                sData->request.addNewLine();
            }
        }

        if (method == reqns::http_get || method == reqns::http_delete)
            sData->request.addNewLine();
    }

    void returnResult(async_data *sData) { *sData->refResult = sData->aResult; }

    void setAuthTs(uint32_t ts) { auth_ts = ts; }

    void addRemoveClientVec(uint32_t cvec_addr, bool add)
    {
        this->cvec_addr = cvec_addr;
        if (cvec_addr > 0)
        {
            std::vector<uint32_t> *cVec = reinterpret_cast<std::vector<uint32_t> *>(cvec_addr);
            List v;
            v.addRemoveList(*cVec, this->addr, add);
        }
    }

    void handleRemove()
    {
        for (size_t slot = 0; slot < slotCount(); slot++)
        {
            const async_data *sData = getData(slot);
            if (sData && sData->to_remove)
                removeSlot(slot);
        }
    }

    void setEvent(async_data *sData, int code, const String &msg)
    {
        setEventBase(app_event, code, msg);
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
        sData->request.closeFile();
        setLastError(sData);
        // data available from sync and asyn request except for sse
        returnResult(sData, true);
        reset(sData, sData->auth_used);
        if (!sData->auth_used)
            delete sData;
        sData = nullptr;
        sVec.erase(sVec.begin() + slot);
    }

    size_t slotCount() { return sVec.size(); }

    void exitProcess(bool status)
    {
        inProcess = status;
    }

    void process(bool async)
    {

        if (processLocked())
            return;

        if (slotCount())
        {
            size_t slot = 0;
            async_data *sData = getData(slot);

            if (!sData)
                return exitProcess(false);

            updateDebug(app_debug);
            updateEvent(app_event);
            sData->aResult.updateData();

            if (!sData->auth_used && (sData->request.ota || sData->download || sData->upload) && sData->request.ul_dl_task_running_addr > 0)
            {
                bool *ul_dl_task_running = reinterpret_cast<bool *>(sData->request.ul_dl_task_running_addr);
                *ul_dl_task_running = true;
            }

            if (networkConnect(sData) == ret_failure)
            {
                // TCP (network) disconnected error.
                setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_DISCONNECTED, !sData->sse, false);
                if (sData->async)
                {
                    returnResult(sData, false);
                    reset(sData, true);
                }

                return exitProcess(false);
            }

            if (sData->async && !async)
                return exitProcess(false);

            // Restart connection when authenticate, client or network changed
            if ((sData->sse && sData->auth_ts != auth_ts) || conn.isChanged())
            {
                stop();
                sData->state = astate_send_header;
            }

            bool sending = false;
            if (sData->state == astate_undefined || sData->state == astate_send_header || sData->state == astate_send_payload)
            {
                sData->response.clear();
                sData->request.feedTimer(!sData->async && sync_send_timeout_sec > 0 ? sync_send_timeout_sec : -1);
                sending = true;
                sData->return_type = send(sData);

                while (sData->state == astate_send_header || sData->state == astate_send_payload)
                {
                    sData->return_type = send(sData);
                    handleSendTimeout(sData);
                    if (sData->async || sData->return_type == ret_failure)
                        break;
                }

                // The response time-out timer should be initiated here with appropriate timeout
                // when the request was sucessfully sent (with or without payload).
                // Without this initialization, the subsequent sData->response.feedTimer may not execute in case
                // no server response returns because of server is out of reach or network/router is not responding.
                if (sData->state == astate_read_response)
                    sData->response.feedTimer(!sData->async && sync_read_timeout_sec > 0 ? sync_read_timeout_sec : -1);
            }

            if (sending)
            {
                handleSendTimeout(sData);
                if (sData->async && sData->return_type == ret_continue)
                    return exitProcess(false);
            }

            sys_idle();

            if (sData->state == astate_read_response)
            {
                // it can be complete response from payload sending
                if (sData->return_type == ret_complete)
                    sData->return_type = ret_continue;

                if (sData->async && !sData->response.tcpAvailable())
                {
                    if (sData->sse)
                    {
#if defined(ENABLE_DATABASE)
                        handleEventTimeout(sData);
#endif
                    }
                    else
                        handleReadTimeout(sData);
                    return exitProcess(false);
                }
                else if (!sData->async) // wait for non async
                {
                    while (!sData->response.tcpAvailable() && networkConnect(sData) == ret_complete)
                    {
                        sys_idle();
                        if (handleReadTimeout(sData))
                            break;
                    }
                }
            }

            // Read until status code > 0, header finished and payload read complete
            if (sData->state == astate_read_response)
            {
                sData->error.code = 0;
                while (sData->return_type == ret_continue && (sData->response.httpCode == 0 || sData->response.flags.header_remaining || sData->response.flags.payload_remaining))
                {
                    sData->response.feedTimer(!sData->async && sync_read_timeout_sec > 0 ? sync_read_timeout_sec : -1);
                    sData->return_type = receive(sData);

                    handleReadTimeout(sData);

                    bool allRead = sData->response.httpCode > 0 && sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_OK && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining;
                    if (allRead && sData->response.httpCode >= FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST)
                    {
                        if (sData->sse)
                        {
#if defined(ENABLE_DATABASE)
                            clearAppData(sData->aResult.app_data);
                            clearSSE(&sData->aResult.rtdbResult);
#endif
                        }
                        sData->return_type = ret_failure;
                    }

                    if (sData->async || allRead || sData->return_type == ret_failure)
                        break;
                }
            }

            handleProcessFailure(sData);

#if defined(ENABLE_DATABASE)
            handleEventTimeout(sData);
#endif

            if (!sData->sse && sData->return_type == ret_complete && sData->state != astate_send_payload)
                sData->to_remove = true;

            if (sData->to_remove)
                removeSlot(slot);
        }

        exitProcess(false);
    }

    std::vector<uint32_t> rVec; // AsyncResult vector

public:
    AsyncClientClass()
    {
        this->addr = reinterpret_cast<uint32_t>(this);
        client_type = tcpc_sync;
    }

    AsyncClientClass(Client &client, network_config_data &net) : client(&client)
    {
        conn.setNetwork(net);
        this->addr = reinterpret_cast<uint32_t>(this);
        client_type = tcpc_sync;
    }

#if defined(ENABLE_ASYNC_TCP_CLIENT)
    AsyncClientClass(AsyncTCPConfig &tcpClientConfig, network_config_data &net) : atcp_config(&tcpClientConfig)
    {
        conn.setNetwork(net);
        this->addr = reinterpret_cast<uint32_t>(this);
        client_type = tcpc_async;
    }
#endif

    ~AsyncClientClass()
    {
        stop();
        for (size_t i = 0; i < sVec.size(); i++)
        {
            reset(getData(i), true);
            async_data *sData = getData(i);
            delete sData;
            sData = nullptr;
        }
        addRemoveClientVec(cvec_addr, false);
    }

    /**
     * Set the external async result to use with the sync task.
     *
     * @param result The AsyncResult to set.
     *
     * If no async result was set (unset) for sync task, the internal async result will be used and shared usage for all sync tasks.
     *
     */
    void setAsyncResult(AsyncResult &result)
    {
        refResult = &result;
        result_addr = reinterpret_cast<uint32_t>(refResult);
    }

    /**
     * Unset the external async result use with the sync task.
     *
     * The internal async result will be used and shared usage for all sync tasks.
     *
     */
    void unsetAsyncResult()
    {
        refResult = nullptr;
        result_addr = 0;
    }

    /**
     * Get the network connection status.
     *
     * @return bool Returns true if network is connected.
     */
    bool networkStatus() { return conn.getNetworkStatus(); }

    /**
     * Stop and remove the async/sync task from the queue.
     *
     * @param all The option to stop and remove all tasks. If false, only running task will be stop and removed from queue.
     */
    void stopAsync(bool all = false) { stopAsyncImpl(all); }

    /**
     * Stop and remove the specific async/sync task from the queue.
     *
     * @param uid The task identifier of the task to stop and remove from the queue.
     */
    void stopAsync(const String &uid) { stopAsyncImpl(false, uid); }

    /**
     * Get the number of async/sync tasks that stored in the queue.
     *
     * @return size_t The total tasks in the queue.
     */
    size_t taskCount() const { return slotCount(); }

    /**
     * Get the last error information from async client.
     *
     * @return FirebaseError The FirebaseError object that contains the last error information.
     */
    FirebaseError lastError() const { return lastErr; }

    /**
     * Get the response ETag.
     *
     * @return String The response ETag header.
     */
    String etag() const { return resETag; }

    /**
     * Set the ETag header to the task.
     *
     * @param etag The ETag to set to the task.
     *
     * ETag of async client which obtained from etag() function will be empty after it assign to the task.
     */
    void setETag(const String &etag) { reqEtag = etag; }

    /**
     * Set the sync task's send timeout in seconds.
     *
     * @param timeoutSec The TCP write timeout in seconds.
     */
    void setSyncSendTimeout(uint32_t timeoutSec) { sync_send_timeout_sec = timeoutSec; }

    /**
     * Set the sync task's read timeout in seconds.
     *
     * @param timeoutSec The TCP read timeout in seconds.
     */
    void setSyncReadTimeout(uint32_t timeoutSec) { sync_read_timeout_sec = timeoutSec; }

    /**
     * Set the TCP session timeout in seconds.
     *
     * @param timeoutSec The TCP session timeout in seconds.
     */
    void setSessionTimeout(uint32_t timeoutSec) { session_timeout_sec = timeoutSec; }

    /**
     * Get the network disconnection time.
     *
     * @return unsigned long The millisec of network disconnection since device boot.
     */
    unsigned long networkLastSeen() { return conn.networkLastSeen(); }

    /**
     * Return the current network type enum.
     *
     * @return firebase_network_type The firebase_network_type enums are firebase_network_default, firebase_network_generic, firebase_network_ethernet and firebase_network_gsm.
     */
    firebase_network_type getNetworkType() { return conn.getNetworkType(); }

    /**
     * Set the network interface.
     *
     * The SSL client set here should work for the type of network set.
     *
     * @param client The SSL client that working with this type of network interface.
     * @param net The network config data can be obtained from the networking classes via the static function called `getNetwork`.
     */
    void setNetwork(Client &client, network_config_data &net)
    {
        // Check client changes.
       bool client_changed = reinterpret_cast<uint32_t>(&client) != reinterpret_cast<uint32_t>(this->client);

        // Some changes, stop the current network client.
        if (client_changed && this->client)
            conn.stop();

        // Change the network interface.
        // Should not check the type changes, just overwrite
        conn.setNetwork(net);

        // Change the client.
        if (client_changed)
        {
            this->client = &client;
            conn.setClientChange();
        }
    }
};

#endif