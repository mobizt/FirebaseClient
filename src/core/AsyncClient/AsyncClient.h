/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_CLIENT_ASYNC_CLIENT_H
#define CORE_ASYNC_CLIENT_ASYNC_CLIENT_H

#include <vector>
#include "./core/Error.h"
#include "./core/Utils/OTA.h"
#include "./core/Utils/StringUtil.h"
#include "./core/AsyncClient/SlotManager.h"
#if defined(ENABLE_DATABASE)
#define PUBLIC_DATABASE_RESULT_IMPL_BASE : public RTDBResultImpl
#else
#define PUBLIC_DATABASE_RESULT_IMPL_BASE
#endif

using namespace firebase_ns;

class AsyncClientClass PUBLIC_DATABASE_RESULT_IMPL_BASE
{
    friend class AppBase;
    friend class RealtimeDatabase;
    friend class Databases;
    friend class Documents;
    friend class CollectionGroups;
    friend class CloudFunctions;
    friend class Messaging;
    friend class MessagingBase;
    friend class MessagingInstance;
    friend class Storage;
    friend class CloudStorage;
    friend class FirestoreBase;
    friend class RuleSets;
    friend class Releases;

private:
    StringUtil sut;
    URLUtil uut;
    SlotManager sman;
    String header, resETag;
    uint32_t addr = 0, auth_ts = 0, cvec_addr = 0, sync_send_timeout_sec = 0, sync_read_timeout_sec = 0;
    Memory mem;
    Base64Util b64ut;
    OTAUtil otaut;
    bool inProcess = false, inStopAsync = false;

    // Friends access
    std::vector<uint32_t> &getResultList() { return sman.rVec; }
    app_log_t &getAppDebug() { return sman.debug_log; }
    app_log_t &getAppEvent() { return sman.event_log; }
    void stop() { sman.stop(); }
    void removeSlot(uint8_t slot, bool sse = true) { sman.removeSlot(slot, sse); }
    template <typename T>
    void setClientError(T &request, int code) { sman.setClientError(request, code); }
    async_data *createSlot(slot_options_t &options) { return sman.createSlot(options); }
    void eventPushBack(int code, const String &msg) { sman.event_log.push_back(code, msg); }
    size_t slotCount() const { return sman.sVec.size(); }
    AsyncResult *getResult() { return sman.getResult(); }
    void handleRemove()
    {
        for (size_t slot = 0; slot < slotCount(); slot++)
        {
            const async_data *sData = sman.getData(slot);
            if (sData && sData->to_remove)
                removeSlot(slot);
        }
    }

    function_return_type sendHeader(async_data *sData, const char *data) { return sendImpl(sData, reinterpret_cast<const uint8_t *>(data), data ? strlen(data) : 0, data ? strlen(data) : 0, astate_send_header); }

    function_return_type sendHeader(async_data *sData, const uint8_t *data, size_t len) { return sendImpl(sData, data, len, len, astate_send_header); }

    // Handles file/blob data sending process of raw, base64 encoded string and resumable upload.
    function_return_type sendFileData(async_data *sData, async_state state = astate_send_payload)
    {
        function_return_type ret = ret_continue;

        size_t totalLen = sData->request.file_data.file_size;
        bool fileopen = sData->request.payloadIndex == 0;

        if (sData->upload)
            sData->upload_progress_enabled = true;

        if (fileopen)
        {
#if defined(ENABLE_FS)
            if (sData->request.file_data.filename.length() > 0 &&
                sData->request.file_data.file_status == file_config_data::file_status_closed &&
                !sData->request.openFile(file_mode_open_read))
            {
                // File open error.
                sman.setAsyncError(sData, state, FIREBASE_ERROR_OPEN_FILE, !sData->sse, true);
                return ret_failure;
            }
#endif
            (void)state;
            if (sData->request.base64)
            {
                ret = sendImpl(sData, reinterpret_cast<const uint8_t *>("\""), 1, totalLen, astate_send_payload);
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
                if (sData->request.file_data.filename.length() > 0 && sData->request.file_data.file.available() < toSend)
                    toSend = sData->request.file_data.file.available();
#endif
                if (sData->request.file_data.data && sData->request.file_data.data_size &&
                    (int)(sData->request.file_data.data_size - sData->request.file_data.data_pos) < toSend)
                    toSend = sData->request.file_data.data_size - sData->request.file_data.data_pos;

                buf = reinterpret_cast<uint8_t *>(mem.alloc(toSend));
#if defined(ENABLE_FS)
                if (sData->request.file_data.filename.length() > 0)
                {
                    toSend = sData->request.file_data.file.read(buf, toSend);
                    if (toSend == 0)
                    {
                        // File read error.
                        sman.setAsyncError(sData, state, FIREBASE_ERROR_FILE_READ, !sData->sse, true);
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
                        sman.setAsyncError(sData, state, FIREBASE_ERROR_FILE_READ, !sData->sse, true);
                        ret = ret_failure;
                        goto exit;
                    }
                }
#endif
                if (sData->request.file_data.data && sData->request.file_data.data_size)
                    memcpy(buf, sData->request.file_data.data + sData->request.file_data.data_pos, toSend);
                sData->request.file_data.data_pos += toSend;
            }

            ret = sendImpl(sData, buf, toSend, totalLen, astate_send_payload);
        }
        else if (sData->request.base64)
            ret = sendImpl(sData, reinterpret_cast<const uint8_t *>("\""), 1, totalLen, astate_send_payload);

#if defined(ENABLE_FS)
    exit:
#endif

        if (buf)
            mem.release(&buf);

        return ret;
    }

