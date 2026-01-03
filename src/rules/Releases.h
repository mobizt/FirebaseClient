/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef RULES_RELEASES_H
#define RULES_RELEASES_H

#if defined(ENABLE_RULESETS)

#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./rules/Rules.h"
#include "./core/AsyncClient/AsyncClient.h"
#include "./rules/DataOptions.h"

/**
 * A named reference to a Ruleset.
 * Once a Release refers to a Ruleset, rules-enabled services will be able to enforce the Ruleset.
 */
class Release : public BaseO4
{
private:
    ObjectWriter owriter;
    BufWriter wr;
    JSONUtil jut;

public:
    Release() {}
    explicit Release(const String &releaseId, const String &rulesetId)
    {
        this->releaseId(releaseId);
        this->rulesetId(rulesetId);
    }
    /**
     * The release Id is the part of the release name i.e. the release name structure "projects/projectId/releaseId".
     * Release name/id should reflect the developer's deployment practices. For example, the release Id may include the environment name, application name, application version, or any other name meaningful to the developer.
     * Once a Release refers to a Ruleset, the rules can be enforced by Firebase Rules-enabled services.
     * More than one Release may be 'live' concurrently. Consider the following three Release names for projects/foo and the Ruleset to which they refer.
     * Release Name -> Ruleset Name:
     * projects/foo/releases/prod -> projects/foo/rulesets/uuid123
     * projects/foo/releases/prod/beta -> projects/foo/rulesets/uuid123
     * projects/foo/releases/prod/v23 -> projects/foo/rulesets/uuid456
     * The relationships reflect a Ruleset rollout in progress. The prod and prod/beta releases refer to the same Ruleset. However, prod/v23 refers to a new Ruleset. The Ruleset reference for a Release may be updated using the releases.patch method.
     */
    void releaseId(const String &releaseId) { wr.set<Release &, String>(*this, releaseId.indexOf("projects/") == -1 && releaseId.indexOf("/releases/") == -1 ? "projects/<projectId>/releases/" + releaseId : "projects/<projectId>/releases/" + releaseId.substring(releaseId.indexOf("/releases/") + 10, releaseId.length()), buf, bufSize, 1, "name"); }
    /**
     * The ruleset Id of the Ruleset referred to by this Release.
     * The Ruleset must exist for the Release to be created.
     */
    void rulesetId(const String &rulesetId) { wr.set<Release &, String>(*this, rulesetId.indexOf("projects/") == -1 && rulesetId.indexOf("/rulesets/") == -1 ? "projects/<projectId>/rulesets/" + rulesetId : "projects/<projectId>/rulesets/" + rulesetId.substring(rulesetId.indexOf("/rulesets/") + 10, rulesetId.length()), buf, bufSize, 2, "rulesetName"); }
    const char *c_str() const { return buf[0].c_str(); }
    void clear() { owriter.clearBuf(buf, bufSize); }
};

/**
 * The Release class for releases operations.
 */
