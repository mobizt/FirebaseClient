# #ifndef ASYNC_DATABASE_H

Created March 18, 2024

The MIT License (MIT)
Copyright (c) 2024 K. Suwatchai (Mobizt)


Permission is hereby granted, free of charge, to any person returning a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

```cpp
#ifndef ASYNC_DATABASE_H
```


# template <typename T1 = int>

Get value at the node path.

Example:

String json = Database.get<String>(aClient, "/path/to/data");

```cpp
template <typename T1 = int>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.

**Returns:**

- value that casts from response payload.

# template <typename T1 = int>

Get value at the node path.

The DatabaseOptions related to the Conditional Requests and Query Parameters supported by
Firebase Realtime Database REST API are following.

readTimeout, the timeout (number) in ms which limits how long the read takes on the server side.

writeSizeLimit, the size of a write limit can be "tiny", "small", "meduim", "large" and "unlimited".

shallow, the option (boolean) for shallowing (truncating) the JSON object data into true while JSON primitive values
(string, number and boolean) will not shallow.

silent, the option (boolean) to mute the return reponse payload.

classicRequest, the option (boolean) to use HTTP POST for PUT (set) and DELETE (remove).

customHeaders, the custom header which is currently supported only X-Firebase-ETag header.

ETAG, the ETAG value for The if-match condition request header for the data you want to update.

Filter, the options for complex data filtering which included the properties i.e., orderBy, startAt, endAt, limitToFirst, limitToLast, and equalTo.

Example:

DatabaseOptions options;
options.filter.orderBy("Data");
options.filter.startAt(105);
options.filter.endAt(120);
options.filter.limitToLast(8);

String json = Database.get<String>(aClient, "/path/to/data", options);

```cpp
template <typename T1 = int>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `options` - The database options (DatabaseOptions).

**Returns:**

- value that casts from response payload.

# 💡 void get(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, bool sse = false)

Get value at the node path.

Example:

Database.get(aClient, "/path/to/data", aResult);

```cpp
void get(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, bool sse = false)
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `aResult` - The async result (AsyncResult)
- `sse` - The Server-sent events (Stream) mode


# 💡 void get(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, bool sse = false, const String &uid = "")

Get value at the node path.

Example:

Database.get(aClient, "/path/to/data", cb);

```cpp
void get(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, bool sse = false, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `cb` - The async result callback (AsyncResultCallback).
- `sse` - The Server-sent events (Stream) mode.
- `uid` - The user specified UID of async result (optional).


# 💡 void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResult &aResult)

Get value at the node path.

Example:

DatabaseOptions options;
options.filter.orderBy("Data");
options.filter.startAt(105);
options.filter.endAt(120);
options.filter.limitToLast(8);

Database.get<String>(aClient, "/path/to/data", options, aResult);

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

# 💡 void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResultCallback cb, const String &uid = "")

Get value at the node path.

Example:

DatabaseOptions options;
options.filter.orderBy("Data");
options.filter.startAt(105);
options.filter.endAt(120);
options.filter.limitToLast(8);

Database.get<String>(aClient, "/path/to/data", options, cb);

```cpp
void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `options` - The database options (DatabaseOptions).
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


# 💡 void get(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)

Get value at the node path.
The FileConfig object constructor should be included filename and FileConfigCallback.

The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

Example of FileConfigCallback when SPIFFS used as filesystem.

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


Example:

FileConfig fileConfig("/example.txt", fileCallback);

Database.get(aClient, "/path/to/data", getFile(fileConfig), aResult);


```cpp
void get(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `aResult` - The async result (AsyncResult)


# 💡 void get(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")

Get value at the node path.
The FileConfig object constructor should be included filename and FileConfigCallback.

The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

Example of FileConfigCallback when SPIFFS used as filesystem.

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


Example:

FileConfig fileConfig("/example.txt", fileCallback);

Database.get(aClient, "/path/to/data", getFile(fileConfig), cb);


```cpp
void get(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


# 💡 bool existed(AsyncClientClass &aClient, const String &path)

Checks if path exists in database

Example:

bool status = Database.existed(aClient, "/path/to/data");

```cpp
bool existed(AsyncClientClass &aClient, const String &path)
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to check.

**Returns:**

- boolean value indicates the operating status.

# 💡 void ota(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)

Perform OTA update using a firmware file from the database.

The data of node to download should be base64 encoded string of the firmware file.

Example:

Database.ota(aClient, "/path/to/data", aResult);

```cpp
void ota(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to download.
- `aResult` - The async result (AsyncResult)


# 💡 void ota(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")

Perform OTA update using a firmware file from the database.

The data of node to download should be base64 encoded string of the firmware file.

Example:

Database.ota(aClient, "/path/to/data", aResult);

```cpp
void ota(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to download.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


# template <typename T = const char *>

Set value to database.

The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.

The string_t is for string placeholder e.g. string_t("hello there").
The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
The boolean_t is for boolean placeholder e.g. boolean_t(true).
The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").

Example:

bool status = Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));

```cpp
template <typename T = const char *>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to set the value.
- `value` - The value to set.

**Returns:**

- boolean value indicates the operating status.

# template <typename T = const char *>

Set value to database.

The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.

The string_t is for string placeholder e.g. string_t("hello there").
The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
The boolean_t is for boolean placeholder e.g. boolean_t(true).
The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").

Example:

Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), aResult);

```cpp
template <typename T = const char *>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to set the value.
- `value` - The value to set.
- `aResult` - The async result (AsyncResult)


# template <typename T = const char *>

Set value to database.

The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.

The string_t is for string placeholder e.g. string_t("hello there").
The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
The boolean_t is for boolean placeholder e.g. boolean_t(true).
The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").

Example:

Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), cb);

