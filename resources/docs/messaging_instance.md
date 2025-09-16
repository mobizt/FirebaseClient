# MessagingInstance

## Description

Google Cloud Messaging App Instance class


```cpp
class MessagingInstance
```

## Functions

1. ### 🔹 void resetApp()

Unbind or remove FirebaseApp.


```cpp
void resetApp()
```


2. ## 🔹  void loop()

Perform the async task repeatedly (DEPRECATED).

```cpp
void loop()
```


3. ## 🔹 String batchAdd(AsyncClientClass &aClient, const String &topic, const object_t &IID)


Create relationship maps for app instances (subscribe the apps to the topic).

```cpp
String batchAdd(AsyncClientClass &aClient, const String &topic, const object_t &IID)
```

**Params:**

- `aClient` - The async client.
- `topic` - The topic to subscribe.
- `IID` - The object_t object represents the instance ID tokens or registration tokens array.

**Returns:**

- `String` - The response payload.

4. ## 🔹  void batchAdd(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResult &aResult)


Create relationship maps for app instances (subscribe the apps to the topic).

```cpp
void batchAdd(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `topic` - The topic to subscribe.
- `IID` - The object_t object represents the instance ID tokens or registration tokens array.
- `aResult` - The async result (AsyncResult).


5. ## 🔹  void batchAdd(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResultCallback cb, const String &uid = "")


Create relationship maps for app instances (subscribe the apps to the topic).

```cpp
void batchAdd(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `topic` - The topic to subscribe.
- `IID` - The object_t object represents the instance ID tokens or registration tokens array.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).



6. ## 🔹 String batchRemove(AsyncClientClass &aClient, const String &topic, const object_t &IID)


Remove relationship maps for app instances (unsubscribe the apps to the topic).

```cpp
String batchRemove(AsyncClientClass &aClient, const String &topic, const object_t &IID)
```

**Params:**

- `aClient` - The async client.
- `topic` - The topic to unsubscribe.
- `IID` - The object_t object represents the instance ID tokens or registration tokens array.

**Returns:**

- `String` - The response payload.


7. ## 🔹  void batchRemove(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResult &aResult)


Remove relationship maps for app instances (unsubscribe the apps to the topic).

```cpp
void batchRemove(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `topic` - The topic to unsubscribe.
- `IID` - The object_t object represents the instance ID tokens or registration tokens array.
- `aResult` - The async result (AsyncResult).


8. ## 🔹  void batchRemove(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResultCallback cb, const String &uid = "")


Remove relationship maps for app instances (unsubscribe the apps to the topic).

```cpp
void batchRemove(AsyncClientClass &aClient, const String &topic, const object_t &IID, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `topic` - The topic to unsubscribe.
- `IID` - The object_t object represents the instance ID tokens or registration tokens array.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


9. ## 🔹 String batchImport(AsyncClientClass &aClient, const String &application, bool sandbox, const object_t &APNs)


Create registration tokens for APNs tokens.

```cpp
String batchImport(AsyncClientClass &aClient, const String &application, bool sandbox, const object_t &APNs)
```

**Params:**

- `aClient` - The async client.
- `application` - The Bundle id of the app.
- `sandbox` - The Boolean to indicate sandbox environment (TRUE) or production (FALSE).
- `APNs` - The object_t object represents the iOS APNs tokens array.

**Returns:**

- `String` - The response payload.


10. ## 🔹  void batchImport(AsyncClientClass &aClient, const String &application, bool sandbox, const object_t &APNs, AsyncResult &aResult)


Create registration tokens for APNs tokens.

```cpp
void batchImport(AsyncClientClass &aClient, const String &application, bool sandbox, const object_t &APNs, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `application` - The Bundle id of the app.
- `sandbox` - The Boolean to indicate sandbox environment (TRUE) or production (FALSE).
- `APNs` - The object_t object represents the iOS APNs tokens array.
- `aResult` - The async result (AsyncResult).


11. ## 🔹  void batchImport(AsyncClientClass &aClient, const String &application, bool sandbox, const object_t &APNs, AsyncResultCallback cb, const String &uid = "") 


Create registration tokens for APNs tokens.

```cpp
void batchImport(AsyncClientClass &aClient, const String &application, bool sandbox, const object_t &APNs, AsyncResultCallback cb, const String &uid = "") 
```

**Params:**

- `aClient` - The async client.
- `application` - The Bundle id of the app.
- `sandbox` - The Boolean to indicate sandbox environment (TRUE) or production (FALSE).
- `APNs` - The object_t object represents the iOS APNs tokens array.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


12. ## 🔹 String get(AsyncClientClass &aClient, const String &IID)


Get information about app instances.

```cpp
String get(AsyncClientClass &aClient, const String &IID)
```

**Params:**

- `aClient` - The async client.
- `IID` - The instance ID token.

**Returns:**

- `String` - The response payload.


13. ## 🔹  void get(AsyncClientClass &aClient, const String &IID, AsyncResult &aResult)


Get information about app instances.

```cpp
void get(AsyncClientClass &aClient, const String &IID, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `IID` - The instance ID token.
- `aResult` - The async result (AsyncResult).


14. ## 🔹  void get(AsyncClientClass &aClient, const String &IID, AsyncResultCallback cb, const String &uid = "")


Get information about app instances.

```cpp
void get(AsyncClientClass &aClient, const String &IID, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `IID` - The instance ID token.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

