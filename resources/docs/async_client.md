# AsyncClientClass

## Description

The async client class.

```cpp
class AsyncClientClass
```

## Constructors

1. ### 🔹 AsyncClientClass()

Default constructor.

```cpp
AsyncClientClass()
```

2. ### 🔹 AsyncClientClass(Client &client, bool reconnect = true)


```cpp
AsyncClientClass(Client &client, bool reconnect = true)
```

**Params:**

- `client` - The SSL client that working with the network interface.

- `reconnect` - Optional The network reconnection option when the network lost connection.

3. ### 🔹 AsyncClientClass(Client &client, network_config_data &net)


```cpp
AsyncClientClass(Client &client, network_config_data &net)
```

**Params:**

- `client` - The SSL client that working with the network interface.

- `net` - The network config data can be obtained from the networking classes via the static function called `getNetwork`.

4. ### 🔹 AsyncClientClass(AsyncTCPConfig &tcpClientConfig, network_config_data &net)


```cpp
AsyncClientClass(AsyncTCPConfig &tcpClientConfig, network_config_data &net)
```

**Params:**

- `tcpClientConfig` - The `AsyncTCPConfig` object. See `src/core/AsyncTCPConfig.h`

- `net` - The network config data can be obtained from the networking classes via the static function called `getNetwork`.


## Functions

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

3. ## 🔹 void stopAsync(bool all = false)

Stop and remove the async/sync task from the queue.

```cpp
void stopAsync(bool all = false)
```

**Params:**

- `all` - The option to stop and remove all tasks. If false, only running task will be stop and removed from queue.

4. ## 🔹  void stopAsync(const String &uid)

Stop and remove the specific async/sync task from the queue.

```cpp
void stopAsync(const String &uid)
```

**Params:**

- `uid` - The task identifier of the task to stop and remove from the queue.


5. ## 🔹  size_t taskCount() const

Get the number of async/sync tasks that stored in the queue.

```cpp
size_t taskCount() const
```

**Returns:**

- `size_t` - The total tasks in the queue.


6. ## 🔹   FirebaseError lastError() const

Get the last error information from async client.

```cpp
FirebaseError lastError() const
```

**Returns:**

- `FirebaseError` - The `FirebaseError` object that contains the last error information.


7. ## 🔹  String etag() const

Get the response etag.

```cpp
String etag() const
```

**Returns:**

- `String` - The response etag header.

8. ## 🔹  void setETag(const String &etag) 

Set the etag header to the task (DEPRECATED).

The etag can be set via the functions that support etag.

```cpp
void setEtag(const String &etag) 
```

**Params:**

- `etag` - The ETag to set to the task.

9. ## 🔹  void setSyncSendTimeout(uint32_t timeoutSec)

Set the sync task's send timeout in seconds.

```cpp
void setSyncSendTimeout(uint32_t timeoutSec)
```

**Params:**

- `timeoutSec` - The TCP write timeout in seconds.


10. ## 🔹  void setSyncReadTimeout(uint32_t timeoutSec) 

Set the sync task's read timeout in seconds.

```cpp
void setSyncReadTimeout(uint32_t timeoutSec) 
```

**Params:**

- `timeoutSec` - The TCP read timeout in seconds.


11. ## 🔹  void setSessionTimeout(uint32_t timeoutSec)

Set the TCP session timeout in seconds.

```cpp
void setSessionTimeout(uint32_t timeoutSec)
```
   
**Params:**

- `timeoutSec` - The TCP session timeout in seconds.
   

12. ### 🔹 void setSSEFilters(const String &filter = "")

Filtering response payload for for Relatime Database SSE streaming. 
    
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
    
To clear all prevousely set filter to allow all Stream events, use `AsyncClientClass::setSSEFilters()`.

This will overwrite the value sets by `RealtimeDatabase::setSSEFilters`.

### Example
```cpp
    
// To all all tream events.
aClient.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");
    
// SSE streaming
Database.get(aClient, "/path/to/stream/data", cb, true);
```

```cpp
void setSSEFilters(const String &filter = "")
```
**Params:**
- `filter` - The event keywords for filtering.
   


13. ## 🔹  void setClient(Client &client)

Set the SSL client.

```cpp
void setClient(Client &client)
```

**Params:**

- `client` - The SSL client.