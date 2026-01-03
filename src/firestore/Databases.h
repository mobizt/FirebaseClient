/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FIRESTORE_DATABASES_H
#define FIRESTORE_DATABASES_H

#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./firestore/DataOptions.h"
#include "./firestore/FirestoreBase.h"

#if defined(ENABLE_FIRESTORE)

using namespace firebase_ns;

#if defined(ENABLE_FIRESTORE_QUERY)
#include "./firestore/Query.h"
#include "./firestore/Query.cpp"
#endif

namespace Firestore
{
    class Databases : public FirestoreBase
    {
        friend class AppBase;

    public:
        using Parent = Firestore::Parent;

        ~Databases() {}

        Databases() {}

        /** Export the documents in the database to the Firebase Storage data bucket.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param exportOptions The EximDocumentOptions object included collectionIds, bucketID and storagePath in its constructor.
         * The EximDocumentOptions constructor parameters.
         * - collectionIds is collection ids to export. Unspecified means all collections. Use comma (,)
         *   to separate between the collection ids.
         * - bucketID is the Firebase storage bucket ID in the project.
         * - storagePath is the path in the Firebase Storage data bucket to store the exported database.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        String exportDocuments(AsyncClientClass &aClient, const Parent &parent, const EximDocumentOptions &exportOptions) { return eximDocs(aClient, getResultBase(&aClient), NULL, "", parent, exportOptions, false, false)->c_str(); }

        /** Export the documents in the database to the Firebase Storage data bucket.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void exportDocuments(AsyncClientClass &aClient, const Parent &parent, const EximDocumentOptions &exportOptions, AsyncResult &aResult) { eximDocs(aClient, &aResult, NULL, "", parent, exportOptions, false, true); }

        /** Export the documents in the database to the Firebase Storage data bucket.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void exportDocuments(AsyncClientClass &aClient, const Parent &parent, const EximDocumentOptions &exportOptions, AsyncResultCallback cb, const String &uid = "") { eximDocs(aClient, nullptr, cb, uid, parent, exportOptions, false, true); }

        /** Import the exported documents stored in the Firebase Storage data bucket.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database id should be (default) or empty "".
         * @param importOptions The EximDocumentOptions object included collectionIds, bucketID and storagePath in its constructor.
         * The EximDocumentOptions constructor parameters.
         * - collectionIds is collection ids to import. Unspecified means all collections included in the import.
         *   Use comma (,) to separate between the collection ids.
         * - bucketID is the Firebase storage bucket ID in the project.
         * - storagePath is the path in the Firebase Storage data bucket that stores the exported database.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        String importDocuments(AsyncClientClass &aClient, const Parent &parent, const EximDocumentOptions &importOptions) { return eximDocs(aClient, getResultBase(&aClient), NULL, "", parent, importOptions, true, false)->c_str(); }

        /** Import the exported documents stored in the Firebase Storage data bucket.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void importDocuments(AsyncClientClass &aClient, const Parent &parent, const EximDocumentOptions &importOptions, AsyncResult &aResult) { eximDocs(aClient, &aResult, NULL, "", parent, importOptions, true, true); }

        /** Import the exported documents stored in the Firebase Storage data bucket.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
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
        void importDocuments(AsyncClientClass &aClient, const Parent &parent, const EximDocumentOptions &importOptions, AsyncResultCallback cb, const String &uid = "") { eximDocs(aClient, nullptr, cb, uid, parent, importOptions, true, true); }

        /** Create a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to create.
         * @param database The Firestore::Database object that hold the database information to create.
         *
         * The Database information should be set via the following functions befor creation.
         * concurrencyMode, appEngineIntegrationMode, deleteProtectionState, pointInTimeRecoveryEnablement, databaseType, locationId and name.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        String create(AsyncClientClass &aClient, const Parent &parent, const Database &database) { return manageDatabase(aClient, getResultBase(&aClient), NULL, "", parent, database.c_str(), "", Firestore::cf_mode_create, false)->c_str(); }

        /** Create a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to create.
         * @param database The Firestore::Database object that hold the database information to create.
         * @param aResult The async result (AsyncResult).
         *
         * The Database information should be set via the following functions befor creation.
         * concurrencyMode, appEngineIntegrationMode, deleteProtectionState, pointInTimeRecoveryEnablement, databaseType, locationId and name.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void create(AsyncClientClass &aClient, const Parent &parent, const Database &database, AsyncResult &aResult) { manageDatabase(aClient, &aResult, NULL, "", parent, database.c_str(), "", Firestore::cf_mode_create, true); }

        /** Create a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to create.
         * @param database The Firestore::Database object that hold the database information to create.
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * The Database information should be set via the following functions befor creation.
         * concurrencyMode, appEngineIntegrationMode, deleteProtectionState, pointInTimeRecoveryEnablement, databaseType, locationId and name.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void create(AsyncClientClass &aClient, const Parent &parent, const Database &database, AsyncResultCallback cb, const String &uid = "") { manageDatabase(aClient, nullptr, cb, uid, parent, database.c_str(), "", Firestore::cf_mode_create, true); }

        /** Deletes a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to delete.
         * @param etag The current etag of the Database.
         * If an etag is provided and does not match the current etag of the database, deletion will be blocked and a FAILED_PRECONDITION error will be returned.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        String deleteDatabase(AsyncClientClass &aClient, const Parent &parent, const String &etag) { return manageDatabase(aClient, getResultBase(&aClient), NULL, "", parent, "", etag, Firestore::cf_mode_delete, false)->c_str(); }

        /** Deletes a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to delete.
         * @param etag The current etag of the Database.
         * If an etag is provided and does not match the current etag of the database, deletion will be blocked and a FAILED_PRECONDITION error will be returned.
         * @param aResult The async result (AsyncResult).
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void deleteDatabase(AsyncClientClass &aClient, const Parent &parent, const String &etag, AsyncResult &aResult) { manageDatabase(aClient, &aResult, NULL, "", parent, "", etag, Firestore::cf_mode_delete, true); }

        /** Deletes a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to delete.
         * @param etag The current etag of the Database.
         * If an etag is provided and does not match the current etag of the database, deletion will be blocked and a FAILED_PRECONDITION error will be returned.
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void deleteDatabase(AsyncClientClass &aClient, const Parent &parent, const String &etag, AsyncResultCallback cb, const String &uid = "") { manageDatabase(aClient, nullptr, cb, uid, parent, "", etag, Firestore::cf_mode_delete, true); }

        /** Gets information about a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to get the infomation.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        String get(AsyncClientClass &aClient, const Parent &parent) { return manageDatabase(aClient, getResultBase(&aClient), NULL, "", parent, "", "", Firestore::cf_mode_get, false)->c_str(); }

        /** Gets information about a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to get the infomation.
         * @param aResult The async result (AsyncResult).
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void get(AsyncClientClass &aClient, const Parent &parent, AsyncResult &aResult) { manageDatabase(aClient, &aResult, NULL, "", parent, "", "", Firestore::cf_mode_get, true); }

        /** Gets information about a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to get the infomation.
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void get(AsyncClientClass &aClient, const Parent &parent, AsyncResultCallback cb, const String &uid = "") { manageDatabase(aClient, nullptr, cb, uid, parent, "", "", Firestore::cf_mode_get, true); }

        /** List all the databases in the project.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * Leave the Firestore database Id to be empty for this case.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        String list(AsyncClientClass &aClient, const Parent &parent) { return manageDatabase(aClient, getResultBase(&aClient), NULL, "", parent, "", "", Firestore::cf_mode_list, false)->c_str(); }

        /** List all the databases in the project.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * Leave the Firestore database Id to be empty for this case.
         * @param aResult The async result (AsyncResult).
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void list(AsyncClientClass &aClient, const Parent &parent, AsyncResult &aResult) { manageDatabase(aClient, &aResult, NULL, "", parent, "", "", Firestore::cf_mode_list, true); }

        /** List all the databases in the project.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * Leave the Firestore database Id to be empty for this case.
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void list(AsyncClientClass &aClient, const Parent &parent, AsyncResultCallback cb, const String &uid = "") { manageDatabase(aClient, nullptr, cb, uid, parent, "", "", Firestore::cf_mode_list, true); }

        /** Updates a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to update.
         * @param database The Firestore::Database object that hold the database information to update.
         * @param updateMask The list of fields to be updated. This is a comma-separated list of fully qualified names of fields.
         * Example: "user.displayName,photo"
         *
         * The Database information should be set via the following functions befor creation.
         * concurrencyMode, appEngineIntegrationMode, deleteProtectionState, pointInTimeRecoveryEnablement, databaseType, locationId and name.
         *
         * @return String The response payload.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        String patch(AsyncClientClass &aClient, const Parent &parent, const Database &database, const String &updateMask) { return manageDatabase(aClient, getResultBase(&aClient), NULL, "", parent, database.c_str(), updateMask, Firestore::cf_mode_patch, false)->c_str(); }

        /** Updates a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to update.
         * @param database The Firestore::Database object that hold the database information to update.
         * @param updateMask The list of fields to be updated. This is a comma-separated list of fully qualified names of fields.
         * Example: "user.displayName,photo"
         * @param aResult The async result (AsyncResult).
         *
         * The Database information should be set via the following functions befor creation.
         * concurrencyMode, appEngineIntegrationMode, deleteProtectionState, pointInTimeRecoveryEnablement, databaseType, locationId and name.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void patch(AsyncClientClass &aClient, const Parent &parent, const Database &database, const String &updateMask, AsyncResult &aResult) { manageDatabase(aClient, &aResult, NULL, "", parent, database.c_str(), updateMask, Firestore::cf_mode_patch, true); }

        /** Updates a database.
         *
         * @param aClient The async client.
         * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
         * The Firebase project Id should be only the name without the firebaseio.com.
         * The Firestore database Id is the Id of database to update.
         * @param database The Firestore::Database object that hold the database information to update.
         * @param updateMask The list of fields to be updated. This is a comma-separated list of fully qualified names of fields.
         * Example: "user.displayName,photo"
         * @param cb The async result callback (AsyncResultCallback).
         * @param uid The user specified UID of async result (optional).
         *
         * The Database information should be set via the following functions befor creation.
         * concurrencyMode, appEngineIntegrationMode, deleteProtectionState, pointInTimeRecoveryEnablement, databaseType, locationId and name.
         *
         * This function requires ServiceAuth or AccessToken authentication.
         *
         */
        void patch(AsyncClientClass &aClient, const Parent &parent, const Database &database, const String &updateMask, AsyncResultCallback cb, const String &uid = "") { manageDatabase(aClient, nullptr, cb, uid, parent, database.c_str(), updateMask, Firestore::cf_mode_patch, true); }

