/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FUNCTIONS_FUNCTIONS_H
#define FUNCTIONS_FUNCTIONS_H

#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./functions/DataOptions.h"

using namespace firebase_ns;

#if defined(ENABLE_FUNCTIONS)
class CloudFunctions : public AppBase
{
    friend class AppBase;

public:
    using Parent = GoogleCloudFunctions::Parent;
    using Function = GoogleCloudFunctions::Function;
    using ListOptions = GoogleCloudFunctions::ListOptions;

    ~CloudFunctions() {}

    CloudFunctions &operator=(const CloudFunctions &rhs)
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
     * Perform the async task repeatedly (DEPRECATED).
     */
    void loop() { loopImpl(); }

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
    String create(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const Function &function) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_create, function.c_str(), false)->c_str(); }

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
    void create(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const Function &function, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_create, function.c_str(), true); }

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
    void create(AsyncClientClass &aClient, const Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_create, function.c_str(), true); }

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
    String patch(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const Function &function, const String &updateMask) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, updateMask, GoogleCloudFunctions::fn_patch, function.c_str(), false)->c_str(); }

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
    void patch(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const Function &function, const String &updateMask, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, updateMask, GoogleCloudFunctions::fn_patch, function.c_str(), true); }

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
    void patch(AsyncClientClass &aClient, const Parent &parent, const String &functionId, Function &function, const String &updateMask, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, updateMask, GoogleCloudFunctions::fn_patch, function.c_str(), true); }

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
    String deleteFunction(AsyncClientClass &aClient, const Parent &parent, const String &functionId) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_delete, "", false)->c_str(); }

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
    void deleteFunction(AsyncClientClass &aClient, const Parent &parent, const String &functionId, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_delete, "", true); }

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
    void deleteFunction(AsyncClientClass &aClient, const Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_delete, "", true); }

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
    String list(AsyncClientClass &aClient, const Parent &parent, ListOptions &listOptions) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", &listOptions, "", GoogleCloudFunctions::fn_list, "", false)->c_str(); }

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
    void list(AsyncClientClass &aClient, const Parent &parent, ListOptions &listOptions, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", &listOptions, "", GoogleCloudFunctions::fn_list, "", true); }

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
    void list(AsyncClientClass &aClient, const Parent &parent, ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", &listOptions, "", GoogleCloudFunctions::fn_list, "", true); }

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
    String get(AsyncClientClass &aClient, const Parent &parent, const String &functionId) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, "", nullptr, "", GoogleCloudFunctions::fn_get, "", false)->c_str(); }

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
    void get(AsyncClientClass &aClient, const Parent &parent, const String &functionId, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, "", nullptr, "", GoogleCloudFunctions::fn_get, "", true); }

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
    void get(AsyncClientClass &aClient, const Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, "", nullptr, "", GoogleCloudFunctions::fn_get, "", true); }

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
    String call(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const String &payload) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_call, payload, false)->c_str(); }

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
    void call(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const String &payload, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_call, payload, true); }

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
    void call(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const String &payload, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_call, payload, true); }

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
    String generateDownloadURL(AsyncClientClass &aClient, const Parent &parent, const String &functionId) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_gen_downloadUrl, "", false)->c_str(); }

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
    void generateDownloadURL(AsyncClientClass &aClient, const Parent &parent, const String &functionId, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_gen_downloadUrl, "", true); }

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
    void generateDownloadURL(AsyncClientClass &aClient, const Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_gen_downloadUrl, "", true); }

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
    String generateUploadURL(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const GoogleCloudFunctions::UploadURLOptions &options) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_gen_uploadUrl, options.c_str(), false)->c_str(); }

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
    void generateUploadURL(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const GoogleCloudFunctions::UploadURLOptions &options, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_gen_uploadUrl, options.c_str(), true); }

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
    void generateUploadURL(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const GoogleCloudFunctions::UploadURLOptions &options, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_gen_uploadUrl, options.c_str(), true); }

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
    String getIamPolicy(AsyncClientClass &aClient, const Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_get_iam_policy, options.c_str(), false)->c_str(); }

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
    void getIamPolicy(AsyncClientClass &aClient, const Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_get_iam_policy, options.c_str(), true); }

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
    void getIamPolicy(AsyncClientClass &aClient, const Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_get_iam_policy, options.c_str(), true); }

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
    String setIamPolicy(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const GoogleCloudFunctions::SetPolicyOptions &options) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_set_iam_policy, options.c_str(), false)->c_str(); }

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
    void setIamPolicy(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const GoogleCloudFunctions::SetPolicyOptions &options, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_set_iam_policy, options.c_str(), true); }

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
    void setIamPolicy(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const GoogleCloudFunctions::SetPolicyOptions &options, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_set_iam_policy, options.c_str(), true); }

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
    String testIamPermissions(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const GoogleCloudFunctions::Permissions &permissions) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_test_iam_permission, permissions.c_str(), false)->c_str(); }

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
    void testIamPermissions(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const GoogleCloudFunctions::Permissions &permissions, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_test_iam_permission, permissions.c_str(), true); }

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
    void testIamPermissions(AsyncClientClass &aClient, const Parent &parent, const String &functionId, const GoogleCloudFunctions::Permissions &permissions, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, nullptr, functionId, nullptr, "", GoogleCloudFunctions::fn_test_iam_permission, permissions.c_str(), true); }

