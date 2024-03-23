# CloudFunctions

## Description

Google Cloud Functions class


```cpp
class CloudFunctions
```

## Functions

1. ## 🔹  void url(const String &url)

Set the Cloud Functions URL

```cpp
void url(const String &url)
```

**Params:**

- `url` - The Cloud Functions URL.


1. ## 🔹  void loop()

Perform the async task repeatedly.
Should be places in main loop function.

```cpp
void loop()
```


2. ## 🔹  bool create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function)

If a function with the given name already exists in the specified project,
the long running operation will return ALREADY_EXISTS error.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function to create.
- `function` - The GoogleCloudFunctions::Function object that holds the information of function to create.

**Returns:**

- Boolean type status indicates the success of the operation.

3. ## 🔹  void create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, AsyncResult &aResult)

If a function with the given name already exists in the specified project,
the long running operation will return ALREADY_EXISTS error.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function to create.
- `function` - The GoogleCloudFunctions::Function object that holds the information of function to create.
- `aResult` - The async result (AsyncResult).


4. ## 🔹  void create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, AsyncResultCallback cb, const String &uid = "")

If a function with the given name already exists in the specified project,
the long running operation will return ALREADY_EXISTS error.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function to create.
- `function` - The GoogleCloudFunctions::Function object that holds the information of function to create.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


5. ## 🔹  bool patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask)

If a function with the given name already exists in the specified project,
the long running operation will return ALREADY_EXISTS error.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.
This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `function` - The GoogleCloudFunctions::Function object that holds the information of function to update.
- `updateMask` - The list of fields to be updated. If no field mask is provided, all provided fields in the request will be updated.

**Returns:**

- Boolean type status indicates the success of the operation.

6. ## 🔹  void patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask, AsyncResult &aResult)

If a function with the given name already exists in the specified project,
the long running operation will return ALREADY_EXISTS error.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.
This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `function` - The GoogleCloudFunctions::Function object that holds the information of function to update.
- `updateMask` - The list of fields to be updated. If no field mask is provided, all provided fields in the request will be updated.
- `aResult` - The async result (AsyncResult).


7. ## 🔹  void patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask, AsyncResultCallback cb, const String &uid = "")

If a function with the given name already exists in the specified project,
the long running operation will return ALREADY_EXISTS error.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.
This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `function` - The GoogleCloudFunctions::Function object that holds the information of function to update.
- `updateMask` - The list of fields to be updated. If no field mask is provided, all provided fields in the request will be updated.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


8. ## 🔹  bool deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.

**Returns:**

- Boolean type status indicates the success of the operation.

9. ## 🔹  void deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResult &aResult)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `aResult` - The async result (AsyncResult).


10. ## 🔹  void deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "")


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


11. ## 🔹  bool list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `listOptions` - The GoogleCloudFunctions::ListOptions object that holds the query parameters e.g. pageSize, pageToken, filter, and orderBy.

**Returns:**

- Boolean type status indicates the success of the operation.

12. ## 🔹  void list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions, AsyncResult &aResult)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `listOptions` - The GoogleCloudFunctions::ListOptions object that holds the query parameters e.g. pageSize, pageToken, filter, and orderBy.
- `aResult` - The async result (AsyncResult).


13. ## 🔹  void list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "")


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `listOptions` - The GoogleCloudFunctions::ListOptions object that holds the query parameters e.g. pageSize, pageToken, filter, and orderBy.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


14. ## 🔹  bool get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.

**Returns:**

- Boolean type status indicates the success of the operation.

15. ## 🔹  void get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResult &aResult)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `aResult` - The async result (AsyncResult).


16. ## 🔹  void get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "")


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


17. ## 🔹  bool call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `payload` - The Input to be passed to the function.

**Returns:**

- Boolean type status indicates the success of the operation.

18. ## 🔹  void call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload, AsyncResult &aResult)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `payload` - The Input to be passed to the function.
- `aResult` - The async result (AsyncResult).


19. ## 🔹  void call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload, AsyncResultCallback cb, const String &uid = "")


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `payload` - The Input to be passed to the function.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


20. ## 🔹  bool generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.

**Returns:**

- Boolean type status indicates the success of the operation.

21. ## 🔹  void generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResult &aResult)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `aResult` - The async result (AsyncResult).


22. ## 🔹  void generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "")


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


23. ## 🔹  bool generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::UploadURLOptions options)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::UploadURLOptions options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::UploadURLOptions object that provides the kmsKeyName and environment options.

**Returns:**

- Boolean type status indicates the success of the operation.

24. ## 🔹  void generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::UploadURLOptions options, AsyncResult &aResult)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::UploadURLOptions options, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::UploadURLOptions object that provides the kmsKeyName and environment options.
- `aResult` - The async result (AsyncResult).


25. ## 🔹  void generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::UploadURLOptions options, AsyncResultCallback cb, const String &uid = "")


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::UploadURLOptions options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::UploadURLOptions object that provides the kmsKeyName and environment options.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


26. ## 🔹  bool getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::GetPolicyOptions options that provides requestedPolicyVersion option.

**Returns:**

- Boolean type status indicates the success of the operation.

27. ## 🔹  void getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options, AsyncResult &aResult)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::GetPolicyOptions options that provides requestedPolicyVersion option.
- `aResult` - The async result (AsyncResult).


28. ## 🔹  void getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options, AsyncResultCallback cb, const String &uid = "")


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::GetPolicyOptions options that provides requestedPolicyVersion option.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


29. ## 🔹  bool setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::SetPolicyOptions options)

Can return NOT_FOUND, INVALID_ARGUMENT, and PERMISSION_DENIED errors.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::SetPolicyOptions options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::SetPolicyOptions options that provides Policy and updateMask settings.

**Returns:**

- Boolean type status indicates the success of the operation.

30. ## 🔹  void setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::SetPolicyOptions options, AsyncResult &aResult)

Can return NOT_FOUND, INVALID_ARGUMENT, and PERMISSION_DENIED errors.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::SetPolicyOptions options, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::SetPolicyOptions options that provides Policy and updateMask settings.
- `aResult` - The async result (AsyncResult).


31. ## 🔹  void setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::SetPolicyOptions options, AsyncResultCallback cb, const String &uid = "")

Can return NOT_FOUND, INVALID_ARGUMENT, and PERMISSION_DENIED errors.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::SetPolicyOptions options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::SetPolicyOptions options that provides Policy and updateMask settings.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


32. ## 🔹  bool testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Permissions permissions)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.
For the list of permissions, see https://cloud.google.com/functions/docs/reference/iam/permissions

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
bool testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Permissions permissions)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `permissions` - The GoogleCloudFunctions::Permissions object that holds the list of permission string.

**Returns:**

- Boolean type status indicates the success of the operation.

33. ## 🔹  void testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Permissions permissions, AsyncResult &aResult)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.
For the list of permissions, see https://cloud.google.com/functions/docs/reference/iam/permissions

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Permissions permissions, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `permissions` - The GoogleCloudFunctions::Permissions object that holds the list of permission string.
- `aResult` - The async result (AsyncResult).


34. ## 🔹  void testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Permissions permissions, AsyncResultCallback cb, const String &uid = "")


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.
For the list of permissions, see https://cloud.google.com/functions/docs/reference/iam/permissions

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
void testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Permissions permissions, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `permissions` - The GoogleCloudFunctions::Permissions object that holds the list of permission string.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

