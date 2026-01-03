/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef MESSAGING_MESSAGING_BASE_H
#define MESSAGING_MESSAGING_BASE_H

#if defined(ENABLE_MESSAGING)
#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./messaging/DataOptions.h"

class MessagingBase : public AppBase
{
    friend class AppBase;
    friend class Messaging;
    friend class MessagingInstance;

public:
    ~MessagingBase() {}

    explicit MessagingBase(const String &url = "") { this->service_url = url; }

    MessagingBase &operator=(const MessagingBase &rhs)
    {
        this->service_url = rhs.service_url;
        this->app_token = rhs.app_token;
        return *this;
    }

    /**
     * Unbind or remove the FirebaseApp
     */
    void resetApp() { resetAppImpl(); }

    /**
     * Perform the async task repeatedly.
     * Should be placed in main loop function.
     */
    void loop() { loopImpl(); }

private:
    String path, uid;

    AsyncResult *sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Messages::Parent &parent, const String &payload, Messages::firebase_cloud_messaging_request_type requestType, bool async)
    {
        using namespace Messages;
        DataOptions options;
        options.requestType = requestType;
        options.parent = parent;
        if (requestType == fcm_send)
        {
            JSONUtil jut;
            jut.addObject(options.payload, "message", payload, false, true);
            options.extras += "/messages:send";
        }

        req_data aReq(&aClient, path, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *sendIIDAPIRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const String &var1, const object_t *var2, bool var3, Messages::firebase_cloud_messaging_request_type requestType, bool async)
    {
        using namespace Messages;
        DataOptions options;
        options.requestType = requestType;
        if (requestType == fcm_instance_batch_add || requestType == fcm_instance_batch_remove)
        {
            JSONUtil jut;
            jut.addObject(options.payload, "to", String("/topics/" + var1), true, false);
            jut.addObject(options.payload, "registration_tokens", var2->c_str(), false, true);
            options.extras += requestType == fcm_instance_batch_add ? "/v1:batchAdd" : "/v1:batchRemove";
        }
        else if (requestType == fcm_instance_batch_import)
        {
            JSONUtil jut;
            jut.addObject(options.payload, "application", var1, true, false);
            jut.addObject(options.payload, "sandbox", var3 ? "true" : "false", false, false);
            jut.addObject(options.payload, "apns_tokens", var2->c_str(), false, true);
            options.extras += "/v1:batchImport";
        }
        else if (requestType == fcm_instance_get_info)
        {
            options.extras += "/info/";
            options.extras += var1;
            options.extras += "?details=true";
        }

        req_data aReq(&aClient, path, requestType == fcm_instance_get_info ? reqns::http_get : reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    void asyncRequest(Messages::req_data &request, int beta = 0)
    {
        using namespace Messages;
        app_token_t *atoken = appToken();

        if (!atoken)
            return request.aClient->setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        request.opt.user_auth = user_auth;
        String extras;

        if (request.options->requestType == fcm_send)
        {
            sut.printTo(request.path, 20, "/v1%s%s/projects/", beta == 0 ? "" : "beta", beta == 0 ? "" : String(beta).c_str());

            request.path += request.options->parent.getProjectId().length() == 0 ? atoken->val[app_tk_ns::pid] : request.options->parent.getProjectId();

            sut.addParams(request.options->extras, extras);

            url("fcm.googleapis.com");
        }
        else
        {
            request.path = "/iid";
            sut.addParams(request.options->extras, extras);
            url("iid.googleapis.com");
        }

        async_data *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return request.aClient->setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        request.aClient->newRequest(sData, service_url, request.path, extras, request.method, request.opt, request.uid, "");

        if (request.options->payload.length())
        {
            sData->request.val[reqns::payload] = request.options->payload;
            sData->request.addContentType("application/json");
            sData->request.setContentLengthFinal(request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        request.aClient->addRemoveClientVec(reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(request.aClient->getResultList())));

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }
};

#endif
#endif