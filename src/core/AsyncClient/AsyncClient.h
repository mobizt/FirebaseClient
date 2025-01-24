/**
 * 2025-01-25
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
#include "./core/AsyncClient/RequestHandler.h"
#include "./core/AsyncClient/ResponseHandler.h"
#include "./core/NetConfig.h"
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

enum function_return_type
{
    ret_undefined = -2,
    ret_failure = -1,
    ret_continue = 0,
    ret_complete = 1,
    ret_retry = 2
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
    app_debug_t app_debug;
    app_event_t app_event;
    FirebaseError lastErr;
    String header, reqEtag, resETag, sse_events_filter;
    AsyncResult *refResult = nullptr;
    AsyncResult aResult;
    int netErrState = 0;
    uint32_t addr = 0, auth_ts = 0, cvec_addr = 0, result_addr = 0, sync_send_timeout_sec = 0, sync_read_timeout_sec = 0, session_timeout_sec = 0;
    Timer session_timer;
    Client *client = nullptr;
    bool client_changed = false, network_changed = false;
#if defined(ENABLE_ASYNC_TCP_CLIENT)
    AsyncTCPConfig *async_tcp_config = nullptr;
#else
    void *async_tcp_config = nullptr;
#endif
    reqns::tcp_client_type client_type = reqns::tcpc_sync;
    bool sse = false, connected = false;
    String host;
    uint16_t port;
    std::vector<uint32_t> sVec;
    Memory mem;
    Base64Util b64ut;
    OTAUtil otaut;
    network_config_data net;
    bool inProcess = false, inStopAsync = false;

    void closeFile(async_data *sData)
    {
#if defined(ENABLE_FS)
        if (sData->request.file_data.file && sData->request.file_data.file_status == file_config_data::file_status_opened)
        {
            sData->request.file_data.file_size = 0;
            sData->request.payloadIndex = 0;
            sData->request.dataIndex = 0;
            sData->request.file_data.file_status = file_config_data::file_status_closed;
            sData->request.file_data.file.close();
        }
#endif
    }

    bool openFile(async_data *sData, file_operating_mode mode)
    {
#if defined(ENABLE_FS)
        sData->request.file_data.cb(sData->request.file_data.file, sData->request.file_data.filename.c_str(), mode);
        if (!sData->request.file_data.file)
            return false;
#else
        return false;
#endif
        sData->request.file_data.file_status = file_config_data::file_status_opened;
        return true;
    }

    void newCon(async_data *sData, const char *host, uint16_t port)
    {

        if ((!sData->sse && session_timeout_sec >= FIREBASE_SESSION_TIMEOUT_SEC && session_timer.remaining() == 0) || (sse && !sData->sse) || (!sse && sData->sse) || (sData->auth_used && sData->state == astate_undefined) ||
            strcmp(this->host.c_str(), host) != 0 || this->port != port)
        {
            stop(sData);
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
        function_return_type ret = netConnect(sData);

        if (!sData || ret == ret_failure)
        {
            // In case TCP (network) disconnected error.
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
                    if (!openFile(sData, file_mode_open_read))
                    {
                        // In case file open error.
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
                        // In case file read error.
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
                        // In case file read error.
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

            size_t sent = sData->request.tcpWrite(client_type, client, async_tcp_config, data + sData->request.dataIndex, toSend);
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

        // In case TCP write error.
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
            newCon(sData, getHost(sData, true).c_str(), sData->request.port);

            if ((client_type == reqns::tcpc_sync && !isConnected()) || client_type == reqns::tcpc_async)
            {
                ret = connect(sData, getHost(sData, true).c_str(), sData->request.port);

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
                    // In case missing auth token error.
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
            // In case HTTP or TCP read error.
            setAsyncError(sData, sData->state, sData->response.httpCode > 0 ? sData->response.httpCode : FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT, !sData->sse, false);
            return ret_failure;
        }

        if (sData->response.httpCode == 0)
            return ret_continue;

#if defined(ENABLE_CLOUD_STORAGE)

        if (sData->request.file_data.resumable.isEnabled() && sData->request.file_data.resumable.getLocation().length() && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining)
        {
            String ext;
            String _host = getHost(sData, false, &ext);

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
            String _host = getHost(sData, false, &ext);
            if (client)
            {
                client->stop();
                this->connected = false;
            }
            if (connect(sData, _host.c_str(), sData->request.port) > ret_failure)
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
        // In case TCP connection error.
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
            closeFile(sData);

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

    int readLine(async_data *sData, String &buf)
    {
        int p = 0;

        while (sData->response.tcpAvailable(client_type, client, async_tcp_config))
        {
            int res = sData->response.tcpRead(client_type, client, async_tcp_config);
            if (res > -1)
            {
                buf += (char)res;
                p++;
                if (res == '\n')
                    return p;
            }
        }
        return p;
    }

    uint32_t hex2int(const char *hex)
    {
        uint32_t val = 0;
        while (*hex)
        {
            // get current character then increment
            uint8_t byte = *hex++;
            // transform hex character to the 4bit equivalent number, using the ascii table indexes
            if (byte >= '0' && byte <= '9')
                byte = byte - '0';
            else if (byte >= 'a' && byte <= 'f')
                byte = byte - 'a' + 10;
            else if (byte >= 'A' && byte <= 'F')
                byte = byte - 'A' + 10;
            // shift 4 to make space for new digit, and add the 4 bits of the new digit
            val = (val << 4) | (byte & 0xF);
        }
        return val;
    }

    void clear(String &str) { sut.clear(str); }

    bool readResponse(async_data *sData)
    {
        if (!client || !sData)
            return false;

        if (sData->response.tcpAvailable(client_type, client, async_tcp_config) > 0)
        {
            // status line or data?
            if (!readStatusLine(sData))
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

                                    if ((*payload)[payload->length() - 1] == '\n' && sData->response.tcpAvailable(client_type, client, async_tcp_config) == 0)
                                    {
                                        setRefPayload(&sData->aResult.rtdbResult, payload);
                                        parseSSE(&sData->aResult.rtdbResult);

                                        // Event filtering.
                                        String event = sData->aResult.rtdbResult.event();
                                        if (sse_events_filter.length() == 0 ||
                                            (sData->response.flags.http_response && sse_events_filter.indexOf("get") > -1 && event.indexOf("put") > -1) ||
                                            (!sData->response.flags.http_response && sse_events_filter.indexOf("put") > -1 && event.indexOf("put") > -1) ||
                                            (sse_events_filter.indexOf("patch") > -1 && event.indexOf("patch") > -1) ||
                                            (sse_events_filter.indexOf("keep-alive") > -1 && event.indexOf("keep-alive") > -1) ||
                                            (sse_events_filter.indexOf("cancel") > -1 && event.indexOf("cancel") > -1) ||
                                            (sse_events_filter.indexOf("auth_revoked") > -1 && event.indexOf("auth_revoked") > -1))
                                        {
                                            // save payload to slot result
                                            sData->aResult.setPayload(*payload);
                                            clear(*payload);
                                            sData->response.flags.payload_available = true;
                                            returnResult(sData, true);
                                        }
                                        else
                                        {
                                            clear(*payload);
                                        }

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

    int getStatusCode(const String &header)
    {
        String out;
        int p1 = header.indexOf("HTTP/1.");
        if (p1 > -1)
        {
            out = header.substring(p1 + 9, header.indexOf(' ', p1 + 9));
            return atoi(out.c_str());
        }
        return 0;
    }

    bool readStatusLine(async_data *sData)
    {
        if (sData->response.httpCode > 0)
            return false;

        sData->response.val[resns::header].reserve(1024);

        // the first chunk (line) can be http response status or already connected stream payload
        readLine(sData, sData->response.val[resns::header]);
        int status = getStatusCode(sData->response.val[resns::header]);
        if (status > 0)
        {
            // http response status
            sData->response.flags.header_remaining = true;
            sData->response.httpCode = status;
        }
        return true;
    }

    void readHeader(async_data *sData)
    {
        if (sData->response.flags.header_remaining)
        {
            int read = readLine(sData, sData->response.val[resns::header]);
            if ((read == 1 && sData->response.val[resns::header][sData->response.val[resns::header].length() - 1] == '\n') ||
                (read == 2 && sData->response.val[resns::header][sData->response.val[resns::header].length() - 2] == '\r' && sData->response.val[resns::header][sData->response.val[resns::header].length() - 1] == '\n'))
            {
                sData->response.flags.http_response = true;
                clear(sData->response.val[resns::etag]);
                String temp[5];
#if defined(ENABLE_CLOUD_STORAGE)
                if (sData->upload)
                    parseRespHeader(sData, sData->response.val[resns::header], sData->request.file_data.resumable.getLocationRef(), "Location");
#else
                parseRespHeader(sData, sData->response.val[resns::header], sData->response.val[resns::location], "Location");

#endif
                parseRespHeader(sData, sData->response.val[resns::header], sData->response.val[resns::etag], "ETag");
                resETag = sData->response.val[resns::etag];
                sData->aResult.val[ares_ns::res_etag] = sData->response.val[resns::etag];
                sData->aResult.val[ares_ns::data_path] = sData->request.val[reqns::path];
#if defined(ENABLE_DATABASE)
                setNullETagOption(&sData->aResult.rtdbResult, sData->response.val[resns::etag].indexOf("null_etag") > -1);
#endif

                parseRespHeader(sData, sData->response.val[resns::header], temp[0], "Content-Length");

                sData->response.payloadLen = atoi(temp[0].c_str());

                parseRespHeader(sData, sData->response.val[resns::header], temp[1], "Connection");
                sData->response.flags.keep_alive = temp[1].length() && temp[1].indexOf("keep-alive") > -1;

                parseRespHeader(sData, sData->response.val[resns::header], temp[2], "Transfer-Encoding");
                sData->response.flags.chunks = temp[2].length() && temp[2].indexOf("chunked") > -1;

                parseRespHeader(sData, sData->response.val[resns::header], temp[3], "Content-Type");
                sData->response.flags.sse = temp[3].length() && temp[3].indexOf("text/event-stream") > -1;

                if (sData->upload)
                    parseRespHeader(sData, sData->response.val[resns::header], temp[4], "Range");

                clear(sData);

#if defined(ENABLE_CLOUD_STORAGE)
                if (sData->upload && sData->request.file_data.resumable.isEnabled())
                {
                    sData->request.file_data.resumable.setHeaderState();
                    if (sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT && temp[4].indexOf("bytes=") > -1)
                        sData->request.file_data.resumable.updateRange();
                }
#endif
                for (size_t i = 0; i < 5; i++)
                    sut.clear(temp[i]);

                if (sData->response.httpCode > 0 && sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
                    sData->response.flags.payload_remaining = true;

                if (!sData->sse && (sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_OK || sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT || sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_NO_CONTENT) && !sData->response.flags.chunks && sData->response.payloadLen == 0)
                    sData->response.flags.payload_remaining = false;

                if (sData->request.method == reqns::http_delete && sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
                    setDebugBase(app_debug, FPSTR("Delete operation complete"));
            }
        }
    }

    void parseRespHeader(async_data *sData, const String &src, String &out, const char *header)
    {
        if (sData->response.httpCode > 0)
        {
            int p1 = -1, p2 = -1, p3 = -1;
            p1 = src.indexOf(header);
            if (p1 > -1)
                p2 = src.indexOf(':', p1);

            if (p2 > -1)
                p3 = src.indexOf("\r\n", p2);

            if (p2 > -1 && p3 > -1)
                out = src.substring(p2 + 1, p3);

            out.trim();
        }
    }

    int getChunkSize(async_data *sData, Client *client, String &line)
    {
        if (line.length() == 0)
            readLine(sData, line);

        int p = line.indexOf(";");
        if (p == -1)
            p = line.indexOf("\r\n");
        if (p != -1)
            sData->response.chunkInfo.chunkSize = hex2int(line.substring(0, p).c_str());

        return sData->response.chunkInfo.chunkSize;
    }

    // Returns -1 when complete
    int decodeChunks(async_data *sData, Client *client, String *out)
    {
        if (!client || !sData || !out)
            return 0;
        int res = 0, read = 0;
        String line;

        // because chunks might span multiple reads, we need to keep track of where we are in the chunk
        // chunkInfo.dataLen is our current position in the chunk
        // chunkInfo.chunkSize is the total size of the chunk

        // readline() only reads while there is data available, so it might return early
        // when available() is less than the remaining amount of data in the chunk

        // read chunk-size, chunk-extension (if any) and CRLF
        if (sData->response.chunkInfo.phase == res_handler::READ_CHUNK_SIZE)
        {
            sData->response.chunkInfo.phase = res_handler::READ_CHUNK_DATA;
            sData->response.chunkInfo.chunkSize = -1;
            sData->response.chunkInfo.dataLen = 0;
            res = getChunkSize(sData, client, line);
            sData->response.payloadLen += res > -1 ? res : 0;
        }
        // read chunk-data and CRLF
        // append chunk-data to entity-body
        else
        {
            // if chunk-size is 0, it's the last chunk, and can be skipped
            if (sData->response.chunkInfo.chunkSize > 0)
            {
                read = readLine(sData, line);

                // if we read till a CRLF, we have a chunk (or the rest of it)
                // if the last two bytes are NOT CRLF, we have a partial chunk
                // if we read 0 bytes, read next chunk size

                // check for \n and \r, remove them if present (they're part of the protocol, not the data)
                if (read >= 2 && line[read - 2] == '\r' && line[read - 1] == '\n')
                {
                    // last chunk?
                    if (line[0] == '0')
                        return -1;

                    // remove the \r\n
                    line.remove(line.length() - 2);
                    read -= 2;
                }

                // if we still have data, append it and update the chunkInfo
                if (read)
                {
                    *out += line;
                    sData->response.chunkInfo.dataLen += read;
                    sData->response.payloadRead += read;

                    // check if we're done reading this chunk
                    if (sData->response.chunkInfo.dataLen == sData->response.chunkInfo.chunkSize)
                        sData->response.chunkInfo.phase = res_handler::READ_CHUNK_SIZE;
                }
                // if we read 0 bytes, read next chunk size
                else
                {
                    sData->response.chunkInfo.phase = res_handler::READ_CHUNK_SIZE;
                }
            }
            else
            {

                read = readLine(sData, line);

                // CRLF (end of chunked body)
                if (read == 2 && line[0] == '\r' && line[1] == '\n')
                    res = -1;
                else // another chunk?
                    getChunkSize(sData, client, line);
            }
        }

        return res;
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
                    int res = decodeChunks(sData, client, &temp);
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
                                        // In case OTA error.
                                        setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                        return false;
                                    }
                                }
#if defined(ENABLE_FS)
                                else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                                {
                                    closeFile(sData);

                                    if (!openFile(sData, file_mode_open_write))
                                    {
                                        // In case file open error.
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
                                read = sData->response.tcpRead(client_type, client, async_tcp_config, buf, toRead);
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
                                            // In case OTA error.
                                            setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                            goto exit;
                                        }

                                        if (sData->request.b64Pad > -1)
                                        {
                                            otaut.endDownloadOTA(b64ut, sData->request.b64Pad, sData->request.ota_error);
                                            if (sData->request.ota_error != 0)
                                            {
                                                // In case OTA error.
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
                                            // In case file write error.
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
                                                // In case OTA error.
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
                                            // In case file write error.
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
                        size_t len = readLine(sData, temp);
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

        if (sData->response.payloadLen > 0 && sData->response.payloadRead >= sData->response.payloadLen && sData->response.tcpAvailable(client_type, client, async_tcp_config) == 0)
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
                stop(sData);

            // In case HTTP error.
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

            closeFile(sData);

            if (sData->auth_used)
                sData->response.auth_data_available = true;
        }

        return sData->error.code == 0;
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
                int currentRead = sData->response.tcpRead(client_type, client, async_tcp_config, sData->response.toFill + sData->response.toFillIndex, sData->response.toFillLen);
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

    void clear(async_data *sData)
    {
        clear(sData->response.val[resns::header]);
        if (!sData->auth_used)
            clear(sData->response.val[resns::payload]);
        sData->response.flags.header_remaining = false;
        sData->response.flags.payload_remaining = false;
        sData->response.payloadRead = 0;
        sData->response.error.resp_code = 0;
        clear(sData->response.error.string);
        sData->response.chunkInfo.chunkSize = 0;
        sData->response.chunkInfo.dataLen = 0;
        sData->response.chunkInfo.phase = res_handler::READ_CHUNK_SIZE;
    }

    void reset(async_data *sData, bool disconnect)
    {
        if (disconnect)
            stop(sData);

        sData->response.httpCode = 0;
        sData->error.code = 0;
        sData->response.flags.reset();
        sData->state = astate_undefined;
        sData->return_type = ret_undefined;
        clear(sData->response.val[resns::etag]);
        sData->aResult.download_data.reset();
        sData->aResult.upload_data.reset();
        clear(sData);
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

        if (!isConnected() && !sData->auth_used) // This info is already show in auth task
            setDebugBase(app_debug, FPSTR("Connecting to server..."));

        if (!isConnected() && client_type == reqns::tcpc_sync)
            sData->return_type = client->connect(host, port) > 0 ? ret_complete : ret_failure;
        else if (client_type == reqns::tcpc_async)
        {

#if defined(ENABLE_ASYNC_TCP_CLIENT)
            if (async_tcp_config && async_tcp_config->tcpStatus && async_tcp_config->tcpConnect)
            {
                bool status = false;
                if (async_tcp_config->tcpStatus)
                    async_tcp_config->tcpStatus(status);

                if (!status)
                {
                    if (async_tcp_config->tcpConnect)
                        async_tcp_config->tcpConnect(host, port);

                    if (async_tcp_config->tcpStatus)
                        async_tcp_config->tcpStatus(status);
                }

                sData->return_type = status ? ret_complete : ret_continue;
            }
#endif
        }

        this->host = host;
        this->port = port;
        this->connected = sData->return_type == ret_complete;

        if (isConnected() && session_timeout_sec >= FIREBASE_SESSION_TIMEOUT_SEC)
            session_timer.feed(session_timeout_sec);

        return sData->return_type;
    }

    /**
     * Get the ethernet link status.
     * @return true for link up or false for link down.
     */
    bool ethLinkUp()
    {
        bool ret = false;

#if defined(ENABLE_ETHERNET_NETWORK)

#if defined(FIREBASE_LWIP_ETH_IS_AVAILABLE)

#if defined(ESP32)
        if (validIP(ETH.localIP()))
        {
            ETH.linkUp();
            ret = true;
        }
#elif defined(ESP8266) || defined(CORE_ARDUINO_PICO)

        if (!net.eth)
            return false;

#if defined(ESP8266) && defined(ESP8266_CORE_SDK_V3_X_X)

#if defined(INC_ENC28J60_LWIP)
        if (net.eth->enc28j60)
        {
            ret = net.eth->enc28j60->status() == WL_CONNECTED;
            goto ex;
        }
#endif
#if defined(INC_W5100_LWIP)
        if (net.eth->w5100)
        {
            ret = net.eth->w5100->status() == WL_CONNECTED;
            goto ex;
        }
#endif
#if defined(INC_W5500_LWIP)
        if (net.eth->w5500)
        {
            ret = net.eth->w5500->status() == WL_CONNECTED;
            goto ex;
        }
#endif

#elif defined(CORE_ARDUINO_PICO)

#endif

#endif

        return ret;

#if defined(INC_ENC28J60_LWIP) || defined(INC_W5100_LWIP) || defined(INC_W5500_LWIP)
    ex:
#endif

        // workaround for ESP8266 Ethernet
        delayMicroseconds(0);

        return ret;
#endif

#endif

        return ret;
    }

    /**
     * Checking for valid IP.
     * @return true for valid.
     */
    bool validIP(IPAddress ip)
    {
        char buf[16];
        sprintf(buf, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
        return strcmp(buf, "0.0.0.0") != 0;
    }

    function_return_type gprsConnect(async_data *sData)
    {

#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE)
        TinyGsm *gsmModem = (TinyGsm *)net.gsm.modem;

        if (gsmModem)
        {
            if (net.gsm.conn_status == network_config_data::gsm_conn_status_idle)
            {
                net.gsm.conn_status = network_config_data::gsm_conn_status_waits_network;

                gprsDisconnect();

                // Unlock your SIM card with a PIN if needed
                if (net.gsm.pin.length() && gsmModem->getSimStatus() != 3)
                    gsmModem->simUnlock(net.gsm.pin.c_str());

#if defined(TINY_GSM_MODEM_XBEE)
                // The XBee must run the gprsConnect function BEFORE waiting for network!
                gsmModem->gprsConnect(_apn.c_str(), _user.c_str(), _password.c_str());
#endif

                setDebugBase(app_debug, FPSTR("Waiting for network..."));
                return ret_continue;
            }
            else if (net.gsm.conn_status == network_config_data::gsm_conn_status_waits_network)
            {
                if (!gsmModem->waitForNetwork())
                {
                    setDebugBase(app_debug, FPSTR("Network connection failed"));
                    netErrState = 1;
                    net.network_status = false;
                    net.gsm.conn_status = network_config_data::gsm_conn_status_idle;
                    return ret_failure;
                }

                net.gsm.conn_status = network_config_data::gsm_conn_status_waits_gprs;

                setDebugBase(app_debug, FPSTR("Network connected"));

                if (gsmModem->isNetworkConnected())
                {
                    if (netErrState == 0)
                    {
                        String debug = FPSTR("Connecting to ");
                        debug += net.gsm.apn.c_str();
                        setDebugBase(app_debug, debug);
                    }
                }

                return ret_continue;
            }
            else if (net.gsm.conn_status == network_config_data::gsm_conn_status_waits_gprs)
            {
                if (gsmModem->isNetworkConnected())
                {

                    net.network_status = gsmModem->gprsConnect(net.gsm.apn.c_str(), net.gsm.user.c_str(), net.gsm.password.c_str()) && gsmModem->isGprsConnected();

                    if (netErrState == 0)
                        setDebugBase(app_debug, net.network_status ? FPSTR("GPRS/EPS connected") : FPSTR("GPRS/EPS connection failed"));

                    if (!net.network_status)
                        netErrState = 1;
                }
            }

            net.gsm.conn_status = network_config_data::gsm_conn_status_idle;

            return net.network_status ? ret_complete : ret_failure;
        }

#endif
        return ret_failure;
    }

    bool gprsConnected()
    {
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE)
        TinyGsm *gsmModem = (TinyGsm *)net.gsm.modem;
        net.network_status = gsmModem && gsmModem->isGprsConnected();
#endif
        return net.network_status;
    }

    bool gprsDisconnect()
    {
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE)
        TinyGsm *gsmModem = (TinyGsm *)net.gsm.modem;
        net.network_status = gsmModem && gsmModem->gprsDisconnect();
