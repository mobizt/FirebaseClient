# Firebase Client Class and Static Functions

## Description

The Firebase Client class and static functions interfaces 

## Static Functions


1. ## 🔹  static user_auth_data &getAuth(T &auth)

Get the user authentication/autorization credentials data.

```cpp
static user_auth_data &getAuth(T &auth)
```

**Params:**

- `auth` - The user auth data (`user_auth_data`) which is the struct that holds the user sign-in credentials and tokens that obtained from the authentication/authorization classes via getAuth function.

**Returns:**

- `user_auth_data` - The user authentication/authorization data.


2. ## 🔹  static void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)

Initialize the FirebaseApp.

```cpp
static void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which is the struct that holds the user sign-in credentials and tokens that obtained from the authentication/authorization classes via getAuth function.


3. ## 🔹  static void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)

Initialize the FirebaseApp without callback.

```cpp
static void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which is the struct that holds the user sign-in credentials and tokens that obtained from the authentication/authorization classes via getAuth function.
- `aResult` - The async result (AsyncResult).


4. ## 🔹  static void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")

Initialize the FirebaseApp with callback.

```cpp
static void initializeApp(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which is the struct that holds the user sign-in credentials and tokens that obtained from the authentication/authorization classes via getAuth function.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


5. ## 🔹  static void dnitializeApp(FirebaseApp &app)

Deinitialize the FirebaseApp.

```cpp
static void dnitializeApp(FirebaseApp &app)
```

**Params:**

- `app` - The FirebaseApp class object to handle authentication/authorization task.


6. ## 🔹  static void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)

Signup a new user.

```cpp
static void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.


7. ## 🔹  static void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)

Signup a new user without callback.

```cpp
static void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.
- `aResult` - The async result (AsyncResult).

8. ## 🔹  static void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")

Signup a new user with callback.

```cpp
static void signup(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

9. ## 🔹  static void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)

Reset the user password.

```cpp
static void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.


10. ## 🔹  static void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)

Reset the user password without callback.

```cpp
static void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.
- `aResult` - The async result (AsyncResult).


11. ## 🔹  static void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")

Reset the user password with callback.

```cpp
static void resetPassword(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


12. ## 🔹  static void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth) 

Send the user verification link to email.

```cpp
static void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth) 
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.


13. ## 🔹  static void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)

Send the user verification link to email without callback.

```cpp
static void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.
- `aResult` - The async result (AsyncResult).


14. ## 🔹  static void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")

Send the user verification link to email with callback.

```cpp
static void verify(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


15. ## 🔹  static void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)

Delete a user.

```cpp
static void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth)
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.


16. ## 🔹  static void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)

Delete a user without callback.

```cpp
static void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.
- `aResult` - The async result (AsyncResult).


17. ## 🔹  static void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")

Delete a user with callback.

```cpp
static void deleteUser(AsyncClientClass &aClient, FirebaseApp &app, user_auth_data &auth, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client to work for authentication/authorization task.
- `app` - The FirebaseApp class object to handle authentication/authorization task.
- `auth` - The user auth data (`user_auth_data`) which holds the user credentials from `USerAccount` class.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

