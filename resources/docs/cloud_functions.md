# CloudFunctions

## Description

Google Cloud Functions class


```cpp
class CloudFunctions
```

## Functions

1. ### 🔹 void resetApp()

Unbind or remove FirebaseApp.


```cpp
void resetApp()
```


1. ## 🔹  void loop()

Perform the async task repeatedly.
Should be places in main loop function.

```cpp
void loop()
```


2. ## 🔹  String create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function)

If a function with the given name already exists in the specified project,
the long running operation will return ALREADY_EXISTS error.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String create(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function to create.
- `function` - The GoogleCloudFunctions::Function object that holds the information of function to create.

**Returns:**

- `String` - The response payload.

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


5. ## 🔹  String patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask)

If a function with the given name already exists in the specified project,
the long running operation will return ALREADY_EXISTS error.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.
This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String patch(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Function &function, const String &updateMask)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `function` - The GoogleCloudFunctions::Function object that holds the information of function to update.
- `updateMask` - The list of fields to be updated. If no field mask is provided, all provided fields in the request will be updated.

**Returns:**

- `String` - The response payload.

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


8. ## 🔹  String deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String deleteFunction(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.

**Returns:**

- `String` - The response payload.

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


11. ## 🔹  String list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String list(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, GoogleCloudFunctions::ListOptions &listOptions)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `listOptions` - The GoogleCloudFunctions::ListOptions object that holds the query parameters e.g. pageSize, pageToken, filter, and orderBy.

**Returns:**

- `String` - The response payload.

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


14. ## 🔹  String get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String get(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.

**Returns:**

- `String` - The response payload.

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


17. ## 🔹  String call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String call(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, const String &payload)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `payload` - The Input to be passed to the function.

**Returns:**

- `String` - The response payload.

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


20. ## 🔹  String generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String generateDownloadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.

**Returns:**

- `String` - The response payload.

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


23. ## 🔹  String generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::UploadURLOptions options)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String generateUploadURL(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::UploadURLOptions options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::UploadURLOptions object that provides the kmsKeyName and environment options.

**Returns:**

- `String` - The response payload.

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


26. ## 🔹  String getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String getIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::GetPolicyOptions options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::GetPolicyOptions options that provides requestedPolicyVersion option.

**Returns:**

- `String` - The response payload.

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


29. ## 🔹  String setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::SetPolicyOptions options)

Can return NOT_FOUND, INVALID_ARGUMENT, and PERMISSION_DENIED errors.

The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String setIamPolicy(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::SetPolicyOptions options)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `options` - The GoogleCloudFunctions::SetPolicyOptions options that provides Policy and updateMask settings.

**Returns:**

- `String` - The response payload.

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


32. ## 🔹  String testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Permissions permissions)


The Firebase project Id should be only the name without the firebaseio.com.
The location name is the project location.
For the list of permissions, see https://cloud.google.com/functions/docs/reference/iam/permissions

@note Use FirebaseData.payload() to get the returned payload.

This function requires OAuth2.0 authentication.


```cpp
String testIamPermissions(AsyncClientClass &aClient, const GoogleCloudFunctions::Parent &parent, const String &functionId, GoogleCloudFunctions::Permissions permissions)
```

**Params:**

- `aClient` - The async client.
- `parent` - The GoogleCloudFunctions::Parent object included project Id and location name in its constructor.
- `functionId` - The name of function.
- `permissions` - The GoogleCloudFunctions::Permissions object that holds the list of permission string.

**Returns:**

- `String` - The response payload.

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

