
# RealtimeDatabase

## Description

Realtime database class


```cpp
class RealtimeDatabase
```


## Constructors

1. ### 🔹 RealtimeDatabase(const String &url = "")

    Create new RealtimeDatabase instance with defined url.

    ### Example
    ```cpp
    RealtimeDatabase Database("xxxxxxxxx.firebasedatabase.app");
    ```

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

2. ### 🔹 void url(const String &url)

    Set the Firebase database URL

    ### Example
    ```cpp
    Database.url("xxxxxxxxx.firebasedatabase.app");
    ```

    ```cpp
    void url(const String &url)
    ```
    **Params:**
    - `url` - The Firebase database URL.

3. ### 🔹 void resetApp()

    Unbind or remove FirebaseApp.


    ```cpp
    void resetApp()
    ```

4. ### 🔹 T get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options)

    Get value at the node path.

    The DatabaseOptions related to the Conditional Requests and Query Parameters supported by Firebase Realtime Database REST API are following.
    
    `readTimeout`, the timeout (number) in ms which limits how long the read takes on the server side.
    If a read request doesn't finish within the allotted time, it terminates with an HTTP 400 error.
    The default value is 15 min or 900,000 ms.

    `writeSizeLimit`, the size of a write limit can be "tiny", "small", "meduim", "large" and "unlimited".
    To limit the size of a write, you can specify the writeSizeLimit query parameter as tiny (target=1s), small (target=10s), medium (target=30s), large (target=60s). Realtime Database estimates the size of each write request and aborts requests that will take longer than the target time.

    `shallow`, the option (boolean) for shallowing (truncating) the JSON object data into true while JSON primitive values (string, number and boolean) will not shallow.

    This option cannot be mixed with any other options.

    `silent`, the option (boolean) to mute the return reponse payload.
    `classicRequest`, the option (boolean) to use HTTP POST for PUT (set) and DELETE (remove).
    `Filter`, the options for complex data filtering which included the properties i.e., orderBy, startAt, endAt,`limitToFirst`, limitToLast, and equalTo.

    ### Example
    ```cpp
    DatabaseOptions options;

    options.readTimeout = 5000;

    Database.get(aClient, "/path/to/data", options);
    ```

    ```cpp
    T get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options)
    ```
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `options` - The database options (DatabaseOptions).

    **Returns:**

    - `T` - The value of type T that casts from response payload.
    
    
5. ### 🔹 void get(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, bool sse = false)

    Get value at the node path.

    This function has only async version.

    ### Example
    ```cpp
    // Non-Stream
    Database.get(aClient, "/path/to/data", aResult);

    // Since v1.2.1, in SSE mode (HTTP Streaming) task, you can filter the Stream events by using RealtimeDatabase::setSSEFilters(<keywords>), 
    // which the <keywords> is the comma separated events.
    // The event keywords supported are: 
    // get - To allow the http get response (first put event since stream connected).
    // put - To allow the put event.
    // patch - To allow the patch event.
    // keep-alive - To allow the keep-alive event.
    // cancel - To allow the cancel event.
    // auth_revoked - To allow the auth_revoked event.
    // To clear all prevousely set filter to allow all Stream events, use RealtimeDatabase::setSSEFilters().
    Database.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");

    Database.get(aClient, "/path/to/stream/data", aResult, true /* SSE mode (HTTP Streaming) */);
    ```
    
    ```cpp
    void get(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, bool sse = false)
    ```
    **Params:**
    
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `aResult` - The async result (AsyncResult)
    - `sse` - The Server-sent events (Stream) mode


6. ### 🔹 void get(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, bool sse = false, const String &uid = "")

    Get value at the node path.

    This function has only async version.

    ### Example
    ```cpp
    // Non-Stream
    Database.get(aClient, "/path/to/data", cb);
         
    // Since v1.2.1, in SSE mode (HTTP Streaming) task, you can filter the Stream events by using RealtimeDatabase::setSSEFilters(<keywords>), 
    // which the <keywords> is the comma separated events.
    // The event keywords supported are: 
    // get - To allow the http get response (first put event since stream connected).
    // put - To allow the put event.
    // patch - To allow the patch event.
    // keep-alive - To allow the keep-alive event.
    // cancel - To allow the cancel event.
    // auth_revoked - To allow the auth_revoked event.
    // To clear all prevousely set filter to allow all Stream events, use RealtimeDatabase::setSSEFilters().
    Database.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");

    Database.get(aClient, "/path/to/stream/data", cb, true /* SSE mode (HTTP Streaming) */);
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


7. ### 🔹 void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResult &aResult)

    Get value at the node path.

    ### Example
    ```cpp
    // Filtering data.
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


