/**
 * Created March 13, 2024
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
using namespace std;

using namespace firebase;

#if defined(ENABLE_MESSAGING)

#include "./messaging/DataOptions.h"

class Messaging
{
private:
    AsyncClientClass *aClient = nullptr;
    String service_url;
    String path;
    String uid;
    uint32_t app_addr = 0;
    app_token_t *app_token = nullptr;

public:
    ~Messaging(){};
    Messaging(const String &url = "")
    {
        this->service_url = url;
    };

    Messaging &operator=(Messaging &rhs)
    {
        this->service_url = rhs.service_url;
        return *this;
    }

    /**
     * Set the Messaging URL
     * @param url The Messaging URL.
     */
    void url(const String &url)
    {
        this->service_url = url;
    }

    void setApp(uint32_t app_addr, app_token_t *app_token)
    {
        this->app_addr = app_addr;
        this->app_token = app_token;
    }

    app_token_t *appToken()
    {
        List vec;
        return vec.existed(aVec, app_addr) ? app_token : nullptr;
    }

    /**
     * Perform the async task repeatedly.
     * Should be places in main loop function.
     */
    void loop()
    {
        for (size_t i = 0; i < cVec.size(); i++)
        {
            AsyncClientClass *aClient = reinterpret_cast<AsyncClientClass *>(cVec[i]);
            if (aClient)
            {
                aClient->process(true);
                aClient->handleRemove();
            }
        }
    }

    /** Send Firebase Cloud Messaging to the devices using the FCM HTTP v1 API.
     *
     * @param aClient The async client.
     * @param parent The Messages::Parent object included project Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * @param message The Messages::Message object that holds the information to send.
     * @return Boolean type status indicates the success of the operation.
     *
     * This function requires ServiceAuth authentication.
     *
     * Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages
     */
    bool send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message)
    {
        AsyncResult result;
        sendRequest(aClient, &result, NULL, "", parent, message.c_str(), Messages::firebase_cloud_messaging_request_type_send, false);
        return result.lastError.code() == 0;
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

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Messages::Parent &parent, const String &payload, Messages::firebase_cloud_messaging_request_type requestType, bool async)
    {
        Messages::DataOptions options;
        options.requestType = requestType;
        options.parent = parent;
        if (requestType == Messages::firebase_cloud_messaging_request_type_send)
        {
            JsonHelper jh;
            jh.addObject(options.payload, "message", payload, false, true);
            options.extras += FPSTR("/messages:send");
        }

        Messages::async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void asyncRequest(Messages::async_request_data_t &request, int beta = 0)
    {
        URLHelper uh;
        app_token_t *app_token = appToken();

        if (!app_token)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = app_token;
        String extras;

        if (beta == 2)
            uh.addGAPIv1beta2Path(request.path);
        else if (beta == 1)
            uh.addGAPIv1beta1Path(request.path);
        else
            uh.addGAPIv1Path(request.path);

        request.path += request.options->parent.getProjectId().length() == 0 ? app_token->val[app_tk_ns::pid] : request.options->parent.getProjectId();

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

        if (request.aResult)
            sData->setRefResult(request.aResult);

        sData->download = request.method == async_request_handler_t::http_get && sData->request.file_data.filename.length();

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void setClientError(Messages::async_request_data_t &request, int code)
    {
        AsyncResult *aResult = request.aResult;

        if (!aResult)
            aResult = new AsyncResult();

        aResult->error_available = true;
        aResult->lastError.setClientError(code);

        if (request.cb)
            request.cb(*aResult);

        if (!request.aResult)
        {
            delete aResult;
            aResult = nullptr;
        }
    }

    void addParams(Messages::async_request_data_t &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }
};

#endif

#endif