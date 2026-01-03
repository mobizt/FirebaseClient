/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FIRESTORE_FIRESTORE_BASE_H
#define FIRESTORE_FIRESTORE_BASE_H

#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./firestore/DataOptions.h"

#if defined(ENABLE_FIRESTORE)

using namespace firebase_ns;

#include "./firestore/Query.h"
class FirestoreBase : public AppBase
{
    friend class FirebaseApp;
    friend class AppBase;

public:
    using Parent = Firestore::Parent;

    ~FirestoreBase() {}

    explicit FirestoreBase(const String &url = "") { this->service_url = url; }

    FirestoreBase &operator=(const FirestoreBase &rhs)
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

protected:
    struct req_data
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path, uid;
        reqns::http_request_method method = reqns::http_undefined;
        slot_options_t opt;
        Firestore::DataOptions *options = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        req_data() {}
        explicit req_data(AsyncClientClass *aClient, reqns::http_request_method method, slot_options_t opt, Firestore::DataOptions *options, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
        {
            this->aClient = aClient;
            this->method = method;
            this->opt = opt;
            this->options = options;
            this->aResult = aResult;
            this->cb = cb;
            this->uid = uid;
        }
    };

    void asyncRequest(req_data &request, int beta = 0)
    {
        app_token_t *atoken = appToken();

        if (!atoken)
            return request.aClient->setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        request.opt.user_auth = user_auth;
        String extras;
        sut.printTo(request.path, 20, "/v1%s%s/projects/", beta == 0 ? "" : "beta", beta == 0 ? "" : String(beta).c_str());
        request.path += strlen(request.options->parent.getProjectId()) == 0 ? atoken->val[app_tk_ns::pid] : request.options->parent.getProjectId();
        request.path += "/databases";
        if (!request.options->parent.isDatabaseIdParam())
            sut.printTo(request.path, strlen(request.options->parent.getDatabaseId()) + 20, "/%s", strlen(request.options->parent.getDatabaseId()) > 0 ? request.options->parent.getDatabaseId() : "(default)");

        sut.addParams(request.options->extras, extras);

        url("firestore.googleapis.com");

        async_data *sData = createSlotBase(request.aClient, request.opt);

        if (!sData)
            return request.aClient->setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        newRequestBase(request.aClient, sData, service_url, request.path, extras, request.method, request.opt, request.uid, "");

        if (request.options->payload.length())
        {
            sData->request.val[reqns::payload] = request.options->payload;
            sData->request.setContentLengthFinal(request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        addRemoveClientVecBase(request.aClient, reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(getRVec(request.aClient))));

        processBase(request.aClient, sData->async);
        handleRemoveBase(request.aClient);
    }

    AsyncResult *eximDocs(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const EximDocumentOptions &eximOptions, bool isImport, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = isImport ? cf_import_docs : cf_export_docs;
        options.parent = parent;
        options.payload = eximOptions.c_str();
        if (!isImport)
            options.payload.replace("inputUriPrefix", "outputUriPrefix");
        options.extras += isImport ? ":importDocuments" : ":exportDocuments";
        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *manageDatabase(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const String &database, const String &key, Firestore::firestore_database_mode mode, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_manage_database;
        options.parent = parent;

        if (strlen(database.c_str()))
        {
            options.payload = database.c_str();
            if (mode == Firestore::cf_mode_create)
            {
                options.parent.setDatabaseIdParam(true);
                sut.printTo(options.extras, strlen(options.parent.getDatabaseId()), "?databaseId=%s", options.parent.getDatabaseId());
            }
        }

        if (key.length())
        {
            if (mode == Firestore::cf_mode_delete)
                options.extras += "?etag=";
            else if (mode == Firestore::cf_mode_patch)
                options.extras += "?updateMask=";
            options.extras += key;
        }

        if (mode == Firestore::cf_mode_list)
            options.parent.setDatabaseIdParam(true);

        reqns::http_request_method method = reqns::http_undefined;

        if (strlen(database.c_str()) > 0 && mode == Firestore::cf_mode_create)
            method = reqns::http_post; // create
        else if (strlen(options.parent.getDatabaseId()) && (mode == Firestore::cf_mode_delete || mode == Firestore::cf_mode_get))
            method = mode == Firestore::cf_mode_delete ? reqns::http_delete : reqns::http_get; // get index or delete by id
        else if (strlen(database.c_str()) == 0 && mode == Firestore::cf_mode_list)
            method = reqns::http_get; // list
        else if (strlen(database.c_str()) > 0 && mode == Firestore::cf_mode_patch)
            method = reqns::http_patch; // patch

        req_data aReq(&aClient, method, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *createDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const String &collectionId, const String &documentId, DocumentMask &mask, Document<Values::Value> &document, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_create_doc;
        options.parent = parent;
        options.collectionId = collectionId;
        options.documentId = documentId;
        options.payload = document.c_str();
        sut.printTo(options.extras, options.collectionId.length(), "/documents/%s", options.collectionId.c_str());
        bool hasQueryParams = false;
        uut.addParam(options.extras, "documentId", options.documentId, hasQueryParams, true);
        options.extras += mask.getQuery("mask", hasQueryParams);
        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *patchDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const String &documentPath, PatchDocumentOptions patchOptions, Document<Values::Value> &document, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_patch_doc;
        options.parent = parent;
        options.payload = document.c_str();
        sut.printTo(options.extras, documentPath.length() + strlen(patchOptions.c_str()), "/documents/%s%s", documentPath.c_str(), patchOptions.c_str());

        req_data aReq(&aClient, reqns::http_patch, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *commitDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const Writes &writes, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_commit_document;
        options.parent = parent;
        options.payload = writes.c_str();
        options.extras += "/documents:commit";
        options.payload.replace(reinterpret_cast<const char *>(RESOURCE_PATH_BASE), makeResourcePath(parent));

        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *batchWriteDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const Writes &writes, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_batch_write_doc;
        options.parent = parent;
        options.payload = writes.c_str();
        options.payload.replace(reinterpret_cast<const char *>(RESOURCE_PATH_BASE), makeResourcePath(parent));
        options.extras += "/documents:batchWrite";

        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *getDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const String &documentPath, GetDocumentOptions getOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_get_doc;
        options.parent = parent;
        sut.printTo(options.extras, documentPath.length() + strlen(getOptions.c_str()), "/documents/%s%s", documentPath.c_str(), getOptions.c_str());

        req_data aReq(&aClient, reqns::http_get, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *batchGetDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const BatchGetDocumentOptions &batchOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_batch_get_doc;
        options.parent = parent;
        options.payload = batchOptions.c_str();
        options.payload.replace(reinterpret_cast<const char *>(RESOURCE_PATH_BASE), makeResourcePath(parent));
        options.extras += "/documents:batchGet";

        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *beginTrans(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, TransactionOptions transOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_begin_transaction;
        options.parent = parent;
        JSONUtil jut;
        jut.addObject(options.payload, "options", transOptions.c_str(), false, true);
        options.extras += "/documents:beginTransaction";

        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *transRollback(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const String &transaction, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_rollback;
        options.parent = parent;
        JSONUtil jut;
        jut.addObject(options.payload, "transaction", transaction, true, true);
        options.extras += "/documents:rollback";

        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

#if defined(ENABLE_FIRESTORE_QUERY)
    AsyncResult *runQueryImpl(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const String &documentPath, const QueryOptions &queryOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_run_query;
        options.parent = parent;
        options.parent.setDocPath(documentPath);
        options.payload = queryOptions.c_str();
        sut.printTo(options.extras, documentPath.length(), "/documents/%s:runQuery", documentPath.c_str());

        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }
#endif

    AsyncResult *deleteDocBase(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const String &documentPath, Precondition currentDocument, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_delete_doc;
        options.parent = parent;
        options.parent.setDocPath(documentPath);
        const char *qr = currentDocument.getQuery("currentDocument");
        sut.printTo(options.extras, strlen(qr) + documentPath.length(), "/documents/%s%s", documentPath.c_str(), qr);

        req_data aReq(&aClient, reqns::http_delete, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *listDocs(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const String &collectionId, const ListDocumentsOptions &listDocsOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_list_doc;
        options.parent = parent;
        sut.printTo(options.extras, collectionId.length() + strlen(listDocsOptions.c_str()), "/documents/%s%s", collectionId.c_str(), listDocsOptions.c_str());

        req_data aReq(&aClient, reqns::http_get, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *listCollIds(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const String &documentPath, const ListCollectionIdsOptions &listCollectionIdsOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_list_collection;
        options.parent = parent;
        options.parent.setDocPath(documentPath);
        options.payload = listCollectionIdsOptions.c_str();
        sut.printTo(options.extras, documentPath.length(), "/documents/%s:listCollectionIds", documentPath.c_str());

        req_data aReq(&aClient, reqns::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    AsyncResult *databaseIndexManager(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const DatabaseIndex::Index &index, const String &indexId, bool deleteMode, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_create_field_index;
        options.parent = parent;
        options.payload = index.c_str();
        sut.printTo(options.extras, indexId.length(), "/indexes/%s", indexId.c_str());
        reqns::http_request_method method = reqns::http_undefined;
        if (strlen(index.c_str()) > 0)
            method = reqns::http_post; // create
        else if (indexId.length() > 0)
            method = deleteMode ? reqns::http_delete : reqns::http_get; // get index or delete by id
        if (strlen(index.c_str()) == 0 && indexId.length() == 0)
            method = reqns::http_get; // list

        req_data aReq(&aClient, method, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq, 1);
        return aClient.getResult();
    }

    AsyncResult *collectionGroupIndexManager(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Parent &parent, const CollectionGroupsIndex::Index &index, const String &collectionId, const String &indexId, bool deleteMode, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = cf_create_composite_index;
        options.parent = parent;
        options.payload = index.c_str();
        sut.printTo(options.extras, collectionId.length() + indexId.length() + 2, "/collectionGroups/%s/indexes%s%s", collectionId.c_str(), indexId.length() ? "/" : "", indexId.length() ? index.c_str() : "");
        reqns::http_request_method method = reqns::http_undefined;
        if (strlen(index.c_str()) > 0)
            method = reqns::http_post; // create
        else if (collectionId.length() > 0)
            method = deleteMode ? reqns::http_delete : reqns::http_get; // get index or delete by id
        if (strlen(index.c_str()) == 0 && indexId.length() == 0)
            method = reqns::http_get; // list

        req_data aReq(&aClient, method, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
        return aClient.getResult();
    }

    String makeResourcePath(const Parent &parent)
    {
        String str;
        sut.printTo(str, strlen(parent.getProjectId()) + strlen(parent.getDatabaseId()) + 20, "projects/%s/databases/%s/documents", parent.getProjectId(), strlen(parent.getDatabaseId()) ? parent.getDatabaseId() : "(default)");
        return str.c_str();
    }
};
#endif
#endif