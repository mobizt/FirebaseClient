# ServiceAuth

## Description

OAuth2.0 Access Token Authorization Using Service Account.

```cpp
class ServiceAuth
```

## Constructors

1. ### 🔹 explicit ServiceAuth(TimeStatusCallback timeCb, const String &clientEmail, const String &projectId, const String &privateKey, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A ServiceAuth constructor.

```cpp
explicit ServiceAuth(TimeStatusCallback timeCb, const String &clientEmail, const String &projectId, const String &privateKey, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**

- `timeCb` -  The time status callback that provides the UNIX timestamp used to set the issue time and expiration time in the JWT token payload.

- `clientEmail` - The service account client Email.

- `projectId` - The service account project ID.

- `privateKey` - The service account private key.

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.

2. ### 🔹 explicit ServiceAuth(TimeStatusCallback timeCb, const file_config_data &safile, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A ServiceAuth constructor.

```cpp
explicit ServiceAuth(TimeStatusCallback timeCb, const file_config_data &safile, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**

- `timeCb` -  The time status callback that provides the UNIX timestamp used to set the issue time and expiration time in the JWT token payload.

- `safile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of service account key file.

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.


3. ### 🔹 explicit ServiceAuth(const String &clientEmail, const String &projectId, const String &privateKey, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A ServiceAuth constructor.

```cpp
explicit ServiceAuth(const String &clientEmail, const String &projectId, const String &privateKey, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**

- `clientEmail` - The service account client Email.

- `projectId` - The service account project ID.

- `privateKey` - The service account private key.

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.


4. ### 🔹 explicit ServiceAuth(const file_config_data &safile, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A ServiceAuth constructor.

```cpp
explicit ServiceAuth(const file_config_data &safile, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**

- `safile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of service account key file.

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.


## Functions

1. ## 🔹  void setTime(uint32_t ts)

Set the timestamp for the JWT token generation and signing process when authenticated with `ServiceAuth` and `CustomAuth`.

Once the timestamp was set, the internal timestamp value will be set and increased by the millisec of the system timer count since it was set, deviced by 1000.

The precision of the system timer depends on the crystal oscillator.

```cpp
 void setTime(uint32_t ts)
```

**Params:**

- `ts` - The UNIX timestamp.

2. ## 🔹 void clear()

Clear the internal credentials.

```cpp
void clear()
```

3. ## 🔹 user_auth_data &get()

Returns the internal user_auth_data reference.

```cpp
user_auth_data &get()
```

**Returns:**

- `user_auth_data` - Return internal user_auth_data reference.

4. ## 🔹 bool isInitialized() const

Returns the status of this class initialization.

```cpp
bool isInitialized() const
```

**Returns:**

- `bool` - Return true if initialized.



# CustomAuth

## Description

ID Token Authorization Using Service Account with custom claims and UID.

For more details about claims, please visit https://firebase.google.com/docs/auth/admin/custom-claims.

```cpp
class CustomAuth
```

## Constructors

1. ### 🔹 explicit CustomAuth(TimeStatusCallback timeCb, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A CustomAuth constructor.

```cpp
explicit CustomAuth(TimeStatusCallback timeCb, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**


- `timeCb` - The time status callback that provides the UNIX timestamp used to set the issue time and expiration time in the JWT token payload.

- `apiKey` - The web API key of the project.

- `clientEmail` - The service account client Email.

- `projectId` - The service account project ID.

- `privateKey` - The service account private key.

- `uid` - The unique identifier of the signed-in user must be a string, between 1-128 characters long, inclusive. Shorter uids offer better performance.

- `claims` - Optional custom claims to include in the Security Rules auth / request.auth variables. 

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.

2. ### 🔹 explicit CustomAuth(TimeStatusCallback timeCb, const file_config_data &safile, const String &apiKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A CustomAuth constructor.

```cpp
explicit CustomAuth(TimeStatusCallback timeCb, const file_config_data &safile, const String &apiKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**

- `timeCb` -  The time status callback that provides the UNIX timestamp used to set the issue time and expiration time in the JWT token payload.

- `safile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of service account key file.

- `apiKey` - The web API key of the project.

- `uid` - The unique identifier of the signed-in user must be a string, between 1-128 characters long, inclusive. Shorter uids offer better performance.

- `claims` - Optional custom claims to include in the Security Rules auth / request.auth variables. 

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.


3. ### 🔹 explicit CustomAuth(const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A CustomAuth constructor.

```cpp
explicit CustomAuth(const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**

- `apiKey` - The web API key of the project.

- `clientEmail` - The service account client Email.

- `projectId` - The service account project ID.

- `privateKey` - The service account private key.

- `uid` - The unique identifier of the signed-in user must be a string, between 1-128 characters long, inclusive. Shorter uids offer better performance.

- `claims` - Optional custom claims to include in the Security Rules auth / request.auth variables. 

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.


4. ### 🔹 explicit CustomAuth(const file_config_data &safile, const String &apiKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A CustomAuth constructor.

```cpp
explicit CustomAuth(const file_config_data &safile, const String &apiKey, const String &uid, const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**

- `safile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of service account key file.

- `apiKey` - The web API key of the project.

- `uid` - The unique identifier of the signed-in user must be a string, between 1-128 characters long, inclusive. Shorter uids offer better performance.

- `claims` - Optional custom claims to include in the Security Rules auth / request.auth variables. 

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.


## Functions


1. ## 🔹  void setTime(uint32_t ts)

Set the timestamp for the JWT token generation and signing process when authenticated with `ServiceAuth` and `CustomAuth`.

Once the timestamp was set, the internal timestamp value will be set and increased by the millisec of the system timer count since it was set, deviced by 1000.

The precision of the system timer depends on the crystal oscillator.

```cpp
 void setTime(uint32_t ts)
```

**Params:**

- `ts` - The UNIX timestamp.

2. ## 🔹 void clear()

Clear the internal credentials.

```cpp
void clear()
```

3. ## 🔹 void setClaims(const String &claims)

Set the additional claims.

Because this library does not include JSON library to parse the claims, this function will replace or set the internal claims data.

For more details about claims, please visit https://firebase.google.com/docs/auth/admin/custom-claims.

```cpp
void setClaims(const String &claims)
```

4. ## 🔹 user_auth_data &get()

Returns the internal user_auth_data reference.

```cpp
user_auth_data &get()
```

**Returns:**

- `user_auth_data` - Return internal user_auth_data reference.


5. ## 🔹 bool isInitialized() const

Returns the status of this class initialization.

```cpp
bool isInitialized() const
```

**Returns:**

- `bool` - Return true if initialized.

**Params:**

- `claims` - The custom claims to include in the Security Rules auth / request.auth variables.


# UserAuth

## Description

User Sign-In Authentication.

```cpp
class UserAuth
```

## Constructors

1. ### 🔹 explicit UserAuth(const String &api_key, const String &email, const String &password, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A UserAuth constructor.

```cpp
explicit UserAuth(const String &api_key, const String &email, const String &password, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**

- `api_key` - The API key which can be obtained from Firebase console > Project Overview > Project settings.

- `email` - The Email of user in the project.

- `password` - The password of user in the project.

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.


2. ### 🔹 explicit UserAuth(const file_config_data &userFile)

A UserAuth constructor.

```cpp
explicit UserAuth(const file_config_data &userFile)
```

**Params:**

- `userFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `UserAuth` credentials will be saved to or read from.



## Functions

1. ## 🔹 void clear()

Clear the internal credentials.

```cpp
void clear()
```

2. ## 🔹 user_auth_data &get()

Returns the internal user_auth_data reference.

```cpp
user_auth_data &get()
```

**Returns:**

- `user_auth_data` - Return internal user_auth_data reference.


3. ## 🔹 bool isInitialized() const

Returns the status of this class initialization.

```cpp
bool isInitialized() const
```

**Returns:**

- `bool` - Return true if initialized.

4. ## 🔹 bool save(file_config_data &userFile)

Save the user config data.

```cpp
bool save(file_config_data &userFile)
```

**Params:**

- `userFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `UserAuth` credentials will be saved to or read from.


# NoAuth

## Description

Non-Authentication.

```cpp
class UserAuth
```

## Constructors

1. ### 🔹 NoAuth()

A NoAuth constructor.

```cpp
NoAuth()
```



## Functions


1. ## 🔹 user_auth_data &get()

Returns the internal user_auth_data reference.

```cpp
user_auth_data &get()
```

**Returns:**

- `user_auth_data` - Return internal user_auth_data reference.



# AccessToken

## Description

OAuth2.0 Access Token Authorization

```cpp
class AccessToken
```

## Constructors

1. ### 🔹 explicit AccessToken(const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL, const String &refresh = "", const String &client_id = "", const String &client_secret = "")

A AccessToken constructor.

```cpp
explicit AccessToken(const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL, const String &refresh = "", const String &client_id = "", const String &client_secret = "")
```

**Params:**

- `token` - The access token.

- `expire` - The expiry period in seconds (less than 3600), 3300 is the default value.

- `refresh` - The refresh token.

- `client_id` - The Client ID.

- `client_secret` - The Client secret.



2. ### 🔹 explicit AccessToken(const file_config_data &tokenFile)

A AccessToken constructor.

```cpp
explicit AccessToken(const file_config_data &tokenFile)
```

**Params:**

- `tokenFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `AccessToken` credentials will be saved to or read from.



## Functions

1. ## 🔹 void clear()

Clear the internal credentials.

```cpp
void clear()
```

2. ## 🔹 user_auth_data &get()

Returns the internal user_auth_data reference.

```cpp
user_auth_data &get()
```

**Returns:**

- `user_auth_data` - Return internal user_auth_data reference.


3. ## 🔹 bool isInitialized() const

Returns the status of this class initialization.

```cpp
bool isInitialized() const
```

**Returns:**

- `bool` - Return true if initialized.

4. ## 🔹 bool save(file_config_data &userFile)

Save the user config data.

```cpp
bool save(file_config_data &userFile)
```

**Params:**

- `userFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `UserAuth` credentials will be saved to or read from.




# CustomToken

## Description

ID Token Authorization Using Custom Token

```cpp
class CustomToken
```

## Constructors

1. ### 🔹 explicit CustomToken(const String &api_key, const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)

A CustomToken constructor.

```cpp
explicit CustomToken(const String &api_key, const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
```

**Params:**

- `api_key` - API key can be obtained from Firebase console > Project Overview > Project settings.

- `token` - The custom token (jwt signed token).

- `expire` - Expiry period in seconds (less than 3600), 3300 is the default value.

2. ### 🔹  explicit CustomToken(const file_config_data &tokenFile)

A CustomToken constructor.

```cpp
 explicit CustomToken(const file_config_data &tokenFile)
```

**Params:**

- `tokenFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `CustomToken` credentials will be saved to or read from.


## Functions

1. ## 🔹 void clear()

Clear the internal credentials.

```cpp
void clear()
```

2. ## 🔹 user_auth_data &get()

Returns the internal user_auth_data reference.

```cpp
user_auth_data &get()
```

**Returns:**

- `user_auth_data` - Return internal user_auth_data reference.


3. ## 🔹 bool isInitialized() const

Returns the status of this class initialization.

```cpp
bool isInitialized() const
```

**Returns:**

- `bool` - Return true if initialized.

4. ## 🔹 bool save(file_config_data &userFile)

Save the user config data.

```cpp
bool save(file_config_data &userFile)
```

**Params:**

- `userFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `UserAuth` credentials will be saved to or read from.




# IDToken

## Description

ID token Authorization.

```cpp
class IDToken
```

## Constructors

1. ### 🔹 explicit IDToken(const String &api_key, const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL, const String &refresh = "")

A IDToken constructor.

```cpp
explicit IDToken(const String &api_key, const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL, const String &refresh = "")
```

**Params:**

- `api_key` - API key can be obtained from Firebase console > Project Overview > Project settings.

- `token` - The ID token.

- `expire` - Expire period in seconds.

- `refresh` - The refresh token.


2. ### 🔹  explicit IDToken(const file_config_data &tokenFile)

A IDToken constructor.

```cpp
 explicit IDToken(const file_config_data &tokenFile)
```

**Params:**

- `tokenFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `CustomToken` credentials will be saved to or read from.


## Functions

1. ## 🔹 void clear()

Clear the internal credentials.

```cpp
void clear()
```

2. ## 🔹 user_auth_data &get()

Returns the internal user_auth_data reference.

```cpp
user_auth_data &get()
```

**Returns:**

- `user_auth_data` - Return internal user_auth_data reference.


3. ## 🔹 bool isInitialized() const

Returns the status of this class initialization.

```cpp
bool isInitialized() const
```

**Returns:**

- `bool` - Return true if initialized.

4. ## 🔹 bool save(file_config_data &userFile)

Save the user config data.

```cpp
bool save(file_config_data &userFile)
```

**Params:**

- `userFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `UserAuth` credentials will be saved to or read from.




# LegacyToken

## Description

The Realtime Database access with the database Secret.

```cpp
class LegacyToken
```

## Constructors

1. ### 🔹 explicit LegacyToken(const String &token)

A LegacyToken constructor.

```cpp
explicit LegacyToken(const String &token)
```

**Params:**

- `token` - The `Realtime Database` secret key.


2. ### 🔹  explicit LegacyToken(const file_config_data &tokenFile)

A LegacyToken constructor.

```cpp
 explicit LegacyToken(const file_config_data &tokenFile)
```

**Params:**

- `tokenFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `CustomToken` credentials will be saved to or read from.


## Functions

1. ## 🔹 void clear()

Clear the internal credentials.

```cpp
void clear()
```

2. ## 🔹 user_auth_data &get()

Returns the internal user_auth_data reference.

```cpp
user_auth_data &get()
```

**Returns:**

- `user_auth_data` - Return internal user_auth_data reference.


3. ## 🔹 bool isInitialized() const

Returns the status of this class initialization.

```cpp
bool isInitialized() const
```

**Returns:**

- `bool` - Return true if initialized.


4. ## 🔹 bool save(file_config_data &userFile)

Save the user config data.

```cpp
bool save(file_config_data &userFile)
```

**Params:**

- `userFile` - The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `UserAuth` credentials will be saved to or read from.
