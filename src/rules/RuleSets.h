/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef RULES_RULESETS_H
#define RULES_RULESETS_H

#if defined(ENABLE_RULESETS)

#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./rules/Rules.h"
#include "./core/AsyncClient/AsyncClient.h"
#include "./rules/DataOptions.h"

using namespace firebase_ns;

class Ruleset : public BaseO4
{
private:
    ObjectWriter owriter;
    BufWriter wr;
    JSONUtil jut;

public:
    Ruleset() {}
    explicit Ruleset(const Rules::Source &source, const String &attachment_point)
    {
        this->source(source);
        attachmentPoint(attachment_point);
    }
    void source(const Rules::Source &source) { wr.set<Ruleset &, Rules::Source>(*this, source, buf, bufSize, 1, __func__); }
    void attachmentPoint(const String &attachment_point) { wr.set<Ruleset &, String>(*this, attachment_point, buf, bufSize, 2, "attachment_point"); }
    const char *c_str() const { return buf[0].c_str(); }
    void clear() { owriter.clearBuf(buf, bufSize); }
};

class RuleSets : public AppBase
{
    friend class FirebaseApp;
    friend class AppBase;

private:
    struct req_data
    {
        friend class RuleSets;

    public:
        AsyncClientClass *aClient = nullptr;
        String path, uid;
        reqns::http_request_method method = reqns::http_undefined;
        slot_options_t opt;
        RulesetOptions options;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        req_data() {}
        explicit req_data(AsyncClientClass *aClient, reqns::http_request_method method, slot_options_t opt, RulesetOptions &options, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
        {
            this->aClient = aClient;
            this->method = method;
            this->opt = opt;
            this->aResult = aResult;
            this->cb = cb;
            this->uid = uid;
            this->options = options;
        }
    };

    void asyncRequest(req_data &request)
    {
        app_token_t *atoken = appToken();

        if (!atoken)
            return request.aClient->setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        request.opt.user_auth = user_auth;
        String extras;
        sut.printTo(request.path, request.options.projectId.length(), "/v1/projects/%s/rulesets", request.options.projectId.c_str());
        if (request.options.requestType == rs_get || request.options.requestType == rs_delete)
            sut.printTo(request.path, request.options.rulesetId.length(), "/%s", request.options.rulesetId.c_str());

        sut.addParams(request.options.extras, extras);

        url("firebaserules.googleapis.com");

        async_data *sData = createSlotBase(request.aClient, request.opt);

        if (!sData)
            return request.aClient->setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        newRequestBase(request.aClient, sData, service_url, request.path, extras, request.method, request.opt, request.uid, "");

        if (request.options.payload.length())
        {
            sData->request.val[reqns::payload] = request.options.payload;
            sData->request.setContentLengthFinal(request.options.payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        addRemoveClientVecBase(request.aClient, reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(getRVec(request.aClient))));

        processBase(request.aClient, sData->async);
        handleRemoveBase(request.aClient);
    }

    AsyncResult *createRuleset(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const String &projectId, const Ruleset &ruleSet, bool async)
    {
        RulesetOptions options;
        options.requestType = rs_create;
        options.projectId = projectId;
        options.payload = ruleSet.c_str();
        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *getDeleteRuleset(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const String &projectId, const String &rulesetId, firebase_rulesets_request_type type, const String &listOptions, bool async)
    {
        RulesetOptions options;
        options.requestType = type;
        options.projectId = projectId;
        options.rulesetId = rulesetId.indexOf("projects/") == -1 && rulesetId.indexOf("/rulesets/") == -1 ? rulesetId : rulesetId.substring(rulesetId.indexOf("/rulesets/") + 10, rulesetId.length());
        if (listOptions.length())
            options.extras = listOptions;
        req_data aReq(&aClient, type == rs_list || type == rs_get ? reqns::http_get : reqns::http_delete, slot_options_t(false, false, async, false, false, false), options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

public:
    explicit RuleSets() {}

    RuleSets &operator=(const RuleSets &rhs)
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

    void create(AsyncClientClass &aClient, const String &projectId, const Ruleset &ruleSet, AsyncResultCallback cb, const String &uid = "") { createRuleset(aClient, nullptr, cb, uid, projectId, ruleSet, true); }
    void create(AsyncClientClass &aClient, const String &projectId, const Ruleset &ruleSet, AsyncResult &aResult) { createRuleset(aClient, &aResult, NULL, "", projectId, ruleSet, true); }
    String create(AsyncClientClass &aClient, const String &projectId, const Ruleset &ruleSet) { return createRuleset(aClient, getResultBase(&aClient), NULL, "", projectId, ruleSet, false)->c_str(); }

    void remove(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResultCallback cb, const String &uid = "") { getDeleteRuleset(aClient, nullptr, cb, uid, projectId, rulesetId, rs_delete, "", true); }
    void remove(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResult &aResult) { getDeleteRuleset(aClient, &aResult, NULL, "", projectId, rulesetId, rs_delete, "", true); }
    String remove(AsyncClientClass &aClient, const String &projectId, const String &rulesetId) { return getDeleteRuleset(aClient, getResultBase(&aClient), NULL, "", projectId, rulesetId, rs_delete, "", false)->c_str(); }

    void get(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResultCallback cb, const String &uid = "") { getDeleteRuleset(aClient, nullptr, cb, uid, projectId, rulesetId, rs_get, "", true); }
    void get(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResult &aResult) { getDeleteRuleset(aClient, &aResult, NULL, "", projectId, rulesetId, rs_get, "", true); }
    String get(AsyncClientClass &aClient, const String &projectId, const String &rulesetId) { return getDeleteRuleset(aClient, getResultBase(&aClient), NULL, "", projectId, rulesetId, rs_get, "", false)->c_str(); }

    void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "") { getDeleteRuleset(aClient, nullptr, cb, uid, projectId, "", rs_list, listOptions.c_str(), true); }
    void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResult &aResult) { getDeleteRuleset(aClient, &aResult, NULL, "", projectId, "", rs_list, listOptions.c_str(), true); }
    String list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions) { return getDeleteRuleset(aClient, getResultBase(&aClient), NULL, "", projectId, "", rs_list, listOptions.c_str(), false)->c_str(); }
    String getSha1(String payload)
    {
        String out;
#if defined(ENABLE_JWT)
        br_sha1_context sc;
        br_sha1_init(&sc);
        br_sha1_update(&sc, payload.c_str(), payload.length());
        unsigned char tmp[20];
        br_sha1_out(&sc, tmp);
        for (int i = 0; i < 20; i++)
        {
            out += "0123456789abcdef"[tmp[i] >> 4];
            out += "0123456789abcdef"[tmp[i] & 0xf];
        }
#endif
        return out;
    }
};
#endif
#endif
