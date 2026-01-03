/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef MESSAGING_MESSAGING_INSTANCE_H
#define MESSAGING_MESSAGING_INSTANCE_H

using namespace firebase_ns;

#if defined(ENABLE_MESSAGING)
#include "MessagingBase.h"

class MessagingInstance : public MessagingBase
{
    friend class AppBase;

public:
    ~MessagingInstance() {}

    explicit MessagingInstance(const String &url = "") { this->service_url = url; }

    MessagingInstance &operator=(const MessagingInstance &rhs)
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

    /** Create relationship maps for app instances (subscribe the apps to the topic).
     *
     * @param aClient The async client.
     * @param topic The topic to subscribe.
     * @param IID The object_t object represents the instance ID tokens or registration tokens array.
     * @return String The response payload.
     *
     */
    String batchAdd(AsyncClientClass &aClient, const String &topic, const object_t &IID) { return sendIIDAPIRequest(aClient, aClient.getResult(), NULL, "", topic, &IID, false, Messages::fcm_instance_batch_add, false)->c_str(); }

    /** Create relationship maps for app instances (subscribe the apps to the topic).
     *
     * @param aClient The async client.
     * @param topic The topic to subscribe.
     * @param IID The object_t object represents the instance ID tokens or registration tokens array.
     * @param aResult The async result (AsyncResult).
     *
     */
    void batchAdd(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResult &aResult) { sendIIDAPIRequest(aClient, &aResult, NULL, "", topic, &IID, false, Messages::fcm_instance_batch_add, true); }

    /** Create relationship maps for app instances (subscribe the apps to the topic).
     *
     * @param aClient The async client.
     * @param topic The topic to subscribe.
     * @param IID The object_t object represents the instance ID tokens or registration tokens array.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void batchAdd(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResultCallback cb, const String &uid = "") { sendIIDAPIRequest(aClient, nullptr, cb, uid, topic, &IID, false, Messages::fcm_instance_batch_add, true); }

    /** Remove relationship maps for app instances (unsubscribe the apps from the topic).
     *
     * @param aClient The async client.
     * @param topic The topic to unsubscribe.
     * @param IID The object_t object represents the instance ID tokens or registration tokens array.
     * @return String The response payload.
     *
     */
    String batchRemove(AsyncClientClass &aClient, const String &topic, const object_t &IID) { return sendIIDAPIRequest(aClient, aClient.getResult(), NULL, "", topic, &IID, false, Messages::fcm_instance_batch_remove, false)->c_str(); }

    /** Remove relationship maps for app instances (unsubscribe the apps from the topic).
     *
     * @param aClient The async client.
     * @param topic The topic to unsubscribe.
     * @param IID The object_t object represents the instance ID tokens or registration tokens array.
     * @param aResult The async result (AsyncResult).
     *
     */
    void batchRemove(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResult &aResult) { sendIIDAPIRequest(aClient, &aResult, NULL, "", topic, &IID, false, Messages::fcm_instance_batch_remove, true); }

    /** Remove relationship maps for app instances (unsubscribe the apps from the topic).
     *
     * @param aClient The async client.
     * @param topic The topic to unsubscribe.
     * @param IID The object_t object represents the instance ID tokens or registration tokens array.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void batchRemove(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResultCallback cb, const String &uid = "") { sendIIDAPIRequest(aClient, nullptr, cb, uid, topic, &IID, false, Messages::fcm_instance_batch_remove, true); }

    /** Create registration tokens for APNs tokens.
     *
     * @param aClient The async client.
     * @param application The Bundle id of the app.
     * @param sandbox The Boolean to indicate sandbox environment (TRUE) or production (FALSE).
     * @param APNs The object_t object represents the iOS APNs tokens array.
     * @return String The response payload.
     *
     */
    String batchImport(AsyncClientClass &aClient, const String &application, bool sandbox, const object_t &APNs) { return sendIIDAPIRequest(aClient, aClient.getResult(), NULL, "", application, &APNs, sandbox, Messages::fcm_instance_batch_import, false)->c_str(); }

    /** Create registration tokens for APNs tokens.
     *
     * @param aClient The async client.
     * @param application The Bundle id of the app.
     * @param sandbox The Boolean to indicate sandbox environment (TRUE) or production (FALSE).
     * @param APNs The object_t object represents the iOS APNs tokens array.
     * @param aResult The async result (AsyncResult).
     *
     */
    void batchImport(AsyncClientClass &aClient, const String &application, bool sandbox, const object_t &APNs, AsyncResult &aResult) { sendIIDAPIRequest(aClient, &aResult, NULL, "", application, &APNs, sandbox, Messages::fcm_instance_batch_import, true); }

    /** Create registration tokens for APNs tokens.
     *
     * @param aClient The async client.
     * @param application The Bundle id of the app.
     * @param sandbox The Boolean to indicate sandbox environment (TRUE) or production (FALSE).
     * @param APNs The object_t object represents the iOS APNs tokens array.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void batchImport(AsyncClientClass &aClient, const String &application, bool sandbox, const object_t &APNs, AsyncResultCallback cb, const String &uid = "") { sendIIDAPIRequest(aClient, nullptr, cb, uid, application, &APNs, sandbox, Messages::fcm_instance_batch_import, true); }

    /** Get information about app instances.
     *
     * @param aClient The async client.
     * @param IID The instance ID token.
     * @return String The response payload.
     *
     */
    String get(AsyncClientClass &aClient, const String &IID) { return sendIIDAPIRequest(aClient, aClient.getResult(), NULL, "", IID, nullptr, false, Messages::fcm_instance_get_info, false)->c_str(); }

    /** Get information about app instances.
     *
     * @param aClient The async client.
     * @param IID The instance ID token.
     * @param aResult The async result (AsyncResult).
     *
     */
    void get(AsyncClientClass &aClient, const String &IID, AsyncResult &aResult) { sendIIDAPIRequest(aClient, &aResult, NULL, "", IID, nullptr, false, Messages::fcm_instance_get_info, true); }

    /** Get information about app instances.
     *
     * @param aClient The async client.
     * @param IID The instance ID token.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void get(AsyncClientClass &aClient, const String &IID, AsyncResultCallback cb, const String &uid = "") { sendIIDAPIRequest(aClient, nullptr, cb, uid, IID, nullptr, false, Messages::fcm_instance_get_info, true); }

};
#endif
#endif