/**
 * Created January 29, 2024
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
#ifndef CORE_AUTH_REQUEST_H
#define CORE_AUTH_REQUEST_H
#include <Arduino.h>
#include "./core/AsyncClient/AsyncClient.h"
#include "./core/JSON.h"
#include "./core/JWT.h"

class AuthRequest
{
private:
    AsyncClient::async_data_item_t *sData = nullptr;

public:
    AuthRequest(){};
    ~AuthRequest(){};

    unsigned long request_sent_ms = 0;

    void asyncRequest(AsyncClient *aClient, const String &subdomain, const String &extras, const String &payload, AsyncResult &aResult)
    {
        String host;
        async_request_handler_t req;
        req.addGAPIsHost(host, subdomain.c_str());
        sData = aClient->newSlot(firebase_client_list, host, extras, "", async_request_handler_t::http_post, AsyncClient::slot_options_t(true, false, true, false, false));
        req.addContentTypeHeader(sData->request.header, "application/json");
        sData->request.payload = payload;
        aClient->setContentLength(sData, sData->request.payload.length());
        sData->refResult = &aResult;
        sData->ref_result_addr = aResult.addr;
        request_sent_ms = millis();
        aClient->process(sData->async);
        aClient->handleRemove();
    }

    void setLastError(AsyncResult &aResult, int code, const String &message)
    {
        aResult.lastError.err.message = message;
        aResult.lastError.err.code = code;
    }

    void clearLastError(AsyncResult &aResult)
    {
        aResult.lastError.err.message.remove(0, aResult.lastError.err.message.length());
        aResult.lastError.err.code = 0;
    }

    void process(AsyncClient *aClient, AsyncResult &aResult, AsyncResultCallback resultCb)
    {
        aClient->process(true);
        aClient->handleRemove();

        if (resultCb && aResult.lastError.code() != 0 && aResult.error_available)
        {
            aResult.data_available = false;
            resultCb(aResult);
        }
    }

    FirebaseError err()
    {
        if (sData)
            return sData->refResult->error();
        return FirebaseError();
    }

    void remove(AsyncClient *aClient)
    {
        aClient->handleRemove();
    }

    void stop(AsyncClient *aClient)
    {
        aClient->stop();
    }

    void setEventResult(AsyncResult &aResult, const String &msg, int code)
    {
        aResult.app_event.ev_code = code;
        aResult.app_event.ev_msg = msg;
    }
};

#endif
