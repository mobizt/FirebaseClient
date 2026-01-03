/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FIRESTORE_DOCUMENTS_H
#define FIRESTORE_DOCUMENTS_H

#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./firestore/DataOptions.h"
#include "./firestore/FirestoreBase.h"

#if defined(ENABLE_FIRESTORE)

using namespace firebase_ns;

#include "./firestore/Query.h"
namespace Firestore
{
    class Documents : public FirestoreBase
    {
        friend class FirebaseApp;
        friend class AppBase;

    public:
        using Parent = Firestore::Parent;

        /** Gets multiple documents.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
         * @return String The response payload.
         *
         * This function requires ServiceAuth authentication.
         *
         * For more detail, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchGet
         *
         */
        String batchGet(AsyncClientClass &aClient, const Parent &parent, const BatchGetDocumentOptions &batchOptions) { return batchGetDoc(aClient, getResultBase(&aClient), NULL, "", parent, batchOptions, false)->c_str(); }

        /** Gets multiple documents.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void batchGet(AsyncClientClass &aClient, const Parent &parent, const BatchGetDocumentOptions &batchOptions, AsyncResult &aResult) { batchGetDoc(aClient, &aResult, NULL, "", parent, batchOptions, true); }

        /** Gets multiple documents.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void batchGet(AsyncClientClass &aClient, const Parent &parent, const BatchGetDocumentOptions &batchOptions, AsyncResultCallback cb, const String &uid = "") { batchGetDoc(aClient, nullptr, cb, uid, parent, batchOptions, true); }

        /** Applies a batch of write operations.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param writes The writes to apply. This Writes object accepts the Write object and Labels (MapValue) associated with this batch write, in its constructor.
         *
         * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth authentication.
         *
         * For more description, see https://cloud.google.com/firestore/docs/reference/rest/v1/projects.databases.documents/batchWrite
         *
         */
        String batchWrite(AsyncClientClass &aClient, const Parent &parent, Writes &writes) { return batchWriteDoc(aClient, getResultBase(&aClient), NULL, "", parent, writes, false)->c_str(); }

        /** Applies a batch of write operations.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void batchWrite(AsyncClientClass &aClient, const Parent &parent, Writes &writes, AsyncResult &aResult) { batchWriteDoc(aClient, &aResult, NULL, "", parent, writes, true); }

        /** Applies a batch of write operations.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void batchWrite(AsyncClientClass &aClient, const Parent &parent, Writes &writes, AsyncResultCallback cb, const String &uid = "") { batchWriteDoc(aClient, nullptr, cb, uid, parent, writes, true); }

        /** Starts a new transaction.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
         * @return String The response payload.
         *
         * This function requires ServiceAuth authentication.
         */
        String beginTransaction(AsyncClientClass &aClient, const Parent &parent, const TransactionOptions &transOptions) { return beginTrans(aClient, getResultBase(&aClient), NULL, "", parent, transOptions, false)->c_str(); }

