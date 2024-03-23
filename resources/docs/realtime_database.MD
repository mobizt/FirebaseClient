
# RealtimeDatabase

## Description

Realtime database operating class


```cpp
class RealtimeDatabase
```


## Constructors

1. ### 🔹 RealtimeDatabase(const String &url = "")

    Create new RealtimeDatabase instance with defined url.

    ```cpp
    RealtimeDatabase(const String &url = "")
    ```

    **Params:**

    - `url` - The database url

## Destructors

1. ### ~RealtimeDatabase()
    
    Class destructor 

    ```cpp
    ~RealtimeDatabase()
    ```

## Functions

1. ### 🔹 RealtimeDatabase &operator=(RealtimeDatabase &rhs)

    Assignment operator.

    ```cpp
    RealtimeDatabase &operator=(RealtimeDatabase &rhs)
    ```

    **Params:**

    - `rhs` - The object to be assigned to self instance

    **Returns:**

    - `RealtimeDatabase` reference to self instance

2. ### 🔹 void setApp(uint32_t app_addr, app_token_t *app_token)
    
    Set pointer of FirebaseApp's app token.

    ```cpp
    void setApp(uint32_t app_addr, app_token_t *app_token)
    ```

    **Params:**

    - `app_addr` - The app address.
    - `app_token` - The pointer to app_token_t

3. ### 🔹 app_token_t *appToken()
    
    Get FirebaseApp's app token pointer.

    ```cpp
    app_token_t *appToken()
    ```

    **Returns:**

    - `app_token_t*` - FirebaseApp's app token pointer.

4. ### 🔹 T get(AsyncClientClass &aClient, const String &path)

    Get value at the node path.

    ```cpp
    T get(AsyncClientClass &aClient, const String &path)
    ```
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.

    **Returns:**

    - `T` - The value of type T that casts from response payload.

5. ### 🔹 T get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options)

    Get value at the node path.

    The DatabaseOptions related to the Conditional Requests and Query Parameters supported by Firebase Realtime Database REST API are following.
    
    `readTimeout`, the timeout (number) in ms which limits how long the read takes on the server side.
    `writeSizeLimit`, the size of a write limit can be "tiny", "small", "meduim", "large" and "unlimited".
    `shallow`, the option (boolean) for shallowing (truncating) the JSON object data into true while JSON primitive values (string, number and boolean) will not shallow.
    `silent`, the option (boolean) to mute the return reponse payload.
    `classicRequest`, the option (boolean) to use HTTP POST for PUT (set) and DELETE (remove).
    `ETAG`, the ETAG value for The if-match condition request header for the data you want to update.
    `Filter`, the options for complex data filtering which included the properties i.e., orderBy, startAt, endAt,`limitToFirst`, limitToLast, and equalTo.

    ```cpp
    T get(AsyncClientClass &aClient, const String &path)
    ```
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `options` - The database options (DatabaseOptions).

    **Returns:**

    - `T` - The value of type T that casts from response payload.
    
    
6. ### 🔹 void get(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, bool sse = false)

    Get value at the node path.

    ### Example
    ```cpp
    Database.get(aClient, "/path/to/data", aResult);
    ```
    
    ```cpp
    void get(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, bool sse = false)
    ```
    **Params:**
    
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `aResult` - The async result (AsyncResult)
    - `sse` - The Server-sent events (Stream) mode


7. ### 🔹 void get(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, bool sse = false, const String &uid = "")

    Get value at the node path.

    ### Example
    ```cpp
    Database.get(aClient, "/path/to/data", cb);
    ```
    
    ```cpp
    void get(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, bool sse = false, const String &uid = "")
    ```
    
    **Params:**
    
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `cb` - The async result callback (AsyncResultCallback).
    - `sse` - The Server-sent events (Stream) mode.
    - `uid` - The user specified UID of async result (optional).


8. ### 🔹 void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResult &aResult)

    Get value at the node path.

    ### Example
    ```cpp
    DatabaseOptions options;
    options.filter.orderBy("Data");
    options.filter.startAt(105);
    options.filter.endAt(120);
    options.filter.limitToLast(8);

    Database.get<String>(aClient, "/path/to/data", options, aResult);
    ```

    ```cpp
    void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResult &aResult)
    ```
    
    **Params:**
    
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `options` - The database options (DatabaseOptions).
    - `aResult` - The async result (AsyncResult)
    
    **Returns:**
    
    - value that casts from response payload.

