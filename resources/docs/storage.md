# Storage

## Description

Firebase Storage class


```cpp
class Storage
```

## Functions


1. ## 🔹  void url(const String &url)

Set the Firebase Storage URL

```cpp
void url(const String &url)
```

**Params:**

- `url` - The Firebase Storage URL.


2. ## 🔹  void loop()

Perform the async task repeatedly.
Should be places in main loop function.

```cpp
void loop()
```


3. ## 🔹  bool download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file)


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).


```cpp
bool download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.

**Returns:**

- Boolean value, indicates the success of the operation.

4. ## 🔹  void download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).


```cpp
void download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `aResult` - The async result (AsyncResult).


5. ## 🔹  void download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).


```cpp
void download(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


6. ## 🔹  bool upload(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, const String &mime)


The bucketid is the Storage bucket Id of object to upload.
The object is the object to be stored in the Storage bucket.
The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).


```cpp
bool upload(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, const String &mime)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `mime` - The file MIME type

**Returns:**

- Boolean value, indicates the success of the operation.

7. ## 🔹  void upload(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, const String &mime, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to upload.
The object is the object to be stored in the Storage bucket.



```cpp
void upload(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, const String &mime, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `mime` - The file MIME type
- `aResult` - The async result (AsyncResult).


8. ## 🔹  void upload(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, const String &mime, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to upload.
The object is the object to be stored in the Storage bucket.


```cpp
void upload(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, file_config_data &file, const String &mime, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `mime` - The file MIME type
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


9. ## 🔹  bool ota(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).


```cpp
bool ota(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.

**Returns:**

- Boolean value, indicates the success of the operation.

10. ## 🔹  void ota(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).


```cpp
void ota(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
- `aResult` - The async result (AsyncResult).


11. ## 🔹  void ota(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to download.
The object is the object in Storage bucket to download.
The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).


```cpp
void ota(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id, object and/or access token in its constructor.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


12. ## 🔹  String getMetadata(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)


The bucketid is the Storage bucket Id of object to get metadata.
The object is the object in Storage bucket to get metadata.
The access token is the Firebase Storage's file access token which used only for priviledge file download access in non-authentication mode (NoAuth).



```cpp
String getMetadata(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.

**Returns:**

- `String` - The response payload.

13. ## 🔹  void getMetadata(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to get metadata.
The object is the object in Storage bucket to get metadata.


```cpp
void getMetadata(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
- `aResult` - The async result (AsyncResult).


14. ## 🔹  void getMetadata(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to get metadata.
The object is the object in Storage bucket to get metadata.


```cpp
void getMetadata(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


15. ## 🔹  String list(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)


The bucketid is the Storage bucket Id to list all objects.



```cpp
String list(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id in its constructor.

**Returns:**

- `String` - The response payload.

16. ## 🔹  void list(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)


The bucketid is the Storage bucket Id to list all objects.


```cpp
void list(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id in its constructor.
- `aResult` - The async result (AsyncResult).


17. ## 🔹  void list(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id to list all objects.


```cpp
void list(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id in its constructor.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


18. ## 🔹  bool deleteObject(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)


The bucketid is the Storage bucket Id of object to delete.
The object is the object in Storage bucket to delete.



```cpp
bool deleteObject(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.

**Returns:**

- Boolean value, indicates the success of the operation.

19. ## 🔹  void deleteObject(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)


The bucketid is the Storage bucket Id of object to delete.
The object is the object in Storage bucket to delete.


```cpp
void deleteObject(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
- `aResult` - The async result (AsyncResult).


20. ## 🔹  void deleteObject(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")


The bucketid is the Storage bucket Id of object to delete.
The object is the object in Storage bucket to delete.


```cpp
void deleteObject(AsyncClientClass &aClient, const FirebaseStorage::Parent &parent, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The FirebaseStorage::Parent object included Storage bucket Id and object in its constructor.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

21. ## 🔹  void setOTAStorage(OTAStorage &storage)

Set Arduino OTA Storage.

```cpp
void setOTAStorage(OTAStorage &storage)
```

**Params:**

- `storage` - The Arduino `OTAStorage` class object.