/**
 * Created February 17, 2024
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
#ifndef ASYNC_FIRESTORE_H
#define ASYNC_FIRESTORE_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./firestore/DataOptions.h"

using namespace firebase;

#if defined(ENABLE_FIRESTORE)

#include "./firestore/Query.h"

class Firestore
{
    friend class FirebaseApp;

private:
    AsyncClientClass *aClient = nullptr;
    String service_url;
    String path;
    String uid;
    uint32_t app_addr = 0;
    app_token_t *app_token = nullptr;

    struct async_request_data_t
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path;
        String uid;
        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;
        AsyncClientClass::slot_options_t opt;
        FirestoreOptions *options = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        async_request_data_t() {}
        async_request_data_t(AsyncClientClass *aClient, const String &path, async_request_handler_t::http_request_method method, AsyncClientClass::slot_options_t opt, FirestoreOptions *options, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
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

public:
    ~Firestore(){};

    Firestore(const String &url = "")
    {
        this->service_url = url;
    };

    Firestore &operator=(Firestore &rhs)
    {
        this->service_url = rhs.service_url;
        return *this;
    }

    /**
     * Set the Firestore URL
     * @param url The Firestore URL.
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

    /** Export the documents in the database to the Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param exportOptions The EximDocumentOptions object included collectionIds, bucketID and storagePath in its constructor.
     * The EximDocumentOptions constructor parameters.
     * - collectionIds is collection ids to export. Unspecified means all collections. Use comma (,)
     *   to separate between the collection ids.
     * - bucketID is the Firebase storage bucket ID in the project.
     * - storagePath is the path in the Firebase Storage data bucket to store the exported database.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth or AccessToken authentication.
     *
     */
    bool exportDocuments(AsyncClientClass &aClient, const ParentResource &parent, EximDocumentOptions exportOptions)
    {
        AsyncResult result;
        eximDocs(aClient, &result, NULL, "", parent, exportOptions, false, false);
        return result.lastError.code() == 0;
    }

    /** Export the documents in the database to the Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param exportOptions The EximDocumentOptions object included collectionIds, bucketID and storagePath in its constructor.
     * The EximDocumentOptions constructor parameters.
     * - collectionIds is collection ids to export. Unspecified means all collections. Use comma (,)
     *   to separate between the collection ids.
     * - bucketID is the Firebase storage bucket ID in the project.
     * - storagePath is the path in the Firebase Storage data bucket to store the exported database.
     * @param aResult The async result (AsyncResult)
     *
     * This function requires ServiceAuth or AccessToken authentication.
     *
     */
    void exportDocuments(AsyncClientClass &aClient, const ParentResource &parent, EximDocumentOptions exportOptions, AsyncResult &aResult)
    {
        eximDocs(aClient, &aResult, NULL, "", parent, exportOptions, false, true);
    }

    /** Export the documents in the database to the Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param exportOptions The EximDocumentOptions object included collectionIds, bucketID and storagePath in its constructor.
     * The EximDocumentOptions constructor parameters.
     * - collectionIds is collection ids to export. Unspecified means all collections. Use comma (,)
     *   to separate between the collection ids.
     * - bucketID is the Firebase storage bucket ID in the project.
     * - storagePath is the path in the Firebase Storage data bucket to store the exported database.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth or AccessToken authentication.
     *
     */
    void exportDocuments(AsyncClientClass &aClient, const ParentResource &parent, EximDocumentOptions exportOptions, AsyncResultCallback cb, const String &uid = "")
    {
        eximDocs(aClient, nullptr, cb, uid, parent, exportOptions, false, true);
    }

    /** Import the exported documents stored in the Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param importOptions The EximDocumentOptions object included collectionIds, bucketID and storagePath in its constructor.
     * The EximDocumentOptions constructor parameters.
     * - collectionIds is collection ids to import. Unspecified means all collections included in the import.
     *   Use comma (,) to separate between the collection ids.
     * - bucketID is the Firebase storage bucket ID in the project.
     * - storagePath is the path in the Firebase Storage data bucket that stores the exported database.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth or AccessToken authentication.
     *
     */
    bool importDocuments(AsyncClientClass &aClient, const ParentResource &parent, EximDocumentOptions importOptions)
    {
        AsyncResult result;
        eximDocs(aClient, &result, NULL, "", parent, importOptions, true, false);
        return result.lastError.code() == 0;
    }

    /** Import the exported documents stored in the Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param importOptions The EximDocumentOptions object included collectionIds, bucketID and storagePath in its constructor.
     * The EximDocumentOptions constructor parameters.
     * - collectionIds is collection ids to import. Unspecified means all collections included in the import.
     *   Use comma (,) to separate between the collection ids.
     * - bucketID is the Firebase storage bucket ID in the project.
     * - storagePath is the path in the Firebase Storage data bucket that stores the exported database.
     * @param aResult The async result (AsyncResult)
     *
     * This function requires ServiceAuth or AccessToken authentication.
     *
     */
    void importDocuments(AsyncClientClass &aClient, const ParentResource &parent, EximDocumentOptions importOptions, AsyncResult &aResult)
    {
        eximDocs(aClient, &aResult, NULL, "", parent, importOptions, true, true);
    }

    /** Import the exported documents stored in the Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param importOptions The EximDocumentOptions object included collectionIds, bucketID and storagePath in its constructor.
     * The EximDocumentOptions constructor parameters.
     * - collectionIds is collection ids to import. Unspecified means all collections included in the import.
     *   Use comma (,) to separate between the collection ids.
     * - bucketID is the Firebase storage bucket ID in the project.
     * - storagePath is the path in the Firebase Storage data bucket that stores the exported database.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth or AccessToken authentication.
     *
     */
    void importDocuments(AsyncClientClass &aClient, const ParentResource &parent, EximDocumentOptions importOptions, AsyncResultCallback cb, const String &uid = "")
    {
        eximDocs(aClient, nullptr, cb, uid, parent, importOptions, true, true);
    }

    /** Create a document at the defined document path.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to create in the collection.
     * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
     * @param document A Firestore document.
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    bool createDocument(AsyncClientClass &aClient, ParentResource parent, const String &documentPath, DocumentMask mask, Document<Values::Value> &document)
    {
        AsyncResult result;
        parent.documentPath = documentPath;
        String collectionId, documentId;
        parent.pathResove(collectionId, documentId);
        createDoc(aClient, &result, NULL, "", parent, collectionId, documentId, mask, document, false);
        return result.lastError.code() == 0;
    }

    /** Create a document at the defined document path.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to create in the collection.
     * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
     * @param document A Firestore document.
     * @param aResult The async result (AsyncResult)
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void createDocument(AsyncClientClass &aClient, ParentResource parent, const String &documentPath, DocumentMask mask, Document<Values::Value> &document, AsyncResult &aResult)
    {
        parent.documentPath = documentPath;
        String collectionId, documentId;
        parent.pathResove(collectionId, documentId);
        createDoc(aClient, &aResult, NULL, "", parent, collectionId, documentId, mask, document, true);
    }

    /** Create a document at the defined document path.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to create in the collection.
     * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
     * @param document A Firestore document.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void createDocument(AsyncClientClass &aClient, ParentResource parent, const String &documentPath, DocumentMask mask, Document<Values::Value> &document, AsyncResultCallback cb, const String &uid = "")
    {
        parent.documentPath = documentPath;
        String collectionId, documentId;
        parent.pathResove(collectionId, documentId);
        createDoc(aClient, nullptr, cb, uid, parent, collectionId, documentId, mask, document, true);
    }

    /** Create a document in the defined collection id.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param collectionId The relative path of document collection id to create the document.
     * @param documentId The document id of document to be created.
     * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
     * @param document A Firestore document.
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    bool createDocument(AsyncClientClass &aClient, ParentResource parent, const String &collectionId, const String &documentId, DocumentMask mask, Document<Values::Value> &document)
    {
        AsyncResult result;
        createDoc(aClient, &result, NULL, "", parent, collectionId, documentId, mask, document, false);
        return result.lastError.code() == 0;
    }

    /** Create a document in the defined collection id.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param collectionId The relative path of document collection id to create the document.
     * @param documentId The document id of document to be created.
     * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
     * @param document A Firestore document.
     * @param aResult The async result (AsyncResult)
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     *
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void createDocument(AsyncClientClass &aClient, ParentResource parent, const String &collectionId, const String &documentId, DocumentMask mask, Document<Values::Value> &document, AsyncResult &aResult)
    {
        createDoc(aClient, &aResult, NULL, "", parent, collectionId, documentId, mask, document, true);
    }

    /** Create a document in the defined collection id.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param collectionId The relative path of document collection id to create the document.
     * @param documentId The document id of document to be created.
     * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
     * @param document A Firestore document.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     *
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void createDocument(AsyncClientClass &aClient, ParentResource parent, const String &collectionId, const String &documentId, DocumentMask mask, Document<Values::Value> &document, AsyncResultCallback cb, const String &uid = "")
    {
        createDoc(aClient, nullptr, cb, uid, parent, collectionId, documentId, mask, document, true);
    }

    /** Patch or update a document at the defined path.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to patch with the input document.
     * @param content A Firestore document.
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     * @param updateMask The fields to update. If the document exists on the server and has fields not referenced in the mask,
     * they are left unchanged.
     * Fields referenced in the mask, but not present in the input document (content), are deleted from the document on the server.
     * Use comma (,) to separate between the field names.
     * @param mask The fields to return. If not set, returns all fields. If the document has a field that is not present in
     * this mask, that field will not be returned in the response. Use comma (,) to separate between the field names.
     * @param document A Firestore document.
     * @param currentDocument An optional precondition on the document. The request will fail if this is set and not met by the target document.
     *
     * When currentDocument as defined with true or false as constructor argument, the exists condition will be set.
     * When currentDocument as defined with string(Timestamp format) as constructor argument, the updateTime condition will be set.
     *
     * When exists condition was set to true, the target document must exist. When set to false, the target document must not exist.
     * When updateTime was set, the target document must exist and have been last updated at that time.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    bool patchDocument(AsyncClientClass &aClient, const ParentResource &parent, const String &documentPath, patchDocumentOptions patchOptions, Document<Values::Value> &document)
    {
        AsyncResult result;
        patchDoc(aClient, &result, NULL, "", parent, documentPath, patchOptions, document, false);
        return result.lastError.code() == 0;
    }

    /** Patch or update a document at the defined path.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to patch with the input document.
     * @param content A Firestore document.
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     * @param updateMask The fields to update. If the document exists on the server and has fields not referenced in the mask,
     * they are left unchanged.
     * Fields referenced in the mask, but not present in the input document (content), are deleted from the document on the server.
     * Use comma (,) to separate between the field names.
     * @param mask The fields to return. If not set, returns all fields. If the document has a field that is not present in
     * this mask, that field will not be returned in the response. Use comma (,) to separate between the field names.
     * @param document A Firestore document.
     * @param currentDocument An optional precondition on the document. The request will fail if this is set and not met by the target document.
     *
     * When currentDocument as defined with true or false as constructor argument, the exists condition will be set.
     * When currentDocument as defined with string(Timestamp format) as constructor argument, the updateTime condition will be set.
     *
     * When exists condition was set to true, the target document must exist. When set to false, the target document must not exist.
     * When updateTime was set, the target document must exist and have been last updated at that time.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @param aResult The async result (AsyncResult).
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void patchDocument(AsyncClientClass &aClient, const ParentResource &parent, const String &documentPath, patchDocumentOptions patchOptions, Document<Values::Value> &document, AsyncResult &aResult)
    {
        patchDoc(aClient, &aResult, NULL, "", parent, documentPath, patchOptions, document, true);
    }

    /** Patch or update a document at the defined path.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to patch with the input document.
     * @param content A Firestore document.
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     * @param updateMask The fields to update. If the document exists on the server and has fields not referenced in the mask,
     * they are left unchanged.
     * Fields referenced in the mask, but not present in the input document (content), are deleted from the document on the server.
     * Use comma (,) to separate between the field names.
     * @param mask The fields to return. If not set, returns all fields. If the document has a field that is not present in
     * this mask, that field will not be returned in the response. Use comma (,) to separate between the field names.
     * @param document A Firestore document.
     * @param currentDocument An optional precondition on the document. The request will fail if this is set and not met by the target document.
     *
     * When currentDocument as defined with true or false as constructor argument, the exists condition will be set.
     * When currentDocument as defined with string(Timestamp format) as constructor argument, the updateTime condition will be set.
     *
     * When exists condition was set to true, the target document must exist. When set to false, the target document must not exist.
     * When updateTime was set, the target document must exist and have been last updated at that time.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void patchDocument(AsyncClientClass &aClient, const ParentResource &parent, const String &documentPath, patchDocumentOptions patchOptions, Document<Values::Value> &document, AsyncResultCallback cb, const String &uid = "")
    {
        patchDoc(aClient, nullptr, cb, uid, parent, documentPath, patchOptions, document, true);
    }

    /** Commits a transaction, while optionally updating documents.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param writes The writes to apply.
     *
     * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    bool commitDocument(AsyncClientClass &aClient, const ParentResource &parent, Writes &writes)
    {
        AsyncResult result;
        commitDoc(aClient, &result, NULL, "", parent, writes, false);
        return result.lastError.code() == 0;
    }

    /** Commits a transaction, while optionally updating documents.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param writes The writes to apply.
     * @param aResult The async result (AsyncResult).
     *
     * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void commitDocument(AsyncClientClass &aClient, const ParentResource &parent, Writes &writes, AsyncResult &aResult)
    {
        commitDoc(aClient, &aResult, NULL, "", parent, writes, true);
    }

    /** Commits a transaction, while optionally updating documents.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param writes The writes to apply. This Writes object accepts the Write object and transaction string (base64 encoded string) in its constructor.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void commitDocument(AsyncClientClass &aClient, const ParentResource &parent, Writes &writes, AsyncResultCallback cb, const String &uid = "")
    {
        commitDoc(aClient, nullptr, cb, uid, parent, writes, true);
    }

    /** Applies a batch of write operations.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param writes The writes to apply. This Writes object accepts the Write object and Labels (MapValue) associated with this batch write, in its constructor.
     *
     * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth authentication.
     *
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchWrite
     *
     */
    bool batchWriteDocuments(AsyncClientClass &aClient, const ParentResource &parent, Writes &writes)
    {
        AsyncResult result;
        batchWriteDoc(aClient, &result, NULL, "", parent, writes, false);
        return result.lastError.code() == 0;
    }

    /** Applies a batch of write operations.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param writes The writes to apply. This Writes object accepts the Write object and Labels (MapValue) associated with this batch write, in its constructor.
     * @param aResult The async result (AsyncResult).
     * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
     *
     * This function requires ServiceAuth authentication.
     *
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchWrite
     *
     */
    void batchWriteDocuments(AsyncClientClass &aClient, const ParentResource &parent, Writes &writes, AsyncResult &aResult)
    {
        batchWriteDoc(aClient, &aResult, NULL, "", parent, writes, true);
    }

    /** Applies a batch of write operations.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param writes The writes to apply. This Writes object accepts the Write object and Labels (MapValue) associated with this batch write, in its constructor.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
     *
     * This function requires ServiceAuth authentication.
     *
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchWrite
     *
     */
    void batchWriteDocuments(AsyncClientClass &aClient, const ParentResource &parent, Writes &writes, AsyncResultCallback cb, const String &uid = "")
    {
        batchWriteDoc(aClient, nullptr, cb, uid, parent, writes, true);
    }

    /** Get a document at the defined path.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to get.
     * @param options The GetDocumentOptions object included mask, transaction and readTime in its constructor.
     * - mask is the fields to return. If not set, returns all fields. If the document has a field that is not present in this mask,
     * that field will not be returned in the response. Use comma (,) to separate between the field names.
     * - transaction is a base64-encoded string. If set, reads the document in a transaction.
     * - readTime is a timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * If set, reads the version of the document at the given time. This may not be older than 270 seconds.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    bool getDocument(AsyncClientClass &aClient, const ParentResource &parent, const String &documentPath, const GetDocumentOptions &options)
    {
        AsyncResult result;
        getDoc(aClient, &result, NULL, "", parent, documentPath, options, false);
        return result.lastError.code() == 0;
    }

    /** Get a document at the defined path.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to get.
     * @param options The GetDocumentOptions object included mask, transaction and readTime in its constructor.
     * - mask is the fields to return. If not set, returns all fields. If the document has a field that is not present in this mask,
     * that field will not be returned in the response. Use comma (,) to separate between the field names.
     * - transaction is a base64-encoded string. If set, reads the document in a transaction.
     * - readTime is a timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * If set, reads the version of the document at the given time. This may not be older than 270 seconds.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     * @param aResult The async result (AsyncResult).
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void getDocument(AsyncClientClass &aClient, const ParentResource &parent, const String &documentPath, const GetDocumentOptions &options, AsyncResult &aResult)
    {
        getDoc(aClient, &aResult, NULL, "", parent, documentPath, options, true);
    }

    /** Get a document at the defined path.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to get.
     * @param options The GetDocumentOptions object included mask, transaction and readTime in its constructor.
     * - mask is the fields to return. If not set, returns all fields. If the document has a field that is not present in this mask,
     * that field will not be returned in the response. Use comma (,) to separate between the field names.
     * - transaction is a base64-encoded string. If set, reads the document in a transaction.
     * - readTime is a timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * If set, reads the version of the document at the given time. This may not be older than 270 seconds.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     */
    void getDocument(AsyncClientClass &aClient, const ParentResource &parent, const String &documentPath, const GetDocumentOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        getDoc(aClient, nullptr, cb, uid, parent, documentPath, options, true);
    }

    /** Gets multiple documents.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param batchOptions The BatchGetDocumentOptions object which provided the member functions to construct the requst body.
     * addDocument, mask, transaction, newTransaction and readTime functions.
     *
     * addDocument used for adding the document path to read.
     * mask used for setting the mask fields to return. If not set, returns all fields. If the document has a field that is not present in this mask,
     * that field will not be returned in the response. Use comma (,) to separate between the field names.
     *
     * The following function used for creating the union field consistency_selector and can be only one of the following field e.g.
     * transaction, newTransaction and readTime functions.
     *
     * Then the following functions can't be mixed used.
     * - transaction used for reading the document in a transaction. A base64-encoded string.
     * - newTransaction used for creating the transaction.
     * - readTime used for setting the documents as they were at the given time. This may not be older than 270 seconds.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth authentication.
     *
     * For more detail, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchGet
     *
     */
    bool batchGetDocuments(AsyncClientClass &aClient, const ParentResource &parent, BatchGetDocumentOptions batchOptions)
    {
        AsyncResult result;
        batchGetDoc(aClient, &result, NULL, "", parent, batchOptions, false);
        return result.lastError.code() == 0;
    }

    /** Gets multiple documents.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param batchOptions The BatchGetDocumentOptions object which provided the member functions to construct the requst body.
     * addDocument, mask, transaction, newTransaction and readTime functions.
     *
     * addDocument used for adding the document path to read.
     * mask used for setting the mask fields to return. If not set, returns all fields. If the document has a field that is not present in this mask,
     * that field will not be returned in the response. Use comma (,) to separate between the field names.
     *
     * The following function used for creating the union field consistency_selector and can be only one of the following field e.g.
     * transaction, newTransaction and readTime functions.
     *
     * Then the following functions can't be mixed used.
     * - transaction used for reading the document in a transaction. A base64-encoded string.
     * - newTransaction used for creating the transaction.
     * - readTime used for setting the documents as they were at the given time. This may not be older than 270 seconds.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @param aResult The async result (AsyncResult).
     *
     * This function requires ServiceAuth authentication.
     *
     * For more detail, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchGet
     *
     */
    void batchGetDocuments(AsyncClientClass &aClient, const ParentResource &parent, BatchGetDocumentOptions batchOptions, AsyncResult &aResult)
    {
        batchGetDoc(aClient, &aResult, NULL, "", parent, batchOptions, true);
    }

    /** Gets multiple documents.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param batchOptions The BatchGetDocumentOptions object which provided the member functions to construct the requst body.
     * addDocument, mask, transaction, newTransaction and readTime functions.
     *
     * addDocument used for adding the document path to read.
     * mask used for setting the mask fields to return. If not set, returns all fields. If the document has a field that is not present in this mask,
     * that field will not be returned in the response. Use comma (,) to separate between the field names.
     *
     * The following function used for creating the union field consistency_selector and can be only one of the following field e.g.
     * transaction, newTransaction and readTime functions.
     *
     * Then the following functions can't be mixed used.
     * - transaction used for reading the document in a transaction. A base64-encoded string.
     * - newTransaction used for creating the transaction.
     * - readTime used for setting the documents as they were at the given time. This may not be older than 270 seconds.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth authentication.
     *
     * For more detail, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchGet
     *
     */
    void batchGetDocuments(AsyncClientClass &aClient, const ParentResource &parent, BatchGetDocumentOptions batchOptions, AsyncResultCallback cb, const String &uid = "")
    {
        batchGetDoc(aClient, nullptr, cb, uid, parent, batchOptions, true);
    }

    /** Starts a new transaction.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param transOptions Options for creating a new transaction.
     *
     * The TransactionOptions object can be initialized with two objects represent two modes
     * readOnly and readWrite.
     *
     * Use readOnly option used when transaction can only be used for read operations.
     * Use readWrite option used when transaction can be used for both read and write operations.
     *
     * The readOnly object (option) accepts the readTime (timestamp) in the constructor for reading the documents at the given time.
     * This must be a microsecond precision timestamp within the past one hour, or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     *
     * The readWrite object (option) accepts the retryTransaction (base64 encoded string) in the constructor represents a transaction that can be used to read and write documents.
     *
     * See https://cloud.google.com/firestore/docs/reference/rest/v1/TransactionOptions for transaction options.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth authentication.
     */
    bool beginTransaction(AsyncClientClass &aClient, const ParentResource &parent, const TransactionOptions &transOptions)
    {
        AsyncResult result;
        beginTrans(aClient, &result, NULL, "", parent, transOptions, false);
        return result.lastError.code() == 0;
    }

    /** Starts a new transaction.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param transOptions Options for creating a new transaction.
     *
     * The TransactionOptions object can be initialized with two objects represent two modes
     * readOnly and readWrite.
     *
     * Use readOnly option used when transaction can only be used for read operations.
     * Use readWrite option used when transaction can be used for both read and write operations.
     *
     * The readOnly object (option) accepts the readTime (timestamp) in the constructor for reading the documents at the given time.
     * This must be a microsecond precision timestamp within the past one hour, or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     *
     * The readWrite object (option) accepts the retryTransaction (base64 encoded string) in the constructor represents a transaction that can be used to read and write documents.
     *
     * See https://cloud.google.com/firestore/docs/reference/rest/v1/TransactionOptions for transaction options.
     *
     * @param aResult The async result (AsyncResult).
     *
     * This function requires ServiceAuth authentication.
     */
    void beginTransaction(AsyncClientClass &aClient, const ParentResource &parent, const TransactionOptions &transOptions, AsyncResult &aResult)
    {
        beginTrans(aClient, &aResult, NULL, "", parent, transOptions, true);
    }

    /** Starts a new transaction.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param transOptions Options for creating a new transaction.
     *
     * The TransactionOptions object can be initialized with two objects represent two modes
     * readOnly and readWrite.
     *
     * Use readOnly option used when transaction can only be used for read operations.
     * Use readWrite option used when transaction can be used for both read and write operations.
     *
     * The readOnly object (option) accepts the readTime (timestamp) in the constructor for reading the documents at the given time.
     * This must be a microsecond precision timestamp within the past one hour, or if Point-in-Time Recovery is enabled, can additionally be a whole minute timestamp within the past 7 days.
     *
     * The readWrite object (option) accepts the retryTransaction (base64 encoded string) in the constructor represents a transaction that can be used to read and write documents.
     *
     * See https://cloud.google.com/firestore/docs/reference/rest/v1/TransactionOptions for transaction options.
     *
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth authentication.
     */
    void beginTransaction(AsyncClientClass &aClient, const ParentResource &parent, const TransactionOptions &transOptions, AsyncResultCallback cb, const String &uid = "")
    {
        beginTrans(aClient, nullptr, cb, uid, parent, transOptions, true);
    }

    /** Rolls back a transaction.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param transaction Required. A base64-encoded string of the transaction to roll back.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth authentication.
     */
    bool rollback(AsyncClientClass &aClient, const ParentResource &parent, const String &transaction)
    {
        AsyncResult result;
        transRollback(aClient, &result, NULL, "", parent, transaction, false);
        return result.lastError.code() == 0;
    }

    /** Rolls back a transaction.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param transaction Required. A base64-encoded string of the transaction to roll back.
     *
     * @param aResult The async result (AsyncResult).
     *
     * This function requires ServiceAuth authentication.
     */
    void rollback(AsyncClientClass &aClient, const ParentResource &parent, const String &transaction, AsyncResult &aResult)
    {
        transRollback(aClient, &aResult, NULL, "", parent, transaction, true);
    }

    /** Rolls back a transaction.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param transaction Required. A base64-encoded string of the transaction to roll back.
     *
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth authentication.
     */
    void rollback(AsyncClientClass &aClient, const ParentResource &parent, const String &transaction, AsyncResultCallback cb, const String &uid = "")
    {
        transRollback(aClient, nullptr, cb, uid, parent, transaction, true);
    }