8. ### 🔹 void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResultCallback cb, const String &uid = "")

    Get value at the node path.

    ### Example
    ```cpp
    // Filtering data.
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


9. ### 🔹 void get(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult)

    Get value at the node path.
    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

    ### Example
    ```cpp
    #include <FS.h>
    File myFile; // Define the File object globally.

    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        myFile = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        myFile = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        myFile = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }

      // Set the internal FS object with global File object.
      file = myFile;
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.get(aClient, "/path/to/data", getFile(fileConfig), aResult);
    ```
    
    ```cpp
    void get(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `aResult` - The async result (AsyncResult)

10. ### 🔹 void get(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "")

    Get value at the node path.
    The FileConfig object constructor should be included filename and FileConfigCallback.
    
    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

    ### Example
    ```cpp
    #include <FS.h>
    File myFile; // Define the File object globally.

    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        myFile = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        myFile = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        myFile = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }

      // Set the internal FS object with global File object.
      file = myFile;
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.get(aClient, "/path/to/data", getFile(fileConfig), cb);
    ```
    
    ```cpp
    void get(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "")
    ```
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


11. ### 🔹 bool existed(AsyncClientClass &aClient, const String &path)

    Check if data exists in database.

    This function has only sync version.
    
    ### Example
    ```cpp
    // Check if the data exists in database.
    bool status = Database.existed(aClient, "/path/to/data");

    // The status will be true if data exists or returns false if data does not exist or error.

    // To check the operation status and error information in case of error after the task was done.   
    if (aClient.lastError().code() == 0)
        Serial.println("Operation is success.");
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    ```
    
    ```cpp
    bool existed(AsyncClientClass &aClient, const String &path)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to check.
    
    **Returns:**
    - boolean value indicates the operating status.

12. ### 🔹 void ota(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)

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


13. ### 🔹 void ota(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")

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


14. ### 🔹 bool set(AsyncClientClass &aClient, const String &path, T value)

    Set value to database.

    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.
    
    ### Example
    ```cpp
    // Set float value with 3 decimal places limit.
    bool status = Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));

    // To check the operation status and error information in case of error after the task was done.   
    if (aClient.lastError().code() == 0)
        Serial.println("Operation is success.");
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
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


15. ### 🔹 void set(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)

    Set value to database.

    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`. 

    ### Example
    ```cpp
    // Set float value with 3 decimal places limit.
    Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), aResult);
    ```

    ```cpp
    void set(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to set the value.
    - `value` - The value to set.
    - `aResult` - The async result (AsyncResult)
   

16. ### 🔹 void set(AsyncClientClass &aClient, const String &path, T value, AsyncResultCallback cb, const String &uid = "")

    Set value to database.

    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.
    
    ### Example
    ```cpp
    // Set float value with 3 decimal places limit.
    Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), cb);
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


17. ### 🔹 void set(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult)

    Set content from file to database.
    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

    ### Example
    ```cpp
    #include <FS.h>
    File myFile; // Define the File object globally.

    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        myFile = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        myFile = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        myFile = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }

      // Set the internal FS object with global File object.
      file = myFile;
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);
    ```

    ```cpp
    void set(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to set value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `aResult` - The async result (AsyncResult)


18. ### 🔹 void set(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "")

    Set content from file to database.
    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

    ### Example
    ```cpp
    #include <FS.h>
    File myFile; // Define the File object globally.

    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        myFile = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        myFile = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        myFile = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }

      // Set the internal FS object with global File object.
      file = myFile;
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.set(aClient, "/path/to/data", getFile(fileConfig), cb);
    ```
    
    ```cpp
    void set(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "")
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


19. ### 🔹 String push(AsyncClientClass &aClient, const String &path, T value)

    Push value to database.
    
    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.

    ### Example
    ```cpp
    // Push float value with 3 decimal places limit.
    String name = Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));

    // To check the operation status and error information in case of error after the task was done.   
    if (aClient.lastError().code() == 0)
        Serial.println("Operation is success.");
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    ```

    ```cpp
    String push(AsyncClientClass &aClient, const String &path, T value)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to push the value.
    - `value` - The value to push.
    
    **Returns:**
    - String random uuid string of a new node that created.

20. ### 🔹 void push(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)

    Push value to database.
    
    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.

    ### Example
    ```cpp
    // Push float value with 3 decimal places limit.
    void name = Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), aResult);

    // To check the operation status and error information in case of error after the task was done.   
    if (aClient.lastError().code() == 0)
        Serial.println("Operation is success.");
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    ```

    ```cpp
    void push(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to push the value.
    - `value` - The value to push.
    - `aResult` - The async result (AsyncResult).

21. ### 🔹 void push(AsyncClientClass &aClient, const String &path, T value,  AsyncResultCallback cb, const String &uid = "")

    Push value to database.
    
    The value type can be primitive types, Arduino `String`, `string_t`, `number_t`, `boolean_t` and `object_t`.
    
    The `string_t` is for string placeholder e.g. `string_t("hello there")`.
    The `number_t` is for number (integer, float, double) placeholder with decimal places e.g. `number_t(123.45678, 2)`.
    The `boolean_t` is for boolean placeholder e.g. `boolean_t(true)`.
    The `object_t` is for JSON and JSON Array objects placeholder e.g. `object_t("{\"name\":\"Jack\"}")` or `object_t("[123,true,\"hello\"]")`.

    ### Example
    ```cpp
    // Push float value with 3 decimal places limit.
    Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), cb);
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