private:
    AsyncResult *sendRequest(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, file_config_data *file, const String &functionId, const ListOptions *listOptions, const String &updateMask, GoogleCloudFunctions::google_cloud_functions_request_type requestType, const String &payload, bool async)
    {
        using namespace GoogleCloudFunctions;
        DataOptions options;
        options.requestType = requestType;
        options.parent = parent;
        options.parent.setFunctionId(functionId);
        bool hasParam = false;
        reqns::http_request_method method = reqns::http_post;

        if (payload.length())
        {
            if (requestType == fn_call)
                options.payload = "{\"data\":\"" + payload + "\"}";
            else if (requestType == fn_get_iam_policy)
                options.extras = "?" + payload;
            else
                options.payload = payload;
        }

        switch (requestType)
        {
        case fn_create:
            hasParam = true;
            method = reqns::http_post;
            options.extras = "?functionId=";
            options.extras += functionId;
            break;

        case fn_patch:
            method = reqns::http_patch;
            break;

        case fn_delete:
            method = reqns::http_delete;
            break;

        case fn_get:
        case fn_list:
        case fn_get_iam_policy:
            method = reqns::http_get;
            break;

        default:
            break;
        }

        if (listOptions && strlen(listOptions->c_str()))
        {
            uut.addUrl(options.extras, listOptions->c_str());
            hasParam = true;
        }

        if (updateMask.length())
            uut.addParamsTokens(options.extras, "updateMask=", updateMask, hasParam);

        req_data aReq(&aClient, method, slot_options_t(false, false, async, false, false, false), &options, file, result, cb, uid);
        asyncRequest(aReq, requestType);
        return aClient.getResult();
    }

    void asyncRequest(GoogleCloudFunctions::req_data &request, GoogleCloudFunctions::google_cloud_functions_request_type requestType)
    {
        using namespace GoogleCloudFunctions;
        app_token_t *atoken = appToken();

        if (!atoken)
            return request.aClient->setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        request.opt.user_auth = user_auth;
        String extras;

        sut.printTo(request.path, request.options->parent.getProjectId().length() + request.options->parent.getLocationId().length(), "/v%d/projects/%s/locations/%s/functions", requestType == fn_call ? 1 : 2, request.options->parent.getProjectId().c_str(), request.options->parent.getLocationId().c_str());

        if ((request.method == reqns::http_get && request.options->parent.getFunctionId().length() > 0) ||
            requestType == fn_call || requestType == fn_gen_downloadUrl || request.method == reqns::http_patch ||
            request.method == reqns::http_delete || requestType == fn_get_iam_policy || requestType == fn_set_iam_policy ||
            requestType == fn_test_iam_permission)
        {

            uut.addEncUrl(request.path, "/", request.options->parent.getFunctionId());

            if (requestType == fn_call)
                request.path += ":call";
            else if (requestType == fn_gen_downloadUrl)
                request.path += ":generateDownloadUrl";
            else if (requestType == fn_get_iam_policy)
                request.path += ":getIamPolicy";
            else if (requestType == fn_set_iam_policy)
                request.path += ":setIamPolicy";
            else if (requestType == fn_test_iam_permission)
                request.path += ":testIamPermissions";
        }

        if (requestType == fn_gen_uploadUrl)
            request.path += ":generateUploadUrl";

        sut.addParams(request.options->extras, extras);

        url("cloudfunctions.googleapis.com");

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

        if (requestType == fn_gen_downloadUrl || (requestType == fn_gen_uploadUrl && request.options->payload.length() == 0))
            sData->request.setContentLengthFinal(0);

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