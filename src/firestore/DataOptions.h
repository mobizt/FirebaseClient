/**
 * Created February 10, 2024
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
#ifndef FIRESTORE_DATAOPTIONS_H
#define FIRESTORE_DATAOPTIONS_H

#include <Arduino.h>
#include "./Config.h"

#if defined(ENABLE_FIRESTORE)

enum firebase_firestore_request_type
{
    firebase_firestore_request_type_undefined,
    firebase_firestore_request_type_rollback = 200,
    firebase_firestore_request_type_begin_transaction,
    firebase_firestore_request_type_commit_document,
    firebase_firestore_request_type_batch_write_doc,
    firebase_firestore_request_type_run_query,
    firebase_firestore_request_type_list_collection,
    firebase_firestore_request_type_export_docs,
    firebase_firestore_request_type_import_docs,
    firebase_firestore_request_type_create_doc,
    firebase_firestore_request_type_batch_get_doc,
    firebase_firestore_request_type_create_index,

    firebase_firestore_request_type_get_doc = 300,
    firebase_firestore_request_type_list_doc,
    firebase_firestore_request_type_list_index,
    firebase_firestore_request_type_get_index,

    firebase_firestore_request_type_patch_doc = 400,

    firebase_firestore_request_type_delete_doc = 500,
    firebase_firestore_request_type_delete_index
};

typedef struct firebase_firestore_project_resource_t
{
    String projectId;
    String databaseId;
    String documentPath;

public:
    firebase_firestore_project_resource_t(){}
    firebase_firestore_project_resource_t(const String &projectId, const String &databaseId)
    {
        this->projectId = projectId;
        this->databaseId = databaseId;
    }

    firebase_firestore_project_resource_t(const String &projectId, const String &databaseId, const String &documentPath)
    {
        this->projectId = projectId;
        this->databaseId = databaseId;
        this->documentPath = documentPath;
    }

} ProjectResource;

class FirestoreOptions
{
    friend class Firestore;

public:
    ProjectResource resource;
    String collectionId;
    String documentId;
    String mask;
    String updateMask;
    String payload;
    String exists;
    String updateTime;
    String readTime;
    String transaction;
    int pageSize = 10;
    String pageToken;
    String orderBy;
    bool showMissing = false;
    size_t size = 0;
    firebase_firestore_request_type requestType = firebase_firestore_request_type_undefined;
    unsigned long requestTime = 0;

    void copy(FirestoreOptions &rhs)
    {
        this->resource = rhs.resource;
        this->collectionId = rhs.collectionId;
        this->documentId = rhs.documentId;
        this->mask = rhs.mask;
        this->updateMask = rhs.updateMask;
        this->payload = rhs.payload;
        this->exists = rhs.exists;
        this->updateTime = rhs.updateTime;
        this->readTime = rhs.readTime;
        this->transaction = rhs.transaction;
        this->pageSize = rhs.pageSize;
        this->pageToken = rhs.pageToken;
        this->orderBy = rhs.orderBy;
        this->showMissing = rhs.showMissing;
        this->size = rhs.size;
        this->requestType = rhs.requestType;
        this->requestTime = rhs.requestTime;
    }

private:
};

#endif

#endif