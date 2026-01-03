/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_APP_BASE_H
#define CORE_APP_BASE_H

#include <Arduino.h>
#include "./core/AsyncResult/AsyncResult.h"

struct cvec_address_info_t
{
    uint32_t cvec_addr = 0;
    uint32_t app_addr = 0;
    app_token_t *app_token = nullptr;
};

class AppBase
{
    friend class RealtimeDatabase;
    friend class FirestoreBase;
    friend class CloudFunctions;
    friend class Messaging;
    friend class MessagingBase;
    friend class MessagingInstance;
    friend class Storage;
    friend class CloudStorage;
    friend class RuleSets;
    friend class Releases;

public:
    AppBase() {}

    static void removeCvecAddressList(std::vector<cvec_address_info_t> *cvec_address_list, uint32_t app_addr)
    {
        for (int i = cvec_address_list->size() - 1; i >= 0; i--)
        {
            if (app_addr == (*cvec_address_list)[i].app_addr)
                cvec_address_list->erase(cvec_address_list->begin() + i);
        }
    }

    static void staticLoop(const app_token_t *aToken, uint32_t cvec_addr)
    {
        std::vector<uint32_t> cVec = *reinterpret_cast<std::vector<uint32_t> *>(cvec_addr);

        for (size_t i = 0; i < cVec.size(); i++)
        {
            AsyncClientClass *client = reinterpret_cast<AsyncClientClass *>(cVec[i]);
            if (client)
            {
                // Store the auth time in all async clients.
                // The auth time will be used to reconnect the Stream when auth changed.
                if (aToken && aToken->auth_ts > 0 && aToken->authenticated)
                    client->setAuthTs(aToken->auth_ts);
                client->process(true);
                client->handleRemove();
            }
        }
    }

private:
    // FirebaseApp address and FirebaseApp vector address
    uint32_t app_addr = 0, avec_addr = 0, ul_dl_task_running_addr = 0, ota_storage_addr = 0, cvec_address_list_addr = 0, app_loop_count_addr = 0;
    Timer displayInfoTimer;
    app_token_t *app_token = nullptr;
    user_auth_data *user_auth = nullptr;
    String service_url;
    std::vector<uint32_t> cVec; // AsyncClient vector
    URLUtil uut;
    StringUtil sut;

    void setApp(uint32_t app_addr, auth_data_t *auth_data, uint32_t avec_addr, uint32_t ul_dl_task_running_addr, uint32_t cvec_address_list_addr, uint32_t app_loop_count_addr)
    {
        this->app_addr = app_addr;
        this->app_token = &auth_data->app_token;
        this->user_auth = &auth_data->user_auth;
        this->avec_addr = avec_addr; // AsyncClient vector (list) address
        this->cvec_address_list_addr = cvec_address_list_addr;
        this->app_loop_count_addr = app_loop_count_addr;
        this->ul_dl_task_running_addr = ul_dl_task_running_addr;
    }

    app_token_t *appToken()
    {
        if (avec_addr > 0)
        {
            const std::vector<uint32_t> *aVec = reinterpret_cast<std::vector<uint32_t> *>(avec_addr);
            List vec;
            return vec.existed(*aVec, app_addr) ? app_token : nullptr;
        }
        return nullptr;
    }

    void url(const String &url) { this->service_url = url; }

    void resetAppImpl()
    {
        if (this->app_addr && this->cvec_address_list_addr > 0)
            removeCvecAddressList(reinterpret_cast<std::vector<cvec_address_info_t> *>(this->cvec_address_list_addr), this->app_addr);
        this->app_addr = 0;
        this->app_token = nullptr;
        this->avec_addr = 0; // AsyncClient vector (list) address
        this->ul_dl_task_running_addr = 0;
    }

    void loopImpl()
    {

        if (displayInfoTimer.remaining() == 0 || !displayInfoTimer.isRunning())
        {
            if (app_loop_count_addr && *reinterpret_cast<uint16_t *>(app_loop_count_addr) <= 1)
                Serial.println("🔥 FirebaseApp::loop() or FirebaseApp::ready() has never been called.");

            displayInfoTimer.feed(60);
        }

        staticLoop(appToken(), reinterpret_cast<uint32_t>(&cVec));
    }

protected:
    void setResultUID(AsyncResult *aResult, const String &uid) { aResult->val[ares_ns::res_uid] = uid; }
    void setRVec(AsyncResult *aResult, uint32_t addr) { aResult->rvec_addr = addr; }
    std::vector<uint32_t> &getRVec(AsyncClientClass *aClient) { return aClient->getResultList(); }
    app_log_t *getAppDebug(AsyncClientClass *aClient) { return &aClient->getAppDebug(); }
    void resultSetDebug(AsyncResult *aResult, app_log_t *debug_log) { aResult->debug_log = debug_log; }
    void resultSetEvent(AsyncResult *aResult, app_log_t *event_log) { aResult->event_log = event_log; }
    app_log_t *getAppEvent(AsyncClientClass *aClient) { return &aClient->getAppEvent(); }
    void stopAsync(AsyncClientClass *aClient) { aClient->stop(); }
    async_data *createSlotBase(AsyncClientClass *aClient, slot_options_t &soption)
    {
        if (aClient)
            return aClient->createSlot(soption);
        return nullptr;
    }
    AsyncResult *getResultBase(AsyncClientClass *aClient) { return aClient->getResult(); }
    void newRequestBase(AsyncClientClass *aClient, async_data *sData, const String &url, const String &path, const String &extras, reqns::http_request_method method, const slot_options_t &options, const String &uid, const String &etag) { aClient->newRequest(sData, url, path, extras, method, options, uid, etag); }
    void setAuthTsBase(AsyncClientClass *aClient, uint32_t ts) { aClient->auth_ts = ts; }
    void addRemoveClientVecBase(AsyncClientClass *aClient, uint32_t cvec_addr, bool add) { aClient->addRemoveClientVec(cvec_addr, add); }
    void handleRemoveBase(AsyncClientClass *aClient) { aClient->handleRemove(); }
    void removeSlotBase(AsyncClientClass *aClient, uint8_t slot, bool sse = true) { aClient->removeSlot(slot, sse); }
    size_t slotCountBase(const AsyncClientClass *aClient) { return aClient->slotCount(); }
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
    uint32_t cVecAddr(T &app) { return reinterpret_cast<uint32_t>(&app.cVec); }
    template <typename T>
    void setAppBase(T &app, uint32_t app_addr, auth_data_t *auth_data, uint32_t avec_addr, uint32_t app_ota_status_addr, uint32_t cvec_address_list_addr, uint32_t app_loop_count_addr) { app.setApp(app_addr, auth_data, avec_addr, app_ota_status_addr, cvec_address_list_addr, app_loop_count_addr); }
};

#endif