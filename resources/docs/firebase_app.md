# FirebaseApp

## Description

The Firebase App class that handles the authentication/authorization task.

```cpp
class FirebaseApp
```

## Functions

1. ## 🔹  bool isInitialized() const

Get the app initialization status.

```cpp
bool isInitialized() const
```

**Returns:**

- `bool` - Return true if initialized.


2. ## 🔹  void loop()

The authentication/authorization handler.

```cpp
void loop()
```

3. ## 🔹  bool ready()

Get the authentication/autorization process status.

```cpp
bool ready()
```

**Returns:**

- `bool` - Return true if the auth process was finished. Returns false if `isExpired()` returns true.


4. ## 🔹  void getApp(T &app)

Appy the authentication/authorization credentials to the Firebase services app.

```cpp
void getApp(T &app)
```

**Params:**

- `app` - The Firebase services calss object e.g. RealtimeDatabase, Storage, Messaging, CloudStorage and CloudFunctions.


5. ## 🔹   String getToken() const

Get the auth token.

```cpp
String getToken() const
```

**Returns:**

- `String` - String of auth tokens based on the authentication/authoeization e.g. ID token and access token.


6. ## 🔹  String getRefreshToken() const

Get the refresh token.

```cpp
String getRefreshToken() const
```

**Returns:**

- `String` - String of refresh token after user sign in or authorization using ID token.

7. ## 🔹  String getUid() const

Get unique identifier.

```cpp
String getUid() const
```

**Returns:**

- `String` - String of unique identifier after user sign in or authorization using ID token.

8. ## 🔹  bool isAuthenticated() const

Get the authentication status since app initialized.

```cpp
bool isAuthenticated() const
```

**Returns:**

- `bool` - Return true once authenticated since initialized. It will reset when app re-initialized and user management task was executed.


9. ## 🔹  bool isExpired()

Get the auth token expiration status.

```cpp
bool isExpired()
```

**Returns:**

- `bool` - Return true if auth token was expired upon the expire period setup.


10. ## 🔹  unsigned long ttl()

Get the remaining time to live of token until expired.

```cpp
unsigned long ttl()
```

**Params:**

- `unsigned long` - The integer value of ttl.


11. ## 🔹  void setCallback(AsyncResultCallback cb) 

Set the async result callback function.

```cpp
void setCallback(AsyncResultCallback cb)
```

**Params:**

- `cb` - The async result callback function (AsyncResultCallback).


12. ## 🔹  void setAsyncResult(AsyncResult &aResult)

Set the async result class object.

```cpp
void setAsyncResult(AsyncResult &aResult)
```

**Params:**

- `aResult` - The async result (AsyncResult).


13. ## 🔹  void setUID(const String &uid)

Set the UID for authentication task.

The UID will be applied when used with the async result callback only.

The async result object set via `initializeApp` and `FirebaseApp::setAsyncResult` will not take effect.

```cpp
void setUID(const String &uid)
```

**Params:**

- `uid` - The unique identifier for the authentication task.

14. ## 🔹  void setJWTProcessor(JWTClass &jwtClass)

Set the JWT token processor object.

This function should be executed before calling `initializeApp`.

```cpp
void setJWTProcessor(JWTClass &jwtClass)
```

**Params:**

- `jwtClass` - The JWT token processor object.

15. ## 🔹  auth_data_t *getAuth()

Get the pointer to the internal auth data.

```cpp
auth_data_t *getAuth()
```

**Returns:**

- `auth_data_t*` - The pointer to internal auth data.


16. ## 🔹  void deinitializeApp()

Reset or clear app (deinitailize).

```cpp
void deinitializeApp()
```