```cpp
template <typename T = const char *>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to set the value.
- `value` - The value to set.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


# 💡 void set(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)

Set content from file to database.
The FileConfig object constructor should be included filename and FileConfigCallback.

The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

Example of FileConfigCallback when SPIFFS used as filesystem.

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


Example:

FileConfig fileConfig("/example.txt", fileCallback);

Database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);


```cpp
void set(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `aResult` - The async result (AsyncResult)


# 💡 void set(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")

Set content from file to database.
The FileConfig object constructor should be included filename and FileConfigCallback.

The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

Example of FileConfigCallback when SPIFFS used as filesystem.

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


Example:

FileConfig fileConfig("/example.txt", fileCallback);

Database.set(aClient, "/path/to/data", getFile(fileConfig), cb);


```cpp
void set(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


# template <typename T = const char *>

Push value to database.

The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.

The string_t is for string placeholder e.g. string_t("hello there").
The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
The boolean_t is for boolean placeholder e.g. boolean_t(true).
The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").

Example:

String name = Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));

```cpp
template <typename T = const char *>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to set the value.
- `value` - The value to set.

**Returns:**

- String name of new node that created.

# template <typename T = const char *>

Push value to database.

The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.

The string_t is for string placeholder e.g. string_t("hello there").
The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
The boolean_t is for boolean placeholder e.g. boolean_t(true).
The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").

Example:

Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), aResult);

```cpp
template <typename T = const char *>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to set the value.
- `value` - The value to set.
- `aResult` - The async result (AsyncResult).


# template <typename T = const char *>

Push value to database.

The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.

The string_t is for string placeholder e.g. string_t("hello there").
The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
The boolean_t is for boolean placeholder e.g. boolean_t(true).
The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").

Example:

Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), cb);

```cpp
template <typename T = const char *>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to set the value.
- `value` - The value to set.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


# 💡 void push(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)

Push content from file to database.
The FileConfig object constructor should be included filename and FileConfigCallback.

The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

Example of FileConfigCallback when SPIFFS used as filesystem.

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


Example:

FileConfig fileConfig("/example.txt", fileCallback);

Database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);


```cpp
void push(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `aResult` - The async result (AsyncResult)


# 💡 void push(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")

Push content from file to database.
The FileConfig object constructor should be included filename and FileConfigCallback.

The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.

Example of FileConfigCallback when SPIFFS used as filesystem.

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


Example:

FileConfig fileConfig("/example.txt", fileCallback);

Database.set(aClient, "/path/to/data", getFile(fileConfig), cb);


```cpp
void push(AsyncClientClass &aClient, const String &path, file_config_data file, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to get value.
- `file` - The filesystem data (file_config_data) obtained from FileConfig class object.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


# template <typename T = object_t>

Update (patch) JSON object to database.

Example:

bool status = Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"));

```cpp
template <typename T = object_t>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to update.
- `value` - The JSON object (object_t) to update.

**Returns:**

- boolean value indicates the operating status.

# template <typename T = object_t>

Update (patch) JSON object to database.

Example:

bool status = Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"), aResult);

```cpp
template <typename T = object_t>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to update.
- `value` - The JSON object (object_t) to update.
- `aResult` - The async result (AsyncResult).


# template <typename T = object_t>

Update (patch) JSON object to database.

Example:

bool status = Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"), cb);

```cpp
template <typename T = object_t>
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to update.
- `value` - The JSON object (object_t) to update.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


# 💡 bool remove(AsyncClientClass &aClient, const String &path)

Remove node from database

Example:

bool status = Database.remove(aClient, "/path/to/data");

```cpp
bool remove(AsyncClientClass &aClient, const String &path)
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to remove.

**Returns:**

- boolean value indicates the operating status.

# 💡 void remove(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)

Remove node from database

Example:

Database.remove(aClient, "/path/to/data", aResult);

```cpp
void remove(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to remove.
- `aResult` - The async result (AsyncResult).


# 💡 void remove(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")

Remove node from database

Example:

Database.remove(aClient, "/path/to/data", aResult);

```cpp
void remove(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `path` - The node path to remove.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


# 💡 void url(const String &url)

Set the Firebase database URL

```cpp
void url(const String &url)
```

**Params:**

- `url` - The Firebase database URL.


# 💡 void loop()

Perform the async task repeatedly.
Should be places in main loop function.

```cpp
void loop()
```

