/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FIRESTORE_COLLECTION_GROUPS_H
#define FIRESTORE_COLLECTION_GROUPS_H

#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./firestore/DataOptions.h"
#include "./firestore/FirestoreBase.h"

#if defined(ENABLE_FIRESTORE)

using namespace firebase_ns;

namespace Firestore
{
    class CollectionGroups : public FirestoreBase
    {
    public:
        using Parent = Firestore::Parent;

        CollectionGroups() {}

        class Indexes : public FirestoreBase
        {
        public:
            Indexes() {}

            /** Creates a composite index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param collectionId The collection Id.
             * @param index The Cloud Firestore indexes enable simple and complex queries against documents in a database.
             *
             * @return String The response payload.
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/create
             *
             * This function requires ServiceAuth authentication.
             *
             */
            String create(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const CollectionGroupsIndex::Index &index) { return collectionGroupIndexManager(aClient, getResultBase(&aClient), NULL, "", parent, index, collectionId, "", false, false)->c_str(); }

            /** Creates a composite index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param collectionId The collection Id.
             * @param index The Cloud Firestore indexes enable simple and complex queries against documents in a database.
             * @param aResult The async result (AsyncResult).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/create
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void create(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const CollectionGroupsIndex::Index &index, AsyncResult &aResult) { collectionGroupIndexManager(aClient, &aResult, NULL, "", parent, index, collectionId, "", false, true); }

            /** Creates a composite index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param collectionId The collection Id.
             * @param index The Cloud Firestore indexes enable simple and complex queries against documents in a database.
             * @param cb The async result callback (AsyncResultCallback).
             * @param uid The user specified UID of async result (optional).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/create
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void create(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const CollectionGroupsIndex::Index &index, AsyncResultCallback cb, const String &uid = "") { collectionGroupIndexManager(aClient, nullptr, cb, uid, parent, index, collectionId, "", false, true); }

            /** Deletes a composite index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param collectionId The collection Id.
             * @param indexId The index to delete.
             *
             * @return String The response payload.
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/delete
             *
             * This function requires ServiceAuth authentication.
             *
             */
            String deleteIndex(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const String &indexId)
            {
                CollectionGroupsIndex::Index idx;
                return collectionGroupIndexManager(aClient, getResultBase(&aClient), NULL, "", parent, idx, collectionId, indexId, true, false)->c_str();
            }

            /** Deletes a composite index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param collectionId The collection Id.
             * @param indexId The index to delete.
             * @param aResult The async result (AsyncResult).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/delete
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void deleteIndex(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const String &indexId, AsyncResult &aResult)
            {
                CollectionGroupsIndex::Index idx;
                collectionGroupIndexManager(aClient, &aResult, NULL, "", parent, idx, collectionId, indexId, true, true);
            }

            /** Deletes a composite index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param collectionId The collection Id.
             * @param indexId The index to delete.
             * @param cb The async result callback (AsyncResultCallback).
             * @param uid The user specified UID of async result (optional).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/delete
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void deleteIndex(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const String &indexId, AsyncResultCallback cb, const String &uid = "")
            {
                CollectionGroupsIndex::Index idx;
                collectionGroupIndexManager(aClient, nullptr, cb, uid, parent, idx, collectionId, indexId, true, true);
            }

            /** Gets a composite index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param collectionId The collection Id.
             * @param indexId The index to get.
             *
             * @return String The response payload.
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/get
             *
             * This function requires ServiceAuth authentication.
             *
             */
            String get(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const String &indexId)
            {
                CollectionGroupsIndex::Index idx;
                return collectionGroupIndexManager(aClient, getResultBase(&aClient), NULL, "", parent, idx, collectionId, indexId, false, false)->c_str();
            }

            /** Gets a composite index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param collectionId The collection Id.
             * @param indexId The index to get.
             * @param aResult The async result (AsyncResult).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/get
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void get(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const String &indexId, AsyncResult &aResult)
            {
                CollectionGroupsIndex::Index idx;
                collectionGroupIndexManager(aClient, &aResult, NULL, "", parent, idx, collectionId, indexId, false, true);
            }

            /** Gets a composite index.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param collectionId The collection Id.
             * @param indexId The index to get.
             * @param cb The async result callback (AsyncResultCallback).
             * @param uid The user specified UID of async result (optional).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/get
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void get(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, const String &indexId, AsyncResultCallback cb, const String &uid = "")
            {
                CollectionGroupsIndex::Index idx;
                collectionGroupIndexManager(aClient, nullptr, cb, uid, parent, idx, collectionId, indexId, false, true);
            }

            /** Lists composite indexes.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             *
             * @return String The response payload.
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/list
             *
             * This function requires ServiceAuth authentication.
             *
             */
            String list(AsyncClientClass &aClient, const Parent &parent, const String &collectionId)
            {
                CollectionGroupsIndex::Index idx;
                return collectionGroupIndexManager(aClient, getResultBase(&aClient), NULL, "", parent, idx, collectionId, "", false, false)->c_str();
            }

            /** Lists composite indexes.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param indexId The index to get.
             * @param aResult The async result (AsyncResult).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/list
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void list(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, AsyncResult &aResult)
            {
                CollectionGroupsIndex::Index idx;
                collectionGroupIndexManager(aClient, &aResult, NULL, "", parent, idx, collectionId, "", false, true);
            }

            /** Lists composite indexes.
             *
             * @param aClient The async client.
             * @param parent The Firestore::Parent object included project Id and database Id in its constructor.
             * The Firebase project Id should be only the name without the firebaseio.com.
             * The Firestore database id should be (default) or empty "".
             * @param cb The async result callback (AsyncResultCallback).
             * @param uid The user specified UID of async result (optional).
             *
             * For more description, see https://firebase.google.com/docs/firestore/reference/rest/v1/projects.databases.collectionGroups.indexes/list
             *
             * This function requires ServiceAuth authentication.
             *
             */
            void list(AsyncClientClass &aClient, const Parent &parent, const String &collectionId, AsyncResultCallback cb, const String &uid = "")
            {
                CollectionGroupsIndex::Index idx;
                collectionGroupIndexManager(aClient, nullptr, cb, uid, parent, idx, collectionId, "", false, true);
            }
        };
    };
}
#endif
#endif