        class Indexes : public FirestoreBase
        {
        public:
            Indexes() {}

            /** Creates the specified index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param index The Index object that provides an index definition.
             *
             * @return String The response payload.
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/create
             *
             * This function requires ServiceAuth authentication.
             *
             */
            String create(AsyncClientClass &aClient, const Parent &parent, const DatabaseIndex::Index &index) { return databaseIndexManager(aClient, getResultBase(&aClient), NULL, "", parent, index, "", false, false)->c_str(); }

            /** Creates the specified index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param index The Index object that provides an index definition.
             * @param aResult The async result (AsyncResult).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/create
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void create(AsyncClientClass &aClient, const Parent &parent, const DatabaseIndex::Index &index, AsyncResult &aResult) { databaseIndexManager(aClient, &aResult, NULL, "", parent, index, "", false, true); }

            /** Creates the specified index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param index The Index object that provides an index definition.
             * @param cb The async result callback (AsyncResultCallback).
             * @param uid The user specified UID of async result (optional).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/create
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void create(AsyncClientClass &aClient, const Parent &parent, const DatabaseIndex::Index &index, AsyncResultCallback cb, const String &uid = "") { databaseIndexManager(aClient, nullptr, cb, uid, parent, index, "", false, true); }

            /** Deletes an index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param indexId The index to delete.
             *
             * @return String The response payload.
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/delete
             *
             * This function requires ServiceAuth authentication.
             *
             */
            String deleteIndex(AsyncClientClass &aClient, const Parent &parent, const String &indexId) { return databaseIndexManager(aClient, getResultBase(&aClient), NULL, "", parent, DatabaseIndex::Index(), indexId, true, false)->c_str(); }