9. ### 🔹 void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResultCallback cb, const String &uid = "")

    Get value at the node path.

    ### Example
    ```cpp
    DatabaseOptions options;
    options.filter.orderBy("Data");
    options.filter.startAt(105);
    options.filter.endAt(120);
    options.filter.limitToLast(8);
    
    Database.get<String>(aClient, "/path/to/data", options, cb);
    ```
    
    ```cpp
    void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResultCallback cb, const String &uid = "")
    ```
    
    **Params:**
    
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `options` - The database options (DatabaseOptions).
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


10. ### 🔹 void get(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)

    Get value at the node path.
    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

    ### Example
    ```cpp
    // Example of FileConfigCallback when SPIFFS used as filesystem.
    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        file = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        file = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        file = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.get(aClient, "/path/to/data", getFile(fileConfig), aResult);
    ```
    
    ```cpp
    void get(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `aResult` - The async result (AsyncResult)

11. ### 🔹 void get(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")

    Get value at the node path.
    The FileConfig object constructor should be included filename and FileConfigCallback.
    
    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

    ### Example
    ```cpp
    // Example of FileConfigCallback when SPIFFS used as filesystem.
    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        file = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        file = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        file = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.get(aClient, "/path/to/data", getFile(fileConfig), cb);
    ```
    
    ```cpp
    void get(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")
    ```
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


12. ### 🔹 bool existed(AsyncClientClass &aClient, const String &path)

    Checks if path exists in database
    
    ### Example
    ```cpp
    bool status = Database.existed(aClient, "/path/to/data");
    ```
    
    ```cpp
    bool existed(AsyncClientClass &aClient, const String &path)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to check.
    
    **Returns:**
    - boolean value indicates the operating status.

13. ### 🔹 void ota(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)

    Perform OTA update using a firmware file from the database.
    
    The data of node to download should be base64 encoded string of the firmware file.

    ### Example
    ```cpp
    Database.ota(aClient, "/path/to/data", aResult);
    ```
    
    ```cpp
    void ota(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to download.
    - `aResult` - The async result (AsyncResult)


14. ### 🔹 void ota(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")

    Perform OTA update using a firmware file from the database.

    The data of node to download should be base64 encoded string of the firmware file.

    ### Example
    ```cpp
    Database.ota(aClient, "/path/to/data", cb);
    ```
    ```cpp
    void ota(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to download.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


15. ### 🔹 bool set(AsyncClientClass &aClient, const String &path, T value)

    Set value to database.

    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.
    
    ### Example
    ```cpp
    bool status = Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));
    ```
    
    ```cpp
    bool set(AsyncClientClass &aClient, const String &path, T value)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to set the value.
    - `value` - The value to set.

    **Returns:**
    - boolean value indicates the operating status.


16. ### 🔹 void set(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)

    Set value to database.

    ```cpp
    void set(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to set the value.
    - `value` - The value to set.
    - `aResult` - The async result (AsyncResult)

    **Note**
    
    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.
    
    ### Example
    ```cpp
    bool status = Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), aResult);
    ```
    

17. ### 🔹 void set(AsyncClientClass &aClient, const String &path, T value, AsyncResultCallback cb, const String &uid = "")

    Set value to database.

    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.
    
    ### Example
    ```cpp
    bool status = Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), cb);
    ```

    ```cpp
    void set(AsyncClientClass &aClient, const String &path, T value, AsyncResultCallback cb, const String &uid = "")
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to set the value.
    - `value` - The value to set.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


18. ### 🔹 void set(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)

    Set content from file to database.
    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

    ### Example
    ```cpp
    // Example of FileConfigCallback when SPIFFS used as filesystem.
    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        file = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        file = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        file = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);
    ```

    ```cpp
    void set(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to set value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `aResult` - The async result (AsyncResult)


19. ### 🔹 void set(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")

    Set content from file to database.
    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

    ### Example
    ```cpp
    // Example of FileConfigCallback when SPIFFS used as filesystem.
    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        file = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        file = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        file = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.set(aClient, "/path/to/data", getFile(fileConfig), cb);
    ```
    
    ```cpp
    void set(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


20. ### 🔹 String push(AsyncClientClass &aClient, const String &path, T value)

    Push value to database.
    
    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.

    ### Example
    ```cpp
    String name = Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));
    ```

    ```cpp
    String push(AsyncClientClass &aClient, const String &path, T value)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to push the value.
    - `value` - The value to push.
    
    **Returns:**
    - String name of new node that created.

21. ### 🔹 void push(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)

    Push value to database.
    
    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.

    ### Example
    ```cpp
    void name = Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), aResult);
    ```

    ```cpp
    void push(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to push the value.
    - `value` - The value to push.
    - `aResult` - The async result (AsyncResult).

22. ### 🔹 void push(AsyncClientClass &aClient, const String &path, T value,  AsyncResultCallback cb, const String &uid = "")

    Push value to database.
    
    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.

    ### Example
    ```cpp
    void name = Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), cb);
    ```

    ```cpp
    void push(AsyncClientClass &aClient, const String &path, T value,  AsyncResultCallback cb, const String &uid = "")
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to push the value.
    - `value` - The value to push.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


