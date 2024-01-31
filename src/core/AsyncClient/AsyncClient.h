/**
 * Created January 31, 2024
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
#include "./core/Storage.h"
#include "./core/Base64.h"
#include "./core/Error.h"
#include "./core/OTA.h"
#include "./core/AsyncResult/AsyncResult.h"
#include "./core/AuthConfig.h"
#include "./core/List.h"

using namespace firebase;

class AsyncClient
{
    friend class AuthRequest;
    friend class Database;

private:
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

    struct slot_options_t
    {
    public:
        bool auth_used = false;
        bool sse = false;
        bool async = false;
        bool sv = false;
        bool ota = false;
        bool no_etag = false;
        app_token_t *app_token = nullptr;
        slot_options_t() {}
        slot_options_t(bool auth_used, bool sse, bool async, bool sv, bool ota, bool no_etag, app_token_t *app_token = nullptr)
        {
            this->auth_used = auth_used;
            this->sse = sse;
            this->async = async;
            this->sv = sv;
            this->ota = ota;
            this->no_etag = no_etag;
            app_token = nullptr;
        }
    };

    struct async_data_item_t
    {

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
        uint32_t addr = 0;
        AsyncResult aResult;
        AsyncResult *refResult = nullptr;
        uint32_t ref_result_addr = 0;
        AsyncResultCallback cb = NULL;
        unsigned long last_error_notify_ms = 0;
        async_data_item_t()
        {
            addr = reinterpret_cast<uint32_t>(this);
        }
    };

    FirebaseError lastErr;
    String reqEtag, resETag;
    int netErrState = 0;
    Client *client = nullptr;
    bool sse = false;
    bool asyncCon = false;
    std::vector<uint32_t> aDataList;
    Memory mem;
    network_config_data net;
    uint32_t addr = 0;
    bool inProcess = false;

    void closeFile(async_data_item_t *sData)
    {
        if (sData->request.file_data.file && sData->request.file_data.file_status == file_config_data::file_status_opened)
        {
            sData->request.file_data.file_size = 0;
            sData->request.payloadIndex = 0;
            sData->request.dataIndex = 0;
            sData->request.file_data.file_status = file_config_data::file_status_closed;
            sData->request.file_data.file.close();
        }
    }

    bool openFile(async_data_item_t *sData, file_operating_mode mode)
    {
        sData->request.file_data.cb(sData->request.file_data.file, sData->request.file_data.filename.c_str(), mode);
        if (!sData->request.file_data.file)
            return false;
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

        if (sData->request.payloadIndex == 0)
        {
            if (sData->request.file_data.filename.length() > 0)
            {
                closeFile(sData);
                if (!openFile(sData, file_mode_open_read))
                {
                    setAsyncError(sData, state, FIREBASE_ERROR_OPEN_FILE, !sData->sse, true);
                    return function_return_type_failure;
                }
            }

            if (sData->request.base64)
            {
                ret = send(sData, (uint8_t *)"\"", 1, sData->request.file_data.file_size, async_state_send_payload);
                if (ret != function_return_type_continue)
                    return ret;
            }
        }

        uint8_t *buf = nullptr;
        int toSend = 0;
        Memory mem;

        if (sData->request.file_data.filename.length() > 0 ? sData->request.file_data.file.available() : sData->request.file_data.data_pos < sData->request.file_data.data_size)
        {
            if (sData->request.base64)
            {
                Base64Helper bh;
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
                else
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
                if (sData->request.file_data.filename.length() > 0)
                    toSend = sData->request.file_data.file.available() < FIREBASE_CHUNK_SIZE ? sData->request.file_data.file.available() : FIREBASE_CHUNK_SIZE;
                else
                    toSend = sData->request.file_data.data_size - sData->request.file_data.data_pos < FIREBASE_CHUNK_SIZE ? sData->request.file_data.data_size - sData->request.file_data.data_pos : FIREBASE_CHUNK_SIZE;

                buf = reinterpret_cast<uint8_t *>(mem.alloc(toSend));
                if (sData->request.file_data.filename.length() > 0)
                    toSend = sData->request.file_data.file.read(buf, toSend);
                if (toSend == 0)
                {
                    setAsyncError(sData, state, FIREBASE_ERROR_FILE_READ, !sData->sse, true);
                    ret = function_return_type_failure;
                    goto exit;
                }
                else
                {
                    memcpy(buf, sData->request.file_data.data + sData->request.file_data.data_pos, toSend);
                    sData->request.file_data.data_pos += toSend;
                }
            }

            ret = send(sData, buf, toSend, sData->request.file_data.file_size, async_state_send_payload);
        }
        else if (sData->request.base64)
            ret = send(sData, (uint8_t *)"\"", 1, sData->request.file_data.file_size, async_state_send_payload);

    exit:

        if (buf)
            mem.release(&buf);

        return ret;
    }

    function_return_type send(async_data_item_t *sData, const char *data, async_state state = async_state_send_payload)
    {
        return send(sData, (uint8_t *)data, data ? strlen(data) : 0, data ? strlen(data) : 0, state);
    }

    function_return_type send(async_data_item_t *sData, uint8_t *data, size_t len, size_t size, async_state state = async_state_send_payload)
    {
        sData->state = state;

        if (sData->cancel)
        {
            sData->return_type = function_return_type_complete;
            return sData->return_type;
        }

        if (data && len && this->client)
        {
            uint16_t toSend = len - sData->request.dataIndex > FIREBASE_CHUNK_SIZE ? FIREBASE_CHUNK_SIZE : len - sData->request.dataIndex;
            size_t sent = this->client->write(data + sData->request.dataIndex, toSend);
            async_request_handler_t req;
            req.idle();

            if (sent == toSend)
            {
                sData->request.dataIndex += toSend;
                sData->request.payloadIndex += toSend;

                if (sData->async && sData->request.file_data.file_size)
                {
                    sData->aResult.upload_data.total = sData->request.file_data.file_size;
                    sData->aResult.upload_data.uploaded = sData->request.payloadIndex;
                    returnResult(sData, false);
                }

                if (sData->request.dataIndex == len)
                    sData->request.dataIndex = 0;

                if (sData->request.payloadIndex < size)
                {
                    sData->return_type = function_return_type_continue;
                    return sData->return_type;
                }
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
                sData->state = async_state_send_payload;
            else if (state == async_state_send_payload)
                sData->state = async_state_read_response;
        }

        return sData->return_type;
    }

    function_return_type send(async_data_item_t *sData)
    {
        if (!sData || !netConnect())
        {
            setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_DISCONNECTED, !sData->sse, false);
            return function_return_type_failure;
        }

        function_return_type ret = function_return_type_continue;

        if (sData->state == async_state_undefined || sData->state == async_state_send_header)
        {
            if ((sse && !sData->sse) || (!sse && sData->sse) || (sData->auth_used && sData->state == async_state_undefined))
                stop();

            if (!client->connected())
            {
                ret = connect(sData, getHost(sData, true).c_str(), sData->request.port);
                if (ret != function_return_type_complete)
                    return connErrorHandler(sData, sData->state);
                sse = sData->sse;
            }
            return sendHeader(sData, sData->request.header.c_str());
        }
        else if (sData->state == async_state_send_payload)
        {
            if (sData->request.method == async_request_handler_t::http_get || sData->request.method == async_request_handler_t::http_delete)
                sData->state = async_state_read_response;
            else
            {
                if (sData->request.payload.length())
                    ret = send(sData, sData->request.payload.c_str());
                else if (sData->request.data && sData->request.dataLen)
                    ret = send(sData, sData->request.data, sData->request.dataLen, sData->request.dataLen);
                else if (sData->request.file_data.file_size && ((sData->request.file_data.filename.length() && sData->request.file_data.cb) || (sData->request.file_data.data && sData->request.file_data.data_size)))
                    ret = sendBuff(sData);
            }
        }
        return ret;
    }

    function_return_type receive(async_data_item_t *sData)
    {
        if (!sData || !netConnect())
        {
            setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_DISCONNECTED, !sData->sse, false);
            return function_return_type_failure;
        }

        if (!readResponse(sData))
        {
            setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT, !sData->sse, false);
            return function_return_type_failure;
        }

        if (sData->response.httpCode == 0)
            return function_return_type_continue;

        if (sData->response.location.length())
        {
            if (connect(sData, getHost(sData, false).c_str(), sData->request.port) > function_return_type_failure)
                return function_return_type_continue;

            return connErrorHandler(sData, sData->state);
        }

        if (!sData->sse && sData->response.httpCode > 0 && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining)
        {
            sData->state = async_state_undefined;
            return function_return_type_complete;
        }

        return function_return_type_continue;
    }

    function_return_type connErrorHandler(async_data_item_t *sData, async_state state)
    {
        // allow non-blocking connection
        if (sData->async && asyncCon)
            return function_return_type_continue;

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
        if (slot < aDataList.size())
            return reinterpret_cast<async_data_item_t *>(aDataList[slot]);
        return nullptr;
    }

    async_data_item_t *addSlot(int index = -1)
    {
        async_data_item_t *sData = new async_data_item_t();
        if (index > -1)
            aDataList.insert(aDataList.begin() + index, sData->addr);
        else
            aDataList.push_back(sData->addr);

        return sData;
    }

    void returnResult(async_data_item_t *sData, bool setData)
    {

        bool error_notify_timeout = false;
        if (sData->last_error_notify_ms == 0 || millis() - sData->last_error_notify_ms > 5000)
        {
            sData->last_error_notify_ms = millis();
            error_notify_timeout = true;
        }

        List list;

        if (sData->refResult)
        {
            if (setData || error_notify_timeout)
            {
                *sData->refResult = sData->aResult;

                if (setData)
                    sData->refResult->setPayload(sData->aResult.data_payload);

                sData->refResult->setETag(sData->aResult.res_etag);
                sData->refResult->setPath(sData->aResult.data_path);
            }
        }

        if (sData->cb && (setData || error_notify_timeout))
        {
            if (!sData->auth_used)
                sData->cb(sData->aResult);
        }
    }

    void setLastError(async_data_item_t *sData)
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
            sData->aResult.lastError.setResponseError(sData->response.payload, sData->response.httpCode);
            lastErr.setResponseError(sData->response.payload, sData->response.httpCode);
            sData->aResult.error_available = true;
            sData->aResult.data_available = false;
        }
    }

    void removeSlot(uint8_t slot)
    {

        async_data_item_t *sData = getData(slot);
#if defined(ENABLE_DATABASE)
        sData->aResult.database.clearSSE();
#endif

        closeFile(sData);

        if (!sData->cancel)
        {
            setLastError(sData);
            // data available from sync and asyn request except for sse
            returnResult(sData, true);
        }

        reset(sData, sData->auth_used);

        delete sData;
        sData = nullptr;

        aDataList.erase(aDataList.begin() + slot);
    }

    int readLine(Client *client, String &buf)
    {
        if (!client)
            return 0;

        int p = 0;

        while (client->available())
        {
            if (!client)
                break;

            int res = client->read();
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

    void clear(String &str)
    {
        str.remove(0, str.length());
    }

    bool readResponse(async_data_item_t *sData)
    {
        if (!netConnect() || !client || !sData)
            return false;

        if (sData->cancel)
        {
            sData->return_type = function_return_type_complete;
            return true;
        }

        if (client->available() > 0)
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

                        sData->aResult.setPayload(sData->response.payload);

                        if (sData->aResult.download_data.total > 0)
                            sData->aResult.data_available = false;
#if defined(ENABLE_DATABASE)

                        if (sData->request.method == async_request_handler_t::http_post)
                            sData->aResult.database.parseNodeName();

                        // data available from sse event
                        if (sData->response.flags.sse && sData->response.payload.length())
                        {
                            // order of checking: event, data, newline
                            if (sData->response.payload.indexOf("event: ") > -1 && sData->response.payload.indexOf("data: ") > -1 && sData->response.payload.indexOf("\n") > -1)
                            {
                                // save payload to slot result
                                sData->aResult.setPayload(sData->response.payload);
                                clear(sData->response.payload);
                                sData->aResult.database.parseSSE();
                                sData->response.flags.payload_available = true;
                                returnResult(sData, true);
                            }
                        }
#endif
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

        sData->response.dataTime = millis();
        sData->response.header.reserve(1024);

        // the first chunk (line) can be http response status or already connected stream payload
        readLine(client, sData->response.header);
        int status = getStatusCode(sData->response.header);
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
            int read = readLine(client, sData->response.header);
            if ((read == 1 && sData->response.header[sData->response.header.length() - 1] == '\r') ||
                (read == 2 && sData->response.header[sData->response.header.length() - 2] == '\r' && sData->response.header[sData->response.header.length() - 1] == '\n'))
            {
                clear(sData->response.location);
                clear(sData->response.etag);

                String cl, con, te, ct;
                parseRespHeader(sData, sData->response.header, sData->response.location, "Location");
                parseRespHeader(sData, sData->response.header, sData->response.etag, "ETag");
                resETag = sData->response.etag;
                sData->aResult.res_etag = sData->response.etag;
                sData->aResult.data_path = sData->request.path;
#if defined(ENABLE_DATABASE)
                sData->aResult.database.null_etag = sData->response.etag.indexOf("null_etag") > -1;
#endif

                parseRespHeader(sData, sData->response.header, cl, "Content-Length");

                sData->response.payloadLen = atoi(cl.c_str());

                parseRespHeader(sData, sData->response.header, con, "Connection");
                sData->response.flags.keep_alive = con.length() && con.indexOf("keep-alive") > -1;

                parseRespHeader(sData, sData->response.header, te, "Transfer-Encoding");
                sData->response.flags.chunks = te.length() && te.indexOf("chunked") > -1;

                parseRespHeader(sData, sData->response.header, ct, "Content-Type");
                sData->response.flags.sse = ct.length() && ct.indexOf("text/event-stream") > -1;

                clear(sData);

                if (sData->response.httpCode > 0 && sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
                    sData->response.flags.payload_remaining = true;
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
        readLine(client, line);
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
                int read = readLine(client, *out);
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
            sData->response.last_response_ms = millis();

            // the next chunk data is the payload
            if (sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
            {
                if (sData->response.flags.chunks)
                {
                    if (decodeChunks(sData, client, &sData->response.payload) == -1)
                        sData->response.flags.payload_remaining = false;
                }
                else
                {
                    if (sData->request.ota || (sData->request.file_data.filename.length() && sData->request.file_data.cb) || (sData->request.file_data.data && sData->request.file_data.data_size))
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
                                read = client->read(buf, toRead);
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
                                        if (!bh.decodeToFile(mem, sData->request.file_data.file, (const char *)buf + ofs))
                                        {
                                            setAsyncError(sData, async_state_read_response, FIREBASE_ERROR_FILE_WRITE, !sData->sse, true);
                                            goto exit;
                                        }
                                    }
                                    else
                                        bh.decodeToBlob(mem, &sData->request.file_data.outB, (const char *)buf + ofs);
                                }
                                else
                                {
                                    if (sData->request.ota)
                                    {
                                        Base64Helper bh;
                                        OtaHelper oh;
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
                                        int write = sData->request.file_data.file.write(buf, read);
                                        if (write < read)
                                        {
                                            setAsyncError(sData, async_state_read_response, FIREBASE_ERROR_FILE_WRITE, !sData->sse, true);
                                            goto exit;
                                        }
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
                        sData->response.payloadRead += readLine(client, sData->response.payload);
                }
            }
        }
    exit:

        if (buf)
            mem.release(&buf);

        if (sData->response.payloadLen > 0 && sData->response.payloadRead >= sData->response.payloadLen)
        {
            if (sData->response.flags.chunks && client->available() <= 2)
            {
                while (client->available())
                {
                    client->read();
                }
                client->stop();
            }

            if (sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_OK && (sData->request.ota || (sData->request.file_data.filename.length() && sData->request.file_data.cb) || (sData->request.file_data.data && sData->request.file_data.data_size)))
            {
                sData->aResult.download_data.total = sData->response.payloadLen;
                sData->aResult.download_data.downloaded = sData->response.payloadRead;
                returnResult(sData, false);
            }

            sData->response.flags.payload_remaining = false;
            closeFile(sData);
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
                int currentRead = client->read(sData->response.toFill + sData->response.toFillIndex, sData->response.toFillLen);
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
        clear(sData->response.header);
        clear(sData->response.payload);
        sData->response.flags.header_remaining = false;
        sData->response.flags.payload_remaining = false;
        sData->response.payloadRead = 0;
        sData->response.error.resp_code = 0;
        clear(sData->response.error.string);
        sData->response.error.resp_type = async_response_handler_t::RES_UNDEFINED;
        sData->response.chunkInfo.chunkSize = 0;
        sData->response.chunkInfo.dataLen = 0;
        sData->response.chunkInfo.phase = async_response_handler_t::READ_CHUNK_SIZE;
    }

    void reset(async_data_item_t *sData, bool disconnect)
    {
        if (disconnect)
            stop();
        sData->response.httpCode = 0;
        sData->error.code = 0;
        sData->response.flags.reset();
        sData->state = async_state_undefined;
        sData->return_type = function_return_type_undefined;
        clear(sData->response.location);
        clear(sData->response.etag);
        sData->aResult.download_data.reset();
        sData->aResult.upload_data.reset();
        clear(sData);
    }

    function_return_type connect(async_data_item_t *sData, const char *host, uint16_t port)
    {
        sData->aResult.lastError.clearError();
        lastErr.clearError();
        sData->return_type = client && client->connect(host, port) > 0 ? function_return_type_complete : function_return_type_failure;
        return sData->return_type;
    }

    /**
     * Get the ethernet link status.
     * @return true for link up or false for link down.
     */
    bool ethLinkUp()
    {
        bool ret = false;

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

    bool gprsConnect()
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

#if defined(DEFAULT_DEBUG_PORT)
            if (netErrState == 0)
                DEFAULT_DEBUG_PORT.print((const char *)FPSTR("Waiting for network..."));
#endif
            if (!gsmModem->waitForNetwork())
            {
#if defined(DEFAULT_DEBUG_PORT)
                if (netErrState == 0)
                    DEFAULT_DEBUG_PORT.println((const char *)FPSTR(" fail"));
#endif
                netErrState = 1;
                net.network_status = false;
                return false;
            }
#if defined(DEFAULT_DEBUG_PORT)
            if (netErrState == 0)
                DEFAULT_DEBUG_PORT.println((const char *)FPSTR(" success"));
#endif
            if (gsmModem->isNetworkConnected())
            {
#if defined(DEFAULT_DEBUG_PORT)
                if (netErrState == 0)
                {
                    DEFAULT_DEBUG_PORT.print((const char *)FPSTR("Connecting to "));
                    DEFAULT_DEBUG_PORT.print(_apn.c_str());
                }
#endif
                net.network_status = gsmModem->gprsConnect(_apn.c_str(), _user.c_str(), _password.c_str()) &&
                                     gsmModem->isGprsConnected();

#if defined(DEFAULT_DEBUG_PORT)
                if (netErrState == 0)
                {
                    if (net.network_status)
                        DEFAULT_DEBUG_PORT.println((const char *)FPSTR(" success"));
                    else
                        DEFAULT_DEBUG_PORT.println((const char *)FPSTR(" fail"));
                }
            }
#endif
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

    uint32_t gprsGetTime()
    {
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE) && defined(TINY_GSM_MODEM_HAS_TIME)

        if (!gprsConnected())
            return 0;

        TinyGsm *gsmModem = (TinyGsm *)_modem;
        int year3 = 0;
        int month3 = 0;
        int day3 = 0;
        int hour3 = 0;
        int min3 = 0;
        int sec3 = 0;
        float timezone = 0;
        for (int8_t i = 5; i; i--)
        {
            if (gsmModem->getNetworkTime(&year3, &month3, &day3, &hour3, &min3, &sec3, &timezone))
            {

                struct tm timeinfo;
                timeinfo.tm_year = year3 - 1900;
                timeinfo.tm_mon = month3 - 1;
                timeinfo.tm_mday = day3;
                timeinfo.tm_hour = hour3;
                timeinfo.tm_min = min3;
                timeinfo.tm_sec = sec3;
                time_t ts = mktime(&timeinfo);
                return ts;
            }
        }
#endif
        return 0;
    }

    bool ethernetConnect()
    {
        bool ret = false;

#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE)

        if (net.ethernet.ethernet_cs_pin > -1)
            ETH_MODULE_CLASS.init(net.ethernet.ethernet_cs_pin);

        if (net.ethernet.ethernet_reset_pin > -1)
        {
#if defined(DEFAULT_DEBUG_PORT)
            DEFAULT_DEBUG_PORT.println((const char *)FPSTR("Resetting Ethernet Board..."));
#endif
            pinMode(net.ethernet.ethernet_reset_pin, OUTPUT);
            digitalWrite(net.ethernet.ethernet_reset_pin, HIGH);
            delay(200);
            digitalWrite(net.ethernet.ethernet_reset_pin, LOW);
            delay(50);
            digitalWrite(net.ethernet.ethernet_reset_pin, HIGH);
            delay(200);
        }
#if defined(DEFAULT_DEBUG_PORT)
        DEFAULT_DEBUG_PORT.println((const char *)FPSTR("Starting Ethernet connection..."));
#endif
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

        unsigned long to = millis();

        while (ETH_MODULE_CLASS.linkStatus() == LinkOFF && millis() - to < FIREBASE_ETHERNET_MODULE_TIMEOUT)
        {
            delay(100);
        }

        ret = ethernetConnected();
#if defined(DEFAULT_DEBUG_PORT)
        if (ret)
        {
            DEFAULT_DEBUG_PORT.print((const char *)FPSTR("Connected with IP "));
            DEFAULT_DEBUG_PORT.println(ETH_MODULE_CLASS.localIP());
        }
#endif

#if defined(DEFAULT_DEBUG_PORT)
        if (!ret)
            DEFAULT_DEBUG_PORT.println((const char *)FPSTR("Can't connect"));
#endif

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

    bool netConnect()
    {
        if (!netStatus())
        {
            if (net.reconnect && (millis() - net.net_reconnect_ms > net.net_reconnect_timeout || net.net_reconnect_ms == 0))
            {
                net.net_reconnect_ms = millis();

                if (net.network_data_type == firebase_network_data_generic_network)
                {
#if defined(FIREBASE_HAS_WIFI_DISCONNECT)
                    // We can reconnect WiFi when device connected via built-in WiFi that supports reconnect
                    if (WiFI_CONNECTED)
                    {
                        WiFi.reconnect();
                        return netStatus();
                    }

#endif

                    if (net.generic.net_con_cb)
                        net.generic.net_con_cb();
                }
                else if (net.network_data_type == firebase_network_data_gsm_network)
                {
                    gprsDisconnect();
                    gprsConnect();
                }
                else if (net.network_data_type == firebase_network_data_ethernet_network)
                {
                    ethernetConnect();
                }
                else if (net.network_data_type == firebase_network_data_default_network)
                {

#if defined(FIREBASE_WIFI_IS_AVAILABLE)
#if defined(ESP32) || defined(ESP8266)

                    WiFi.reconnect();
#else
                        if (net.wifi && net.wifi->credentials.size())
                            net.wifi->reconnect();
#endif
#endif
                }
            }
        }

        return netStatus();
    }

    bool netStatus()
    {

        // We will not invoke the network status request when device has built-in WiFi or Ethernet and it is connected.
        if (net.network_data_type == firebase_network_data_gsm_network)
        {
            net.network_status = gprsConnected();
            if (!net.network_status)
                gprsConnect();
        }
        else if (net.network_data_type == firebase_network_data_ethernet_network)
        {
            if (!ethernetConnected())
                ethernetConnect();
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
            for (size_t i = 0; i < aDataList.size(); i++)
            {
                if (getData(i)->auth_used)
                    auth_index = i;
                else if (getData(i)->sse)
                    sse_index = i;
            }

            if (auth_index > -1)
                slot = auth_index + 1;
            else if (sse_index > -1)
                slot = sse_index;

            // Multiple SSE modes
            if (sse_index > -1 && options.sse)
                slot = -2;

            if (slot >= (int)aDataList.size())
                slot = -1;
        }

        return slot;
    }

    async_data_item_t *newSlot(std::vector<uint32_t> &clientList, const String &url, const String &path, const String &extras, async_request_handler_t::http_request_method method, slot_options_t options)
    {
        int slot_index = sIndex(options);

        // Only one SSE mode is allowed
        if (slot_index == -2)
            return nullptr;

        async_request_handler_t req;
        async_data_item_t *sData = addSlot(slot_index);

        sData->async = options.async;
        sData->request.url = url;
        sData->request.path = path;
        sData->request.method = method;
        sData->sse = options.sse;
        sData->request.etag = reqEtag;
        clear(reqEtag);
        req.addRequestHeaderFirst(sData->request.header, method);
        if (path.length() == 0)
            sData->request.header += '/';
        else if (path.length() && path[0] != '/')
            sData->request.header += '/';
        sData->request.header += path;
        sData->request.header += extras;
        req.addRequestHeaderLast(sData->request.header);
        req.addHostHeader(sData->request.header, getHost(sData, true).c_str());

        sData->auth_used = options.auth_used;

        if (!options.auth_used)
        {
            if (options.app_token && (options.app_token->auth_type == auth_access_token || options.app_token->auth_type == auth_sa_access_token))
            {
                req.addAuthHeaderFirst(sData->request.header, options.app_token->auth_type);
                sData->request.header += options.app_token->token;
                req.addNewLine(sData->request.header);
            }

            sData->request.header += FPSTR("Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0");
            req.addNewLine(sData->request.header);
            req.addConnectionHeader(sData->request.header, true);
            if (!options.sv && !options.no_etag && method != async_request_handler_t::http_patch && extras.indexOf("orderBy") == -1)
            {
                sData->request.header += FPSTR("X-Firebase-ETag: true");
                req.addNewLine(sData->request.header);
            }

            if (sData->request.etag.length() > 0 && (method == async_request_handler_t::http_put || method == async_request_handler_t::http_delete))
            {
                sData->request.header += FPSTR("if-match: ");
                sData->request.header += sData->request.etag;
                req.addNewLine(sData->request.header);
            }

            if (options.sse)
            {
                sData->request.header += FPSTR("Accept: text/event-stream");
                req.addNewLine(sData->request.header);
            }
        }

        if (method == async_request_handler_t::http_get || method == async_request_handler_t::http_delete)
            req.addNewLine(sData->request.header);
        return sData;
    }

    void setContentLength(async_data_item_t *sData, size_t len)
    {
        if (sData->request.method == async_request_handler_t::http_post || sData->request.method == async_request_handler_t::http_put || sData->request.method == async_request_handler_t::http_patch)
        {
            async_request_handler_t req;
            req.addContentLengthHeader(sData->request.header, len);
            req.addNewLine(sData->request.header);
        }
    }

    void setFileContentLength(async_data_item_t *sData)
    {
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
            setContentLength(sData, sData->request.file_data.file_size);
            sData->request.file_data.file.close();
        }
    }

    uint8_t slotCount()
    {
        return aDataList.size();
    }

    bool processLocked()
    {
        if (inProcess)
            return true;

        inProcess = true;
        return false;
    }

    void handleRemove()
    {
        for (size_t slot = 0; slot < slotCount(); slot++)
        {
            async_data_item_t *sData = getData(slot);
            if (sData->cancel || sData->to_remove)
                removeSlot(slot);
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

            if (!netConnect())
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

                sData->request.last_request_ms = millis();
                sending = true;
                sData->return_type = send(sData);

                while (sData->state == async_state_send_header || sData->state == async_state_send_payload)
                {
                    sData->return_type = send(sData);
                    sData->response.last_response_ms = millis();
                    if (millis() - sData->request.last_request_ms > sData->request.request_tmo)
                    {
                        setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_SEND, !sData->sse, false);
                        sData->return_type = function_return_type_failure;
                    }
                    if (sData->cancel || sData->async || sData->return_type == function_return_type_failure)
                        break;
                }
            }

            if (sending && sData->async && sData->return_type == function_return_type_continue)
            {
                inProcess = false;
                return;
            }

            async_request_handler_t req;
            req.idle();

            if (sData->state == async_state_read_response)
            {
                // it can be complete response from payload sending
                if (sData->return_type == function_return_type_complete)
                    sData->return_type = function_return_type_continue;

                if (sData->async && !client->available())
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
                    async_request_handler_t req;
                    while (!client->available() && netConnect())
                    {
                        delay(1);
                        req.idle();
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
                    sData->response.last_response_ms = millis();
                    sData->return_type = receive(sData);

                    if (sData->response.last_response_ms > 0 && millis() - sData->response.last_response_ms > sData->response.response_tmo)
                    {
                        setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT, !sData->sse, false);
                        sData->return_type = function_return_type_failure;
                    }
                    bool allRead = sData->response.httpCode > 0 && sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_OK && !sData->response.flags.header_remaining && !sData->response.flags.payload_remaining;
                    if (sData->cancel || sData->async || allRead || sData->return_type == function_return_type_failure)
                        break;
                }
            }

            handleProcessFailure(sData);

