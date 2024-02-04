/**
 * Created February 1, 2024
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
using namespace std;

using namespace firebase;

#if defined(ENABLE_FIRESTORE)

typedef void (*FirestoreBatchOperationsCallback)(const char *);

class Firestore
{
    friend class FirebaseApp;

private:
    AsyncClient *aClient = nullptr;
    String service_url;
    String path;
    String uid;
    uint32_t app_addr = 0;
    app_token_t *app_token = nullptr;

public:
    enum firebase_firestore_transform_type
    {
        firebase_firestore_transform_type_undefined,
        firebase_firestore_transform_type_set_to_server_value,
        firebase_firestore_transform_type_increment,
        firebase_firestore_transform_type_maaximum,
        firebase_firestore_transform_type_minimum,
        firebase_firestore_transform_type_append_missing_elements,
        firebase_firestore_transform_type_remove_all_from_array
    };

    enum firebase_firestore_document_write_type
    {
        firebase_firestore_document_write_type_undefined,
        firebase_firestore_document_write_type_update,
        firebase_firestore_document_write_type_delete,
        firebase_firestore_document_write_type_transform
    };

    enum firebase_firestore_consistency_mode
    {
        firebase_firestore_consistency_mode_undefined,
        firebase_firestore_consistency_mode_transaction,
        firebase_firestore_consistency_mode_newTransaction,
        firebase_firestore_consistency_mode_readTime
    };

    struct firebase_firestore_document_write_field_transforms_t
    {
        String fieldPath; // string The path of the field. See Document.fields for the field path syntax reference.
        firebase_firestore_transform_type transform_type = firebase_firestore_transform_type_undefined;
        String transform_content; // string of enum of ServerValue for setToServerValue, string of object of values for increment, maximum and minimum
        //, string of array object for appendMissingElements or removeAllFromArray.
    };

    struct firebase_firestore_document_precondition_t
    {
        String exists;      // bool
        String update_time; // string of timestamp. When set, the target document must exist and have been last updated at that time.
        // A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.Examples : "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
    };

    struct firebase_firestore_document_write_document_transform_t
    {
        String transform_document_path;                                                     // The relative path of document to transform.
        std::vector<firebase_firestore_document_write_field_transforms_t> field_transforms; // array of firebase_firestore_document_write_field_transforms_t data.
    };

    struct firebase_firestore_document_write_t
    {
        String update_masks; // string The fields to update. Use comma (,) to separate between the field names
        firebase_firestore_document_write_field_transforms_t update_transforms;
        firebase_firestore_document_precondition_t current_document; // An optional precondition on the document.
        firebase_firestore_document_write_type type = firebase_firestore_document_write_type_undefined;
        String update_document_content;                                            // A document object to write for firebase_firestore_document_write_type_update.
        String update_document_path;                                               // The relative path of document to update for firebase_firestore_document_write_type_update.
        String delete_document_path;                                               // The relative path of document to delete for firebase_firestore_document_write_type_delete.
        firebase_firestore_document_write_document_transform_t document_transform; // for firebase_firestore_document_write_type_transform
    };

    struct firebase_firestore_transaction_read_only_option_t
    {
        String readTime;
    };

    struct firebase_firestore_transaction_read_write_option_t
    {
        String retryTransaction;
    };

    typedef struct firebase_firestore_transaction_options_t
    {
        firebase_firestore_transaction_read_only_option_t readOnly;
        firebase_firestore_transaction_read_write_option_t readWrite;
    } TransactionOptions;

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
     * Set the database URL
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
        List list;
        return list.existed(firebase_app_list, app_addr) ? app_token : nullptr;
    }

    /** Export the documents in the database to the Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param bucketID The Firebase storage bucket ID in the project.
     * @param storagePath The path in the Firebase Storage data bucket to store the exported database.
     * @param collectionIds Which collection ids to export. Unspecified means all collections. Use comma (,)
     * to separate between the collection ids.
     * .
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    bool exportDocuments(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &bucketID, const String &storagePath, const String &collectionIds = "")
    {
        return false;
    }

    /** Import the exported documents stored in the Firebase Storage data bucket.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param bucketID The Firebase storage bucket ID in the project.
     * @param storagePath The path in the Firebase Storage data bucket that stores the exported database.
     * @param collectionIds Which collection ids to import. Unspecified means all collections included in the import.
     * Use comma (,) to separate between the collection ids.
     * .
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     */
    bool importDocuments(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &bucketID, const String &storagePath, const String &collectionIds = "")
    {
        return false;
    }

    /** Create a document at the defined document path.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param documentPath The relative path of document to create in the collection.
     * @param content A Firestore document.
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
     * .
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication.
     *
     */
    bool createDocument(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &documentPath, const String &content, const String &mask = "")
    {
        return false;
    }

    /** Create a document in the defined collection id.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param collectionId The relative path of document collection id to create the document.
     * @param documentId The document id of document to be created.
     * @param content A Firestore document.
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
     * .
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication.
     *
     */
    bool createDocument(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &collectionId, const String &documentId, const String &content, const String &mask = "")
    {
        return false;
    }

    /** Patch or update a document at the defined path.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param documentPath The relative path of document to patch with the input document.
     * @param content A Firestore document.
     * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
     * @param updateMask The fields to update. If the document exists on the server and has fields not referenced in the mask,
     * they are left unchanged.
     * Fields referenced in the mask, but not present in the input document (content), are deleted from the document on the server.
     * Use comma (,) to separate between the field names.
     * @param mask The fields to return. If not set, returns all fields. If the document has a field that is not present in
     * this mask, that field
     * will not be returned in the response. Use comma (,) to separate between the field names.
     * @param exists When set to true, the target document must exist. When set to false, the target document must not exist.
     * @param updateTime When set, the target document must exist and have been last updated at that time.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication.
     *
     */
    bool patchDocument(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &documentPath, const String &content,
                       const String &updateMask, const String &mask = "", const String &exists = "", const String &updateTime = "")
    {
        return false;
    }

    /** Commits a transaction, while optionally updating documents.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param writes The dyamic array of write object firebase_firestore_document_write_t.
     *
     * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
     *
     * @param transaction A base64-encoded string. If set, applies all writes in this transaction, and commits it.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication.
     *
     */
    bool commitDocument(AsyncClient &aClient, const String &projectId, const String &databaseId,
                        std::vector<firebase_firestore_document_write_t> writes, const String &transaction = "")
    {
        return false;
    }

    /** Applies a batch of write operations.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param writes The dyamic array of write object firebase_firestore_document_write_t.
     * Method does not apply writes atomically and does not guarantee ordering.
     * Each write succeeds or fails independently.
     * You cannot write to the same document more than once per request.
     *
     * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
     *
     * @param labels The JSON object that represents the Labels (map) associated with this batch write.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication.
     *
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchWrite
     *
     */
    bool batchWriteDocuments(AsyncClient &aClient, const String &projectId, const String &databaseId,
                             std::vector<firebase_firestore_document_write_t> writes, const object_t &labels)
    {
        return false;
    }

    /** Get a document at the defined path.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param documentPath The relative path of document to get.
     * @param mask The fields to return. If not set, returns all fields. If the document has a field that is not present in this mask,
     * that field will not be returned in the response. Use comma (,) to separate between the field names.
     * @param transaction Reads the document in a transaction. A base64-encoded string.
     * @param readTime Reads the version of the document at the given time. This may not be older than 270 seconds.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication.
     *
     */
    bool getDocument(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &documentPath, const String &mask = "",
                     const String &transaction = "", const String &readTime = "")
    {
        return false;
    }

    /** Gets multiple documents.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param documentPaths The list of relative path of documents to get. Use comma (,) to separate between the field names.
     * @param mask The fields to return. If not set, returns all fields. If the document has a field that is not present in this mask,
     * that field will not be returned in the response. Use comma (,) to separate between the field names.
     *@param batchOperationCallback The callback fuction that accepts const char* as argument.
     * Union field consistency_selector can be only one of the following
     * @param transaction Reads the document in a transaction. A base64-encoded string.
     * @param newTransaction JSON object that represents TransactionOptions object.
     * Starts a new transaction and reads the documents.
     * Defaults to a read-only transaction.
     * The new transaction ID will be returned as the first response in the stream.
     * @param readTime Reads documents as they were at the given time. This may not be older than 270 seconds.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication.
     *
     * For more detail, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchGet
     *
     */
    bool batchGetDocuments(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &documentPaths, const String &mask,
                           FirestoreBatchOperationsCallback batchOperationCallback, const String &transaction, const object_t &newTransaction, const String &readTime)
    {
        return false;
    }

    /** Starts a new transaction.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param transactionOptions Optional. The TransactionOptions type data that represents the options for creating a new transaction.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     *
     * The TransactionOptions struct contains two properties i.e.
     * readOnly and readWrite.
     *
     * Use readOnly for options for a transaction that can only be used to read documents.
     * Use readWrite for options for a transaction that can be used to read and write documents.
     *
     * The readOnly property contains one property, readTime.
     * The readTime is for reading the documents at the given time. This may not be older than 60 seconds.
     * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
     * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
     *
     * The readWrite property contains one property, retryTransaction.
     * The retryTransaction is a base64-encoded string represents a transaction that can be used to read and write documents.
     *
     * See https://cloud.google.com/firestore/docs/reference/rest/v1/TransactionOptions for transaction options.
     */
    bool beginTransaction(AsyncClient &aClient, const String &projectId, const String &databaseId, TransactionOptions &transactionOptions)
    {
        return false;
    }

    /** Rolls back a transaction.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param transaction Required. A base64-encoded string of the transaction to roll back.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires OAuth2.0 authentication.
     */
    bool rollback(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &transaction)
    {
        return false;
    }

    /** Runs a query.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param documentPath The relative path of document to get.
     * @param structuredQuery The pointer to JSON object that contains the Firestore query. For the description of structuredQuery, see https://cloud.google.com/firestore/docs/reference/rest/v1/StructuredQuery
     * @param consistencyMode Optional. The consistency mode for this transaction e.g. firebase_firestore_consistency_mode_transaction,firebase_firestore_consistency_mode_newTransaction
     * and firebase_firestore_consistency_mode_readTime
     * @param consistency Optional. The value based on consistency mode e.g. transaction string, TransactionOptions (JSON) and date time string.
     *
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/runQuery#body.request_body.FIELDS
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     *
     */
    bool runQuery(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &documentPath, const object_t &structuredQuery = "",
                  firebase_firestore_consistency_mode consistencyMode = firebase_firestore_consistency_mode_undefined,
                  const String &consistency = "")
    {
        return false;
    }

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
    bool deleteDocument(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &documentPath, const String &exists = "", const String &updateTime = "")
    {
        return false;
    }

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
     * @param showMissing If the list should show missing documents.
     * A missing document is a document that does not exist but has sub-documents.
     *
     * @return Boolean value, indicates the success of the operation.
     *
     * @note Use FirebaseData.payload() to get the returned payload.
     *
     * This function requires Email/password, Custom token or OAuth2.0 authentication (when showMissing is true).
     *
     */
    bool listDocuments(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &collectionId, const String &pageSize,
                       const String &pageToken, const String &orderBy, const String &mask, bool showMissing)
    {
        return false;
    }

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
    bool listCollectionIds(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &documentPath, const String &pageSize, const String &pageToken)
    {
        return false;
    }

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
    bool createIndex(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &collectionId,
                     const String &apiScope, const String &queryScope, const object_t &fields)
    {
        return false;
    }

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
    bool deleteIndex(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &collectionId, const String &indexId)
    {
        return false;
    }

    /** Lists the indexes that match the specified filters.
     *
     * @param aClient The async client.
     * @param projectId The Firebase project id (only the name without the firebaseio.com).
     * @param databaseId The Firebase Cloud Firestore database id which is (default) or empty "".
     * @param collectionId The relative path of document colection.
     * @param filter The filter to apply to list results.
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
     * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.collectionGroups.indexes/list
     *
     */
    bool listIndex(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &collectionId, const String &filter = "", int pageSize = -1, const String &pageToken = "")
    {
        return false;
    }

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
    bool getIndex(AsyncClient &aClient, const String &projectId, const String &databaseId, const String &collectionId, const String &indexId)
    {
        return false;
    }

    /**
     * Perform the async task repeatedly.
     * Should be places in main loop function.
     */
    void loop()
    {
        for (size_t clientSlot = 0; clientSlot < firebase_client_list.size(); clientSlot++)
        {
            AsyncClient *aClient = reinterpret_cast<AsyncClient *>(firebase_client_list[clientSlot]);
            if (aClient)
            {
                for (size_t slot = 0; slot < aClient->slotCount(); slot++)
                {
                    aClient->process(true);
                }
                aClient->handleRemove();
            }
        }
    }
};

#endif

#endif