class Releases : public AppBase
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
        ReleaseOptions options;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        req_data() {}
        explicit req_data(AsyncClientClass *aClient, reqns::http_request_method method, slot_options_t opt, ReleaseOptions &options, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
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
        sut.printTo(request.path, request.options.projectId.length(), "/v1/projects/%s/releases", request.options.projectId.c_str());
        if (request.options.requestType == rl_get || request.options.requestType == rl_get_executable || request.options.requestType == rl_delete)
            sut.printTo(request.path, request.options.releaseId.length(), "/%s", request.options.releaseId.c_str());

        sut.addParams(request.options.extras, extras);

        url("firebaserules.googleapis.com");

        async_data *sData = createSlotBase(request.aClient, request.opt);

        if (!sData)
            return request.aClient->setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        newRequestBase(request.aClient, sData, service_url, request.path, extras, request.method, request.opt, request.uid, "");

        if (request.options.payload.length())
        {
            request.options.payload.replace("<projectId>", request.options.projectId);
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

    AsyncResult *createRelease(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const String &projectId, const Release &release, bool async)
    {
        ReleaseOptions options;
        options.requestType = rl_create;
        options.projectId = projectId;
        options.payload = release.c_str();
        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *getDeleteRelease(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const String &projectId, const String &releaseId, firebase_releases_request_type type, const String &ext, bool async)
    {
        ReleaseOptions options;
        options.requestType = type;
        options.projectId = projectId;
        options.releaseId = releaseId;
        if (ext.length())
            options.extras = type == rl_get_executable ? ":getExecutable?executableVersion=" + ext : ext;
        req_data aReq(&aClient, type == rl_list || type == rl_get || type == rl_get_executable ? reqns::http_get : reqns::http_delete, slot_options_t(false, false, async, false, false, false), options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *patchRelease(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const String &projectId, const Release &release, const String &updateMask, bool async)
    {
        ReleaseOptions options;
        options.requestType = rl_patch;
        options.projectId = projectId;
        JSONUtil jut;
        jut.addObject(options.payload, "release", release.c_str(), false, updateMask.length() == 0);
        if (updateMask.length())
            jut.addObject(options.payload, "updateMask", updateMask.c_str(), true, true);
        req_data aReq(&aClient, reqns::http_patch, slot_options_t(false, false, async, false, false, false), options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

public:
    explicit Releases() {}

    Releases &operator=(const Releases &rhs)
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

    /**
     * Create a Release.
     */
    void create(AsyncClientClass &aClient, const String &projectId, const Release &release, AsyncResultCallback cb, const String &uid = "") { createRelease(aClient, nullptr, cb, uid, projectId, release, true); }
    void create(AsyncClientClass &aClient, const String &projectId, const Release &release, AsyncResult &aResult) { createRelease(aClient, &aResult, NULL, "", projectId, release, true); }
    String create(AsyncClientClass &aClient, const String &projectId, const Release &release) { return createRelease(aClient, getResultBase(&aClient), NULL, "", projectId, release, false)->c_str(); }

    /**
     * Delete a Release by resource name.
     */
    void remove(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResultCallback cb, const String &uid = "") { getDeleteRelease(aClient, nullptr, cb, uid, projectId, releaseId, rl_delete, "", true); }
    void remove(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResult &aResult) { getDeleteRelease(aClient, &aResult, NULL, "", projectId, releaseId, rl_delete, "", true); }
    String remove(AsyncClientClass &aClient, const String &projectId, const String &releaseId) { return getDeleteRelease(aClient, getResultBase(&aClient), NULL, "", projectId, releaseId, rl_delete, "", false)->c_str(); }

    /**
     * Get a Release by name.
     */
    void get(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResultCallback cb, const String &uid = "") { getDeleteRelease(aClient, nullptr, cb, uid, projectId, releaseId, rl_get, "", true); }
    void get(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResult &aResult) { getDeleteRelease(aClient, &aResult, NULL, "", projectId, releaseId, rl_get, "", true); }
    String get(AsyncClientClass &aClient, const String &projectId, const String &releaseId) { return getDeleteRelease(aClient, getResultBase(&aClient), NULL, "", projectId, releaseId, rl_get, "", false)->c_str(); }

    /**
     * Get the Release executable to use when enforcing rules.
     */
    void getExecutable(AsyncClientClass &aClient, const String &projectId, const String &releaseId, Rules::ReleaseExecutableVersion executableVersion, AsyncResultCallback cb, const String &uid = "") { getDeleteRelease(aClient, nullptr, cb, uid, projectId, releaseId, rl_get_executable, Rules::_ReleaseExecutableVersion[executableVersion].text, true); }
    void getExecutable(AsyncClientClass &aClient, const String &projectId, const String &releaseId, Rules::ReleaseExecutableVersion executableVersion, AsyncResult &aResult) { getDeleteRelease(aClient, &aResult, NULL, "", projectId, releaseId, rl_get_executable, Rules::_ReleaseExecutableVersion[executableVersion].text, true); }
    String getExecutable(AsyncClientClass &aClient, const String &projectId, const String &releaseId, Rules::ReleaseExecutableVersion executableVersion) { return getDeleteRelease(aClient, getResultBase(&aClient), NULL, "", projectId, releaseId, rl_get_executable, Rules::_ReleaseExecutableVersion[executableVersion].text, false)->c_str(); }

    /**
     * List the Release values for a project.
     * This list may optionally be filtered by Release name, Ruleset name, TestSuite name, or any combination thereof.
     */
    void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "") { getDeleteRelease(aClient, nullptr, cb, uid, projectId, "", rl_list, listOptions.c_str(), true); }
    void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResult &aResult) { getDeleteRelease(aClient, &aResult, NULL, "", projectId, "", rl_list, listOptions.c_str(), true); }
    String list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions) { return getDeleteRelease(aClient, getResultBase(&aClient), NULL, "", projectId, "", rl_list, listOptions.c_str(), false)->c_str(); }

    /**
     * Update a Release via PATCH.
     * Only updates to rulesetName will be honored. Release rename is not supported. To create a Release use the releases.create method.
     */
    void patch(AsyncClientClass &aClient, const String &projectId, const Release &release, const String &updateMask, AsyncResultCallback cb, const String &uid = "") { patchRelease(aClient, nullptr, cb, uid, projectId, release, updateMask, true); }
    void patch(AsyncClientClass &aClient, const String &projectId, const Release &release, const String &updateMask, AsyncResult &aResult) { patchRelease(aClient, &aResult, NULL, "", projectId, release, updateMask, true); }
    String patch(AsyncClientClass &aClient, const String &projectId, const Release &release, const String &updateMask) { return patchRelease(aClient, getResultBase(&aClient), NULL, "", projectId, release, updateMask, false)->c_str(); }
};

#endif
#endif