#endif
        return !net.network_status;
    }

    function_return_type ethernetConnect(async_data *sData)
    {

#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE) && defined(ENABLE_ETHERNET_NETWORK)

        if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_idle && net.ethernet.ethernet_cs_pin > -1)
            FIREBASE_ETHERNET_MODULE_CLASS_IMPL.init(net.ethernet.ethernet_cs_pin);

        if (net.ethernet.ethernet_reset_pin > -1)
        {
            if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_idle)
            {
                setDebugBase(app_debug, FPSTR("Resetting Ethernet Board..."));

                pinMode(net.ethernet.ethernet_reset_pin, OUTPUT);
                digitalWrite(net.ethernet.ethernet_reset_pin, HIGH);

                net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_rst_pin_unselected;
                net.ethernet.stobe_ms = millis();
            }
            else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_rst_pin_unselected && millis() - net.ethernet.stobe_ms > 200)
            {
                digitalWrite(net.ethernet.ethernet_reset_pin, LOW);
                net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_rst_pin_selected;
                net.ethernet.stobe_ms = millis();
            }
            else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_rst_pin_selected && millis() - net.ethernet.stobe_ms > 50)
            {
                digitalWrite(net.ethernet.ethernet_reset_pin, HIGH);
                net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_rst_pin_released;
                net.ethernet.stobe_ms = millis();
            }
            else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_rst_pin_released && millis() - net.ethernet.stobe_ms > 200)
            {
                net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_begin;
            }
        }
        else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_idle)
            net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_begin;

        if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_begin)
        {

            net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_waits;
            setDebugBase(app_debug, FPSTR("Starting Ethernet connection..."));

            if (validIP(net.ethernet.static_ip.ipAddress))
            {
                if (net.ethernet.static_ip.optional == false)
                    FIREBASE_ETHERNET_MODULE_CLASS_IMPL.begin(net.ethernet.ethernet_mac, net.ethernet.static_ip.ipAddress, net.ethernet.static_ip.dnsServer, net.ethernet.static_ip.defaultGateway, net.ethernet.static_ip.netMask);
                else if (!FIREBASE_ETHERNET_MODULE_CLASS_IMPL.begin(net.ethernet.ethernet_mac))
                {
                    FIREBASE_ETHERNET_MODULE_CLASS_IMPL.begin(net.ethernet.ethernet_mac, net.ethernet.static_ip.ipAddress, net.ethernet.static_ip.dnsServer, net.ethernet.static_ip.defaultGateway, net.ethernet.static_ip.netMask);
                }
            }
            else
                FIREBASE_ETHERNET_MODULE_CLASS_IMPL.begin(net.ethernet.ethernet_mac);

            net.eth_timer.feed(FIREBASE_ETHERNET_MODULE_TIMEOUT / 1000);
        }
        else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_waits)
        {

            if (FIREBASE_ETHERNET_MODULE_CLASS_IMPL.linkStatus() != LinkON && net.eth_timer.remaining() > 0)
                return ret_continue;

            net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_idle;

            bool ret = ethernetConnected();

            if (ret)
            {
                String msg = FPSTR("Connected, IP: ");
                msg += FIREBASE_ETHERNET_MODULE_CLASS_IMPL.localIP().toString();
                setDebugBase(app_debug, msg);
            }
            else
            {
                setDebugBase(app_debug, FPSTR("Can't connect to network"));
            }

            return ret ? ret_complete : ret_failure;
        }