#if defined(ENABLE_FIRESTORE_QUERY)

    /** Runs a query.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to get.
     * @param queryOptions The QueryOptions object that provides the function to create the query (StructuredQuery) and consistency mode which included
     * structuredQuery, transaction, newTransaction and readTime functions.
     *
     * The following function used for creating the union field consistency_selector and can be only one of the following field e.g.
     * transaction, newTransaction and readTime  functions.
     *
     * Then the following functions can't be mixed used.
     * - transaction used for running the query within an already active transaction. A base64-encoded string.
     * - newTransaction used for starting a new transaction and reads the documents. Defaults to a read-only transaction.
     * The new transaction ID will be returned as the first response in the stream.
     * - readTime used for reading the documents as they were at the given time.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.documents/runQuery
     *
     */
    bool runQuery(AsyncClientClass &aClient, const ParentResource &parent, const String &documentPath, QueryOptions queryOptions)
    {
        AsyncResult result;
        runQueryImpl(aClient, &result, NULL, "", parent, documentPath, queryOptions, false);
        return result.lastError.code() == 0;
    }

    /** Runs a query.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to get.
     * @param queryOptions The QueryOptions object that provides the function to create the query (StructuredQuery) and consistency mode which included
     * structuredQuery, transaction, newTransaction and readTime functions.
     *
     * The following function used for creating the union field consistency_selector and can be only one of the following field e.g.
     * transaction, newTransaction and readTime  functions.
     *
     * Then the following functions can't be mixed used.
     * - transaction used for running the query within an already active transaction. A base64-encoded string.
     * - newTransaction used for starting a new transaction and reads the documents. Defaults to a read-only transaction.
     * The new transaction ID will be returned as the first response in the stream.
     * - readTime used for reading the documents as they were at the given time.
     * @param aResult The async result (AsyncResult).
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.documents/runQuery
     *
     */
    void runQuery(AsyncClientClass &aClient, const ParentResource &parent, const String &documentPath, QueryOptions queryOptions, AsyncResult &aResult)
    {
        runQueryImpl(aClient, &aResult, NULL, "", parent, documentPath, queryOptions, true);
    }

    /** Runs a query.
     *
     * @param aClient The async client.
     * @param parent The ParentResource object included project Id and database Id in its constructor.
     * The Firebase project Id should be only the name without the firebaseio.com.
     * The Firestore database id should be (default) or empty "".
     * @param documentPath The relative path of document to get.
     * @param queryOptions The QueryOptions object that provides the function to create the query (StructuredQuery) and consistency mode which included
     * structuredQuery, transaction, newTransaction and readTime functions.
     *
     * The following function used for creating the union field consistency_selector and can be only one of the following field e.g.
     * transaction, newTransaction and readTime  functions.
     *
     * Then the following functions can't be mixed used.
     * - transaction used for running the query within an already active transaction. A base64-encoded string.
     * - newTransaction used for starting a new transaction and reads the documents. Defaults to a read-only transaction.
     * The new transaction ID will be returned as the first response in the stream.
     * - readTime used for reading the documents as they were at the given time.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
     *
     * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.documents/runQuery
     *
     */
    void runQuery(AsyncClientClass &aClient, const ParentResource &parent, const String &documentPath, QueryOptions queryOptions, AsyncResultCallback cb, const String &uid = "")
    {
        runQueryImpl(aClient, nullptr, cb, uid, parent, documentPath, queryOptions, true);
    }

