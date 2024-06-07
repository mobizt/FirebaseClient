# Messaging

## Description

Google Cloud Messaging class


```cpp
class Messaging
```

## Functions

1. ### 🔹 void resetApp()

Unbind or remove FirebaseApp.


```cpp
void resetApp()
```


2. ## 🔹  void loop()

Perform the async task repeatedly.
Should be places in main loop function.

```cpp
void loop()
```


3. ## 🔹  String send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message)


The Firebase project Id should be only the name without the firebaseio.com.

This function requires ServiceAuth authentication.

Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages

```cpp
String send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message)
```

**Params:**

- `aClient` - The async client.
- `parent` - The Messages::Parent object included project Id in its constructor.
- `message` - The Messages::Message object that holds the information to send.

**Returns:**

- `String` - The response payload.

4. ## 🔹  void send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message, AsyncResult &aResult)


The Firebase project Id should be only the name without the firebaseio.com.

This function requires ServiceAuth authentication.

Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages

```cpp
void send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `parent` - The Messages::Parent object included project Id in its constructor.
- `message` - The Messages::Message object that holds the information to send.
- `aResult` - The async result (AsyncResult).


5. ## 🔹  void send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message, AsyncResultCallback cb, const String &uid = "")


The Firebase project Id should be only the name without the firebaseio.com.

This function requires ServiceAuth authentication.

Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages

```cpp
void send(AsyncClientClass &aClient, const Messages::Parent &parent, const Messages::Message &message, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `parent` - The Messages::Parent object included project Id in its constructor.
- `message` - The Messages::Message object that holds the information to send.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