23. ### 🔹 void push(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)

    Push content from file to database.
    
    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
    
    ### Example
    ```cpp
    // Example of FileConfigCallback when SPIFFS used as filesystem.
    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        file = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        file = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        file = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);
    ```
    
    ```cpp
    void push(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `aResult` - The async result (AsyncResult)


24. ### 🔹 void push(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")

    Push content from file to database.

    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
    
    ### Example
    ```cpp
    // Example of FileConfigCallback when SPIFFS used as filesystem.
    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        file = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        file = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        file = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.set(aClient, "/path/to/data", getFile(fileConfig), cb);
    ```
    
    ```cpp
    void push(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


25. ### 🔹 bool update(AsyncClientClass &aClient, const String &path, const T &value)

    Update (patch) JSON object to database.
    
    ### Example
    ```cpp
    bool status = Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"));
    ```

    ```cpp
    bool update(AsyncClientClass &aClient, const String &path, const T &value)
    ```
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to update.
    - `value` - The JSON object (object_t) to update.
    
    **Returns:**
    - boolean value indicates the operating status.

26. ### 🔹 void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResult &aResult)

    Update (patch) JSON object to database.
    
    ### Example
    ```cpp
    Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"), aResult);
    ```

    ```cpp
    void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResult &aResult)
    ```
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to update.
    - `value` - The JSON object (object_t) to update.
    - `aResult` - The async result (AsyncResult).

27. ### 🔹 void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResultCallback cb, const String &uid = "")

    Update (patch) JSON object to database.
    
    ### Example
    ```cpp
    Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"), cb);
    ```

    ```cpp
    void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResultCallback cb, const String &uid = "")
    ```
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to update.
    - `value` - The JSON object (object_t) to update.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


28. ### 🔹 bool remove(AsyncClientClass &aClient, const String &path)

    Remove node from database

    ### Example
    ```cpp
    bool status = Database.remove(aClient, "/path/to/data");
    ```
    
    ```cpp
    bool remove(AsyncClientClass &aClient, const String &path)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to remove.
    
    **Returns:**
    - boolean value indicates the operating status.

29. ### 🔹 void remove(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)

    Remove node from database

    ### Example
    ```cpp
    Database.remove(aClient, "/path/to/data", aResult);
    ```
    ```cpp
    void remove(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client
    - `path` - The node path to remove.
    - `aResult` - The async result (AsyncResult).


30. ### 🔹 void remove(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")

    Remove node from database

    ### Example
    ```cpp
    Database.remove(aClient, "/path/to/data", cb);
    ```
    
    ```cpp
    void remove(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")
    ```
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to remove.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).

31. ### 🔹 void url(const String &url)

    Set the Firebase database URL
    ```cpp
    void url(const String &url)
    ```
    **Params:**
    - `url` - The Firebase database URL.


32. ### 🔹 void loop()

    Perform the async task repeatedly.
    Should be places in main loop function.
    
    ```cpp
    void loop()
    ```

