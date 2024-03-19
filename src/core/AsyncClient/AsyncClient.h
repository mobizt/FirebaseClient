/**
 * Created March 18, 2024
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
#ifndef ASYNC_CLIENT_H
#define ASYNC_CLIENT_H
#include <vector>
#include "./core/AsyncClient/RequestHandler.h"
#include "./core/AsyncClient/ResponseHandler.h"
#include "./core/NetConfig.h"
#include "./core/Memory.h"
#include "./core/FileConfig.h"
#include "./core/Base64.h"
#include "./core/Error.h"
#include "./core/OTA.h"
#include "./core/AsyncResult/AsyncResult.h"
#include "./core/AuthConfig.h"
#include "./core/List.h"
#include "./core/Core.h"
#include "./core/URL.h"

#if defined(ENABLE_ASYNC_TCP_CLIENT)
#include "./core/AsyncTCPConfig.h"
#endif

using namespace firebase;

__attribute__((unused)) static void meminfo(int pos)
{
#if defined(ESP8266)
    Serial.printf("Pos: %d, Heap: %d, Stack: %d\n", pos, ESP.getFreeHeap(), ESP.getFreeContStack());
#elif defined(ESP32)
    Serial.printf("Pos: %d, Heap: %d\n", pos, ESP.getFreeHeap());
#endif
}

enum async_state
{
    async_state_undefined,
    async_state_send_header,
    async_state_send_payload,
    async_state_read_response,
    async_state_complete
};

enum function_return_type
{
    function_return_type_undefined = -2,
    function_return_type_failure = -1,
    function_return_type_continue = 0,
    function_return_type_complete = 1,
    function_return_type_retry = 2
};

struct async_data_item_t
{
    friend class FirebaseApp;

public:
    struct async_error_t
    {
        async_state state = async_state_undefined;
        int code = 0;
    };

    async_state state = async_state_undefined;
    function_return_type return_type = function_return_type_undefined;
    async_request_handler_t request;
    async_response_handler_t response;
    async_error_t error;
    bool to_remove = false;
    bool auth_used = false;
    bool complete = false;
    bool async = false;
    bool cancel = false;
    bool sse = false;
    bool path_not_existed = false;
    bool download = false;
    bool upload_progress_enabled = false;
    bool upload = false;
    uint32_t addr = 0;
    AsyncResult aResult;
    AsyncResult *refResult = nullptr;
    uint32_t ref_result_addr = 0;
    AsyncResultCallback cb = NULL;
    Timer err_timer;
    async_data_item_t()
    {
        addr = reinterpret_cast<uint32_t>(this);
        err_timer.feed(0);
    }

    void setRefResult(AsyncResult *refResult)
    {
        this->refResult = refResult;
        ref_result_addr = refResult->addr;
    }

    void reset()
    {
        state = async_state_undefined;
        return_type = function_return_type_undefined;
        request.clear();
        response.clear();
        error.code = 0;
        error.state = async_state_undefined;
        to_remove = false;
        auth_used = false;
        complete = false;
        async = false;
        cancel = false;
        sse = false;
        path_not_existed = false;
        cb = NULL;
        err_timer.reset();
    }
};

struct slot_options_t
{
public:
    bool auth_used = false;
    bool sse = false;
    bool async = false;
    bool sv = false;
    bool ota = false;
    bool no_etag = false;
    bool auth_param = false;
    app_token_t *app_token = nullptr;
    slot_options_t() {}
    slot_options_t(bool auth_used, bool sse, bool async, bool sv, bool ota, bool no_etag, bool auth_param = false)
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

class AsyncClientClass
{
    friend class FirebaseApp;
    friend class AuthRequest;
    friend class RealtimeDatabase;
    friend class FirestoreDatabase;
    friend class FirestoreDocuments;
    friend class CloudFunctions;
    friend class Messaging;
    friend class Storage;
    friend class CloudStorage;

private:
    FirebaseError lastErr;
    String header, reqEtag, resETag;
    int netErrState = 0;
    Client *client = nullptr;
#if defined(ENABLE_ASYNC_TCP_CLIENT)
    AsyncTCPConfig *async_tcp_config = nullptr;
#else
    void *async_tcp_config = nullptr;
#endif
    async_request_handler_t::tcp_client_type client_type = async_request_handler_t::tcp_client_type_sync;
    bool sse = false;
    std::vector<uint32_t> sVec;
    Memory mem;
    Base64Helper bh;
    network_config_data net;
    uint32_t addr = 0;
    bool inProcess = false;
    bool inStopAsync = false;

    void closeFile(async_data_item_t *sData)
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

    bool openFile(async_data_item_t *sData, file_operating_mode mode)
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

    function_return_type sendHeader(async_data_item_t *sData, const char *data)
    {
        return send(sData, (uint8_t *)data, data ? strlen(data) : 0, data ? strlen(data) : 0, async_state_send_header);
    }

    function_return_type sendHeader(async_data_item_t *sData, uint8_t *data, size_t len)
    {
        return send(sData, data, len, len, async_state_send_header);
    }

    function_return_type sendBuff(async_data_item_t *sData, async_state state = async_state_send_payload)
    {
        function_return_type ret = function_return_type_continue;

#if defined(ENABLE_FS)

        size_t totalLen = sData->request.file_data.file_size;
        bool fopen = sData->request.payloadIndex == 0;

#if defined(ENABLE_CLOUD_STORAGE)

        if (sData->request.file_data.multipart.isEnabled())
        {
            totalLen += sData->request.file_data.multipart.getOptions().length() + sData->request.file_data.multipart.getLast().length();
            if (sData->request.file_data.multipart.isEnabled() && sData->request.file_data.multipart.getState() == file_upload_multipart_data::multipart_state_send_options_payload)
                return send(sData, (uint8_t *)sData->request.file_data.multipart.getOptions().c_str(), sData->request.file_data.multipart.getOptions().length(), totalLen, async_state_send_payload);
            else if (sData->request.file_data.multipart.isEnabled() && sData->request.file_data.multipart.getState() == file_upload_multipart_data::multipart_state_send_last_payload)
                return send(sData, (uint8_t *)sData->request.file_data.multipart.getLast().c_str(), sData->request.file_data.multipart.getLast().length(), totalLen, async_state_send_payload);

            fopen |= sData->request.file_data.multipart.isEnabled() && sData->request.payloadIndex == sData->request.file_data.multipart.getOptions().length();
        }
#endif
        if (sData->upload)
            sData->upload_progress_enabled = true;

        if (fopen)
        {
            if (sData->request.file_data.filename.length() > 0)
            {
                if (sData->request.file_data.file_status == file_config_data::file_status_closed)
                {
                    if (!openFile(sData, file_mode_open_read))
                    {
                        setAsyncError(sData, state, FIREBASE_ERROR_OPEN_FILE, !sData->sse, true);
                        return function_return_type_failure;
                    }
                }
            }

            if (sData->request.base64)
            {
                ret = send(sData, (uint8_t *)"\"", 1, totalLen, async_state_send_payload);
                if (ret != function_return_type_continue)
                    return ret;
            }
        }

        uint8_t *buf = nullptr;
        int toSend = 0;
        if (sData->request.file_data.filename.length() > 0 ? sData->request.file_data.file.available() : sData->request.file_data.data_pos < sData->request.file_data.data_size)
        {
            if (sData->request.base64)
            {

                toSend = FIREBASE_BASE64_CHUNK_SIZE;

                if (sData->request.file_data.filename.length() > 0)
                {
                    if (sData->request.file_data.file.available() < toSend)
                        toSend = sData->request.file_data.file.available();
                }
                else
                {
                    if ((int)(sData->request.file_data.data_size - sData->request.file_data.data_pos) < toSend)
                        toSend = sData->request.file_data.data_size - sData->request.file_data.data_pos;
                }

                buf = reinterpret_cast<uint8_t *>(mem.alloc(toSend));
                if (sData->request.file_data.filename.length() > 0)
                {
                    toSend = sData->request.file_data.file.read(buf, toSend);
                    if (toSend == 0)
                    {
                        setAsyncError(sData, state, FIREBASE_ERROR_FILE_READ, !sData->sse, true);
                        ret = function_return_type_failure;
                        goto exit;
                    }
                }
                else if (sData->request.file_data.data)
                {
                    memcpy(buf, sData->request.file_data.data + sData->request.file_data.data_pos, toSend);
                    sData->request.file_data.data_pos += toSend;
                }

                uint8_t *temp = (uint8_t *)bh.encodeToChars(mem, buf, toSend);
                mem.release(&buf);
                toSend = strlen((char *)temp);
                buf = temp;
            }
            else
            {
#if defined(ENABLE_CLOUD_STORAGE)
                if (sData->request.file_data.resumable.isEnabled() && sData->request.file_data.resumable.isUpload())
                    toSend = sData->request.file_data.resumable.getChunkSize(totalLen, sData->request.payloadIndex, sData->request.file_data.data_pos);
                else if (sData->request.file_data.multipart.isEnabled() && sData->request.file_data.multipart.getState() == file_upload_multipart_data::multipart_state_send_data_payload)
                    toSend = sData->request.file_data.multipart.getChunkSize(totalLen, sData->request.payloadIndex, sData->request.file_data.data_pos);
                else
#endif
                    toSend = totalLen - sData->request.file_data.data_pos < FIREBASE_CHUNK_SIZE ? totalLen - sData->request.file_data.data_pos : FIREBASE_CHUNK_SIZE;

                buf = reinterpret_cast<uint8_t *>(mem.alloc(toSend));

                if (sData->request.file_data.filename.length() > 0)
                {
                    toSend = sData->request.file_data.file.read(buf, toSend);
                    if (toSend == 0)
                    {
                        setAsyncError(sData, state, FIREBASE_ERROR_FILE_READ, !sData->sse, true);
                        ret = function_return_type_failure;
                        goto exit;
                    }
                }
                else if (sData->request.file_data.data)
                {
                    memcpy(buf, sData->request.file_data.data + sData->request.file_data.data_pos, toSend);
                }

                sData->request.file_data.data_pos += toSend;
            }

            ret = send(sData, buf, toSend, totalLen, async_state_send_payload);
        }
        else if (sData->request.base64)
            ret = send(sData, (uint8_t *)"\"", 1, totalLen, async_state_send_payload);

    exit:

        if (buf)
            mem.release(&buf);
#endif

        return ret;
    }

    function_return_type send(async_data_item_t *sData, const char *data, async_state state = async_state_send_payload)
    {
        return send(sData, (uint8_t *)data, data ? strlen(data) : 0, data ? strlen(data) : 0, state);
    }

    function_return_type send(async_data_item_t *sData, uint8_t *data, size_t len, size_t size, async_state state = async_state_send_payload)
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

#if defined(ENABLE_FS)
                if (sData->upload && sData->upload_progress_enabled && sData->request.file_data.file_size)
                {
                    sData->aResult.upload_data.total = size;
                    sData->aResult.upload_data.uploaded = sData->request.payloadIndex;
                    returnResult(sData, false);
                }
#endif

                if (sData->request.dataIndex == len)
                    sData->request.dataIndex = 0;

#if defined(ENABLE_FS) && defined(ENABLE_CLOUD_STORAGE)

                if (sData->request.file_data.resumable.isEnabled() && sData->request.file_data.resumable.isUpload())
                {
                    int ret = sData->request.file_data.resumable.isComplete(size, sData->request.payloadIndex);
                    if (ret == 2)
                    {
                        sData->state = async_state_read_response;
                        sData->return_type = function_return_type_complete;
                        sData->request.dataIndex = 0;
                        sData->request.payloadIndex = 0;
                        sData->response.clear();
                        return sData->return_type;
                    }
                    else if (ret == 1)
                    {
                        sData->return_type = function_return_type_continue;
                        return sData->return_type;
                    }
                }
                else if (sData->request.file_data.multipart.isEnabled() && sData->request.file_data.multipart.isUpload())
                {
                    sData->request.file_data.multipart.updateState(sData->request.payloadIndex);
                    if (sData->request.file_data.multipart.isUpload())
                    {
                        sData->return_type = function_return_type_continue;
                        return sData->return_type;
                    }
                }
                else if (sData->request.payloadIndex < size)
                {
                    sData->return_type = function_return_type_continue;
                    return sData->return_type;
                }

#else
                if (sData->request.payloadIndex < size)
                {
                    sData->return_type = function_return_type_continue;
                    return sData->return_type;
                }
#endif
            }
        }

        sData->return_type = sData->request.payloadIndex == size && size > 0 ? function_return_type_complete : function_return_type_failure;

        if (sData->return_type == function_return_type_failure)
            setAsyncError(sData, state, FIREBASE_ERROR_TCP_SEND, !sData->sse, false);

        sData->request.payloadIndex = 0;
        sData->request.dataIndex = 0;
        sData->request.file_data.data_pos = 0;

        if (sData->return_type == function_return_type_complete)
        {
            if (state == async_state_send_header)
            {
                if (sData->request.val[req_hndlr_ns::header].indexOf("Content-Length: 0\r\n") > -1)
                    sData->state = async_state_read_response;
                else
                    sData->state = async_state_send_payload;
            }
            else if (state == async_state_send_payload)
                sData->state = async_state_read_response;

#if defined(ENABLE_FS) && defined(ENABLE_CLOUD_STORAGE)
            if (sData->upload)
            {
                if (sData->request.file_data.resumable.isEnabled())
                    sData->request.file_data.resumable.updateState(sData->request.payloadIndex);
                else if (sData->request.file_data.multipart.isEnabled())
                    sData->request.file_data.multipart.updateState(sData->request.payloadIndex);
            }
#endif

            if (sData->state != async_state_read_response)
                sData->response.clear();
        }

        return sData->return_type;
    }

    function_return_type send(async_data_item_t *sData)
    {
        if (!sData || !netConnect(sData))
        {
            setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_DISCONNECTED, !sData->sse, false);
            return function_return_type_failure;
        }

        function_return_type ret = function_return_type_continue;

        if (sData->state == async_state_undefined || sData->state == async_state_send_header)
        {
            if ((sse && !sData->sse) || (!sse && sData->sse) || (sData->auth_used && sData->state == async_state_undefined))
                stop(sData);

            if ((client_type == async_request_handler_t::tcp_client_type_sync && !client->connected()) || client_type == async_request_handler_t::tcp_client_type_async)
            {
                ret = connect(sData, getHost(sData, true).c_str(), sData->request.port);

                // allow non-blocking async tcp connection
                if (ret == function_return_type_continue)
                    return ret;

                if (ret != function_return_type_complete)
                    return connErrorHandler(sData, sData->state);

                sse = sData->sse;
            }

            if (sData->upload)
                sData->upload_progress_enabled = false;

            if (sData->request.app_token && sData->request.app_token->auth_data_type != user_auth_data_no_token)
            {
                if (sData->request.app_token->val[app_tk_ns::token].length() == 0)
                {
                    setAsyncError(sData, sData->state, FIREBASE_ERROR_UNAUTHENTICATE, !sData->sse, false);
                    return function_return_type_failure;
                }

                header = sData->request.val[req_hndlr_ns::header];
                header.replace(FIREBASE_AUTH_PLACEHOLDER, sData->request.app_token->val[app_tk_ns::token]);
                ret = sendHeader(sData, header.c_str());
                header.remove(0, header.length());
                return ret;
            }
            return sendHeader(sData, sData->request.val[req_hndlr_ns::header].c_str());
        }
        else if (sData->state == async_state_send_payload)
        {
            if (sData->upload)
                sData->upload_progress_enabled = true;

            if (sData->request.method == async_request_handler_t::http_get || sData->request.method == async_request_handler_t::http_delete)
                sData->state = async_state_read_response;
            else
            {
                if (sData->request.val[req_hndlr_ns::payload].length())
                    ret = send(sData, sData->request.val[req_hndlr_ns::payload].c_str());
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

    function_return_type receive(async_data_item_t *sData)
    {

        if (!sData || !netConnect(sData))
        {
            setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_DISCONNECTED, !sData->sse, false);
            return function_return_type_failure;
        }

        if (!readResponse(sData))
        {
            setAsyncError(sData, sData->state, sData->response.httpCode > 0 ? sData->response.httpCode : FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT, !sData->sse, false);
            return function_return_type_failure;
        }

        if (sData->response.httpCode == 0)
            return function_return_type_continue;

#if defined(ENABLE_FS) && defined(ENABLE_CLOUD_STORAGE)

        if (sData->request.file_data.resumable.isEnabled() && sData->request.file_data.resumable.getLocation().length() && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining)
        {
            String ext;
            String host = getHost(sData, false, &ext);

            if (connect(sData, host.c_str(), sData->request.port) > function_return_type_failure)
            {
                sData->request.val[req_hndlr_ns::payload].remove(0, sData->request.val[req_hndlr_ns::payload].length());
                sData->request.file_data.resumable.getHeader(sData->request.val[req_hndlr_ns::header], host, ext);
                sData->state = async_state_send_header;
                sData->request.file_data.resumable.setHeaderState();
                return function_return_type_continue;
            }

            return connErrorHandler(sData, sData->state);
        }

#else
        if (sData->response.val[res_hndlr_ns::location].length() && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining)
        {
            String ext;
            String host = getHost(sData, false, &ext);
            if (client)
                client->stop();
            if (connect(sData, host.c_str(), sData->request.port) > function_return_type_failure)
            {
                URLHelper uh;
                uh.relocate(sData->request.val[req_hndlr_ns::header], host, ext);
                sData->request.val[req_hndlr_ns::payload].remove(0, sData->request.val[req_hndlr_ns::payload].length());
                sData->state = async_state_send_header;
                return function_return_type_continue;
            }

            return connErrorHandler(sData, sData->state);
        }

#endif

        if (!sData->sse && sData->response.httpCode > 0 && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining)
        {
            sData->state = async_state_undefined;
            return function_return_type_complete;
        }

        return function_return_type_continue;
    }

    function_return_type connErrorHandler(async_data_item_t *sData, async_state state)
    {
        setAsyncError(sData, state, FIREBASE_ERROR_TCP_CONNECTION, !sData->sse, false);
        return function_return_type_failure;
    }

    void setAsyncError(async_data_item_t *sData, async_state state, int code, bool toRemove, bool toCloseFile)
    {
        sData->error.state = state;
        sData->error.code = code;

        if (toRemove)
            sData->to_remove = toRemove;

        if (toCloseFile)
            closeFile(sData);

        setLastError(sData);
    }

    async_data_item_t *getData(uint8_t slot)
    {
        if (slot < sVec.size())
            return reinterpret_cast<async_data_item_t *>(sVec[slot]);
        return nullptr;
    }

    async_data_item_t *addSlot(int index = -1)
    {
        async_data_item_t *sData = new async_data_item_t();
        if (index > -1)
            sVec.insert(sVec.begin() + index, sData->addr);
        else
            sVec.push_back(sData->addr);

        return sData;
    }

    void returnResult(async_data_item_t *sData, bool setData)
    {

        bool error_notify_timeout = false;
        if (sData->err_timer.remaining() == 0)
        {
            sData->err_timer.feed(5000);
            error_notify_timeout = true;
        }

        bool download_status = sData->download && sData->aResult.setDownloadProgress();
        bool upload_status = sData->upload && sData->upload_progress_enabled && sData->aResult.setUploadProgress();

        if (sData->refResult)
        {
            if (setData || error_notify_timeout || download_status || upload_status)
            {
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

        if (sData->cb && (setData || error_notify_timeout || download_status || upload_status))
        {
            if (!sData->auth_used)
                sData->cb(sData->aResult);
        }
    }

    void setLastError(async_data_item_t *sData)
    {
        if (!sData->aResult.error_available)
        {
            if (sData->error.code < 0)
            {
                sData->aResult.lastError.setClientError(sData->error.code);
                lastErr.setClientError(sData->error.code);
                sData->aResult.error_available = true;
                sData->aResult.data_available = false;
            }
            else if (sData->response.httpCode > 0 && sData->response.httpCode >= FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST)
            {
                sData->aResult.lastError.setResponseError(sData->response.val[res_hndlr_ns::payload], sData->response.httpCode);
                lastErr.setResponseError(sData->response.val[res_hndlr_ns::payload], sData->response.httpCode);
                sData->aResult.error_available = true;
                sData->aResult.data_available = false;
            }
        }
    }

    int readLine(async_data_item_t *sData, String &buf)
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

    void clear(String &str) { str.remove(0, str.length()); }

    bool readResponse(async_data_item_t *sData)
    {
        if (!netConnect(sData) || !client || !sData)
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

                    if (sData->response.flags.sse || !sData->response.flags.payload_remaining)
                    {
                        if (!sData->auth_used)
                        {
                            sData->aResult.setPayload(sData->response.val[res_hndlr_ns::payload]);

                            if (sData->aResult.download_data.total > 0)
                                sData->aResult.data_available = false;
#if defined(ENABLE_DATABASE)

                            if (sData->request.method == async_request_handler_t::http_post)
                                sData->aResult.rtdbResult.parseNodeName();

                            // data available from sse event
                            if (sData->response.flags.sse && sData->response.val[res_hndlr_ns::payload].length())
                            {
                                // order of checking: event, data, newline
                                if (sData->response.val[res_hndlr_ns::payload].indexOf("event: ") > -1 && sData->response.val[res_hndlr_ns::payload].indexOf("data: ") > -1 && sData->response.val[res_hndlr_ns::payload].indexOf("\n") > -1)
                                {
                                    // save payload to slot result
                                    sData->aResult.setPayload(sData->response.val[res_hndlr_ns::payload]);
                                    clear(sData->response.val[res_hndlr_ns::payload]);
                                    sData->aResult.rtdbResult.parseSSE();
                                    sData->response.flags.payload_available = true;
                                    returnResult(sData, true);
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

    bool readStatusLine(async_data_item_t *sData)
    {
        if (sData->response.httpCode > 0)
            return false;

        sData->response.val[res_hndlr_ns::header].reserve(1024);

        // the first chunk (line) can be http response status or already connected stream payload
        readLine(sData, sData->response.val[res_hndlr_ns::header]);
        int status = getStatusCode(sData->response.val[res_hndlr_ns::header]);
        if (status > 0)
        {
            // http response status
            sData->response.flags.header_remaining = true;
            sData->response.httpCode = status;
        }
        return true;
    }

    void readHeader(async_data_item_t *sData)
    {
        if (sData->response.flags.header_remaining)
        {
            int read = readLine(sData, sData->response.val[res_hndlr_ns::header]);
            if ((read == 1 && sData->response.val[res_hndlr_ns::header][sData->response.val[res_hndlr_ns::header].length() - 1] == '\r') ||
                (read == 2 && sData->response.val[res_hndlr_ns::header][sData->response.val[res_hndlr_ns::header].length() - 2] == '\r' && sData->response.val[res_hndlr_ns::header][sData->response.val[res_hndlr_ns::header].length() - 1] == '\n'))
            {
                clear(sData->response.val[res_hndlr_ns::etag]);
                String temp[5];
#if defined(ENABLE_FS) && defined(ENABLE_CLOUD_STORAGE)
                if (sData->upload)
                    parseRespHeader(sData, sData->response.val[res_hndlr_ns::header], sData->request.file_data.resumable.getLocationRef(), "Location");
#else
                parseRespHeader(sData, sData->response.val[res_hndlr_ns::header], sData->response.val[res_hndlr_ns::location], "Location");

#endif
                parseRespHeader(sData, sData->response.val[res_hndlr_ns::header], sData->response.val[res_hndlr_ns::etag], "ETag");
                resETag = sData->response.val[res_hndlr_ns::etag];
                sData->aResult.val[ares_ns::res_etag] = sData->response.val[res_hndlr_ns::etag];
                sData->aResult.val[ares_ns::data_path] = sData->request.val[req_hndlr_ns::path];
#if defined(ENABLE_DATABASE)
                sData->aResult.rtdbResult.null_etag = sData->response.val[res_hndlr_ns::etag].indexOf("null_etag") > -1;
#endif

                parseRespHeader(sData, sData->response.val[res_hndlr_ns::header], temp[0], "Content-Length");

                sData->response.payloadLen = atoi(temp[0].c_str());

                parseRespHeader(sData, sData->response.val[res_hndlr_ns::header], temp[1], "Connection");
                sData->response.flags.keep_alive = temp[1].length() && temp[1].indexOf("keep-alive") > -1;

                parseRespHeader(sData, sData->response.val[res_hndlr_ns::header], temp[2], "Transfer-Encoding");
                sData->response.flags.chunks = temp[2].length() && temp[2].indexOf("chunked") > -1;

                parseRespHeader(sData, sData->response.val[res_hndlr_ns::header], temp[3], "Content-Type");
                sData->response.flags.sse = temp[3].length() && temp[3].indexOf("text/event-stream") > -1;

                if (sData->upload)
                    parseRespHeader(sData, sData->response.val[res_hndlr_ns::header], temp[4], "Range");

                clear(sData);

#if defined(ENABLE_FS) && defined(ENABLE_CLOUD_STORAGE)
                if (sData->upload && sData->request.file_data.resumable.isEnabled())
                {
                    sData->request.file_data.resumable.setHeaderState();
                    if (sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT && temp[4].indexOf("bytes=") > -1)
                        sData->request.file_data.resumable.updateRange();
                }
#endif
                for (size_t i = 0; i < 5; i++)
                    temp[i].remove(0, temp[i].length());

                if (sData->response.httpCode > 0 && sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
                    sData->response.flags.payload_remaining = true;

                if (!sData->sse && (sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_OK || sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_PERMANENT_REDIRECT || sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_NO_CONTENT) && !sData->response.flags.chunks && sData->response.payloadLen == 0)
                    sData->response.flags.payload_remaining = false;

                if (sData->request.method == async_request_handler_t::http_delete && sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
                    sData->aResult.setDebug(FPSTR("Delete operation complete"));
            }
        }
    }

    void parseRespHeader(async_data_item_t *sData, const String &src, String &out, const char *header)
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

    int getChunkSize(async_data_item_t *sData, Client *client)
    {
        String line;
        readLine(sData, line);
        int p = line.indexOf(";");
        if (p == -1)
            p = line.indexOf("\r\n");
        if (p != -1)
            sData->response.chunkInfo.chunkSize = hex2int(line.substring(0, p).c_str());

        return sData->response.chunkInfo.chunkSize;
    }

    // Returns -1 when complete
    int decodeChunks(async_data_item_t *sData, Client *client, String *out)
    {
        if (!client || !sData || !out)
            return 0;
        int res = 0;

        // read chunk-size, chunk-extension (if any) and CRLF
        if (sData->response.chunkInfo.phase == async_response_handler_t::READ_CHUNK_SIZE)
        {
            sData->response.chunkInfo.phase = async_response_handler_t::READ_CHUNK_DATA;
            sData->response.chunkInfo.chunkSize = -1;
            sData->response.chunkInfo.dataLen = 0;
            res = getChunkSize(sData, client);
            sData->response.payloadLen += res > -1 ? res : 0;
        }
        // read chunk-data and CRLF
        // append chunk-data to entity-body
        else
        {
            if (sData->response.chunkInfo.chunkSize > -1)
            {
                String chunk;
                int read = readLine(sData, chunk);
                if (read && chunk[0] != '\r')
                    *out += chunk;
                if (read)
                {
                    sData->response.chunkInfo.dataLen += read;
                    sData->response.payloadRead += read;
                    // chunk may contain trailing
                    if (sData->response.chunkInfo.dataLen - 2 >= sData->response.chunkInfo.chunkSize)
                    {
                        sData->response.chunkInfo.dataLen = sData->response.chunkInfo.chunkSize;
                        sData->response.chunkInfo.phase = async_response_handler_t::READ_CHUNK_SIZE;
                    }
                }
                // if all chunks read, returns -1
                else if (sData->response.chunkInfo.dataLen == sData->response.chunkInfo.chunkSize)
                    res = -1;
            }
        }

        return res;
    }

    bool readPayload(async_data_item_t *sData)
    {
        uint8_t *buf = nullptr;
        OtaHelper oh;
        Memory mem;
        Base64Helper bh;

        if (sData->response.flags.payload_remaining)
        {
            sData->response.feedTimer();

            // the next chunk data is the payload
            if (sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
            {

                if (sData->response.flags.chunks)
                {
                    if (decodeChunks(sData, client, &sData->response.val[res_hndlr_ns::payload]) == -1)
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
                                    oh.prepareDownloadOTA(sData->response.payloadLen, sData->request.base64, sData->request.ota_error);
                                    if (sData->request.ota_error != 0)
                                    {
                                        setAsyncError(sData, async_state_read_response, sData->request.ota_error, !sData->sse, false);
                                        return false;
                                    }
                                }
                                else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                                {
                                    closeFile(sData);

                                    if (!openFile(sData, file_mode_open_write))
                                    {
                                        setAsyncError(sData, async_state_read_response, FIREBASE_ERROR_OPEN_FILE, !sData->sse, true);
                                        return false;
                                    }
                                }
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
                                    oh.getPad(buf + ofs, read, sData->request.b64Pad);
                                    if (sData->request.ota)
                                    {
                                        oh.decodeBase64OTA(mem, &bh, (const char *)buf, read - ofs, sData->request.ota_error);
                                        if (sData->request.ota_error != 0)
                                        {
                                            setAsyncError(sData, async_state_read_response, sData->request.ota_error, !sData->sse, false);
                                            goto exit;
                                        }

                                        if (sData->request.b64Pad > -1)
                                        {
                                            oh.endDownloadOTA(sData->request.b64Pad, sData->request.ota_error);
                                            if (sData->request.ota_error != 0)
                                            {
                                                setAsyncError(sData, async_state_read_response, sData->request.ota_error, !sData->sse, false);
                                                goto exit;
                                            }
                                        }
                                    }
                                    else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                                    {
#if defined(ENABLE_FS)
                                        if (!bh.decodeToFile(mem, sData->request.file_data.file, (const char *)buf + ofs))
                                        {
                                            setAsyncError(sData, async_state_read_response, FIREBASE_ERROR_FILE_WRITE, !sData->sse, true);
                                            goto exit;
                                        }
#endif
                                    }
                                    else
                                        bh.decodeToBlob(mem, &sData->request.file_data.outB, (const char *)buf + ofs);
                                }
                                else
                                {
                                    if (sData->request.ota)
                                    {
                                        bh.updateWrite(buf, read);

                                        if (sData->response.payloadRead == sData->response.payloadLen)
                                        {
                                            oh.endDownloadOTA(0, sData->request.ota_error);
                                            if (sData->request.ota_error != 0)
                                            {
                                                setAsyncError(sData, async_state_read_response, sData->request.ota_error, !sData->sse, false);
                                                goto exit;
                                            }
                                        }
                                    }
                                    else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                                    {
#if defined(ENABLE_FS)
                                        int write = sData->request.file_data.file.write(buf, read);
                                        if (write < read)
                                        {
                                            setAsyncError(sData, async_state_read_response, FIREBASE_ERROR_FILE_WRITE, !sData->sse, true);
                                            goto exit;
                                        }
#endif
                                    }
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
                        sData->response.payloadRead += readLine(sData, sData->response.val[res_hndlr_ns::payload]);
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
                sData->response.val[res_hndlr_ns::header] = sData->response.val[res_hndlr_ns::payload].substring(sData->response.payloadRead - sData->response.payloadLen);
                sData->response.val[res_hndlr_ns::payload].remove(0, sData->response.payloadLen);
                sData->return_type = function_return_type_continue;
                sData->state = async_state_read_response;
                sData->response.flags.header_remaining = true;
            }

            if (sData->upload)
            {
                URLHelper uh;
                uh.updateDownloadURL(sData->aResult.upload_data.downloadUrl, sData->response.val[res_hndlr_ns::payload]);
            }

            if (sData->response.flags.chunks && sData->auth_used)
                stop(sData);

            if (sData->response.httpCode >= FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST)
            {
                setAsyncError(sData, sData->state, sData->response.httpCode, !sData->sse, true);
                sData->return_type = function_return_type_failure;
                returnResult(sData, false);
            }

            if (sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_OK && sData->download)
            {
                sData->aResult.download_data.total = sData->response.payloadLen;
                sData->aResult.download_data.downloaded = sData->response.payloadRead;
                returnResult(sData, false);
            }

            sData->response.flags.payload_remaining = false;
            closeFile(sData);

            if (sData->auth_used)
                sData->response.auth_data_available = true;
        }

        return sData->error.code == 0;
    }

    // non-block memory buffer for collecting the multiple of 4 data prepared for base64 decoding
    uint8_t *asyncBase64Buffer(async_data_item_t *sData, Memory &mem, int &toRead, int &read)
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

    void clear(async_data_item_t *sData)
    {
        clear(sData->response.val[res_hndlr_ns::header]);
        if (!sData->auth_used)
            clear(sData->response.val[res_hndlr_ns::payload]);
        sData->response.flags.header_remaining = false;
        sData->response.flags.payload_remaining = false;
        sData->response.payloadRead = 0;
        sData->response.error.resp_code = 0;
        clear(sData->response.error.string);
        sData->response.chunkInfo.chunkSize = 0;
        sData->response.chunkInfo.dataLen = 0;
        sData->response.chunkInfo.phase = async_response_handler_t::READ_CHUNK_SIZE;
    }

    void reset(async_data_item_t *sData, bool disconnect)
    {
        if (disconnect)
            stop(sData);
        sData->response.httpCode = 0;
        sData->error.code = 0;
        sData->response.flags.reset();
        sData->state = async_state_undefined;
        sData->return_type = function_return_type_undefined;
        clear(sData->response.val[res_hndlr_ns::etag]);
        sData->aResult.download_data.reset();
        sData->aResult.upload_data.reset();
        clear(sData);
    }

    function_return_type connect(async_data_item_t *sData, const char *host, uint16_t port)
    {
        sData->aResult.lastError.clearError();
        lastErr.clearError();

        if (client && !client->connected())
            sData->aResult.setDebug(FPSTR("Connecting to server..."));

        if (client && !client->connected() && client_type == async_request_handler_t::tcp_client_type_sync)
            sData->return_type = client->connect(host, port) > 0 ? function_return_type_complete : function_return_type_failure;
        else if (client_type == async_request_handler_t::tcp_client_type_async)
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

                sData->return_type = status ? function_return_type_complete : function_return_type_continue;
            }
#endif
        }

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

#if defined(FIREBASE_ETH_IS_AVAILABLE)

#if defined(ESP32)
        if (validIP(ETH.localIP()))
        {
            ETH.linkUp();
            ret = true;
        }
#elif defined(ESP8266) || defined(MB_ARDUINO_PICO)

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

#elif defined(MB_ARDUINO_PICO)

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

    bool gprsConnect(async_data_item_t *sData)
    {
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE)
        TinyGsm *gsmModem = (TinyGsm *)_modem;
        if (gsmModem)
        {
            // Unlock your SIM card with a PIN if needed
            if (_pin.length() && gsmModem->getSimStatus() != 3)
                gsmModem->simUnlock(_pin.c_str());

#if defined(TINY_GSM_MODEM_XBEE)
            // The XBee must run the gprsConnect function BEFORE waiting for network!
            gsmModem->gprsConnect(_apn.c_str(), _user.c_str(), _password.c_str());
#endif
            if (netErrState == 0 && sData)
                sData->aResult.setDebug(FPSTR("Waiting for network..."));
            if (!gsmModem->waitForNetwork())
            {
                if (netErrState == 0 && sData)
                    sData->aResult.setDebug(FPSTR("Network connection failed"));
                netErrState = 1;
                net.network_status = false;
                return false;
            }

            if (netErrState == 0 && sData)
                sData->aResult.setDebug(FPSTR("Network connected"));

            if (gsmModem->isNetworkConnected())
            {

                if (netErrState == 0 && sData)
                {
                    String debug = FPSTR("Connecting to ");
                    debug += _apn.c_str();
                    sData->aResult.setDebug(debug);
                }

                net.network_status = gsmModem->gprsConnect(_apn.c_str(), _user.c_str(), _password.c_str()) &&
                                     gsmModem->isGprsConnected();

                if (netErrState == 0 && sData)
                {
                    if (net.network_status)
                        sData->aResult.setDebug(FPSTR("GPRS/EPS connected"));
                    else
                        sData->aResult.setDebug(FPSTR("GPRS/EPS connection failed"));
                }
            }

            if (!net.network_status)
                netErrState = 1;

            return net.network_status;
        }

#endif
        return false;
    }

    bool gprsConnected()
    {
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE)
        TinyGsm *gsmModem = (TinyGsm *)_modem;
        net.network_status = gsmModem && gsmModem->isGprsConnected();
#endif
        return net.network_status;
    }

    bool gprsDisconnect()
    {
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE)
        TinyGsm *gsmModem = (TinyGsm *)_modem;
        net.network_status = gsmModem && gsmModem->gprsDisconnect();
#endif
        return !net.network_status;
    }

    bool ethernetConnect(async_data_item_t *sData)
    {
        bool ret = false;

#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE)

        if (net.ethernet.ethernet_cs_pin > -1)
            ETH_MODULE_CLASS.init(net.ethernet.ethernet_cs_pin);

        if (net.ethernet.ethernet_reset_pin > -1)
        {
            if (sData)
                sData->aResult.setDebug(FPSTR("Resetting Ethernet Board..."));

            pinMode(net.ethernet.ethernet_reset_pin, OUTPUT);
            digitalWrite(net.ethernet.ethernet_reset_pin, HIGH);
            delay(200);
            digitalWrite(net.ethernet.ethernet_reset_pin, LOW);
            delay(50);
            digitalWrite(net.ethernet.ethernet_reset_pin, HIGH);
            delay(200);
        }

        if (sData)
            sData->aResult.setDebug(FPSTR("Starting Ethernet connection..."));

        if (net.ethernet.static_ip)
        {

            if (net.ethernet.static_ip->optional == false)
                ETH_MODULE_CLASS.begin(net.ethernet.ethernet_mac, net.ethernet.static_ip->ipAddress, net.ethernet.static_ip->dnsServer, net.ethernet.static_ip->defaultGateway, net.ethernet.static_ip->netMask);
            else if (!ETH_MODULE_CLASS.begin(net.ethernet.ethernet_mac))
            {
                ETH_MODULE_CLASS.begin(net.ethernet.ethernet_mac, net.ethernet.static_ip->ipAddress, net.ethernet.static_ip->dnsServer, net.ethernet.static_ip->defaultGateway, net.ethernet.static_ip->netMask);
            }
        }
        else
            ETH_MODULE_CLASS.begin(net.ethernet.ethernet_mac);

        net.eth_timer.feed(FIREBASE_ETHERNET_MODULE_TIMEOUT);

        while (ETH_MODULE_CLASS.linkStatus() == LinkOFF && net.eth_timer.remaining() > 0)
        {
            delay(100);
        }

        ret = ethernetConnected();

        if (ret && sData)
        {
            String debug = FPSTR("Starting Ethernet connection...");
            debug += ETH_MODULE_CLASS.localIP();
            sData->aResult.setDebug(debug);
        }

        if (!ret && sData)
            sData->aResult.setDebug(FPSTR("Can't connect to network"));

#endif

        return ret;
    }

    bool ethernetConnected()
    {
#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE)
        net.network_status = ETH_MODULE_CLASS.linkStatus() == LinkON && validIP(ETH_MODULE_CLASS.localIP());
        if (!net.network_status)
        {
            delay(FIREBASE_ETHERNET_MODULE_TIMEOUT);
            net.network_status = ETH_MODULE_CLASS.linkStatus() == LinkON && validIP(ETH_MODULE_CLASS.localIP());
        }
#endif
        return net.network_status;
    }

    bool netConnect(async_data_item_t *sData)
    {
        if (!netStatus(sData))
        {
            bool recon = net.reconnect;

            if (net.wifi && net.net_timer.feedCount() == 0)
                recon = true;

            if (recon && (net.net_timer.remaining() == 0))
            {
                net.net_timer.feed(FIREBASE_NET_RECONNECT_TIMEOUT_SEC);

                if (sData)
                    sData->aResult.setDebug(FPSTR("Reconnecting to network..."));

                if (net.network_data_type == firebase_network_data_generic_network)
                {
#if defined(FIREBASE_HAS_WIFI_DISCONNECT)
                    // We can reconnect WiFi when device connected via built-in WiFi that supports reconnect
                    if (WiFI_CONNECTED)
                    {
                        WiFi.reconnect();
                        return netStatus(sData);
                    }
#endif
                    if (net.generic.net_con_cb)
                        net.generic.net_con_cb();
                }
                else if (net.network_data_type == firebase_network_data_gsm_network)
                {
                    gprsDisconnect();
                    gprsConnect(sData);
                }
                else if (net.network_data_type == firebase_network_data_ethernet_network)
                {
                    ethernetConnect(sData);
                }
                else if (net.network_data_type == firebase_network_data_default_network)
                {

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

        return netStatus(sData);
    }

    bool netStatus(async_data_item_t *sData)
    {
        // We will not invoke the network status request when device has built-in WiFi or Ethernet and it is connected.
        if (net.network_data_type == firebase_network_data_gsm_network)
        {
            net.network_status = gprsConnected();
            if (!net.network_status)
                gprsConnect(sData);
        }
        else if (net.network_data_type == firebase_network_data_ethernet_network)
        {
            if (!ethernetConnected())
                ethernetConnect(sData);
        }
        // also check the native network before calling external cb
        else if (net.network_data_type == firebase_network_data_default_network || WiFI_CONNECTED || ethLinkUp())
            net.network_status = WiFI_CONNECTED || ethLinkUp();
        else if (net.network_data_type == firebase_network_data_generic_network)
        {
            if (!net.generic.net_status_cb)
                netErrState = 1;
            else
                net.generic.net_status_cb(net.network_status);
        }
        else
            net.network_status = false;

        return net.network_status;
    }

    int sIndex(slot_options_t &options)
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

    void setContentType(async_data_item_t *sData, const String &type)
    {
        sData->request.addContentTypeHeader(type.c_str());
    }

    void setFileContentLength(async_data_item_t *sData, int headerLen = 0, const String &customHeader = "")
    {
#if defined(ENABLE_FS)
        if ((sData->request.file_data.cb && sData->request.file_data.filename.length()) || (sData->request.file_data.data_size && sData->request.file_data.data))
        {
            Base64Helper bh;
            size_t sz = 0;
            if (sData->request.file_data.cb)
            {
                sData->request.file_data.cb(sData->request.file_data.file, sData->request.file_data.filename.c_str(), file_mode_open_read);
                sz = sData->request.file_data.file.size();
            }
            else
                sz = sData->request.file_data.data_size;

            sData->request.file_data.file_size = sData->request.base64 ? 2 + bh.getBase64Len(sz) : sz;
            if (customHeader.length())
            {
                sData->request.val[req_hndlr_ns::header] += customHeader;
                sData->request.val[req_hndlr_ns::header] += ":";
                sData->request.val[req_hndlr_ns::header] += sData->request.file_data.file_size + headerLen;
                sData->request.val[req_hndlr_ns::header] += "\r\n";
            }
            else
                setContentLength(sData, sData->request.file_data.file_size);

            closeFile(sData);
        }
#endif
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
    void handleEventTimeout(async_data_item_t *sData)
    {
        if (sData->sse && sData->aResult.rtdbResult.eventTimeout() && sData->aResult.rtdbResult.eventResumeStatus() == RealtimeDatabaseResult::event_resume_status_undefined)
        {
            sData->aResult.rtdbResult.setEventResumeStatus(RealtimeDatabaseResult::event_resume_status_resuming);
            setAsyncError(sData, sData->state, FIREBASE_ERROR_STREAM_TIMEOUT, false, false);
            returnResult(sData, false);
            reset(sData, true);
        }
    }
#endif

    bool handleSendTimeout(async_data_item_t *sData)
    {
        if (sData->request.send_timer.remaining() == 0 || sData->cancel)
        {
            setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_SEND, !sData->sse, false);
            sData->return_type = function_return_type_failure;
            return true;
        }
        return false;
    }

    bool handleReadTimeout(async_data_item_t *sData)
    {
        if (!sData->sse && (sData->response.read_timer.remaining() == 0 || sData->cancel))
        {
            setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT, !sData->sse, false);
            sData->return_type = function_return_type_failure;
            return true;
        }
        return false;
    }

    void handleProcessFailure(async_data_item_t *sData)
    {
        if (sData->return_type == function_return_type_failure)
        {
            if (sData->async)
                returnResult(sData, false);
            reset(sData, false);
        }
    }

    String getHost(async_data_item_t *sData, bool fromReq, String *ext = nullptr)
    {
#if defined(ENABLE_FS) && defined(ENABLE_CLOUD_STORAGE)
        String url = fromReq ? sData->request.val[req_hndlr_ns::url] : sData->request.file_data.resumable.getLocation();
#else
        String url = fromReq ? sData->request.val[req_hndlr_ns::url] : sData->response.val[res_hndlr_ns::location];
#endif
        URLHelper uh;
        return uh.getHost(url, ext);
    }

    void stopAsyncImpl(bool all = false, const String &uid = "")
    {
        if (inStopAsync)
            return;

        inStopAsync = true;
        size_t size = slotCount();
        if (size)
        {
            for (size_t i = size - 1; i >= 0; i--)
            {
                sys_idle();
                async_data_item_t *sData = getData(i);
                if (sData && sData->async && !sData->auth_used && !sData->cancel)
                {
                    if (uid.length())
                    {
                        if (strcmp(sData->aResult.uid().c_str(), uid.c_str()) == 0)
                            sData->cancel = true;
                    }
                    else
                    {
                        sData->cancel = true;
                        if (!all)
                            break;
                    }
                }
            }
        }

        inStopAsync = false;
    }

public:
    AsyncClientClass(Client &client, network_config_data &net) : client(&client)
    {
        this->net.copy(net);
        this->addr = reinterpret_cast<uint32_t>(this);
        List list;
        client_type = async_request_handler_t::tcp_client_type_sync;
        list.addRemoveList(cVec, addr, true);
    }

#if defined(ENABLE_ASYNC_TCP_CLIENT)
    AsyncClientClass(AsyncTCPConfig &tcpClientConfig, network_config_data &net) : async_tcp_config(&tcpClientConfig)
    {
        this->net.copy(net);
        this->addr = reinterpret_cast<uint32_t>(this);
        List list;
        client_type = async_request_handler_t::tcp_client_type_async;
        list.addRemoveList(cVec, addr, true);
    }
#endif

    ~AsyncClientClass()
    {
        stop(nullptr);

        for (size_t i = 0; i < sVec.size(); i++)
        {
            reset(getData(i), true);
            async_data_item_t *sData = getData(i);
            if (!sData->auth_used)
                delete sData;
            sData = nullptr;
        }

        List list;
        list.addRemoveList(cVec, addr, false);
    }

    bool networkStatus() { return netStatus(nullptr); }

    void stopAsync(bool all = false) { stopAsyncImpl(all); }
    void stopAsync(const String &uid) { stopAsyncImpl(false, uid); }

    void stop(async_data_item_t *sData)
    {
        if (sData)
            sData->aResult.setDebug(FPSTR("Terminating the server connection..."));
        if (client_type == async_request_handler_t::tcp_client_type_sync)
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
    }

    FirebaseError lastError() const { return lastErr; }

    String etag() const { return resETag; }

    void setETag(const String &etag) { reqEtag = etag; }

    async_data_item_t *createSlot(slot_options_t &options)
    {
        int slot_index = sIndex(options);
        // Only one SSE mode is allowed
        if (slot_index == -2)
            return nullptr;
        async_data_item_t *sData = addSlot(slot_index);
        sData->reset();
        return sData;
    }

    void newRequest(async_data_item_t *sData, const String &url, const String &path, const String &extras, async_request_handler_t::http_request_method method, slot_options_t &options, const String &uid)
    {
        sData->async = options.async;
        sData->request.val[req_hndlr_ns::url] = url;
        sData->request.val[req_hndlr_ns::path] = path;
        sData->request.method = method;
        sData->sse = options.sse;
        sData->request.val[req_hndlr_ns::etag] = reqEtag;

        clear(reqEtag);
        sData->aResult.val[ares_ns::res_uid] = uid;
        clear(sData->request.val[req_hndlr_ns::header]);
        sData->request.addRequestHeaderFirst(method);
        if (path.length() == 0)
            sData->request.val[req_hndlr_ns::header] += '/';
        else if (path.length() && path[0] != '/')
            sData->request.val[req_hndlr_ns::header] += '/';
        sData->request.val[req_hndlr_ns::header] += path;
        sData->request.val[req_hndlr_ns::header] += extras;
        sData->request.addRequestHeaderLast();
        sData->request.addHostHeader(getHost(sData, true).c_str());

        sData->auth_used = options.auth_used;

        if (!options.auth_used)
        {
            sData->request.app_token = options.app_token;
            if (options.app_token && !options.auth_param && (options.app_token->auth_type == auth_id_token || options.app_token->auth_type == auth_user_id_token || options.app_token->auth_type == auth_access_token || options.app_token->auth_type == auth_sa_access_token))
            {
                sData->request.addAuthHeaderFirst(options.app_token->auth_type);
                sData->request.val[req_hndlr_ns::header] += FIREBASE_AUTH_PLACEHOLDER;
                sData->request.addNewLine();
            }

            sData->request.val[req_hndlr_ns::header] += FPSTR("Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0");
            sData->request.addNewLine();
            sData->request.addConnectionHeader(true);
            if (!options.sv && !options.no_etag && method != async_request_handler_t::http_patch && extras.indexOf("orderBy") == -1)
            {
                sData->request.val[req_hndlr_ns::header] += FPSTR("X-Firebase-ETag: true");
                sData->request.addNewLine();
            }

            if (sData->request.val[req_hndlr_ns::etag].length() > 0 && (method == async_request_handler_t::http_put || method == async_request_handler_t::http_delete))
            {
                sData->request.val[req_hndlr_ns::header] += FPSTR("if-match: ");
                sData->request.val[req_hndlr_ns::header] += sData->request.val[req_hndlr_ns::etag];
                sData->request.addNewLine();
            }

            if (options.sse)
            {
                sData->request.val[req_hndlr_ns::header] += FPSTR("Accept: text/event-stream");
                sData->request.addNewLine();
            }
        }

        if (method == async_request_handler_t::http_get || method == async_request_handler_t::http_delete)
            sData->request.addNewLine();
    }

    void setContentLength(async_data_item_t *sData, size_t len)
    {
        if (sData->request.method == async_request_handler_t::http_post || sData->request.method == async_request_handler_t::http_put || sData->request.method == async_request_handler_t::http_patch)
        {
            sData->request.addContentLengthHeader(len);
            sData->request.addNewLine();
        }
    }

    void process(bool async)
    {
        if (processLocked())
            return;

        if (slotCount())
        {
            size_t slot = 0;
            async_data_item_t *sData = getData(slot);

            if (!sData)
            {
                inProcess = false;
                return;
            }

            if (!netConnect(sData))
            {
                setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_DISCONNECTED, !sData->sse, false);
                if (sData->async)
                {
                    returnResult(sData, false);
                    reset(sData, true);
                }
                inProcess = false;
                return;
            }

            if (sData->async && !async)
            {
                inProcess = false;
                return;
            }

            bool sending = false;
            if (sData->state == async_state_undefined || sData->state == async_state_send_header || sData->state == async_state_send_payload)
            {

                sData->response.clear();
                sData->request.feedTimer();
                sending = true;
                sData->return_type = send(sData);

                while (sData->state == async_state_send_header || sData->state == async_state_send_payload)
                {
                    sData->return_type = send(sData);
                    sData->response.feedTimer();
                    handleSendTimeout(sData);
                    if (sData->async || sData->return_type == function_return_type_failure)
                        break;
                }
            }

            if (sending)
            {
                handleSendTimeout(sData);
                if (sData->async && sData->return_type == function_return_type_continue)
                {
                    inProcess = false;
                    return;
                }
            }

            sys_idle();

            if (sData->state == async_state_read_response)
            {
                // if (!sData->download && !sData->upload)
                //    sData->request.clear();

                // it can be complete response from payload sending
                if (sData->return_type == function_return_type_complete)
                    sData->return_type = function_return_type_continue;

                if (sData->async && !sData->response.tcpAvailable(client_type, client, async_tcp_config))
                {
#if defined(ENABLE_DATABASE)
                    handleEventTimeout(sData);
#endif
                    handleReadTimeout(sData);
                    inProcess = false;
                    return;
                }
                else if (!sData->async) // wait for non async
                {
                    while (!sData->response.tcpAvailable(client_type, client, async_tcp_config) && netConnect(sData))
                    {
                        sys_idle();
                        if (handleReadTimeout(sData))
                            break;
                    }
                }
            }

            // Read until status code > 0, header finished and payload read complete
            if (sData->state == async_state_read_response)
            {
                sData->error.code = 0;
                while (sData->return_type == function_return_type_continue && (sData->response.httpCode == 0 || sData->response.flags.header_remaining || sData->response.flags.payload_remaining))
                {
                    sData->response.feedTimer();
                    sData->return_type = receive(sData);

                    handleReadTimeout(sData);

                    bool allRead = sData->response.httpCode > 0 && sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_OK && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining;
                    if (allRead && sData->response.httpCode >= FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST)
                    {
                        if (sData->sse)
                        {
                            sData->aResult.data_available = false;
#if defined(ENABLE_DATABASE)
                            sData->aResult.rtdbResult.clearSSE();
#endif
                        }
                        sData->return_type = function_return_type_failure;
                    }

                    if (sData->async || allRead || sData->return_type == function_return_type_failure)
                        break;
                }
            }

            handleProcessFailure(sData);

#if defined(ENABLE_DATABASE)
            handleEventTimeout(sData);
#endif

            setAsyncError(sData, sData->state, 0, !sData->sse && sData->return_type == function_return_type_complete, false);

            if (sData->to_remove)
                removeSlot(slot);
        }

        inProcess = false;
    }
    void handleRemove()
    {
        for (size_t slot = 0; slot < slotCount(); slot++)
        {
            async_data_item_t *sData = getData(slot);
            if (sData && sData->to_remove)
                removeSlot(slot);
        }
    }

    size_t slotCount() { return sVec.size(); }

    void removeSlot(uint8_t slot, bool sse = true)
    {
        async_data_item_t *sData = getData(slot);

        if (!sData)
            return;

        if (sData->sse && !sse)
            return;

#if defined(ENABLE_DATABASE)
        sData->aResult.rtdbResult.clearSSE();
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
};

#endif