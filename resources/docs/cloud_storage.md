# CloudStorage

## Description

Google Cloud Storage class


```cpp
class CloudStorage
```

## Functions


1. ### 🔹 void resetApp()

Unbind or remove FirebaseApp.


```cpp
void resetApp()
```


2. ## 🔹  void loop()

Perform the async task repeatedly.
Should be places in main loop function.

```cpp
void loop()
```


3. ## 🔹  bool download(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::GetOptions &options)


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters



```cpp
bool download(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::GetOptions &options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `options` - Optional. The GoogleCloudStorage::GetOptions that holds the get options.

**Returns:**

- Boolean value, indicates the success of the operation.

4. ## 🔹  void download(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::GetOptions &options, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters


```cpp
void download(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::GetOptions &options, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `options` - Optional. The GoogleCloudStorage::GetOptions that holds the get options.
- `aResult` - The async result (AsyncResult).


5. ## 🔹  void download(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::GetOptions &options, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters


```cpp
void download(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::GetOptions &options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `options` - Optional. The GoogleCloudStorage::GetOptions that holds the get options.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


6. ## 🔹  bool upload(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::uploadOptions &options)


The bucketid is the Storage bucket Id of object to upload.
The object is the object to be stored in the Storage bucket.
For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties



```cpp
bool upload(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::uploadOptions &options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `options` - Optional. The GoogleCloudStorage::uploadOptions that holds the information for insert options, properties and types of upload.

**Returns:**

- Boolean value, indicates the success of the operation.

7. ## 🔹  void upload(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::uploadOptions &options, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to upload.
The object is the object to be stored in the Storage bucket.
For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties



```cpp
void upload(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::uploadOptions &options, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `options` - Optional. The GoogleCloudStorage::uploadOptions that holds the information for insert options, properties and types of upload.
- `aResult` - The async result (AsyncResult).


8. ## 🔹  void upload(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::uploadOptions &options, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to upload.
The object is the object to be stored in the Storage bucket.
For the insert options (options.insertOptions), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-parameters
For insert properties (options.insertProps), see https://cloud.google.com/storage/docs/json_api/v1/objects/insert#optional-properties


```cpp
void upload(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, file_config_data &file, GoogleCloudStorage::uploadOptions &options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `options` - Optional. The GoogleCloudStorage::uploadOptions that holds the information for insert options, properties and types of upload.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


9. ## 🔹  bool ota(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options)


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters



```cpp
bool ota(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `options` - Optional. The GoogleCloudStorage::GetOptions that holds the get options.

**Returns:**

- Boolean value, indicates the success of the operation.

10. ## 🔹  void ota(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters


```cpp
void ota(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `options` - Optional. The GoogleCloudStorage::GetOptions that holds the get options.
- `aResult` - The async result (AsyncResult).


11. ## 🔹  void ota(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters


```cpp
void ota(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `options` - Optional. The GoogleCloudStorage::GetOptions that holds the get options.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


12. ## 🔹  String getMetadata(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options)


The bucketid is the Storage bucket Id of object to get metadata.
The object is the object in Storage bucket to get metadata.
For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters



```cpp
String getMetadata(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `options` - Optional. The GoogleCloudStorage::GetOptions that holds the get options.

**Returns:**

- `String` - The response payload.

13. ## 🔹  void getMetadata(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to get metadata.
The object is the object in Storage bucket to get metadata.
For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters


```cpp
void getMetadata(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `options` - Optional. The GoogleCloudStorage::GetOptions that holds the get options.
- `aResult` - The async result (AsyncResult).


14. ## 🔹  void getMetadata(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to get metadata.
The object is the object in Storage bucket to get metadata.
For the get options, see https://cloud.google.com/storage/docs/json_api/v1/objects/get#optional-parameters


```cpp
void getMetadata(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::GetOptions &options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `options` - Optional. The GoogleCloudStorage::GetOptions that holds the get options.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


15. ## 🔹  String list(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::ListOptions &options)


The bucketid is the Storage bucket Id to list all objects.
For the list options, see https://cloud.google.com/storage/docs/json_api/v1/objects/list#optional-parameters


```cpp
String list(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::ListOptions &options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id in its constructor.
- `options` - Optional. The GoogleCloudStorage::ListOptions that holds the list options.

**Returns:**

- `String` - The response payload.

16. ## 🔹  void list(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::ListOptions &options, AsyncResult &aResult)


The bucketid is the Storage bucket Id to list all objects.
For the list options, see https://cloud.google.com/storage/docs/json_api/v1/objects/list#optional-parameters


```cpp
void list(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::ListOptions &options, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id in its constructor.
- `options` - Optional. The GoogleCloudStorage::ListOptions that holds the list options.
- `aResult` - The async result (AsyncResult).


17. ## 🔹  void list(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::ListOptions &options, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id to list all objects.
For the list options, see https://cloud.google.com/storage/docs/json_api/v1/objects/list#optional-parameters


```cpp
void list(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::ListOptions &options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id in its constructor.
- `options` - Optional. The GoogleCloudStorage::ListOptions that holds the list options.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


18. ## 🔹  bool deleteObject(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::DeleteOptions options)


The bucketid is the Storage bucket Id of object to delete.
The object is the object in Storage bucket to delete.
For the delete options, see see https://cloud.google.com/storage/docs/json_api/v1/objects/delete#optional-parameters



```cpp
bool deleteObject(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::DeleteOptions options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `options` - Optional. The GoogleCloudStorage::DeleteOptions that holds the delete options.

**Returns:**

- Boolean value, indicates the success of the operation.

19. ## 🔹  void deleteObject(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::DeleteOptions options, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to delete.
The object is the object in Storage bucket to delete.
For the delete options, see see https://cloud.google.com/storage/docs/json_api/v1/objects/delete#optional-parameters


```cpp
void deleteObject(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::DeleteOptions options, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `options` - Optional. The GoogleCloudStorage::DeleteOptions that holds the delete options.
- `aResult` - The async result (AsyncResult).


20. ## 🔹  void deleteObject(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::DeleteOptions options, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to delete.
The object is the object in Storage bucket to delete.
For the delete options, see see https://cloud.google.com/storage/docs/json_api/v1/objects/delete#optional-parameters


```cpp
void deleteObject(AsyncClientClass &aClient, const GoogleCloudStorage::Parent &parent, GoogleCloudStorage::DeleteOptions options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudStorage::Parent object included Storage bucket Id and object in its constructor.
- `options` - Optional. The GoogleCloudStorage::DeleteOptions that holds the delete options.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


21. ## 🔹  void setOTAStorage(OTAStorage &storage)

Set Arduino OTA Storage.

```cpp
void setOTAStorage(OTAStorage &storage)
```

**Params:**

- `storage` - The Arduino `OTAStorage` class object.
