/**
 * Created June 25, 2024
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
#ifndef CORE_APP_H_
#define CORE_APP_H_

#include <Arduino.h>
#include "./core/AsyncResult/AsyncResult.h"

class AppBase
{

public:
    AppBase() {}

protected:
    void setResultUID(AsyncResult *aResult, const String &uid) { aResult->val[ares_ns::res_uid] = uid; }

    void setRVec(AsyncResult *aResult, uint32_t addr) { aResult->rvec_addr = addr; }

    std::vector<uint32_t> &getRVec(AsyncClientClass *aClient) { return aClient->rVec; }

    app_debug_t *getAppDebug(AsyncClientClass *aClient) { return &aClient->app_debug; }

    void resultSetDebug(AsyncResult *aResult, app_debug_t *app_debug) { aResult->app_debug = app_debug; }

    void resultSetEvent(AsyncResult *aResult, app_event_t *app_event) { aResult->app_event = app_event; }

    app_event_t *getAppEvent(AsyncClientClass *aClient) { return &aClient->app_event; }

    void stopAsync(AsyncClientClass *aClient, async_data_item_t *sData) { aClient->stop(sData); }

    async_data_item_t *createSlotBase(AsyncClientClass *aClient, slot_options_t &soption)
    {
        if (aClient)
            return aClient->createSlot(soption);
        return nullptr;
    }

    AsyncResult *getResultBase(AsyncClientClass *aClient) { return aClient->getResult(); }

    void newRequestBase(AsyncClientClass *aClient, async_data_item_t *sData, const String &url, const String &path, const String &extras, async_request_handler_t::http_request_method method, const slot_options_t &options, const String &uid) { aClient->newRequest(sData, url, path, extras, method, options, uid); }

    void setAuthTsBase(AsyncClientClass *aClient, uint32_t ts) { aClient->auth_ts = ts; }

    void addRemoveClientVecBase(AsyncClientClass *aClient, uint32_t cvec_addr, bool add) { aClient->addRemoveClientVec(cvec_addr, add); }

    void setContentLengthBase(AsyncClientClass *aClient, async_data_item_t *sData, size_t len) { aClient->setContentLength(sData, len); }

    void handleRemoveBase(AsyncClientClass *aClient) { aClient->handleRemove(); }

    void removeSlotBase(AsyncClientClass *aClient, uint8_t slot, bool sse = true) { aClient->removeSlot(slot, sse); }

    size_t slotCountBase(AsyncClientClass *aClient) { return aClient->sVec.size(); }

    void setLastErrorBase(AsyncResult *aResult, int code, const String &message)
    {
        if (aResult)
            aResult->lastError.setLastError(code, message);
    }

    void clearLastErrorBase(AsyncResult *aResult)
    {
        if (aResult)
            aResult->lastError.reset();
    }

    void processBase(AsyncClientClass *aClient, bool async)
    {
        if (!aClient)
            return;

        aClient->process(async);
    }

    template <typename T>
    void setAppBase(T &app, uint32_t app_addr, app_token_t *app_token, uint32_t avec_addr, uint32_t app_ota_status_addr) { app.setApp(app_addr, app_token, avec_addr, app_ota_status_addr); }
};

#endif