        /** Starts a new transaction.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void beginTransaction(AsyncClientClass &aClient, const Parent &parent, const TransactionOptions &transOptions, AsyncResult &aResult) { beginTrans(aClient, &aResult, NULL, "", parent, transOptions, true); }

        /** Starts a new transaction.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void beginTransaction(AsyncClientClass &aClient, const Parent &parent, const TransactionOptions &transOptions, AsyncResultCallback cb, const String &uid = "") { beginTrans(aClient, nullptr, cb, uid, parent, transOptions, true); }

        /** Commits a transaction, while optionally updating documents.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param writes The writes to apply.
         *
         * For the write object, see https://firebase.google.com/docs/firestore/reference/rest/v1/Write
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        String commit(AsyncClientClass &aClient, const Parent &parent, Writes &writes) { return commitDoc(aClient, getResultBase(&aClient), NULL, "", parent, writes, false)->c_str(); }

        /** Commits a transaction, while optionally updating documents.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void commit(AsyncClientClass &aClient, const Parent &parent, Writes &writes, AsyncResult &aResult) { commitDoc(aClient, &aResult, NULL, "", parent, writes, true); }

        /** Commits a transaction, while optionally updating documents.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void commit(AsyncClientClass &aClient, const Parent &parent, Writes &writes, AsyncResultCallback cb, const String &uid = "") { commitDoc(aClient, nullptr, cb, uid, parent, writes, true); }

        /** Create a document at the defined document path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to create in the collection.
         * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
         * @param document A Firestore document.
         * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        String createDocument(AsyncClientClass &aClient, Firestore::Parent parent, const String &documentPath, DocumentMask mask, Document<Values::Value> &document)
        {
            parent.setDocPath(documentPath);
            String collectionId, documentId;
            parent.pathResove(collectionId, documentId);
            return createDoc(aClient, getResultBase(&aClient), NULL, "", parent, collectionId, documentId, mask, document, false)->c_str();
        }

        /** Create a document at the defined document path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void createDocument(AsyncClientClass &aClient, Firestore::Parent parent, const String &documentPath, DocumentMask mask, Document<Values::Value> &document, AsyncResult &aResult)
        {
            parent.setDocPath(documentPath);
            String collectionId, documentId;
            parent.pathResove(collectionId, documentId);
            createDoc(aClient, &aResult, NULL, "", parent, collectionId, documentId, mask, document, true);
        }

        /** Create a document at the defined document path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void createDocument(AsyncClientClass &aClient, Firestore::Parent parent, const String &documentPath, DocumentMask mask, Document<Values::Value> &document, AsyncResultCallback cb, const String &uid = "")
        {
            parent.setDocPath(documentPath);
            String collectionId, documentId;
            parent.pathResove(collectionId, documentId);
            createDoc(aClient, nullptr, cb, uid, parent, collectionId, documentId, mask, document, true);
        }

        /** Create a document in the defined collection id.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param collectionId The relative path of document collection id to create the document.
         * @param documentId The document id of document to be created.
         * @param mask The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
         * @param document A Firestore document.
         * See https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.documents#Document
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        String createDocument(AsyncClientClass &aClient, Firestore::Parent parent, const String &collectionId, const String &documentId, DocumentMask mask, Document<Values::Value> &document) { return createDoc(aClient, getResultBase(&aClient), NULL, "", parent, collectionId, documentId, mask, document, false)->c_str(); }

        /** Create a document in the defined collection id.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void createDocument(AsyncClientClass &aClient, Firestore::Parent parent, const String &collectionId, const String &documentId, DocumentMask mask, Document<Values::Value> &document, AsyncResult &aResult) { createDoc(aClient, &aResult, NULL, "", parent, collectionId, documentId, mask, document, true); }

        /** Create a document in the defined collection id.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void createDocument(AsyncClientClass &aClient, Firestore::Parent parent, const String &collectionId, const String &documentId, DocumentMask mask, Document<Values::Value> &document, AsyncResultCallback cb, const String &uid = "") { createDoc(aClient, nullptr, cb, uid, parent, collectionId, documentId, mask, document, true); }

        /** Delete a document at the defined path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to delete.
         * @param currentDocument The Precondition object for an optional precondition on the document.
         * The request will fail if this is set and not met by the target document.
         *
         * There are the member functions exists and updateTime for creating the union field exists and updateTime respectively.
         *
         * The exists option, when set to true, the target document must exist. When set to false, the target document must not exist.
         * The updateTime (timestamp) option, when set, the target document must exist and have been last updated at that time.
         * A timestamp is in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
         * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        String deleteDoc(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, const Precondition &currentDocument) { return deleteDocBase(aClient, getResultBase(&aClient), NULL, "", parent, documentPath, currentDocument, false)->c_str(); }

        /** Delete a document at the defined path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to delete.
         * @param currentDocument The Precondition object for an optional precondition on the document.
         * The request will fail if this is set and not met by the target document.
         *
         * There are the member functions exists and updateTime for creating the union field exists and updateTime respectively.
         *
         * The exists option, when set to true, the target document must exist. When set to false, the target document must not exist.
         * The updateTime (timestamp) option, when set, the target document must exist and have been last updated at that time.
         * A timestamp is in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
         * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
         *
         * @param aResult The async result (AsyncResult).
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        void deleteDoc(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, const Precondition &currentDocument, AsyncResult &aResult) { deleteDocBase(aClient, &aResult, NULL, "", parent, documentPath, currentDocument, true); }

        /** Delete a document at the defined path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to delete.
         * @param currentDocument The Precondition object for an optional precondition on the document.
         * The request will fail if this is set and not met by the target document.
         *
         * There are the member functions exists and updateTime for creating the union field exists and updateTime respectively.
         *
         * The exists option, when set to true, the target document must exist. When set to false, the target document must not exist.
         * The updateTime (timestamp) option, when set, the target document must exist and have been last updated at that time.
         * A timestamp is in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
         * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
         *
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        void deleteDoc(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, const Precondition &currentDocument, AsyncResultCallback cb, const String &uid = "") { deleteDocBase(aClient, nullptr, cb, uid, parent, documentPath, currentDocument, true); }

        /** Get a document at the defined path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
         * @return String The response payload.
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        String get(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, const GetDocumentOptions &options) { return getDoc(aClient, getResultBase(&aClient), NULL, "", parent, documentPath, options, false)->c_str(); }

        /** Get a document at the defined path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void get(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, const GetDocumentOptions &options, AsyncResult &aResult) { getDoc(aClient, &aResult, NULL, "", parent, documentPath, options, true); }

        /** Get a document at the defined path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void get(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, const GetDocumentOptions &options, AsyncResultCallback cb, const String &uid = "") { getDoc(aClient, nullptr, cb, uid, parent, documentPath, options, true); }

        /** List the documents in the defined documents collection.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param collectionId The relative path of document colection.
         * @param listDocsOptions The ListDocumentsOptions object that provides the member functions pageSize, pageToken, orderBy, mask and
         * showMissing for creating the query string options pageSize, pageToken, orderBy, mask and showMissing respectively.
         * The option pageSize is for setting the maximum number of documents to return.
         * The option pageToken is the nextPageToken value returned from a previous List request, if any.
         * The option orderBy is the order to sort results by. For example: priority desc, name.
         * The option mask is for setting the fields to return. If not set, returns all fields.
         * If a document has a field that is not present in this mask, that field will not be returned in the response.
         * The option showMissing is for setting if the list should show missing documents.
         * A missing document is a document that does not exist but has sub-documents.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        String list(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, ListDocumentsOptions listDocsOptions) { return listDocs(aClient, getResultBase(&aClient), NULL, "", parent, collectionId, listDocsOptions, false)->c_str(); }

        /** List the documents in the defined documents collection.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param collectionId The relative path of document colection.
         * @param listDocsOptions The ListDocumentsOptions object that provides the member functions pageSize, pageToken, orderBy, mask and
         * showMissing for creating the query string options pageSize, pageToken, orderBy, mask and showMissing respectively.
         * The option pageSize is for setting the maximum number of documents to return.
         * The option pageToken is the nextPageToken value returned from a previous List request, if any.
         * The option orderBy is the order to sort results by. For example: priority desc, name.
         * The option mask is for setting the fields to return. If not set, returns all fields.
         * If a document has a field that is not present in this mask, that field will not be returned in the response.
         * The option showMissing is for setting if the list should show missing documents.
         * A missing document is a document that does not exist but has sub-documents.
         *
         * @param aResult The async result (AsyncResult).
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        void list(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, ListDocumentsOptions listDocsOptions, AsyncResult &aResult) { listDocs(aClient, &aResult, NULL, "", parent, collectionId, listDocsOptions, true); }

        /** List the documents in the defined documents collection.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param collectionId The relative path of document colection.
         * @param listDocsOptions The ListDocumentsOptions object that provides the member functions pageSize, pageToken, orderBy, mask and
         * showMissing for creating the query string options pageSize, pageToken, orderBy, mask and showMissing respectively.
         * The option pageSize is for setting the maximum number of documents to return.
         * The option pageToken is the nextPageToken value returned from a previous List request, if any.
         * The option orderBy is the order to sort results by. For example: priority desc, name.
         * The option mask is for setting the fields to return. If not set, returns all fields.
         * If a document has a field that is not present in this mask, that field will not be returned in the response.
         * The option showMissing is for setting if the list should show missing documents.
         * A missing document is a document that does not exist but has sub-documents.
         *
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        void list(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const ListDocumentsOptions &listDocsOptions, AsyncResultCallback cb, const String &uid = "") { listDocs(aClient, nullptr, cb, uid, parent, collectionId, listDocsOptions, true); }

        /** List the document collection ids in the defined document path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to get its collections' id.
         * @param listCollectionIdsOptions The ListCollectionIdsOptions object that provides the member functions pageSize, pageToken and readTime
         * for creating the query string options pageSize, pageToken and readTime respectively.
         * The option pageSize is for setting the  maximum number of results to return.
         * The option pageToken is the page token. Must be a value from ListCollectionIdsResponse.
         * The option readTime is the timestamp for reading the documents as they were at the given time.
         * This must be a microsecond precision timestamp within the past one hour, or if Point-in-Time Recovery is enabled,
         * can additionally be a whole minute timestamp within the past 7 days.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth authentication.
         *
         */
        String listCollectionIds(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, ListCollectionIdsOptions listCollectionIdsOptions) { return listCollIds(aClient, getResultBase(&aClient), NULL, "", parent, documentPath, listCollectionIdsOptions, false)->c_str(); }

