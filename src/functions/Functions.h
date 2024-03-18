/**
 * Created March 18, 2024
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
#ifndef ASYNC_FUNCTIONS_H
#define ASYNC_FUNCTIONS_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
using namespace std;

using namespace firebase;

#if defined(ENABLE_FUNCTIONS)

class Functions
{
private:
    AsyncClientClass *aClient = nullptr;
    String service_url;
    String path;
    String uid;
    uint32_t app_addr = 0;
    app_token_t *app_token = nullptr;

public:
    ~Functions(){};
    Functions(const String &url = "")
    {
        this->service_url = url;
    };

    Functions &operator=(Functions &rhs)
    {
        this->service_url = rhs.service_url;
        return *this;
    }

    /**
     * Set the Functions URL
     * @param url The Functions URL.
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

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id, location Id and storage bucket Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location Id is the project location.
     * The bucket Id is the Firebase storage bucket Id in the project.
     * @param functionId The name of function to create.
     * @param function The GoogleCloudFunctions::Function object that holds the information of function to create.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void createFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, functionId, &function, "", GoogleCloudFunctions::google_cloud_functions_request_type_create, true);
    }

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id, location Id and storage bucket Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location Id is the project location.
     * The bucket Id is the Firebase storage bucket Id in the project.
     * @param functionId The name of function to create.
     * @param function The GoogleCloudFunctions::Function object that holds the information of function to create.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void createFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, functionId, &function, "", GoogleCloudFunctions::google_cloud_functions_request_type_create, true);
    }

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id, location Id and storage bucket Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location Id is the project location.
     * The bucket Id is the Firebase storage bucket Id in the project.
     * @param functionId The name of function.
     * @param function The GoogleCloudFunctions::Function object that holds the information of function to update.
     * @param updateMask The list of fields to be updated. If no field mask is provided, all provided fields in the request will be updated.
     * This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void patchFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, functionId, &function, updateMask, GoogleCloudFunctions::google_cloud_functions_request_type_patch, true);
    }

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id, location Id and storage bucket Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location Id is the project location.
     * The bucket Id is the Firebase storage bucket Id in the project.
     * @param functionId The name of function.
     * @param function The GoogleCloudFunctions::Function object that holds the information of function to update.
     * @param updateMask The list of fields to be updated. If no field mask is provided, all provided fields in the request will be updated.
     * This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void patchFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, functionId, &function, updateMask, GoogleCloudFunctions::google_cloud_functions_request_type_patch, true);
    }

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function *function, const String &updateMask, GoogleCloudFunctions::google_cloud_functions_request_type requestType, bool async)
    {
        GoogleCloudFunctions::DataOptions options;
        options.requestType = requestType;
        options.parent = parent;

        String ext;
        bool hasParam = false;
        URLHelper uh;
        if (updateMask.length())
            uh.addParamsTokens(ext, "updateMask=", updateMask, hasParam);

        async_request_handler_t::http_request_method method = async_request_handler_t::http_post;
    }
};

#endif

#endif