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

2. ### 🔹 AsyncClientClass(Client &client, network_config_data &net)


```cpp
AsyncClientClass(Client &client, network_config_data &net)
```

**Params:**

- `client` - The SSL client that working with the network interface.

- `net` - The network config data can be obtained from the networking classes via the static function called `getNetwork`.

3. ### 🔹 AsyncClientClass(AsyncTCPConfig &tcpClientConfig, network_config_data &net)


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

3. ## 🔹   bool networkStatus()

Get the network connection status.

```cpp
bool networkStatus()
```

**Returns:**

- `bool` - Returns true if network is connected.

4. ## 🔹   unsigned long networkLastSeen()

Get the network disconnection time.

```cpp
unsigned long networkLastSeen()
```

**Returns:**

- `unsigned long` - The millisec of network successfully connection since device boot.

5. ## 🔹   firebase_network_data_type getNetworkType()

Return the current network type.

```cpp
firebase_network_data_type getNetworkType()
```

**Returns:**

- `firebase_network_data_type` - The `firebase_network_data_type` enums are `firebase_network_data_default_network`, `firebase_network_data_generic_network`, `firebase_network_data_ethernet_network` and `firebase_network_data_gsm_network`.


6. ## 🔹 void stopAsync(bool all = false)

Stop and remove the async/sync task from the queue.

```cpp
void stopAsync(bool all = false)
```

**Params:**

- `all` - The option to stop and remove all tasks. If false, only running task will be stop and removed from queue.

7. ## 🔹  void stopAsync(const String &uid)

Stop and remove the specific async/sync task from the queue.

```cpp
void stopAsync(const String &uid)
```

**Params:**

- `uid` - The task identifier of the task to stop and remove from the queue.


8. ## 🔹  size_t taskCount() const

Get the number of async/sync tasks that stored in the queue.

```cpp
size_t taskCount() const
```

**Returns:**

- `size_t` - The total tasks in the queue.


9. ## 🔹   FirebaseError lastError() const

Get the last error information from async client.

```cpp
FirebaseError lastError() const
```

**Returns:**

- `FirebaseError` - The `FirebaseError` object that contains the last error information.


10. ## 🔹  String etag() const

Get the response ETag.

```cpp
String etag() const
```

**Returns:**

- `String` - The response ETag header.

11. ## 🔹  void setETag(const String &etag) 

Set the ETag header to the task.

ETag of async client which obtained from etag() function will be empty after it assign to the task.

```cpp
void setETag(const String &etag) 
```

**Params:**

- `etag` - The ETag to set to the task.

12. ## 🔹  void setSyncSendTimeout(uint32_t timeoutSec)

Set the sync task's send timeout in seconds.

```cpp
void setSyncSendTimeout(uint32_t timeoutSec)
```

**Params:**

- `timeoutSec` - The TCP write timeout in seconds.


13. ## 🔹  void setSyncReadTimeout(uint32_t timeoutSec) 

Set the sync task's read timeout in seconds.

```cpp
void setSyncReadTimeout(uint32_t timeoutSec) 
```

**Params:**

- `timeoutSec` - The TCP read timeout in seconds.


14. ## 🔹  void setNetwork(Client &client, network_config_data &net)

Set the network interface.

The SSL client set here should work for the type of network set.

```cpp
void setNetwork(Client &client, network_config_data &net)
```

**Params:**

- `client` - The SSL client that working with this type of network interface.

- `net` - The network config data can be obtained from the networking classes via the static function called `getNetwork`.


