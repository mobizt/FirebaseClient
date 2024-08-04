/**
 * Created August 4, 2024
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
#ifndef ASYNC_MESSAGING_H
#define ASYNC_MESSAGING_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"

using namespace firebase;

#if defined(ENABLE_MESSAGING)

#include "./messaging/DataOptions.h"

class Messaging
{

    friend class AppBase;

public:
    std::vector<uint32_t> cVec; // AsyncClient vector

    ~Messaging() {}
    explicit Messaging(const String &url = "")
    {
        this->service_url = url;
    };

    Messaging &operator=(const Messaging &rhs)
    {
        this->service_url = rhs.service_url;
        this->app_token = rhs.app_token;
        return *this;
    }

    /**
     * Unbind or remove the FirebaseApp
     */
    void resetApp()
    {
        this->app_addr = 0;
        this->app_token = nullptr;
        this->avec_addr = 0; // AsyncClient vector (list) address
        this->ul_dl_task_running_addr = 0;
    }

    /**
     * Perform the async task repeatedly.
     * Should be placed in main loop function.
     */
    void loop()
    {
        for (size_t i = 0; i < cVec.size(); i++)
        {
            AsyncClientClass *client = reinterpret_cast<AsyncClientClass *>(cVec[i]);
            if (client)
            {
                client->process(true);
                client->handleRemove();
            }
        }
    }

    /** Send Firebase Cloud Messaging to the devices using the FCM HTTP v1 API.
     *
     * @param aClient The async client.
     * @param parent The Messages::Parent object included project Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * @param message The Messages::Message object that holds the information to send.
     * @return String The response payload.
     *
     * This function requires ServiceAuth authentication.
     *
     * Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages
     */
    String send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, message.c_str(), Messages::firebase_cloud_messaging_request_type_send, false);
        return aClient.getResult()->c_str();
    }

    /** Send a message to specified target (a registration token, topic or condition) with HTTP v1 API.
     *
     * @param aClient The async client.
     * @param parent The Messages::Parent object included project Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * @param message The Messages::Message object that holds the information to send.
     * @param aResult The async result (AsyncResult).
     *
     * This function requires ServiceAuth authentication.
     *
     * Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages
     */
    void send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, message.c_str(), Messages::firebase_cloud_messaging_request_type_send, true);
    }

    /** Send a message to specified target (a registration token, topic or condition) with HTTP v1 API.
     *
     * @param aClient The async client.
     * @param parent The Messages::Parent object included project Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * @param message The Messages::Message object that holds the information to send.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth authentication.
     *
     * Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages
     */
    void send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, message.c_str(), Messages::firebase_cloud_messaging_request_type_send, true);
    }

private:
    String service_url;
    String path;
    String uid;
    // FirebaseApp address and FirebaseApp vector address
    uint32_t app_addr = 0, avec_addr = 0;
    // Not used but required.
    uint32_t ul_dl_task_running_addr = 0;
    app_token_t *app_token = nullptr;

    void url(const String &url)
    {
        this->service_url = url;
    }

    void setApp(uint32_t app_addr, app_token_t *app_token, uint32_t avec_addr, uint32_t ul_dl_task_running_addr)
    {
        this->app_addr = app_addr;
        this->app_token = app_token;
        this->avec_addr = avec_addr; // AsyncClient vector (list) address
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

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Messages::Parent &parent, const String &payload, Messages::firebase_cloud_messaging_request_type requestType, bool async)
    {
        Messages::DataOptions options;
        options.requestType = requestType;
        options.parent = parent;
        if (requestType == Messages::firebase_cloud_messaging_request_type_send)
        {
            JSONUtil jut;
            jut.addObject(options.payload, "message", payload, false, true);
            options.extras += FPSTR("/messages:send");
        }

        Messages::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void asyncRequest(Messages::async_request_data_t &request, int beta = 0)
    {
        URLUtil uut;
        app_token_t *atoken = appToken();

        if (!atoken)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        String extras;

        if (beta == 2)
            uut.addGAPIv1beta2Path(request.path);
        else if (beta == 1)
            uut.addGAPIv1beta1Path(request.path);
        else
            uut.addGAPIv1Path(request.path);

        request.path += request.options->parent.getProjectId().length() == 0 ? atoken->val[app_tk_ns::pid] : request.options->parent.getProjectId();

        addParams(request, extras);

        url(FPSTR("fcm.googleapis.com"));

        async_data_item_t *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        request.aClient->newRequest(sData, service_url, request.path, extras, request.method, request.opt, request.uid);

        if (request.options->payload.length())
        {
            sData->request.val[req_hndlr_ns::payload] = request.options->payload;
            request.aClient->setContentLength(sData, request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        request.aClient->addRemoveClientVec(reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(request.aClient->rVec)));

        sData->download = request.method == async_request_handler_t::http_get && sData->request.file_data.filename.length();

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void setClientError(Messages::async_request_data_t &request, int code)
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

    void addParams(const Messages::async_request_data_t &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }
};

#endif

#endif