        /** List the document collection ids in the defined document path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to get its collections' id.
         * @param listCollectionIdsOptions The ListCollectionIdsOptions object that provides the member functions pageSize, pageToken and readTime
         * for creating the query string options pageSize, pageToken and readTime respectively.
         * The option pageSize is for setting the  maximum number of results to return.
         * The option pageToken is the page token. Must be a value from ListCollectionIdsResponse.
         * The option readTime is the timestamp for reading the documents as they were at the given time.
         * This must be a microsecond precision timestamp within the past one hour, or if Point-in-Time Recovery is enabled,
         * can additionally be a whole minute timestamp within the past 7 days.
         *
         * @param aResult The async result (AsyncResult).
         *
         * This function requires ServiceAuth authentication.
         *
         */
        void listCollectionIds(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, ListCollectionIdsOptions listCollectionIdsOptions, AsyncResult &aResult) { listCollIds(aClient, &aResult, NULL, "", parent, documentPath, listCollectionIdsOptions, true); }

        /** List the document collection ids in the defined document path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to get its collections' id.
         * @param listCollectionIdsOptions The ListCollectionIdsOptions object that provides the member functions pageSize, pageToken and readTime
         * for creating the query string options pageSize, pageToken and readTime respectively.
         * The option pageSize is for setting the  maximum number of results to return.
         * The option pageToken is the page token. Must be a value from ListCollectionIdsResponse.
         * The option readTime is the timestamp for reading the documents as they were at the given time.
         * This must be a microsecond precision timestamp within the past one hour, or if Point-in-Time Recovery is enabled,
         * can additionally be a whole minute timestamp within the past 7 days.
         *
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * This function requires ServiceAuth authentication.
         *
         */
        void listCollectionIds(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, const ListCollectionIdsOptions &listCollectionIdsOptions, AsyncResultCallback cb, const String &uid = "") { listCollIds(aClient, nullptr, cb, uid, parent, documentPath, listCollectionIdsOptions, true); }

