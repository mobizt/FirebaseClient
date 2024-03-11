/**
 * Created February 9, 2024
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
     * @param message The Message::Message object that holds the information to send.
     * @return Boolean type status indicates the success of the operation.
     *
     * This function requires ServiceAuth authentication.
     *
     * Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages
     */
    bool send(AsyncClientClass &aClient, const Message::Message &message)
    {
        AsyncResult result;
        sendRequest(aClient, &result, NULL, "", message.c_str(), Message::firebase_cloud_messaging_request_type_send, false);
        return result.lastError.code() == 0;
    }

    /** Send a message to specified target (a registration token, topic or condition) with HTTP v1 API.
     *
     * @param aClient The async client.
     * @param message The Message::Message object that holds the information to send.
     * @param aResult The async result (AsyncResult).
     *
     * This function requires ServiceAuth authentication.
     *
     * Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages
     */
    void send(AsyncClientClass &aClient, const Message::Message &message, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", message.c_str(), Message::firebase_cloud_messaging_request_type_send, true);
    }

    /** Send a message to specified target (a registration token, topic or condition) with HTTP v1 API.
     *
     * @param aClient The async client.
     * @param message The Message::Message object that holds the information to send.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth authentication.
     *
     * Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages
     */
    void send(AsyncClientClass &aClient, const Message::Message &message, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, message.c_str(), Message::firebase_cloud_messaging_request_type_send, true);
    }

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const String &payload, Message::firebase_cloud_messaging_request_type requestType, bool async)
    {
        Message::FirebaseCloudMessagingOptions options;
        options.requestType = requestType;
        options.payload = payload;

        if (requestType == Message::firebase_cloud_messaging_request_type_send)
        {
            options.extras += FPSTR("fcm/send");
        }
        else if (requestType == Message::firebase_cloud_messaging_request_type_subscribe)
        {
            options.extras += FPSTR("/iid/v1:batchAdd");
        }
        else if (requestType == Message::firebase_cloud_messaging_request_type_undefined)
        {
            options.extras += FPSTR("/iid/v1:batchRemove");
        }
        else if (requestType == Message::firebase_cloud_messaging_request_type_app_instance_info)
        {
            options.extras += FPSTR("/iid/info/");
            options.extras += payload;
            options.extras += FPSTR("?details=true");
        }
        else if (requestType == Message::firebase_cloud_messaging_request_type_apn_token_registration)
        {
            options.extras += FPSTR("/iid/v1:batchImport");
        }

        Message::async_request_data_t aReq(&aClient, path, options.requestType == Message::firebase_cloud_messaging_request_type_app_instance_info ? async_request_handler_t::http_get : async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void asyncRequest(Message::async_request_data_t &request, int beta = 0)
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

        addParams(request, extras);

        url(FPSTR("fcm.googleapis.com"));

        async_data_item_t *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        request.aClient->newRequest(sData, service_url, request.path, extras, request.method, request.opt, request.uid);

        if (request.options->payload.length())
        {
            sData->request.payload = request.options->payload;
            request.aClient->setContentLength(sData, request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        if (request.aResult)
            sData->setRefResult(request.aResult);

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void setClientError(Message::async_request_data_t &request, int code)
    {
        AsyncResult *aResult = request.aResult;

        if (!aResult)
            aResult = new AsyncResult();

        aResult->error_available = true;
        aResult->lastError.setClientError(code);

        if (request.cb)
            request.cb(*aResult);

        if (!request.aResult)
            delete aResult;
    }

    void addParams(Message::async_request_data_t &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }
};

#endif

#endif