#if defined(ENABLE_DATABASE)
            handleEventTimeout(sData);
#endif

            setAsyncError(sData, sData->state, 0, !sData->sse && sData->return_type == function_return_type_complete, false);

            if (sData->cancel || sData->to_remove)
                removeSlot(slot);
        }

        inProcess = false;
    }
#if defined(ENABLE_DATABASE)
    void handleEventTimeout(async_data_item_t *sData)
    {
        if ((!sData->cancel && sData->sse && !sData->aResult.database.sse_request && sData->sse && sData->aResult.database.eventTimeout()))
        {
            setAsyncError(sData, sData->state, FIREBASE_ERROR_STREAM_TIMEDOUT, false, false);
            returnResult(sData, false);
            reset(sData, true);
        }
    }
#endif

    bool handleReadTimeout(async_data_item_t *sData)
    {
        if (sData->response.last_response_ms > 0 && millis() - sData->response.last_response_ms > sData->response.response_tmo)
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
#if defined(ENABLE_DATABASE)
            sData->aResult.database.sse_request = false;
#endif
            if (sData->async)
                returnResult(sData, false);
            reset(sData, false);
        }
    }

    String getHost(async_data_item_t *sData, bool fromReq)
    {
        async_request_handler_t::url_info_t url_info;
        async_request_handler_t req;
        req.parse(mem, fromReq ? sData->request.url : sData->response.location, url_info);
        return url_info.host;
    }

public:
    AsyncClient(Client &client, network_config_data &net) : client(&client)
    {
        this->net.copy(net);
        this->addr = reinterpret_cast<uint32_t>(this);
        List list;
        list.addRemoveList(firebase_client_list, addr, true);
    }

    ~AsyncClient()
    {
        for (size_t i = 0; i < aDataList.size(); i++)
            reset(getData(i), true);
        List list;
        list.addRemoveList(firebase_client_list, addr, false);
    }

    bool networkStatus() { return netStatus(); }

    void asyncConnect(bool opt)
    {
        asyncCon = opt;
    }

    void stopAsync(bool all = false)
    {
        if (slotCount())
        {
            for (size_t i = 0; i < slotCount(); i++)
            {
                async_data_item_t *sData = getData(i);
                if (sData->async)
                {
                    sData->cancel = true;
                    if (!all)
                        break;
                }
            }
        }
    }

    void stop()
    {
        if (client)
            client->stop();
    }

    FirebaseError lastError() { return lastErr; }

    String etag() { return resETag; }

    void setETag(const String &etag) { reqEtag = etag; }
};

#endif