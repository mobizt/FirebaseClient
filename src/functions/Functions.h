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
#ifndef ASYNC_FUNCTIONS_H
#define ASYNC_FUNCTIONS_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./functions/DataOptions.h"

using namespace firebase;

#if defined(ENABLE_FUNCTIONS)

class CloudFunctions
{

    friend class AppBase;

public:
    std::vector<uint32_t> cVec; // AsyncClient vector

    ~CloudFunctions() {}
    explicit CloudFunctions(const String &url = "")
    {
        this->service_url = url;
    };

    CloudFunctions &operator=(const CloudFunctions &rhs)
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

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function to create.
     * @param function The GoogleCloudFunctions::Function object that holds the information of function to create.
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::Function &function)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_create, function.c_str(), false);
        return aClient.getResult()->c_str();
    }

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function to create.
     * @param function The GoogleCloudFunctions::Function object that holds the information of function to create.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::Function &function, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_create, function.c_str(), true);
    }

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
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
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_create, function.c_str(), true);
    }

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param function The GoogleCloudFunctions::Function object that holds the information of function to update.
     * @param updateMask The list of fields to be updated. If no field mask is provided, all provided fields in the request will be updated.
     * This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::Function &function, const String &updateMask)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, updateMask, GoogleCloudFunctions::google_cloud_functions_request_type_patch, function.c_str(), false);
        return aClient.getResult()->c_str();
    }

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
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
    void patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::Function &function, const String &updateMask, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, updateMask, GoogleCloudFunctions::google_cloud_functions_request_type_patch, function.c_str(), true);
    }

    /** Creates a new function.
     * If a function with the given name already exists in the specified project,
     * the long running operation will return ALREADY_EXISTS error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
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
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, updateMask, GoogleCloudFunctions::google_cloud_functions_request_type_patch, function.c_str(), true);
    }

    /** Deletes a function with the given name from the specified project. If the given function is used by some trigger, the trigger will be updated to remove this function.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_delete, "", false);
        return aClient.getResult()->c_str();
    }

    /** Deletes a function with the given name from the specified project. If the given function is used by some trigger, the trigger will be updated to remove this function.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_delete, "", true);
    }

    /** Deletes a function with the given name from the specified project. If the given function is used by some trigger, the trigger will be updated to remove this function.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_delete, "", true);
    }

    /** Returns a list of functions that belong to the requested project.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param listOptions The GoogleCloudFunctions::ListOptions object that holds the query parameters e.g. pageSize, pageToken, filter, and orderBy.
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", &listOptions, "", GoogleCloudFunctions::google_cloud_functions_request_type_list, "", false);
        return aClient.getResult()->c_str();
    }

    /** Returns a list of functions that belong to the requested project.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param listOptions The GoogleCloudFunctions::ListOptions object that holds the query parameters e.g. pageSize, pageToken, filter, and orderBy.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", &listOptions, "", GoogleCloudFunctions::google_cloud_functions_request_type_list, "", true);
    }

    /** Returns a list of functions that belong to the requested project.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param listOptions The GoogleCloudFunctions::ListOptions object that holds the query parameters e.g. pageSize, pageToken, filter, and orderBy.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", &listOptions, "", GoogleCloudFunctions::google_cloud_functions_request_type_list, "", true);
    }

    /** Returns a function with the given name from the requested project.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_get, "", false);
        return aClient.getResult()->c_str();
    }

    /** Returns a function with the given name from the requested project.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_get, "", true);
    }

    /** Returns a function with the given name from the requested project.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_get, "", true);
    }

    /** Synchronously invokes a deployed Cloud Function. To be used for testing purposes as very limited traffic is allowed. For more information on the actual limits, refer to Rate Limits (https://cloud.google.com/functions/quotas#rate_limits).
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param payload The Input to be passed to the function.
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_call, payload, false);
        return aClient.getResult()->c_str();
    }

    /** Synchronously invokes a deployed Cloud Function. To be used for testing purposes as very limited traffic is allowed. For more information on the actual limits, refer to Rate Limits (https://cloud.google.com/functions/quotas#rate_limits).
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param payload The Input to be passed to the function.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_call, payload, true);
    }

    /** Synchronously invokes a deployed Cloud Function. To be used for testing purposes as very limited traffic is allowed. For more information on the actual limits, refer to Rate Limits (https://cloud.google.com/functions/quotas#rate_limits).
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param payload The Input to be passed to the function.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_call, payload, true);
    }

    /** Returns a signed URL for downloading deployed function source code. The URL is only valid for a limited period and should be used within 30 minutes of generation. For more information about the signed URL usage see: https://cloud.google.com/storage/docs/access-control/signed-urls
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_gen_downloadUrl, "", false);
        return aClient.getResult()->c_str();
    }

    /** Returns a signed URL for downloading deployed function source code. The URL is only valid for a limited period and should be used within 30 minutes of generation. For more information about the signed URL usage see: https://cloud.google.com/storage/docs/access-control/signed-urls
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_gen_downloadUrl, "", true);
    }

    /** Returns a signed URL for downloading deployed function source code. The URL is only valid for a limited period and should be used within 30 minutes of generation. For more information about the signed URL usage see: https://cloud.google.com/storage/docs/access-control/signed-urls
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_gen_downloadUrl, "", true);
    }

    /** RReturns a signed URL for uploading a function source code. For more information about the signed URL usage see: https://cloud.google.com/storage/docs/access-control/signed-urls. Once the function source code upload is complete, the used signed URL should be provided in functions.create or functions.patch request as a reference to the function source code.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param options The GoogleCloudFunctions::UploadURLOptions object that provides the kmsKeyName and environment options.
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::UploadURLOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_gen_uploadUrl, options.c_str(), false);
        return aClient.getResult()->c_str();
    }

    /** Returns a signed URL for uploading a function source code. For more information about the signed URL usage see: https://cloud.google.com/storage/docs/access-control/signed-urls. Once the function source code upload is complete, the used signed URL should be provided in functions.create or functions.patch request as a reference to the function source code.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param options The GoogleCloudFunctions::UploadURLOptions object that provides the kmsKeyName and environment options.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::UploadURLOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_gen_uploadUrl, options.c_str(), true);
    }

    /** Returns a signed URL for uploading a function source code. For more information about the signed URL usage see: https://cloud.google.com/storage/docs/access-control/signed-urls. Once the function source code upload is complete, the used signed URL should be provided in functions.create or functions.patch request as a reference to the function source code.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param options The GoogleCloudFunctions::UploadURLOptions object that provides the kmsKeyName and environment options.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::UploadURLOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_gen_uploadUrl, options.c_str(), true);
    }

    /** Gets the access control policy for a resource. Returns an empty policy if the resource exists and does not have a policy set.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param options The GoogleCloudFunctions::GetPolicyOptions options that provides requestedPolicyVersion option.
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_get_iam_policy, options.c_str(), false);
        return aClient.getResult()->c_str();
    }

    /** Gets the access control policy for a resource. Returns an empty policy if the resource exists and does not have a policy set.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param options The GoogleCloudFunctions::GetPolicyOptions options that provides requestedPolicyVersion option.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_get_iam_policy, options.c_str(), true);
    }

    /** Gets the access control policy for a resource. Returns an empty policy if the resource exists and does not have a policy set.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param options The GoogleCloudFunctions::GetPolicyOptions options that provides requestedPolicyVersion option.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_get_iam_policy, options.c_str(), true);
    }

    /** Sets the access control policy on the specified resource. Replaces any existing policy.
     * Can return NOT_FOUND, INVALID_ARGUMENT, and PERMISSION_DENIED errors.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param options The GoogleCloudFunctions::SetPolicyOptions options that provides Policy and updateMask settings.
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::SetPolicyOptions &options)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_set_iam_policy, options.c_str(), false);
        return aClient.getResult()->c_str();
    }

    /** Sets the access control policy on the specified resource. Replaces any existing policy.
     * Can return NOT_FOUND, INVALID_ARGUMENT, and PERMISSION_DENIED errors.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param options The GoogleCloudFunctions::SetPolicyOptions options that provides Policy and updateMask settings.
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::SetPolicyOptions &options, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_set_iam_policy, options.c_str(), true);
    }

    /** Sets the access control policy on the specified resource. Replaces any existing policy.
     * Can return NOT_FOUND, INVALID_ARGUMENT, and PERMISSION_DENIED errors.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param options The GoogleCloudFunctions::SetPolicyOptions options that provides Policy and updateMask settings.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::SetPolicyOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_set_iam_policy, options.c_str(), true);
    }

    /** Returns permissions that a caller has on the specified resource. If the resource does not exist, this will return an empty set of permissions, not a NOT_FOUND error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param permissions The GoogleCloudFunctions::Permissions object that holds the list of permission string.
     * For the list of permissions, see https://cloud.google.com/functions/docs/reference/iam/permissions
     * @return String The response payload.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    String testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::Permissions &permissions)
    {
        sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_test_iam_permission, permissions.c_str(), false);
        return aClient.getResult()->c_str();
    }

    /** Returns permissions that a caller has on the specified resource. If the resource does not exist, this will return an empty set of permissions, not a NOT_FOUND error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param permissions The GoogleCloudFunctions::Permissions object that holds the list of permission string.
     * For the list of permissions, see https://cloud.google.com/functions/docs/reference/iam/permissions
     * @param aResult The async result (AsyncResult).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::Permissions &permissions, AsyncResult &aResult)
    {
        sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_test_iam_permission, permissions.c_str(), true);
    }

    /** Returns permissions that a caller has on the specified resource. If the resource does not exist, this will return an empty set of permissions, not a NOT_FOUND error.
     *
     * @param aClient The async client.
     * @param parent The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The location name is the project location.
     * @param functionId The name of function.
     * @param permissions The GoogleCloudFunctions::Permissions object that holds the list of permission string.
     * For the list of permissions, see https://cloud.google.com/functions/docs/reference/iam/permissions
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    void testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const GoogleCloudFunctions::Permissions &permissions, AsyncResultCallback cb, const String &uid = "")
    {
        sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::google_cloud_functions_request_type_test_iam_permission, permissions.c_str(), true);
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

    void sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const GoogleCloudFunctions::Parent &parent, file_config_data *file, const String &functionId, const GoogleCloudFunctions::ListOptions *listOptions, const String &updateMask, GoogleCloudFunctions::google_cloud_functions_request_type requestType, const String &payload, bool async)
    {
        GoogleCloudFunctions::DataOptions options;
        options.requestType = requestType;
        options.parent = parent;
        options.parent.setFunctionId(functionId);

        bool hasParam = false;
        URLUtil uut;

        async_request_handler_t::http_request_method method = async_request_handler_t::http_post;

        if (payload.length())
        {
            if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_call)
                options.payload = "{\"data\":\"" + payload + "\"}";
            else if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_get_iam_policy)
                options.extras = "?" + payload;
            else
                options.payload = payload;
        }

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
        else if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_delete)
        {
            method = async_request_handler_t::http_delete;
        }
        else if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_get ||
                 requestType == GoogleCloudFunctions::google_cloud_functions_request_type_list ||
                 requestType == GoogleCloudFunctions::google_cloud_functions_request_type_get_iam_policy)
        {
            method = async_request_handler_t::http_get;
        }

        if (listOptions && strlen(listOptions->c_str()))
        {
            options.extras += options.extras.length() ? '&' : '?';
            options.extras += listOptions->c_str();
            hasParam = true;
        }

        if (updateMask.length())
            uut.addParamsTokens(options.extras, "updateMask=", updateMask, hasParam);

        GoogleCloudFunctions::async_request_data_t aReq(&aClient, path, method, slot_options_t(false, false, async, false, false, false), &options, file, result, cb, uid);

        asyncRequest(aReq, requestType);
    }

    void asyncRequest(GoogleCloudFunctions::async_request_data_t &request, GoogleCloudFunctions::google_cloud_functions_request_type requestType)
    {
        app_token_t *atoken = appToken();

        if (!atoken)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        String extras;

        request.path = "/v";
        request.path += requestType == GoogleCloudFunctions::google_cloud_functions_request_type_call ? 1 : 2;
        request.path += "/projects/";
        request.path += request.options->parent.getProjectId();
        request.path += "/locations/";
        request.path += request.options->parent.getLocationId();
        request.path += "/functions";

        if ((request.method == async_request_handler_t::http_get && request.options->parent.getFunctionId().length() > 0) ||
            requestType == GoogleCloudFunctions::google_cloud_functions_request_type_call ||
            requestType == GoogleCloudFunctions::google_cloud_functions_request_type_gen_downloadUrl ||
            request.method == async_request_handler_t::http_patch ||
            request.method == async_request_handler_t::http_delete ||
            requestType == GoogleCloudFunctions::google_cloud_functions_request_type_get_iam_policy ||
            requestType == GoogleCloudFunctions::google_cloud_functions_request_type_set_iam_policy ||
            requestType == GoogleCloudFunctions::google_cloud_functions_request_type_test_iam_permission)
        {
            URLUtil uut;
            request.path += "/";
            request.path += uut.encode(request.options->parent.getFunctionId());
            if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_call)
                request.path += ":call";
            else if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_gen_downloadUrl)
                request.path += ":generateDownloadUrl";
            else if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_get_iam_policy)
                request.path += ":getIamPolicy";
            else if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_set_iam_policy)
                request.path += ":setIamPolicy";
            else if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_test_iam_permission)
                request.path += ":testIamPermissions";
        }

        if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_gen_uploadUrl)
            request.path += ":generateUploadUrl";

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

        if (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_gen_downloadUrl ||
            (requestType == GoogleCloudFunctions::google_cloud_functions_request_type_gen_uploadUrl && request.options->payload.length() == 0))
            request.aClient->setContentLength(sData, 0);

        if (request.cb)
            sData->cb = request.cb;

        request.aClient->addRemoveClientVec(reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(request.aClient->rVec)));

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void setClientError(GoogleCloudFunctions::async_request_data_t &request, int code)
    {
        AsyncResult *aResult = request.aResult;

        if (!aResult)
            aResult = new AsyncResult();

        aResult->error().setClientError(code);

        if (request.cb)
            request.cb(*aResult);

        if (!request.aResult)
        {
            delete aResult;
            aResult = nullptr;
        }
    }

    void addParams(const GoogleCloudFunctions::async_request_data_t &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }

    void setFileStatus(async_data_item_t *sData, const GoogleCloudFunctions::async_request_data_t &request)
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