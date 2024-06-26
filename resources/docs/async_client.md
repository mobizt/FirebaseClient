# AsyncClientClass

## Description

The async client class.

```cpp
class AsyncClientClass
```

1. ## 🔹 void setAsyncResult(AsyncResult &result)

Set the external async result to use with the sync task.

If no async result was set (unset) for sync task, the internal async result will be used and shared usage for all sync tasks.

```cpp
void setAsyncResult(AsyncResult &result)
```

**Params:**

- `result` - The AsyncResult to set.


2. ## 🔹 void unsetAsyncResult()

Unset the external async result use with the sync task.

The internal async result will be used for sync task.

```cpp
void unsetAsyncResult()
```

3. ## 🔹   bool networkStatus()

Get the network connection status.

```cpp
 bool networkStatus()
```

**Returns:**

- `bool` - Returns true if network is connected.


4. ## 🔹 void stopAsync(bool all = false)

Stop and remove the async/sync task from the queue.

```cpp
void stopAsync(bool all = false)
```

**Params:**

- `all` - The option to stop and remove all tasks. If false, only running task will be stop and removed from queue.

5. ## 🔹  void stopAsync(const String &uid)

Stop and remove the specific async/sync task from the queue.

```cpp
void stopAsync(const String &uid)
```

**Params:**

- `uid` - The task identifier of the task to stop and remove from the queue.


6. ## 🔹  size_t taskCount() const

Get the number of async/sync tasks that stored in the queue.

```cpp
size_t taskCount() const
```

**Returns:**

- `size_t` - The total tasks in the queue.


7. ## 🔹   FirebaseError lastError() const

Get the last error information from async client.

```cpp
FirebaseError lastError() const
```

**Returns:**

- `FirebaseError` - The `FirebaseError` object that contains the last error information.


8. ## 🔹  String etag() const

Get the response ETag.

```cpp
String etag() const
```

**Returns:**

- `String` - The response ETag header.

9. ## 🔹  void setETag(const String &etag) 

Set the ETag header to the task.

ETag of async client which obtained from etag() function will be empty after it assign to the task.

```cpp
void setETag(const String &etag) 
```

**Params:**

- `etag` - The ETag to set to the task.

10. ## 🔹  void setSyncSendTimeout(uint32_t timeoutSec)

Set the sync task's send timeout in seconds.

```cpp
void setSyncSendTimeout(uint32_t timeoutSec)
```

**Params:**

- `timeoutSec` - The TCP write timeout in seconds.


11. ## 🔹  void setSyncReadTimeout(uint32_t timeoutSec) 

Set the sync task's read timeout in seconds.

```cpp
void setSyncReadTimeout(uint32_t timeoutSec) 
```

**Params:**

- `timeoutSec` - The TCP read timeout in seconds.


12. ## 🔹  void setSessionTimeout(uint32_t timeoutSec)

Set the TCP session timeout in seconds.

```cpp
void setSessionTimeout(uint32_t timeoutSec)
```

**Params:**

- `timeoutSec` - The TCP session timeout in seconds.

