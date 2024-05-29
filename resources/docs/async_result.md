# AsyncResult

## Description

The async result class.

```cpp
class AsyncResult
```

1. ## 🔹  const char *c_str()

Get the pointer to the internal response payload string buffer.

```cpp
const char *c_str()
```

**Returns:**

- `const char *` - The pointer to internal response payload string.


2. ## 🔹  String payload() const

Get the copy of server response payload string.

```cpp
String payload() const
```

**Returns:**

- `String` - The copy of payload string.

3. ## 🔹  String path() const

Get the path of the resource of the request.

```cpp
String path() const
```

**Returns:**

- `String` - The path of the resource of the request.


4. ## 🔹  String etag() const

Get the Etag of the server response headers.

```cpp
String etag() const
```

**Params:**

- `String` - The ETag of response header.


5. ## 🔹   String uid() const

Get the unique identifier of async task.

```cpp
String uid() const
```

**Returns:**

- `String` - The UID of async task.


6. ## 🔹  String debug()

Get the debug information.

```cpp
String debug()
```

**Returns:**

- `String` - The debug information.

7. ## 🔹  void clear()

Clear the async result.

```cpp
void clear()
```

8. ## 🔹  RealtimeDatabaseResult &to()

Get the reference to the internal RealtimeDatabaseResult object.

```cpp
RealtimeDatabaseResult &to()
```

**Returns:**

- `RealtimeDatabaseResult &` - The reference to the internal RealtimeDatabaseResult object.


9. ## 🔹  int available()

Get the number of bytes of available response payload.

```cpp
int available()
```

**Returns:**

- `int` - The number of bytes available.


10. ## 🔹  app_event_t &appEvent()

Get the reference of internal app event information.

```cpp
app_event_t &appEvent()
```

**Returns:**

- `app_event_t &` - The reference of internal app event.


11. ## 🔹  bool uploadProgress()

Check if file/BLOB upload information is available.

```cpp
bool uploadProgress()
```

**Returns:**

- `bool` - Returns true if upload information is available.


12. ## 🔹  upload_data_t uploadInfo() const

Get the file/BLOB upload information.

```cpp
upload_data_t uploadInfo() const
```

**Returns:**

- `upload_data_t` - The file/BLOB upload information.


13. ## 🔹  bool downloadProgress()

Check if the file/BLOB download information is availablle.

```cpp
bool downloadProgress()
```

**Returns:**

- `bool` - Returns true if download information is available.


14. ## 🔹  download_data_t downloadInfo() const

Get the file/BLOB download information.

```cpp
download_data_t downloadInfo() const
```

**Returns:**

- `download_data_t` - The file/BLOB download information.


15. ## 🔹  bool isOTA() const

Check if the result is from OTA download task.

```cpp
bool isOTA() const
```

**Returns:**

- `bool` - Returns true if the result is from OTA download task.


16. ## 🔹  bool isError()

Check if the error occurred in async task.

```cpp
bool isError()
```

**Returns:**

- `bool` - Returns true if error occurred.


17. ## 🔹  bool isDebug()

Check if the debug information in available.

```cpp
bool isDebug()
```

**Returns:**

- `bool` - Returns true if debug information in available.


18. ## 🔹  bool isEvent()

Check if the app event information in available.

```cpp
bool isEvent()
```

**Returns:**

- `bool` - Returns true if app event information in available.


19. ## 🔹  FirebaseError &error()

Get the reference of internal FirebaseError object.

```cpp
FirebaseError &error()
```

**Returns:**

- `FirebaseError &` - The internal FirebaseError object.