#endif

    /** Delete a document at the defined path.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param documentPath The relative path of document to delete.
     * @param exists When set to true, the target document must exist. When set to false, the target document must not exist.
     * @param updateTime When set, the target document must exist and have been last updated at that time.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @return Boolean value, indicates the success of the operation.
     *
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication.
     *
     */
    // bool deleteDocument(AsyncClientClass &aClient, const String &projectId, const String &databaseId, const String &documentPath, const String &exists = "", const String &updateTime = "")
    //{
    //  return false;
    //}

    /** List the documents in the defined documents collection.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param collectionId The relative path of document colection.
     * @param pageSize The maximum number of documents to return.
     * @param pageToken The nextPageToken value returned from a previous List request, if any.
     * @param orderBy The order to sort results by. For example: priority desc, name.
     * @param mask The fields to return. If not set, returns all fields.
     * If a document has a field that is not present in this mask, that field will not be returned in the response.
     * @param showMissing If the vec should show missing documents.
     * A missing document is a document that does not exist but has sub-documents.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication (when showMissing is true).
     *
     */
    // bool listDocuments(AsyncClientClass &aClient, const String &projectId, const String &databaseId, const String &collectionId, const String &pageSize,
    //                   const String &pageToken, const String &orderBy, const String &mask, bool showMissing)
    //{
    //  return false;
    //}

    /** List the document collection ids in the defined document path.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param documentPath The relative path of document to get its collections' id.
     * @param pageSize The maximum number of results to return.
     * @param pageToken The nextPageToken value returned from a previous List request, if any.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     */
    // bool listCollectionIds(AsyncClientClass &aClient, const String &projectId, const String &databaseId, const String &documentPath, const String &pageSize, const String &pageToken)
    //{
    // return false;
    //}

    /** Creates a composite index.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param collectionId The relative path of document colection.
     * @param apiScope The API scope enum e.g., ANY_API and DATASTORE_MODE_API
     * @param queryScope The QueryScope enum string e.g., QUERY_SCOPE_UNSPECIFIED, COLLECTION, and COLLECTION_GROUP
     * See https://cloud.google.com/firestore/docs/reference/rest/Shared.Types/QueryScope
     *
     * @param fields The JSON Array that represents array of fields (IndexField JSON object) of indexes.
     * A IndexField object contains the keys "fieldPath" and the uinion field "value_mode" of "order" and "arrayConfig"
     * Where the fieldPath value is the field path string of index.
     * Where order is the enum string of ORDER_UNSPECIFIED, ASCENDING, and DESCENDING.
     * And arrayConfig is the ArrayConfig enum string of ARRAY_CONFIG_UNSPECIFIED and CONTAINS
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.collectionGroups.indexes/create
     *
     */
    // bool createIndex(AsyncClientClass &aClient, const String &projectId, const String &databaseId, const String &collectionId,
    //                const String &apiScope, const String &queryScope, const object_t &fields)
    //{
    // return false;
    //}

    /** Deletes an index.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param collectionId The relative path of document colection.
     * @param indexId The index to delete.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.collectionGroups.indexes/delete
     *
     */
    // bool deleteIndex(AsyncClientClass &aClient, const String &projectId, const String &databaseId, const String &collectionId, const String &indexId)
    //{
    //  return false;
    //}

    /** Lists the indexes that match the specified filters.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param collectionId The relative path of document colection.
     * @param filter The filter to apply to vec results.
     * @param pageSize The number of results to return.
     * @param pageToken A page token, returned from a previous call to FirestoreAdmin.ListIndexes,
     * that may be used to get the next page of results.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.collectionGroups.indexes/vec
     *
     */
    // bool listIndex(AsyncClientClass &aClient, const String &projectId, const String &databaseId, const String &collectionId, const String &filter = "", int pageSize = -1, const String &pageToken = "")
    // {
    // return false;
    //}

    /** Get an index.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param collectionId The relative path of document colection.
     * @param indexId The index to get.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.collectionGroups.indexes/get
     *
     */
    // bool getIndex(AsyncClientClass &aClient, const String &projectId, const String &databaseId, const String &collectionId, const String &indexId)
    //{
    // return false;
    //}

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

    void asyncRequest(async_request_data_t &request)
    {
        URLHelper uh;
        app_token_t *app_token = appToken();

        if (!app_token)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = app_token;
        String extras;

        uh.addGAPIv1Path(request.path);
        request.path += request.options->parent.projectId.length() == 0 ? app_token->project_id : request.options->parent.projectId;
        request.path += FPSTR("/databases/");
        request.path += request.options->parent.databaseId.length() > 0 ? request.options->parent.databaseId : FPSTR("(default)");

        addParams(request, extras);

        url(FPSTR("firestore.googleapis.com"));

        AsyncClientClass::async_data_item_t *sData = request.aClient->newSlot(cVec, service_url, request.path, extras, request.method, request.opt, request.uid);

        if (!sData)
            return setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        if (request.options->payload.length())
        {
            sData->request.payload = request.options->payload;
            request.aClient->setContentLength(sData, request.options->payload.length());
        }

        if (request.cb)
            sData->cb = request.cb;

        if (request.aResult)
            sData->setRefResult(request.aResult);

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void addParams(async_request_data_t &request, String &extras)
    {
        URLHelper uh;

        if (
            request.options->requestType == firebase_firestore_request_type_list_collection ||
            request.options->requestType == firebase_firestore_request_type_list_doc ||
            request.options->requestType == firebase_firestore_request_type_delete_doc)
        {
            extras += FPSTR("/documents");
            if (request.options->requestType == firebase_firestore_request_type_list_collection ||
                request.options->requestType == firebase_firestore_request_type_delete_doc)
            {
                uh.addPath(extras, request.options->parent.documentPath);
                extras += (request.options->requestType == firebase_firestore_request_type_list_collection)
                              ? ":listCollectionIds"
                              : "";
            }

            else if (request.options->requestType == firebase_firestore_request_type_list_doc)
            {
                uh.addPath(extras, request.options->collectionId);
            }
        }
        else if (request.options->requestType == firebase_firestore_request_type_create_index ||
                 request.options->requestType == firebase_firestore_request_type_delete_index ||
                 request.options->requestType == firebase_firestore_request_type_get_index ||
                 request.options->requestType == firebase_firestore_request_type_list_index)
        {
            extras += FPSTR("/collectionGroups/");
            extras += request.options->collectionId;
            extras += FPSTR("/indexes");
        }

        extras += request.options->extras;
        extras.replace(" ", "%20");
    }

    void setClientError(async_request_data_t &request, int code)
    {
        AsyncResult *aResult = request.aResult;

        if (!aResult)
            aResult = new AsyncResult();

        aResult->error_available = true;
        aResult->lastError.setClientError(code);

        if (request.cb)
            request.cb(*aResult);

        if (!request.aResult)
            delete aResult;
    }

    void eximDocs(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, EximDocumentOptions &eximOptions, bool isImport, bool async)
    {
        FirestoreOptions options;
        options.requestType = isImport ? firebase_firestore_request_type_import_docs : firebase_firestore_request_type_export_docs;
        options.parent = parent;
        options.payload = eximOptions.c_str();
        if (!isImport)
            options.payload.replace("inputUriPrefix", "outputUriPrefix");
        options.extras += isImport ? FPSTR(":importDocuments") : FPSTR(":exportDocuments");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void createDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, const String &collectionId, const String &documentId, DocumentMask &mask, Document<Values::Value> &document, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_create_doc;
        options.parent = parent;
        options.collectionId = collectionId;
        options.documentId = documentId;
        options.payload = document.c_str();
        addDocsPath(options.extras);
        options.extras += '/';
        options.extras += options.collectionId;
        URLHelper uh;
        bool hasQueryParams = false;
        uh.addParam(options.extras, "documentId", options.documentId, hasQueryParams);
        options.extras += mask.getQuery("mask", hasQueryParams);
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void patchDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, const String &documentPath, patchDocumentOptions patchOptions, Document<Values::Value> &document, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_patch_doc;
        options.parent = parent;
        options.payload = document.c_str();
        addDocsPath(options.extras);
        options.extras += '/';
        options.extras += documentPath;
        options.extras += patchOptions.c_str();
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_patch, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void commitDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, Writes &writes, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_commit_document;
        options.parent = parent;
        options.payload = writes.c_str();
        addDocsPath(options.extras);
        options.extras += FPSTR(":commit");
        options.payload.replace((const char *)FIRESTORE_RESOURCE_PATH_BASE, makeResourcePath(parent));
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void batchWriteDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, Writes &writes, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_batch_write_doc;
        options.parent = parent;
        options.payload = writes.c_str();
        options.payload.replace((const char *)FIRESTORE_RESOURCE_PATH_BASE, makeResourcePath(parent));
        addDocsPath(options.extras);
        options.extras += FPSTR(":batchWrite");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void getDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, const String &documentPath, GetDocumentOptions getOptions, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_get_doc;
        options.parent = parent;
        addDocsPath(options.extras);
        options.extras += '/';
        options.extras += documentPath;
        options.extras += getOptions.c_str();
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void batchGetDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, BatchGetDocumentOptions batchOptions, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_batch_get_doc;
        options.parent = parent;
        options.payload = batchOptions.c_str();
        options.payload.replace((const char *)FIRESTORE_RESOURCE_PATH_BASE, makeResourcePath(parent));
        addDocsPath(options.extras);
        options.extras += FPSTR(":batchGet");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void beginTrans(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, TransactionOptions transOptions, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_begin_transaction;
        options.parent = parent;
        JsonHelper jh;
        jh.addObject(options.payload, "options", transOptions.c_str(), true);
        addDocsPath(options.extras);
        options.extras += FPSTR(":beginTransaction");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    void transRollback(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, const String &transaction, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_rollback;
        options.parent = parent;
        JsonHelper jh;
        jh.addObject(options.payload, "transaction", jh.toString(transaction), true);
        addDocsPath(options.extras);
        options.extras += FPSTR(":rollback");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }
    
#if defined(ENABLE_FIRESTORE_QUERY)
    void runQueryImpl(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, const String &documentPath, QueryOptions queryOptions, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_run_query;
        options.parent = parent;
        options.parent.documentPath = documentPath;
        options.payload = queryOptions.c_str();

        addDocsPath(options.extras);
        URLHelper uh;
        uh.addPath(options.extras, documentPath);
        options.extras += FPSTR(":runQuery");
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }
#endif

    void listDoc(AsyncClientClass &aClient, AsyncResult *result, AsyncResultCallback cb, const String &uid, const ParentResource &parent, const String &documentPath, GetDocumentOptions getOptions, bool async)
    {
        FirestoreOptions options;
        options.requestType = firebase_firestore_request_type_list_doc;
        options.parent = parent;

        addDocsPath(options.extras);
        options.extras += '/';
        options.extras += documentPath;

        options.extras += getOptions.c_str();
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClientClass::slot_options_t(false, false, async, false, false, false), &options, result, cb, uid);
        asyncRequest(aReq);
    }

    String makeResourcePath(const ParentResource &parent)
    {
        String str = FPSTR("projects/");
        str += parent.projectId;
        addDatabasePath(str);
        str += '/';
        str += parent.databaseId.length() > 0 ? parent.databaseId : FPSTR("(default)");
        addDocsPath(str);
        return str;
    }
    void addDatabasePath(String &buf) { buf += FPSTR("/databases"); }
    void addDocsPath(String &buf) { buf += FPSTR("/documents"); }
};

#endif

#endif