        /** Patch or update a document at the defined path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to patch with the input document.
         * @param patchOptions The PatchDocumentOptions object that included the DocumentMask (updateMask), DocumentMask (mask), Precondition (currentDocument) in its constructor.
         *
         * updateMask is he fields to update. If the document exists on the server and has fields not referenced in the mask,
         * they are left unchanged.
         * Fields referenced in the mask, but not present in the input document (content), are deleted from the document on the server.
         * Use comma (,) to separate between the field names.
         * mask is fields to return. If not set, returns all fields. If the document has a field that is not present in
         * this mask, that field will not be returned in the response. Use comma (,) to separate between the field names.
         * And currentDocument is an optional precondition on the document. The request will fail if this is set and not met by the target document.
         *
         * When currentDocument as defined with true or false as constructor argument, the exists condition will be set.
         * When currentDocument as defined with string(Timestamp format) as constructor argument, the updateTime condition will be set.
         *
         * When exists condition was set to true, the target document must exist. When set to false, the target document must not exist.
         * When updateTime was set, the target document must exist and have been last updated at that time.
         * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
         * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
         *  @param document A Firestore document.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        String patch(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, PatchDocumentOptions patchOptions, Document<Values::Value> &document) { return patchDoc(aClient, getResultBase(&aClient), NULL, "", parent, documentPath, patchOptions, document, false)->c_str(); }

        /** Patch or update a document at the defined path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to patch with the input document.
         * @param patchOptions The PatchDocumentOptions object that included the DocumentMask (updateMask), DocumentMask (mask), Precondition (currentDocument) in its constructor.
         *
         * updateMask is he fields to update. If the document exists on the server and has fields not referenced in the mask,
         * they are left unchanged.
         * Fields referenced in the mask, but not present in the input document (content), are deleted from the document on the server.
         * Use comma (,) to separate between the field names.
         * mask is fields to return. If not set, returns all fields. If the document has a field that is not present in
         * this mask, that field will not be returned in the response. Use comma (,) to separate between the field names.
         * And currentDocument is an optional precondition on the document. The request will fail if this is set and not met by the target document.
         *
         * When currentDocument as defined with true or false as constructor argument, the exists condition will be set.
         * When currentDocument as defined with string(Timestamp format) as constructor argument, the updateTime condition will be set.
         *
         * When exists condition was set to true, the target document must exist. When set to false, the target document must not exist.
         * When updateTime was set, the target document must exist and have been last updated at that time.
         * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
         * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
         *  @param document A Firestore document.
         *
         * @param aResult The async result (AsyncResult).
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        void patch(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, PatchDocumentOptions patchOptions, Document<Values::Value> &document, AsyncResult &aResult) { patchDoc(aClient, &aResult, NULL, "", parent, documentPath, patchOptions, document, true); }

        /** Patch or update a document at the defined path.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param documentPath The relative path of document to patch with the input document.
         * @param patchOptions The PatchDocumentOptions object that included the DocumentMask (updateMask), DocumentMask (mask), Precondition (currentDocument) in its constructor.
         *
         * updateMask is he fields to update. If the document exists on the server and has fields not referenced in the mask,
         * they are left unchanged.
         * Fields referenced in the mask, but not present in the input document (content), are deleted from the document on the server.
         * Use comma (,) to separate between the field names.
         * mask is fields to return. If not set, returns all fields. If the document has a field that is not present in
         * this mask, that field will not be returned in the response. Use comma (,) to separate between the field names.
         * And currentDocument is an optional precondition on the document. The request will fail if this is set and not met by the target document.
         *
         * When currentDocument as defined with true or false as constructor argument, the exists condition will be set.
         * When currentDocument as defined with string(Timestamp format) as constructor argument, the updateTime condition will be set.
         *
         * When exists condition was set to true, the target document must exist. When set to false, the target document must not exist.
         * When updateTime was set, the target document must exist and have been last updated at that time.
         * A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
         * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
         *  @param document A Firestore document.
         *
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         */
        void patch(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, PatchDocumentOptions patchOptions, Document<Values::Value> &document, AsyncResultCallback cb, const String &uid = "") { patchDoc(aClient, nullptr, cb, uid, parent, documentPath, patchOptions, document, true); }

