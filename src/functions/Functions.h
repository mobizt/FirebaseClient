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
#include "./functions/DataOptions.h"
using namespace std;

using namespace firebase;

#if defined(ENABLE_FUNCTIONS)

class CloudFunctions
{

public:
    ~CloudFunctions(){};
    CloudFunctions(const String &url = "")
    {
        this->service_url = url;
    };

    CloudFunctions &operator=(CloudFunctions &rhs)
    {
        this->service_url = rhs.service_url;
        return *this;
    }

    /**
     * Set the Cloud Functions URL
     * @param url The Cloud Functions URL.
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
     * @return Boolean type status indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    bool create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function)
    {
        AsyncResult result;
        file_config_data file;
        sendRequest(aClient, &result, NULL, "", parent, file, functionId, &function, "", GoogleCloudFunctions::google_cloud_functions_request_type_create, false);
        return result.lastError.code() == 0;
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
    void create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, AsyncResult &aResult)
    {
        file_config_data file;
        sendRequest(aClient, &aResult, NULL, "", parent, file, functionId, &function, "", GoogleCloudFunctions::google_cloud_functions_request_type_create, true);
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
    void create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, AsyncResultCallback cb, const String &uid = "")
    {
        file_config_data file;
        sendRequest(aClient, nullptr, cb, uid, parent, file, functionId, &function, "", GoogleCloudFunctions::google_cloud_functions_request_type_create, true);
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
     * @return Boolean type status indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    bool patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask)
    {
        AsyncResult result;
        file_config_data file;
        sendRequest(aClient, &result, NULL, "", parent, file, functionId, &function, updateMask, GoogleCloudFunctions::google_cloud_functions_request_type_patch, true);
        return result.lastError.code() == 0;
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
    void patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask, AsyncResult &aResult)
    {
        file_config_data file;
        sendRequest(aClient, &aResult, NULL, "", parent, file, functionId, &function, updateMask, GoogleCloudFunctions::google_cloud_functions_request_type_patch, true);
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
    void patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask, AsyncResultCallback cb, const String &uid = "")
    {
        file_config_data file;
        sendRequest(aClient, nullptr, cb, uid, parent, file, functionId, &function, updateMask, GoogleCloudFunctions::google_cloud_functions_request_type_patch, true);
    }

private:
    AsyncClientClass *aClient = nullptr;
    String service_url;
    String path;
    String uid;
    uint32_t app_addr = 0;
    app_token_t *app_token = nullptr;

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const GoogleCloudFunctions::Parent &parent, file_config_data &file, const String &functionId, GoogleCloudFunctions::Function *function, const String &updateMask, GoogleCloudFunctions::google_cloud_functions_request_type requestType, bool async)
    {
        GoogleCloudFunctions::DataOptions options;
        options.requestType = requestType;
        options.parent = parent;
        options.parent.setFunctionId(functionId);

        bool hasParam = false;
        URLHelper uh;

        async_request_handler_t::http_request_method method = async_request_handler_t::http_post;

        if (strlen(function->c_str()))
            options.payload = function->c_str();

        if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_create)
        {
            hasParam = true;
            method = async_request_handler_t::http_post;
            options.extras = "?functionId=";
            options.extras += functionId;
        }
        else if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_patch)
        {
            method = async_request_handler_t::http_patch;
        }

        if (updateMask.length())
            uh.addParamsTokens(options.extras, "updateMask=", updateMask, hasParam);

        GoogleCloudFunctions::async_request_data_t aReq(&aClient, path, method, slot_options_t(false, false, async, false, false, false), &options, &file, result, cb, uid);

        asyncRequest(aReq);
    }

    void asyncRequest(GoogleCloudFunctions::async_request_data_t &request, int beta = 0)
    {
        app_token_t *app_token = appToken();

        if (!app_token)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = app_token;
        String extras;

        request.path = "/v2/projects/";
        request.path += request.options->parent.getProjectId();
        request.path += "/locations/";
        request.path += request.options->parent.getLocationId();
        request.path += "/functions";

        if (request.method == async_request_handler_t::http_patch)
        {
            URLHelper uh;
            request.path += "/";
            request.path += uh.encode(request.options->parent.getFunctionId());
        }

        addParams(request, extras);

        url(FPSTR("cloudfunctions.googleapis.com"));

        async_data_item_t *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        request.aClient->newRequest(sData, service_url, request.path, extras, request.method, request.opt, request.uid);

        if (request.file)
            sData->request.file_data.copy(*request.file);

        setFileStatus(sData, request);

        if (request.file && sData->upload)
        {
            sData->request.base64 = false;

            if (request.mime.length())
                request.aClient->setContentType(sData, request.mime);

            request.aClient->setFileContentLength(sData, 0);

            if (sData->request.file_data.file_size == 0)
                return setClientError(request, FIREBASE_ERROR_FILE_READ);
        }
        else if (request.options->payload.length())
        {
            sData->request.val[req_hndlr_ns::payload] = request.options->payload;
            request.aClient->setContentLength(sData, request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        if (request.aResult)
            sData->setRefResult(request.aResult);

        Serial.println(sData->request.val[req_hndlr_ns::header]);
        Serial.println(sData->request.val[req_hndlr_ns::payload]);

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void setClientError(GoogleCloudFunctions::async_request_data_t &request, int code)
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

    void addParams(GoogleCloudFunctions::async_request_data_t &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }

    void setFileStatus(async_data_item_t *sData, GoogleCloudFunctions::async_request_data_t &request)
    {
        if ((request.file && request.file->filename.length()) || request.opt.ota)
        {
            sData->download = request.method == async_request_handler_t::http_get;
            sData->upload = request.method == async_request_handler_t::http_post ||
                            request.method == async_request_handler_t::http_put ||
                            request.method == async_request_handler_t::http_patch;
        }
    }
};

#endif

#endif