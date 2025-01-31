#ifndef CORE_APP_BASE_H
#define CORE_APP_BASE_H

#include <Arduino.h>
#include "./core/AsyncResult/AsyncResult.h"

class AppBase
{
public:
    AppBase() {}

protected:
    void setResultUID(AsyncResult *aResult, const String &uid) { aResult->val[ares_ns::res_uid] = uid; }
    void setRVec(AsyncResult *aResult, uint32_t addr) { aResult->rvec_addr = addr; }
    std::vector<uint32_t> &getRVec(AsyncClientClass *aClient) { return aClient->getResultList(); }
    app_debug_t *getAppDebug(AsyncClientClass *aClient) { return &aClient->getAppDebug(); }
    void resultSetDebug(AsyncResult *aResult, app_debug_t *app_debug) { aResult->app_debug = app_debug; }
    void resultSetEvent(AsyncResult *aResult, app_event_t *app_event) { aResult->app_event = app_event; }
    app_event_t *getAppEvent(AsyncClientClass *aClient) { return &aClient->getAppEvent(); }
    void stopAsync(AsyncClientClass *aClient) { aClient->stop(); }
    async_data *createSlotBase(AsyncClientClass *aClient, slot_options_t &soption)
    {
        if (aClient)
            return aClient->createSlot(soption);
        return nullptr;
    }
    AsyncResult *getResultBase(AsyncClientClass *aClient) { return aClient->getResult(); }
    void newRequestBase(AsyncClientClass *aClient, async_data *sData, const String &url, const String &path, const String &extras, reqns::http_request_method method, const slot_options_t &options, const String &uid) { aClient->newRequest(sData, url, path, extras, method, options, uid); }
    void setAuthTsBase(AsyncClientClass *aClient, uint32_t ts) { aClient->auth_ts = ts; }
    void addRemoveClientVecBase(AsyncClientClass *aClient, uint32_t cvec_addr, bool add) { aClient->addRemoveClientVec(cvec_addr, add); }
    void setContentLengthBase(AsyncClientClass *aClient, async_data *sData, size_t len) { sData->request.setContentLength(len); }
    void handleRemoveBase(AsyncClientClass *aClient) { aClient->handleRemove(); }
    void removeSlotBase(AsyncClientClass *aClient, uint8_t slot, bool sse = true) { aClient->removeSlot(slot, sse); }
    size_t slotCountBase(AsyncClientClass *aClient) { return aClient->slotCount(); }
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