#endif

        return ret_continue;
    }

    bool ethernetConnected()
    {
#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE)
#if defined(ENABLE_ETHERNET_NETWORK)
        if (net.ethernet.conn_satatus != network_config_data::ethernet_conn_status_waits)
            net.network_status = FIREBASE_ETHERNET_MODULE_CLASS_IMPL.linkStatus() == LinkON && validIP(FIREBASE_ETHERNET_MODULE_CLASS_IMPL.localIP());
#else
        net.network_status = FIREBASE_ETHERNET_MODULE_CLASS_IMPL.linkStatus() == LinkON && validIP(FIREBASE_ETHERNET_MODULE_CLASS_IMPL.localIP());
#endif
#endif
        return net.network_status;
    }

    function_return_type netConnect(async_data *sData)
    {
        if (!getNetworkStatus())
        {
            bool recon = net.reconnect;

            if (net.wifi && net.net_timer.feedCount() == 0)
                recon = true;

            // Self network connection controls.
            bool self_connect = net.network_type == firebase_network_gsm || net.network_type == firebase_network_ethernet;

            if (!self_connect && net.net_timer.remaining() == 0)
                net.net_timer.feed(FIREBASE_NET_RECONNECT_TIMEOUT_SEC);

            if (recon && (self_connect || net.net_timer.remaining() == 0))
            {

                if (net.network_type == firebase_network_generic)
                {
                    if (generic_network_owner_addr > 0 && generic_network_owner_addr != reinterpret_cast<uint32_t>(this))
                        return ret_continue;

                    if (generic_network_owner_addr == 0)
                        generic_network_owner_addr = reinterpret_cast<uint32_t>(this);

                    setDebugBase(app_debug, FPSTR("Reconnecting to network..."));

                    if (net.generic.net_con_cb)
                        net.generic.net_con_cb();

                    generic_network_owner_addr = 0;
                }
                else if (net.network_type == firebase_network_gsm)
                {
                    if (gsm_network_owner_addr > 0 && gsm_network_owner_addr != reinterpret_cast<uint32_t>(this))
                        return ret_continue;

                    if (gsm_network_owner_addr == 0)
                        gsm_network_owner_addr = reinterpret_cast<uint32_t>(this);

                    if (gprsConnect(sData) == ret_continue)
                        return ret_continue;

                    gsm_network_owner_addr = 0;
                }
                else if (net.network_type == firebase_network_ethernet)
                {
                    if (ethernet_network_owner_addr > 0 && ethernet_network_owner_addr != reinterpret_cast<uint32_t>(this))
                        return ret_continue;

                    if (ethernet_network_owner_addr == 0)
                        ethernet_network_owner_addr = reinterpret_cast<uint32_t>(this);

                    if (ethernetConnect(sData) == ret_continue)
                        return ret_continue;

                    ethernet_network_owner_addr = 0;
                }
                else if (net.network_type == firebase_network_default)
                {

                    if (wifi_reconnection_ms == 0 || (wifi_reconnection_ms > 0 && millis() - wifi_reconnection_ms > FIREBASE_NET_RECONNECT_TIMEOUT_SEC * 1000))
                    {
                        wifi_reconnection_ms = millis();

                        setDebugBase(app_debug, FPSTR("Reconnecting to WiFi network..."));

#if defined(FIREBASE_WIFI_IS_AVAILABLE)
#if defined(ESP32) || defined(ESP8266)

                        if (net.wifi && net.wifi->credentials.size())
                            net.wifi->reconnect();
                        else
                            WiFi.reconnect();
#else
                        if (net.wifi && net.wifi->credentials.size())
                            net.wifi->reconnect();
#endif
#endif
                    }
                }
            }
        }

        return getNetworkStatus() ? ret_complete : ret_failure;
    }

    bool getNetworkStatus()
    {
        bool net_status = net.network_status;

        // We will not invoke the network status request when device has built-in WiFi or Ethernet and it is connected.
        if (net.network_type == firebase_network_gsm)
        {
            net.network_status = gprsConnected();
        }
        else if (net.network_type == firebase_network_ethernet)
        {
            net.network_status = ethernetConnected();
        }
        // also check the native network before calling external cb
        else if (net.network_type == firebase_network_default || WIFI_CONNECTED || ethLinkUp() || PPP_CONNECTED)
            net.network_status = WIFI_CONNECTED || ethLinkUp() || PPP_CONNECTED;
        else if (net.network_type == firebase_network_generic)
        {
            if (!net.generic.net_status_cb)
                netErrState = 1;
            else
                net.generic.net_status_cb(net.network_status);
        }
        else
            net.network_status = false;

        if (net_status && !net.network_status)
            net.disconnected_ms = millis() - 10;

        return net.network_status;
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

    void setContentType(async_data *sData, const String &type)
    {
        sData->request.addContentTypeHeader(type.c_str());
    }

    void setFileContentLength(async_data *sData, int headerLen = 0, const String &customHeader = "")
    {
        size_t sz = 0;

#if defined(ENABLE_FS)
        if (sData->request.file_data.cb && sData->request.file_data.filename.length())
        {
            sData->request.file_data.cb(sData->request.file_data.file, sData->request.file_data.filename.c_str(), file_mode_open_read);
            sz = sData->request.file_data.file.size();
        }
#endif
        if (sData->request.file_data.data_size && sData->request.file_data.data)
            sz = sData->request.file_data.data_size;

        if (sz > 0)
        {
            sData->request.file_data.file_size = sData->request.base64 ? 2 + b64ut.getBase64Len(sz) : sz;
            if (customHeader.length())
            {
                sData->request.val[reqns::header] += customHeader;
                sData->request.val[reqns::header] += ":";
                sData->request.val[reqns::header] += sData->request.file_data.file_size + headerLen;
                sData->request.val[reqns::header] += "\r\n";
            }
            else
                setContentLength(sData, sData->request.file_data.file_size);

            closeFile(sData);
        }
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
                // In case stream time out.
                // We have to clear app data (available), reset the last error and set the Stream time out error.
                sData->aResult.lastError.reset();
                clearAppData(sData->aResult.app_data);
                setEventResumeStatus(&sData->aResult.rtdbResult, event_resume_status_resuming);
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
            // In case TCP write error.
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
            // In case TCP read error.
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

    String getHost(async_data *sData, bool fromReq, String *ext = nullptr)
    {
#if defined(ENABLE_CLOUD_STORAGE)
        String url = fromReq ? sData->request.val[reqns::url] : sData->request.file_data.resumable.getLocation();
#else
        String url = fromReq ? sData->request.val[reqns::url] : sData->response.val[resns::location];
#endif
        URLUtil uut;
        return uut.getHost(url, ext);
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

    bool isConnected()
    {
        // Re-check the client status only when the connected flag was set.
        // This prevents calling client's connected() if it was not yet connected.
        // This also prevents ESP32 Arduino Core v3.x.x's WiFiClientSecure connected() method
        // (error) warning when it was trying to read 0 byte from, and set the socket option to
        // the unopen socket.
        if (this->connected)
        {
            if (client_type == reqns::tcpc_sync)
                this->connected = client && client->connected();
            else if (client_type == reqns::tcpc_async)
            {
#if defined(ENABLE_ASYNC_TCP_CLIENT)
                if (async_tcp_config && async_tcp_config->tcpStatus)
                    async_tcp_config->tcpStatus(this->connected);
#endif
            }
        }
        return this->connected;
    }

    void stop(async_data *sData)
    {
        if (isConnected())
            setDebugBase(app_debug, FPSTR("Terminating the server connection..."));

        if (client_type == reqns::tcpc_sync)
        {
            if (client)
                client->stop();
        }
        else
        {
#if defined(ENABLE_ASYNC_TCP_CLIENT)
            if (async_tcp_config && async_tcp_config->tcpStop)
                async_tcp_config->tcpStop();
#endif
        }

        clear(host);
        this->port = 0;
        this->connected = false;
        client_changed = false;
        network_changed = false;
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
        sData->request.addHostHeader(getHost(sData, true).c_str());

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

    void setContentLength(async_data *sData, size_t len)
    {
        if (sData->request.method == reqns::http_post || sData->request.method == reqns::http_put || sData->request.method == reqns::http_patch)
        {
            sData->request.addContentLengthHeader(len);
            sData->request.addNewLine();
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
        closeFile(sData);
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
                // In case TCP (network) disconnected error.
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
            if ((sData->sse && sData->auth_ts != auth_ts) || client_changed || network_changed)
            {
                stop(sData);
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

                if (sData->async && !sData->response.tcpAvailable(client_type, client, async_tcp_config))
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
                    while (!sData->response.tcpAvailable(client_type, client, async_tcp_config) && networkConnect(sData) == ret_complete)
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
        client_type = reqns::tcpc_sync;
    }

    AsyncClientClass(Client &client, network_config_data &net) : client(&client)
    {
        this->net.copy(net);
        this->addr = reinterpret_cast<uint32_t>(this);
        client_type = reqns::tcpc_sync;
    }

#if defined(ENABLE_ASYNC_TCP_CLIENT)
    AsyncClientClass(AsyncTCPConfig &tcpClientConfig, network_config_data &net) : async_tcp_config(&tcpClientConfig)
    {
        this->net.copy(net);
        this->addr = reinterpret_cast<uint32_t>(this);
        client_type = reqns::tcpc_async;
    }
#endif

    ~AsyncClientClass()
    {
        stop(nullptr);

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
    bool networkStatus() { return getNetworkStatus(); }

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
    unsigned long networkLastSeen() { return this->net.disconnected_ms; }

    /**
     * Return the current network type enum.
     *
     * @return firebase_network_type The firebase_network_type enums are firebase_network_default, firebase_network_generic, firebase_network_ethernet and firebase_network_gsm.
     */
    firebase_network_type getNetworkType() { return this->net.network_type; }

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
        client_changed = reinterpret_cast<uint32_t>(&client) != reinterpret_cast<uint32_t>(this->client);
        network_changed = true;

        // Some changes, stop the current network client.
        if (client_changed && this->client)
        {
            this->client->stop();
            this->connected = false;
        }

        // Change the network interface.
        // Should not check the type changes, just overwrite
        this->net.copy(net);

        // Change the client.
        if (client_changed)
            this->client = &client;
    }
};

#endif