            /** Deletes an index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param indexId The index to delete.
             * @param aResult The async result (AsyncResult).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/delete
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void deleteIndex(AsyncClientClass &aClient, const Parent &parent, const String &indexId, AsyncResult &aResult) { databaseIndexManager(aClient, &aResult, NULL, "", parent, DatabaseIndex::Index(), indexId, true, true); }

            /** Deletes an index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param indexId The index to delete.
             * @param cb The async result callback (AsyncResultCallback).
             * @param uid The user specified UID of async result (optional).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/delete
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void deleteIndex(AsyncClientClass &aClient, const Parent &parent, const String &indexId, AsyncResultCallback cb, const String &uid = "") { databaseIndexManager(aClient, nullptr, cb, uid, parent, DatabaseIndex::Index(), indexId, true, true); }

            /** Gets an index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param indexId The index to get.
             *
             * @return String The response payload.
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/get
             *
             * This function requires ServiceAuth authentication.
             *
             */
            String get(AsyncClientClass &aClient, const Parent &parent, const String &indexId) { return databaseIndexManager(aClient, getResultBase(&aClient), NULL, "", parent, DatabaseIndex::Index(), indexId, false, false)->c_str(); }

            /** Gets an index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param indexId The index to get.
             * @param aResult The async result (AsyncResult).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/get
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void get(AsyncClientClass &aClient, const Parent &parent, const String &indexId, AsyncResult &aResult) { databaseIndexManager(aClient, &aResult, NULL, "", parent, DatabaseIndex::Index(), indexId, false, true); }

            /** Gets an index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param indexId The index to get.
             * @param cb The async result callback (AsyncResultCallback).
             * @param uid The user specified UID of async result (optional).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/get
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void get(AsyncClientClass &aClient, const Parent &parent, const String &indexId, AsyncResultCallback cb, const String &uid = "") { databaseIndexManager(aClient, nullptr, cb, uid, parent, DatabaseIndex::Index(), indexId, false, true); }

            /** Lists the indexes.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             *
             * @return String The response payload.
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/list
             *
             * This function requires ServiceAuth authentication.
             *
             */
            String list(AsyncClientClass &aClient, const Parent &parent) { return databaseIndexManager(aClient, getResultBase(&aClient), NULL, "", parent, DatabaseIndex::Index(), "", false, false)->c_str(); }

            /** Lists the indexes.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param indexId The index to get.
             * @param aResult The async result (AsyncResult).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/list
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void list(AsyncClientClass &aClient, const Parent &parent, AsyncResult &aResult) { databaseIndexManager(aClient, &aResult, NULL, "", parent, DatabaseIndex::Index(), "", false, true); }

            /** Lists the indexes.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param cb The async result callback (AsyncResultCallback).
             * @param uid The user specified UID of async result (optional).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1beta1/projects.databases.indexes/list
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void list(AsyncClientClass &aClient, const Parent &parent, AsyncResultCallback cb, const String &uid = "") { databaseIndexManager(aClient, nullptr, cb, uid, parent, DatabaseIndex::Index(), "", false, true); }
        };
    };
}
#endif
#endif