# AsyncClientClass

## Description

The async client class.

```cpp
class AsyncClientClass
```

1. ## 🔹   bool networkStatus()

Get the network connection status.

```cpp
 bool networkStatus()
```

**Returns:**

- `bool` - Returns true if network is connected.


2. ## 🔹 void stopAsync(bool all = false)

Stop and remove the async/sync task from the queue.

```cpp
void stopAsync(bool all = false)
```

**Params:**

- `all` - The option to stop and remove all tasks. If false, only running task will be stop and removed from queue.

3. ## 🔹  void stopAsync(const String &uid)

Stop and remove the specific async/sync task from the queue.

```cpp
void stopAsync(const String &uid)
```

**Params:**

- `uid` - The task identifier of the task to stop and remove from the queue.


4. ## 🔹  size_t taskCount() const

Get the number of async/sync tasks that stored in the queue.

```cpp
size_t taskCount() const
```

**Returns:**

- `size_t` - The total tasks in the queue.


5. ## 🔹   FirebaseError lastError() const

Get the last error information from async client.

```cpp
FirebaseError lastError() const
```

**Returns:**

- `FirebaseError` - The `FirebaseError` object that contains the last error information.


6. ## 🔹  String etag() const

Get the response ETag.

```cpp
String etag() const
```

**Returns:**

- `String` - The response ETag header.

7. ## 🔹  void setETag(const String &etag) 

Set the ETag header to the task.

ETag of async client which obtained from etag() function will be empty after it assign to the task.

```cpp
void setETag(const String &etag) 
```

**Params:**

- `etag` - The ETag to set to the task.

8. ## 🔹  void setSyncSendTimeout(uint32_t timeoutSec)

Set the sync task's send time out in seconds.

```cpp
void setSyncSendTimeout(uint32_t timeoutSec)
```

**Params:**

- `timeoutSec` - The TCP write time out in seconds.


9. ## 🔹  void setSyncReadTimeout(uint32_t timeoutSec) 

Set the sync task's read time out in seconds.

```cpp
void setSyncReadTimeout(uint32_t timeoutSec) 
```

**Params:**

- `timeoutSec` - The TCP read time out in seconds.