22. ### 🔹 void push(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult)

    Push content from file to database.
    
    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
    
    ### Example
    ```cpp
    #include <FS.h>
    File myFile; // Define the File object globally.

    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        myFile = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        myFile = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        myFile = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }

      // Set the internal FS object with global File object.
      file = myFile;
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);
    ```
    
    ```cpp
    void push(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `aResult` - The async result (AsyncResult)


23. ### 🔹 void push(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "")

    Push content from file to database.

    The FileConfig object constructor should be included filename and FileConfigCallback.

    The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
    
    ### Example
    ```cpp
    #include <FS.h>
    File myFile; // Define the File object globally.

    void fileCallback(File &file, const char *filename, file_operating_mode mode)
    {
      switch (mode)
      {
        case file_mode_open_read:
        myFile = SPIFFS.open(filename, "r");
        break;

        case file_mode_open_write:
        myFile = SPIFFS.open(filename, "w");
        break;
        
        case file_mode_open_append:
        myFile = SPIFFS.open(filename, "a");
        break;
        
        case file_mode_remove:
        SPIFFS.remove(filename);
        break;
        
        default:
        break;
      }

      // Set the internal FS object with global File object.
      file = myFile;
    }

    FileConfig fileConfig("/example.txt", fileCallback);
    Database.set(aClient, "/path/to/data", getFile(fileConfig), cb);
    ```
    
    ```cpp
    void push(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "")
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to get value.
    - `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
    - `cb` - The async result callback (AsyncResultCallback).
    - `uid` - The user specified UID of async result (optional).


24. ### 🔹 bool update(AsyncClientClass &aClient, const String &path, const T &value)

    Update (patch) JSON object to database.
    
    ### Example
    ```cpp
    bool status = Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"));

    // To check the operation status and error information in case of error after the task was done.   
    if (aClient.lastError().code() == 0)
        Serial.println("Operation is success.");
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
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

25. ### 🔹 void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResult &aResult)

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

26. ### 🔹 void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResultCallback cb, const String &uid = "")

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


27. ### 🔹 bool remove(AsyncClientClass &aClient, const String &path)

    Remove node from database

    ### Example
    ```cpp
    bool status = Database.remove(aClient, "/path/to/data");

    // To check the operation status and error information in case of error after the task was done.   
    if (aClient.lastError().code() == 0)
        Serial.println("Operation is success.");
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    ```
    
    ```cpp
    bool remove(AsyncClientClass &aClient, const String &path)
    ```
    
    **Params:**
    - `aClient` - The async client.
    - `path` - The node path to remove.
    
    **Returns:**
    - boolean value indicates the operating status.

28. ### 🔹 void remove(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)

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


29. ### 🔹 void remove(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")

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


30. ### 🔹 void setSSEFilters(const String &filter = "")

    Filtering response payload for SSE mode (HTTP Streaming). 
    
    This function is available since v1.2.1.

    This is optional to allow specific events filtering.

    The following event keywords are supported.
    
    `get` - To allow the http get response (first put event since stream connected).
    
    `put` - To allow the put event.
    
    `patch` - To allow the patch event.
    
    `keep-alive` - To allow the keep-alive event.
    
    `cancel` - To allow the cancel event.
    
    `auth_revoked` - To allow the auth_revoked event.

    You can separate each keyword with comma or space.
    
    To clear all prevousely set filter to allow all Stream events, use `RealtimeDatabase::setSSEFilters()`.

    ### Example
    ```cpp
    
    // To all all tream events.
    Database.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");
    
    // SSE mode (HTTP Streaming)
    Database.get(aClient, "/path/to/stream/data", cb, true);
    ```

    ```cpp
    void setSSEFilters(const String &filter = "")
    ```
    **Params:**
    - `filter` - The event keywords for filtering.

31. ## 🔹  void setOTAStorage(OTAStorage &storage)

    Set Arduino OTA Storage.

    ```cpp
    void setOTAStorage(OTAStorage &storage)
    ```

    **Params:**

    - `storage` - The Arduino `OTAStorage` class object.

32. ### 🔹 void loop()

    Perform the async task repeatedly.
    Should be places in main loop function.
    
    ```cpp
    void loop()
    ```

