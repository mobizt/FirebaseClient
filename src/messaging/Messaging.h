/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef MESSAGING_MESSAGING_H
#define MESSAGING_MESSAGING_H

using namespace firebase_ns;

#if defined(ENABLE_MESSAGING)
#include "MessagingBase.h"

class Messaging : public MessagingBase
{
    friend class AppBase;

public:
    ~Messaging() {}

    explicit Messaging(const String &url = "") { this->service_url = url; }

    Messaging &operator=(const Messaging &rhs)
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
    String send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message) { return sendRequest(aClient, aClient.getResult(), NULL, "", parent, message.c_str(), Messages::fcm_send, false)->c_str(); }

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
    void send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message, AsyncResult &aResult) { sendRequest(aClient, &aResult, NULL, "", parent, message.c_str(), Messages::fcm_send, true); }

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
    void send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message, AsyncResultCallback cb, const String &uid = "") { sendRequest(aClient, nullptr, cb, uid, parent, message.c_str(), Messages::fcm_send, true); }
};

#endif
#endif