    // Handles string data sending process.
    function_return_type send(async_data *sData, const char *data, async_state state = astate_send_payload)
    {
        return sendImpl(sData, reinterpret_cast<const uint8_t *>(data), data ? strlen(data) : 0, data ? strlen(data) : 0, state);
    }

    // Handles raw data sending process.
    function_return_type sendImpl(async_data *sData, const uint8_t *data, size_t len, size_t size, async_state state = astate_send_payload)
    {
        if (state == astate_send_header && ((sData->sse && sData->response.respCtx.stage == res_handler::response_stage_payload) || sData->response.respCtx.stage == res_handler::response_stage_undefined || sData->response.respCtx.stage == res_handler::response_stage_finished))
        {
            initResponse(sData);
        }

        sys_idle();
        sData->state = state;
        if (data && len && sman.client)
        {
            uint16_t toSend = len - sData->request.dataIndex > FIREBASE_CHUNK_SIZE ? FIREBASE_CHUNK_SIZE : len - sData->request.dataIndex;
            size_t sent = sData->request.tcpWrite(data + sData->request.dataIndex, toSend);
            if (sent == toSend)
            {
                sData->request.dataIndex += toSend;
                sData->request.payloadIndex += toSend;
                if (sData->upload && sData->upload_progress_enabled && sData->request.file_data.file_size)
                {
                    sData->aResult.upload_data.total = sData->request.file_data.file_size > size ? sData->request.file_data.file_size : size;
                    sData->aResult.upload_data.uploaded = sData->request.payloadIndex;
                    sman.returnResult(sData, false);
                }

                if (sData->request.dataIndex == len)
                    sData->request.dataIndex = 0;

#if defined(ENABLE_CLOUD_STORAGE)
                if (sData->request.file_data.resumable.isEnabled() && sData->request.file_data.resumable.isUpload())
                {
                    int ret = sData->request.file_data.resumable.isComplete(size, sData->request.payloadIndex);
                    if (ret == 2)
                    {
                        // All chunks are uploaded.
                        sData->state = astate_read_response;
                        sData->return_type = ret_complete;
                        sData->request.dataIndex = 0;
                        sData->request.payloadIndex = 0;
                        sData->response.clear();
                        return sData->return_type;
                    }
                    else if (ret == 1)
                    {
                        // There are more chunks to upload.
                        sData->return_type = ret_continue;
                        return sData->return_type;
                    }
                }
                else if (sData->request.payloadIndex < size)
                {
                    // There are more chunks to upload.
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
            sman.setAsyncError(sData, state, FIREBASE_ERROR_TCP_SEND, !sData->sse, false);

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

    // Handles TCP data sending process.
    function_return_type send(async_data *sData)
    {
        function_return_type ret = ret_continue;

        if (sData->state == astate_undefined || sData->state == astate_send_header) // Initial connection or payload sending state.
        {
            // Auth token checking
            if (!sData->auth_used && sData->request.app_token &&
                sData->request.app_token->auth_data_type != user_auth_data_no_token &&
                sData->request.app_token->val[app_tk_ns::token].length() == 0)
            {
                // Missing auth token error.
                sman.setAsyncError(sData, sData->state, FIREBASE_ERROR_UNAUTHENTICATE, !sData->sse, false);
                return ret_failure;
            }

            // Stop the server connection when entering the initial/auth token request states or host, port, and SSE mode changes or session timed out.
            sman.newCon(sData, sData->request.getHost(true, &sData->response.val[resns::location]).c_str(), sData->request.port);

            if ((sman.client_type == tcpc_sync && !sman.conn.isConnected()))
            {
                if (sData->request.getHost(true, &sData->response.val[resns::location]).length() == 0)
                {
                    sData->aResult.lastError.setClientError(FIREBASE_ERROR_INVALID_HOST);
                    return sman.connErrorHandler(sData, sData->state);
                }

                // Connect to the server if not connected.
                ret = sman.connect(sData, sData->request.getHost(true, &sData->response.val[resns::location]).c_str(), sData->request.port);

                // Allowing non-blocking async tcp connection
                if (ret == ret_continue)
                    return ret;

                if (ret != ret_complete)
                    return sman.connErrorHandler(sData, sData->state);

                sman.conn.sse = sData->sse;
                sman.conn.async = sData->async;
                sData->auth_ts = auth_ts;
            }

            if (sData->upload)
                sData->upload_progress_enabled = false;
            // Non-auth task header sending.
            if (!sData->auth_used && sData->request.app_token && sData->request.app_token->auth_data_type != user_auth_data_no_token)
            {
                // Set the auth token to the end point uri or authorization header when it is available.
                header = sData->request.val[reqns::header];
                header.replace(FIREBASE_AUTH_PLACEHOLDER, sData->request.app_token->val[app_tk_ns::token]);
                ret = sendHeader(sData, header.c_str());
                sut.clear(header);
                return ret;
            }
            // Auth task header sending.
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
                        ret = sendImpl(sData, sData->request.data, sData->request.dataLen, sData->request.dataLen);
                    else
                        ret = sendFileData(sData);
                }
            }
        }
        return ret;
    }

    function_return_type receive(async_data *sData)
    {
        // HTTP error is allowed in case non-auth task to get its response.
        if (!readResponse(sData))
        {
            // HTTP or TCP read error.
            sman.setAsyncError(sData, sData->state, sData->response.httpCode > 0 ? sData->response.httpCode : FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT, !sData->sse, false);
            return ret_failure;
        }

        if (sData->response.httpCode == 0) // No responses.
            return ret_continue;

#if defined(ENABLE_CLOUD_STORAGE)
        if (sData->request.file_data.resumable.isEnabled() && sData->request.file_data.resumable.getLocation().length() && sData->response.respCtx.stage == res_handler::response_stage_finished)
        {
            // Connect to the new upload location (stop and connect).
            String ext;
            String _host = sData->request.getHost(false, nullptr, &ext);
            sman.stop();
            if (sman.connect(sData, _host.c_str(), sData->request.port) > ret_failure)
            {
                // Get the request header to perform the ranges upload (resumable).
                sut.clear(sData->request.val[reqns::payload]);
                sut.clear(sData->request.val[reqns::header]);
                sData->request.file_data.resumable.getHeader(sData->request.val[reqns::header], _host, ext);
                sData->state = astate_send_header;
                sData->request.file_data.resumable.setHeaderState();
                return ret_continue;
            }
            return sman.connErrorHandler(sData, sData->state);
        }
#else
        if (sData->response.val[resns::location].length() && sData->response.respCtx.stage == res_handler::response_stage_finished)
        {
            // Hadles redirection (stop and connect).
            String ext;
            String _host = sData->request.getHost(false, &sData->response.val[resns::location], &ext);
            sman.stop();
            if (sman.connect(sData, _host.c_str(), sData->request.port) > ret_failure)
            {
                uut.relocate(sData->request.val[reqns::header], _host, ext);
                sut.clear(sData->request.val[reqns::payload]);
                sData->state = astate_send_header;
                return ret_continue;
            }
            return sman.connErrorHandler(sData, sData->state);
        }
#endif
        if (!sData->sse && sData->response.httpCode > 0 && sData->response.respCtx.stage == res_handler::response_stage_finished)
        {
            sData->state = astate_undefined;
            return ret_complete;
        }
        return ret_continue;
    }

    void clear(String &str) { sut.clear(str); }

    bool readResponse(async_data *sData)
    {

        if (!sman.client || !sData)
            return false;

        if (sData->response.tcpAvailable() > 0)
        {
            readHeader(sData);
            readPayload(sData);
        }

        if (sData->response.respCtx.stage == res_handler::response_stage_finished)
        {
            sData->response.respCtx.stage = sData->response.flags.sse ? res_handler::response_stage_payload : res_handler::response_stage_finished;

            String *payload = &sData->response.val[resns::payload];

            if (!sData->response.flags.sse && payload->length())
            {
                sData->aResult.setPayload(*payload);
                if (sData->auth_used)
                    sData->response.auth_data_available = true;
                else
                {
                    clear(*payload);
                    sData->response.flags.payload_available = true;
                    sman.returnResult(sData, true);
                }
            }

            if (sData->aResult.download_data.total > 0)
                sData->aResult.data_log.reset();

#if defined(ENABLE_DATABASE)

            if (sData->request.method == reqns::http_post)
                parseNodeName(&sData->aResult.rtdbResult);

            // Data available from sse event
            if (sData->response.flags.sse && payload->length())
            {

                if (payload->indexOf("event: ") > -1 && payload->indexOf("data: ") > -1 && payload->indexOf("\n") > -1)
                {
                    // Prevent sse timed out due to large sse Stream playload
                    // This is not prevent the timed out from user blocking code and delay.
                    feedSSETimer(&sData->aResult.rtdbResult);

                    uint32_t len = payload->length();

                    // The event data was received while more data (events) are available.
                    bool partial = (*payload)[len - 1] == '\n' && ((len > 2 && (*payload)[len - 2] == '}') || (len > 3 && (*payload)[len - 3] == '}'));

                    if (((*payload)[len - 1] == '\n' && sData->response.tcpAvailable() == 0) || partial)
                    {
                        setRefPayload(&sData->aResult.rtdbResult, payload);
                        parseSSE(&sData->aResult.rtdbResult);

                        // Event filtering.
                        if (sman.sseFilter(sData))
                        {
                            // save payload to slot result
                            sData->aResult.setPayload(*payload);
                            clear(*payload);
                            sData->response.flags.payload_available = true;
                            sman.returnResult(sData, true);
                        }
                        else
                            clear(*payload);

                        sData->response.flags.http_response = false;
                    }
                }
            }
#endif
        }
        return true;
    }

    // Handles response header read process.
    void readHeader(async_data *sData)
    {
        res_handler::response_stage stage = sData->response.respCtx.stage;
        if (stage != res_handler::response_stage_payload)
        {
            sData->response.readMetaData();

            // Read and parse for the specific headers.
            if (sData->response.respCtx.stage != stage)
            {
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
                    sman.debug_log.push_back(-1, "Delete operation complete");
            }
        }
    }

    // Handles response payload reading process.
    // Write raw or base64 decoded response payload to file, blob, and flash (OTA).
    bool readPayload(async_data *sData)
    {
        uint8_t *buf = nullptr;

        if (sData->response.respCtx.stage == res_handler::response_stage_payload)
        {
            sData->response.feedTimer(!sData->async && sync_read_timeout_sec > 0 ? sync_read_timeout_sec : -1);

            // The next chunk data is the payload
            if (sData->response.httpCode != FIREBASE_ERROR_HTTP_CODE_NO_CONTENT)
            {
                if (sData->download)
                {
                    // Raw text and byte array payloads.
                    // Content length is not available for SSE Stream.
                    // The content length header must be set for Byte array payload.
                    if (sData->response.payloadLen)
                    {
                        // At the beginning step, preparing file, tempolary blob data buffer and flash (OTA) to write.
                        if (sData->response.payloadRead == 0)
                        {
                            if (sData->request.ota)
                            {
#if defined(OTA_UPDATE_ENABLED) && defined(FIREBASE_OTA_UPDATER)
#if defined(FIREBASE_OTA_STORAGE)
                                otaut.setOTAStorage(sData->request.ota_storage_addr);
#endif

                                otaut.prepareDownloadOTA(sData->response.payloadLen, sData->request.base64, sData->request.ota_error, sData->request.command);
                                if (sData->request.ota_error != 0)
                                {
                                    // OTA error.
                                    sman.setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                    return false;
                                }
#endif
                            }
#if defined(ENABLE_FS)
                            else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                            {
                                sData->request.closeFile();

                                if (!sData->request.openFile(file_mode_open_write))
                                {
                                    // File open error.
                                    sman.setAsyncError(sData, astate_read_response, FIREBASE_ERROR_OPEN_FILE, !sData->sse, true);
                                    return false;
                                }
                            }
#endif
                            else
                                sData->request.file_data.outB.init(sData->request.file_data.data, sData->request.file_data.data_size);
                        }

                        int toRead = 0, read = 0;
                        uint8_t ofs = 0;

                        // Due to the size of data returns from the SSL Client buffer may be varied and is not suitable for base64 decoder.
                        // The following process will read the data as a chunk of multiple of 4 bytes (FIREBASE_CHUNK_SIZE)
                        // that is suitable for the base64 decoder.
                        buf = asyncBase64Buffer(sData, mem, read, toRead);

                        // There is more bytes (base64) to read and fill in to the chunk buffer.
                        if (sData->response.toFillLen)
                            return true;

                        // No base64 data read? Read the new data (base64 encoded or unencoded) with the expected length.
                        if (!buf)
                        {
                            // if base64, skip the double quote at the beginning of string response payload (in Realtime Database)
                            ofs = sData->request.base64 && sData->response.payloadRead == 0 ? 1 : 0;
                            toRead = (int)(sData->response.payloadLen - sData->response.payloadRead) > FIREBASE_CHUNK_SIZE + ofs ? FIREBASE_CHUNK_SIZE + ofs : sData->response.payloadLen - sData->response.payloadRead;
                            buf = reinterpret_cast<uint8_t *>(mem.alloc(toRead));
                            read = sData->response.tcpRead(buf, toRead);
                        }

                        if (read > 0)
                        {
                            if (sData->request.base64 && read < toRead)
                            {
                                // If the read data (base64 encoded) is still less than the size we expected,
                                // save it in a larger chunk buffer (response.toFill) and keep the offset and
                                // length of the new incoming base64 data to be filled.
                                sData->response.toFillIndex += read;
                                sData->response.toFillLen = toRead - read;
                                sData->response.toFill = reinterpret_cast<uint8_t *>(mem.alloc(toRead));
                                memcpy(sData->response.toFill, buf, read);
                                goto exit;
                            }

                            sData->response.payloadRead += read;
                            if (sData->request.base64) // Base64 encoded response payload
                            {
                                // Write to flash (OTA).
                                otaut.getPad(buf + ofs, read, sData->request.b64Pad);
                                if (sData->request.ota)
                                {
#if defined(OTA_UPDATE_ENABLED) && defined(FIREBASE_OTA_UPDATER)
                                    otaut.decodeBase64OTA(mem, &b64ut, reinterpret_cast<const char *>(buf), sData->request.ota_error);
                                    if (sData->request.ota_error != 0)
                                    {
                                        // OTA error.
                                        sman.setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                        goto exit;
                                    }

                                    if (sData->request.b64Pad > -1)
                                    {
                                        otaut.endDownloadOTA(b64ut, sData->request.b64Pad, sData->request.ota_error);
                                        if (sData->request.ota_error != 0)
                                        {
                                            // OTA error.
                                            sman.setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                            goto exit;
                                        }
                                    }
#endif
                                }
#if defined(ENABLE_FS)
                                else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                                {
                                    // Write to file.
                                    if (!b64ut.decodeToFile(mem, sData->request.file_data.file, reinterpret_cast<const char *>(buf + ofs)))
                                    {
                                        // File write error.
                                        sman.setAsyncError(sData, astate_read_response, FIREBASE_ERROR_FILE_WRITE, !sData->sse, true);
                                        goto exit;
                                    }
                                }
#endif
                                else
                                    b64ut.decodeToBlob(mem, &sData->request.file_data.outB, reinterpret_cast<const char *>(buf + ofs));
                            }
                            else // Raw byte array response payload
                            {
                                // To write flash (OTA)
                                if (sData->request.ota)
                                {
#if defined(OTA_UPDATE_ENABLED) && defined(FIREBASE_OTA_UPDATER)
                                    b64ut.updateWrite(buf, read);

                                    if (sData->response.payloadRead == sData->response.payloadLen)
                                    {
                                        otaut.endDownloadOTA(b64ut, 0, sData->request.ota_error);
                                        if (sData->request.ota_error != 0)
                                        {
                                            // OTA error.
                                            sman.setAsyncError(sData, astate_read_response, sData->request.ota_error, !sData->sse, false);
                                            goto exit;
                                        }
                                    }
#endif
                                }
#if defined(ENABLE_FS)
                                else if (sData->request.file_data.filename.length() && sData->request.file_data.cb)
                                {
                                    // To write file.
                                    int write = sData->request.file_data.file.write(buf, read);
                                    if (write < read)
                                    {
                                        // File write error.
                                        sman.setAsyncError(sData, astate_read_response, FIREBASE_ERROR_FILE_WRITE, !sData->sse, true);
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
                        sman.returnResult(sData, false);
                    }
                }
                else
                    sData->response.readPayload();
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
            }

            if (sData->upload)
                uut.updateDownloadURL(sData->aResult.upload_data.downloadUrl, sData->response.val[resns::payload]);

            if (sData->response.flags.chunks && sData->auth_used)
                sman.stop();

            // HTTP server error.
            if (sData->response.httpCode >= FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST)
            {
                sman.setAsyncError(sData, sData->state, sData->response.httpCode, !sData->sse, true);
                sData->return_type = ret_failure;
                sman.returnResult(sData, false);
            }

            if (sData->response.httpCode == FIREBASE_ERROR_HTTP_CODE_OK && sData->download)
            {
                sData->aResult.download_data.total = sData->response.payloadLen;
                sData->aResult.download_data.downloaded = sData->response.payloadRead;
                sman.returnResult(sData, false);
            }

#if defined(ENABLE_FS)
            // Close file when uploading response complete.
            sData->request.closeFile();
#endif
        }
        return sData->error.code == 0;
    }

    uint8_t *asyncBase64Buffer(async_data *sData, Memory &mem, int &toRead, int &read)
    {
        uint8_t *buf = nullptr;
        // If the chunk buffer was previously allocated and more bytes are required to fill.
        if (sData->request.base64 && sData->response.toFill && sData->response.toFillLen)
        {
            // Read the data with expected length to the chunk buffer at the specific offset.
            int currentRead = sData->response.tcpRead(sData->response.toFill + sData->response.toFillIndex, sData->response.toFillLen);

            // All bytes are filled, allocate the output buffer
            // and copy the data from the chunk buffer.
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
                // There are still more bytes that are remaining to read.
                sData->response.toFillIndex += currentRead;
                sData->response.toFillLen -= currentRead;
            }
        }
        return buf;
    }

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
                // In case Stream was timed out,
                // we have to clear app data (available), reset the last error and set the Stream time out error.
                sData->aResult.lastError.reset();
                sData->aResult.data_log.reset();
                setEventResumeStatus(&sData->aResult.rtdbResult, event_resume_status_resuming);
                // Stream timed out error.
                sman.setAsyncError(sData, sData->state, FIREBASE_ERROR_STREAM_TIMEOUT, false, false);
                sman.returnResult(sData, false);
                sman.reset(sData, true);
            }
        }
    }
#endif

    bool handleSendTimeout(async_data *sData)
    {
        if (sData->request.send_timer.remaining() == 0)
        {
            // TCP write error.
            sman.setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_SEND, !sData->sse, false);
            sData->return_type = ret_failure;
            // This requires by WiFiSSLClient before stating a new connection in case session was reused.
            sman.reset(sData, true);
            return true;
        }
        return false;
    }

    bool handleReadTimeout(async_data *sData)
    {
        if (!sData->sse && sData->response.read_timer.remaining() == 0)
        {
            // TCP read error.
            sman.setAsyncError(sData, sData->state, FIREBASE_ERROR_TCP_RECEIVE_TIMEOUT, !sData->sse, false);
            sData->return_type = ret_failure;
            // This requires by WiFiSSLClient before stating a new connection in case session was reused.
            sman.reset(sData, true);
            return true;
        }
        return false;
    }

    void handleProcessFailure(async_data *sData)
    {
        if (sData->return_type == ret_failure)
        {
            if (sData->async)
                sman.returnResult(sData, false);
            sman.reset(sData, sData->response.httpCode == 0 /* disconnected only when no server response */);
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
                async_data *sData = sman.getData(i);
                if (sData && sData->async && !sData->auth_used && !sData->to_remove)
                {
                    // Reset the app data to reset and clear the available status when the task was canceled.
                    sData->aResult.data_log.reset();
                    if (sman.getResult(sData))
                        sman.getResult(sData)->data_log.reset();

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

    void newRequest(async_data *sData, const String &url, const String &path, const String &extras, reqns::http_request_method method, const slot_options_t &options, const String &uid, const String &etag)
    {
        sData->async = options.async;
        sData->request.val[reqns::url] = url;
        sData->request.val[reqns::path] = path;
        sData->request.method = method;
        sData->sse = options.sse;
        sData->request.val[reqns::etag] = etag;

        sData->aResult.setUID(uid);

        clear(sData->request.val[reqns::header]);
        sData->request.addRequestHeader(method, path, extras);
        sData->request.addHostHeader(sData->request.getHost(true, &sData->response.val[resns::location]).c_str());
        sData->auth_used = options.auth_used;

        if (!options.auth_used)
        {
            sData->request.app_token = options.app_token;
            if (options.app_token && !options.auth_param && (options.user_auth->getAuthTokenType() > auth_unknown_token && options.user_auth->getAuthTokenType() < auth_refresh_token))
                sData->request.addAuthHeader(options.user_auth->getAuthTokenType());

            sData->request.addConnectionHeader(true);

            if (!options.sv && !options.no_etag && method != reqns::http_patch && extras.indexOf("orderBy") == -1)
                sData->request.val[reqns::header] += "X-Firebase-ETag: true\r\n";

            if (sData->request.val[reqns::etag].length() > 0 && (method == reqns::http_put || method == reqns::http_delete))
                sut.printTo(sData->request.val[reqns::header], 100, "if-match: %s\r\n", sData->request.val[reqns::etag].c_str());

            if (options.sse)
                sData->request.val[reqns::header] += "Accept: text/event-stream\r\n";

            if (url.indexOf("iid.googleapis.com") > -1)
                sData->request.val[reqns::header] += "access_token_auth: true\r\n";
        }

        if (method == reqns::http_get || method == reqns::http_delete)
            sData->request.addNewLine();
    }

    void initResponse(async_data *sData)
    {
        sData->response.respCtx.begin();

        String *location = &sData->response.val[resns::location];
#if defined(ENABLE_CLOUD_STORAGE)
        if (sData->upload)
            location = &sData->request.file_data.resumable.getLocationRef();
#endif

        sData->response.respCtx.isAuth = sData->auth_used;
        sData->response.respCtx.isSSE = sData->sse;
        sData->response.respCtx.isUpload = sData->upload;
        sData->response.respCtx.location = location;
        sData->response.httpCode = 0;
        sData->response.payloadLen = 0;
        sData->response.payloadRead = 0;
        sData->response.flags.reset();
        sData->response.toFill = nullptr;
        sData->response.toFillIndex = 0;
        sData->response.toFillLen = 0;
        sData->response.auth_data_available = false;
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

    void exitProcess(bool status) { inProcess = status; }

    void process(bool async)
    {
        if (processLocked())
            return;

        if (slotCount())
        {
            size_t slot = 0;
            async_data *sData = sman.getData(slot);

            if (!sData)
                return exitProcess(false);

            sman.debug_log.pop_front();
            sman.event_log.pop_front();
            sman.getResult()->dataLog().pop_front();
            sData->aResult.dataLog().pop_front();

            if (!sData->auth_used && (sData->request.ota || sData->download || sData->upload) && sData->request.ul_dl_task_running_addr > 0)
            {
                bool *ul_dl_task_running = reinterpret_cast<bool *>(sData->request.ul_dl_task_running_addr);
                *ul_dl_task_running = true;
            }

            if (sData->async && !async)
                return exitProcess(false);

            // Restart connection when authenticate, client or network changed
            if ((sData->sse && (sData->auth_ts != auth_ts || !sman.conn.isConnected())) || sman.conn.isChanged())
            {
                sman.stop();
                sData->state = astate_send_header;
            }

            // Resume incomplete async task from previously stopped.
            if (!sman.conn.async && sData->async && !sData->complete)
            {
                sData->state = astate_send_header;
                sman.conn.async = sData->async;
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

                // We will set complete status when payload was completly uploaded.
                sData->complete = sData->upload;

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
                    while (!sData->response.tcpAvailable())
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
                while (sData->return_type == ret_continue && (sData->response.httpCode == 0 || sData->response.respCtx.stage != res_handler::response_stage_finished))
                {
                    sData->response.feedTimer(!sData->async && sync_read_timeout_sec > 0 ? sync_read_timeout_sec : -1);
                    sData->return_type = receive(sData);

                    handleReadTimeout(sData);

                    if (sData->response.respCtx.stage == res_handler::response_stage_finished && sData->response.httpCode >= FIREBASE_ERROR_HTTP_CODE_BAD_REQUEST)
                    {
                        if (sData->sse)
                        {
#if defined(ENABLE_DATABASE)
                            sData->aResult.data_log.reset();
                            clearSSE(&sData->aResult.rtdbResult);
#endif
                        }
                        sData->return_type = ret_failure;
                        sman.setAsyncError(sData, sData->state, sData->response.httpCode, !sData->sse, false);
                    }

                    if (sData->async || sData->return_type == ret_failure)
                        break;
                }
            }

            handleProcessFailure(sData);

#if defined(ENABLE_DATABASE)
            handleEventTimeout(sData);
#endif

            if (!sData->sse && sData->return_type == ret_complete && sData->state != astate_send_payload)
            {
                sData->to_remove = true;
                sData->complete = true;
            }

            if (sData->to_remove)
                removeSlot(slot);
        }
        exitProcess(false);
    }

    FirebaseError *_lastError() { return &sman.lastErr; }

public:
    AsyncClientClass()
    {
        this->addr = reinterpret_cast<uint32_t>(this);
        sman.client_type = tcpc_sync;
    }

    explicit AsyncClientClass(Client &client) { setClient(client); }

    ~AsyncClientClass()
    {
        sman.stop();
        for (size_t i = 0; i < sman.sVec.size(); i++)
        {
            sman.reset(sman.getData(i), true);
            async_data *sData = sman.getData(i);
            delete sData;
            sData = nullptr;
        }
        addRemoveClientVec(cvec_addr, false);
    }

    /**
     * Set the network status callback function.
     * @param cb The AsyncClientNetworkStatusCallback function pointer.
     */
    void setNetworkStatusCallback(AsyncClientNetworkStatusCallback cb)
    {
        sman.conn.setNetworkStatusCallback(cb);
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
        sman.refResult = &result;
        sman.result_addr = reinterpret_cast<uint32_t>(sman.refResult);
    }

    /**
     * Unset the external async result use with the sync task.
     *
     * The internal async result will be used and shared usage for all sync tasks.
     *
     */
    void unsetAsyncResult()
    {
        sman.refResult = nullptr;
        sman.result_addr = 0;
    }

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
    FirebaseError lastError() const { return sman.lastErr; }

    /**
     * Get the response ETag.
     *
     * @return String The response ETag header.
     */
    String etag() const { return resETag; }

    /**
     * Set the ETag header to the task (DEPRECATED).
     *
     * @param etag The ETag to set to the task.
     *
     * The etag can be set via the functions that support etag.
     */
    void setEtag(const String &etag)
    {
        (void)etag;
        Serial.println("🔥 AsyncClientClass::setEtag is deprecated.");
    }

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
     * @param timeoutSec The TCP session timeout in seconds (> 150 seconds).
     */
    void setSessionTimeout(uint32_t timeoutSec) { sman.session_timeout_sec = timeoutSec; }

    /**
     * Filtering response payload for Relatime Database SSE streaming.
     * @param filter The event keywords for filtering.
     *
     * This is optional to allow specific events filtering.
     *
     * The following event keywords are supported.
     * get - To allow the http get response (first put event since stream connected).
     * put - To allow the put event.
     * patch - To allow the patch event.
     * keep-alive - To allow the keep-alive event.
     * cancel - To allow the cancel event.
     * auth_revoked - To allow the auth_revoked event.
     *
     * To clear all prevousely set filter to allow all Stream events, use AsyncClientClass::setSSEFilters().
     *
     * This will overwrite the value sets by RealtimeDatabase::setSSEFilters.
     */
    void setSSEFilters(const String &sse_events_filter) { sman.sse_events_filter = sse_events_filter; }

    /**
     * Set the SSL client.
     *
     * @param client The SSL client.
     */
    void setClient(Client &client)
    {
        sman.client = &client;
        sman.conn.setClientChange();
        this->addr = reinterpret_cast<uint32_t>(this);
        sman.client_type = tcpc_sync;
    }
};
#endif