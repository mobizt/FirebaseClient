/**
 * Created June 28, 2024
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
#ifndef ASYNC_FIRESTORE_BASE_H
#define ASYNC_FIRESTORE_BASE_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./firestore/DataOptions.h"

#if defined(ENABLE_FIRESTORE)

using namespace firebase;

#include "./firestore/Query.h"

class FirestoreBase : public AppBase
{
    friend class FirebaseApp;
    friend class AppBase;

public:
    std::vector<uint32_t> cVec; // AsyncClient vector

    ~FirestoreBase() {}

    explicit FirestoreBase(const String &url = "")
    {
        this->service_url = url;
    };

    FirestoreBase &operator=(const FirestoreBase &rhs)
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
                processBase(client, true);
                handleRemoveBase(client);
            }
        }
    }

protected:
    String service_url;
    String path;
    String uid;
    // FirebaseApp address and FirebaseApp vector address
    uint32_t app_addr = 0, avec_addr = 0;
    uint32_t ul_dl_task_running_addr = 0;
    app_token_t *app_token = nullptr;

    struct async_request_data_t
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path;
        String uid;
        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;
        slot_options_t opt;
        Firestore::DataOptions *options = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        async_request_data_t() {}
        explicit async_request_data_t(AsyncClientClass *aClient, const String &path, async_request_handler_t::http_request_method method, slot_options_t opt, Firestore::DataOptions *options, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
        {
            this->aClient = aClient;
            this->path = path;
            this->method = method;
            this->opt = opt;
            this->options = options;
            this->aResult = aResult;
            this->cb = cb;
            this->uid = uid;
        }
    };

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

    void asyncRequest(async_request_data_t &request, int beta = 0)
    {
        URLUtil uut;
        app_token_t *atoken = appToken();

        if (!atoken)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        String extras;
        if (beta == 2)
            uut.addGAPIv1beta2Path(request.path);
        else if (beta == 1)
            uut.addGAPIv1beta1Path(request.path);
        else
            uut.addGAPIv1Path(request.path);
        request.path += request.options->parent.getProjectId().length() == 0 ? atoken->val[app_tk_ns::pid] : request.options->parent.getProjectId();
        request.path += FPSTR("/databases");
        if (!request.options->parent.isDatabaseIdParam())
        {
            request.path += '/';
            request.path += request.options->parent.getDatabaseId().length() > 0 ? request.options->parent.getDatabaseId() : FPSTR("(default)");
        }
        addParams(request, extras);

        url(FPSTR("firestore.googleapis.com"));

        async_data_item_t *sData = createSlotBase(request.aClient, request.opt);

        if (!sData)
            return setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        newRequestBase(request.aClient, sData, service_url, request.path, extras, request.method, request.opt, request.uid);

        if (request.options->payload.length())
        {
            sData->request.val[req_hndlr_ns::payload] = request.options->payload;
            setContentLengthBase(request.aClient, sData, request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        addRemoveClientVecBase(request.aClient, reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(getRVec(request.aClient))));

        sData->download = request.method == async_request_handler_t::http_get && sData->request.file_data.filename.length();

        processBase(request.aClient, sData->async);
        handleRemoveBase(request.aClient);
    }

    void addParams(const async_request_data_t &request, String &extras)
    {
        extras += request.options->extras;
        extras.replace(" ", "%20");
        extras.replace(",", "%2C");
    }

    void setClientError(async_request_data_t &request, int code)
    {
        AsyncResult *aResult = request.aResult;

        if (!aResult)
            aResult = new AsyncResult();

        aResult->lastError.setClientError(code);

        if (request.cb)
            request.cb(*aResult);

        if (!request.aResult)
        {
            delete aResult;
            aResult = nullptr;
        }
    }

    void eximDocs(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const EximDocumentOptions &eximOptions, bool isImport, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = isImport ? firebase_firestore_request_type_import_docs : firebase_firestore_request_type_export_docs;
        options.parent = parent;
        options.payload = eximOptions.c_str();
        if (!isImport)
            options.payload.replace("inputUriPrefix", "outputUriPrefix");
        options.extras += isImport ? FPSTR(":importDocuments") : FPSTR(":exportDocuments");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void manageDatabase(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const String &database, const String &key, Firestore::firestore_database_mode mode, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_manage_database;
        options.parent = parent;

        if (strlen(database.c_str()))
        {
            options.payload = database.c_str();
            if (mode == Firestore::firestore_database_mode_create)
            {
                options.parent.setDatabaseIdParam(true);
                options.extras += FPSTR("?databaseId=");
                options.extras += options.parent.getDatabaseId();
            }
        }

        if (key.length())
        {
            if (mode == Firestore::firestore_database_mode_delete)
                options.extras += FPSTR("?etag=");
            else if (mode == Firestore::firestore_database_mode_patch)
                options.extras += FPSTR("?updateMask=");
            options.extras += key;
        }

        if (mode == Firestore::firestore_database_mode_list)
            options.parent.setDatabaseIdParam(true);

        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;

        if (strlen(database.c_str()) > 0 && mode == Firestore::firestore_database_mode_create)
            method = async_request_handler_t::http_post; // create
        else if (options.parent.getDatabaseId().length() > 0 && (mode == Firestore::firestore_database_mode_delete || mode == Firestore::firestore_database_mode_get))
            method = mode == Firestore::firestore_database_mode_delete ? async_request_handler_t::http_delete : async_request_handler_t::http_get; // get index or delete by id
        else if (strlen(database.c_str()) == 0 && mode == Firestore::firestore_database_mode_list)
            method = async_request_handler_t::http_get; // list
        else if (strlen(database.c_str()) > 0 && mode == Firestore::firestore_database_mode_patch)
            method = async_request_handler_t::http_patch; // patch

        async_request_data_t aReq(&aClient, path, method, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void createDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const String &collectionId, const String &documentId, DocumentMask &mask, Document<Values::Value> &document, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_create_doc;
        options.parent = parent;
        options.collectionId = collectionId;
        options.documentId = documentId;
        options.payload = document.c_str();
        addDocsPath(options.extras);
        options.extras += '/';
        options.extras += options.collectionId;
        URLUtil uut;
        bool hasQueryParams = false;
        uut.addParam(options.extras, "documentId", options.documentId, hasQueryParams);
        options.extras += mask.getQuery("mask", hasQueryParams);
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void patchDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const String &documentPath, PatchDocumentOptions patchOptions, Document<Values::Value> &document, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_patch_doc;
        options.parent = parent;
        options.payload = document.c_str();
        addDocsPath(options.extras);
        options.extras += '/';
        options.extras += documentPath;
        options.extras += patchOptions.c_str();
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_patch, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void commitDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const Writes &writes, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_commit_document;
        options.parent = parent;
        options.payload = writes.c_str();
        addDocsPath(options.extras);
        options.extras += FPSTR(":commit");
        options.payload.replace(reinterpret_cast<const char *>(RESOURCE_PATH_BASE), makeResourcePath(parent));
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void batchWriteDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const Writes &writes, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_batch_write_doc;
        options.parent = parent;
        options.payload = writes.c_str();
        options.payload.replace(reinterpret_cast<const char *>(RESOURCE_PATH_BASE), makeResourcePath(parent));
        addDocsPath(options.extras);
        options.extras += FPSTR(":batchWrite");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void getDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const String &documentPath, GetDocumentOptions getOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_get_doc;
        options.parent = parent;
        addDocsPath(options.extras);
        options.extras += '/';
        options.extras += documentPath;
        options.extras += getOptions.c_str();
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void batchGetDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const BatchGetDocumentOptions &batchOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_batch_get_doc;
        options.parent = parent;
        options.payload = batchOptions.c_str();
        options.payload.replace(reinterpret_cast<const char *>(RESOURCE_PATH_BASE), makeResourcePath(parent));
        addDocsPath(options.extras);
        options.extras += FPSTR(":batchGet");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void beginTrans(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, TransactionOptions transOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_begin_transaction;
        options.parent = parent;
        JSONUtil jut;
        jut.addObject(options.payload, "options", transOptions.c_str(), false, true);
        addDocsPath(options.extras);
        options.extras += FPSTR(":beginTransaction");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void transRollback(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const String &transaction, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_rollback;
        options.parent = parent;
        JSONUtil jut;
        jut.addObject(options.payload, "transaction", transaction, true, true);
        addDocsPath(options.extras);
        options.extras += FPSTR(":rollback");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

#if defined(ENABLE_FIRESTORE_QUERY)
    void runQueryImpl(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const String &documentPath, const QueryOptions &queryOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_run_query;
        options.parent = parent;
        options.parent.setDocPath(documentPath);
        options.payload = queryOptions.c_str();

        addDocsPath(options.extras);
        URLUtil uut;
        uut.addPath(options.extras, documentPath);
        options.extras += FPSTR(":runQuery");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }
#endif

    void deleteDocBase(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const String &documentPath, Precondition currentDocument, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_delete_doc;
        options.parent = parent;
        options.parent.setDocPath(documentPath);

        addDocsPath(options.extras);
        URLUtil uut;
        uut.addPath(options.extras, documentPath);
        options.extras += currentDocument.getQuery("currentDocument");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_delete, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void listDocs(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const String &collectionId, const ListDocumentsOptions &listDocsOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_list_doc;
        options.parent = parent;

        addDocsPath(options.extras);
        URLUtil uut;
        uut.addPath(options.extras, collectionId);

        options.extras += listDocsOptions.c_str();
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void listCollIds(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const String &documentPath, const ListCollectionIdsOptions &listCollectionIdsOptions, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_list_collection;
        options.parent = parent;
        options.parent.setDocPath(documentPath);
        options.payload = listCollectionIdsOptions.c_str();

        addDocsPath(options.extras);
        URLUtil uut;
        uut.addPath(options.extras, documentPath);
        options.extras += FPSTR(":listCollectionIds");

        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void databaseIndexManager(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const DatabaseIndex::Index &index, const String &indexId, bool deleteMode, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_create_field_index;
        options.parent = parent;
        options.payload = index.c_str();
        options.extras += FPSTR("/indexes");
        if (indexId.length())
            options.extras += '/';
        options.extras += indexId;

        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;

        if (strlen(index.c_str()) > 0)
            method = async_request_handler_t::http_post; // create
        else if (indexId.length() > 0)
            method = deleteMode ? async_request_handler_t::http_delete : async_request_handler_t::http_get; // get index or delete by id
        if (strlen(index.c_str()) == 0 && indexId.length() == 0)
            method = async_request_handler_t::http_get; // list

        async_request_data_t aReq(&aClient, path, method, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq, 1);
    }

    void collectionGroupIndexManager(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const Firestore::Parent &parent, const CollectionGroupsIndex::Index &index, const String &collectionId, const String &indexId, bool deleteMode, bool async)
    {
        Firestore::DataOptions options;
        options.requestType = firebase_firestore_request_type_create_composite_index;
        options.parent = parent;
        options.payload = index.c_str();
        options.extras += FPSTR("/collectionGroups");
        if (collectionId.length())
            options.extras += '/';
        options.extras += collectionId;
        options.extras += FPSTR("/indexes");

        if (indexId.length())
        {
            options.extras += '/';
            options.extras += indexId;
        }

        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;

        if (strlen(index.c_str()) > 0)
            method = async_request_handler_t::http_post; // create
        else if (collectionId.length() > 0)
            method = deleteMode ? async_request_handler_t::http_delete : async_request_handler_t::http_get; // get index or delete by id
        if (strlen(index.c_str()) == 0 && indexId.length() == 0)
            method = async_request_handler_t::http_get; // list

        async_request_data_t aReq(&aClient, path, method, slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    String makeResourcePath(const Firestore::Parent &parent)
    {
        String str = FPSTR("projects/");
        str += parent.getProjectId();
        addDatabasePath(str);
        str += '/';
        str += parent.getDatabaseId().length() > 0 ? parent.getDatabaseId() : FPSTR("(default)");
        addDocsPath(str);
        return str;
    }
    void addDatabasePath(String &buf) { buf += FPSTR("/databases"); }
    void addDocsPath(String &buf) { buf += FPSTR("/documents"); }
};

#endif

#endif