# RealtimeDatabaseResult

## Description

The Realtime database result class.

```cpp
class RealtimeDatabaseResult
```

1. ## 🔹  T to()

Convert the RealtimeDatabaseResult to any type of values.

```cpp
T to()
```

**Returns:**

- `T` - The T type value e.g. boolean, integer, float, double and string.


2. ## 🔹  bool isStream() const

Check if the async task is SSE mode (HTTP Streaming) task.

```cpp
bool isStream() const
```

**Returns:**

- `bool` - Returns true if the task is SSE mode (HTTP Streaming) task.

3. ## 🔹  String name() const

Get the name (random UID) of the node that will be creaated from `RealtimeDatabase::Push`.

```cpp
String name() const
```

**Returns:**

- `String` - The push UID string.


4. ## 🔹  String ETag() const

Get the ETag from Realtime database get function.

```cpp
String ETag() const
```

**Params:**

- `String` - The ETag string.


5. ## 🔹   String dataPath() const

Get the SSE mode (HTTP Streaming) event data path which is the relative path of the data that has been changed in the database.

```cpp
String dataPath() const
```

**Returns:**

- `String` - The relative path of data that has been changed.


6. ## 🔹  String event()

Get the `SSE mode (HTTP Streaming)` event type string.

```cpp
String event()
```

**Returns:**

- `String` - The event type string e.g. `put`, `patch`, `keep-alive`, `cancel` and `auth_revoked`.

7. ## 🔹  String data()

Get the SSE mode (HTTP Streaming) event data that has been changed.

```cpp
String data()
```

**Returns:**

- `String` - The data that has been changed.

8. ## 🔹  bool eventTimeout()

Get the SSE mode (HTTP Streaming) event timed out status.

```cpp
bool eventTimeout()
```

**Returns:**

- `bool` - The SSE mode (HTTP Streaming) event timed out status.


9. ## 🔹  realtime_database_data_type type()

Get the type of Realtime database data.

The realtime_database_data_type enums are included the following.
  
`realtime_database_data_type_undefined` or -1

`realtime_database_data_type_null` or 5.

`realtime_database_data_type_integer` or 1.

`realtime_database_data_type_float` or 2.

`realtime_database_data_type_double` or 3.

`realtime_database_data_type_boolean` or 4.

`realtime_database_data_type_string` or 5.

`realtime_database_data_type_json` or 6.

`realtime_database_data_type_array` or 7.

```cpp
realtime_database_data_type type()
```

**Returns:**

- `realtime_database_data_type` - The realtime_database_data_type enum represents the type of Realtime database data.