        /** Rolls back a transaction.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param transaction Required. A base64-encoded string of the transaction to roll back.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth authentication.
         */
        String rollback(AsyncClientClass &aClient, const Parent &parent, const String &transaction) { return transRollback(aClient, getResultBase(&aClient), NULL, "", parent, transaction, false)->c_str(); }

        /** Rolls back a transaction.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param transaction Required. A base64-encoded string of the transaction to roll back.
         *
         * @param aResult The async result (AsyncResult).
         *
         * This function requires ServiceAuth authentication.
         */
        void rollback(AsyncClientClass &aClient, const Parent &parent, const String &transaction, AsyncResult &aResult) { transRollback(aClient, &aResult, NULL, "", parent, transaction, true); }

        /** Rolls back a transaction.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param transaction Required. A base64-encoded string of the transaction to roll back.
         *
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * This function requires ServiceAuth authentication.
         */
        void rollback(AsyncClientClass &aClient, const Parent &parent, const String &transaction, AsyncResultCallback cb, const String &uid = "") { transRollback(aClient, nullptr, cb, uid, parent, transaction, true); }

#if defined(ENABLE_FIRESTORE_QUERY)

        /** Runs a query.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
         * @return String The response payload.
         *
         * This function requires ServiceAuth, CustomAuth, UserAuth, CustomToken or IDToken authentication.
         *
         * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.documents/runQuery
         *
         */
        String runQuery(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, QueryOptions queryOptions) { return runQueryImpl(aClient, getResultBase(&aClient), NULL, "", parent, documentPath, queryOptions, false)->c_str(); }

        /** Runs a query.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void runQuery(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, QueryOptions queryOptions, AsyncResult &aResult) { runQueryImpl(aClient, &aResult, NULL, "", parent, documentPath, queryOptions, true); }

        /** Runs a query.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void runQuery(AsyncClientClass &aClient, const Parent &parent, const String &documentPath, const QueryOptions &queryOptions, AsyncResultCallback cb, const String &uid = "") { runQueryImpl(aClient, nullptr, cb, uid, parent, documentPath, queryOptions, true); }

#endif
    };
}
#endif
#endif