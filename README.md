# FirebaseClient

![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/mobizt/FirebaseClient/.github%2Fworkflows%2Fcompile_library.yml?logo=github&label=compile) [![Github Stars](https://img.shields.io/github/stars/mobizt/FirebaseClient?logo=github)](https://github.com/mobizt/FirebaseClient/stargazers) ![Github Issues](https://img.shields.io/github/issues/mobizt/FirebaseClient?logo=github)

![GitHub Release](https://img.shields.io/github/v/release/mobizt/FirebaseClient) ![PlatformIO](https://badges.registry.platformio.org/packages/mobizt/library/FirebaseClient.svg) ![GitHub Release Date](https://img.shields.io/github/release-date/mobizt/FirebaseClient)

[![GitHub Sponsors](https://img.shields.io/github/sponsors/mobizt?logo=github)](https://github.com/sponsors/mobizt)

`2024-04-24T18:37:40Z`

## Table of Contents

1. [Introduction](#introduction)
2. [Frequently Asked Questions](/FAQ.md)
3. [Supported Devices](#supported-devices)
4. [Dependencies](#dependencies)
5. [Incompatability Between Old Firebase Library and This Firebase Library](#incompatability-between-old-firebase-library-and-this-firebase-library)
6. [Installation](#installation)
    1. [Using Library Manager](#using-library-manager)
    2. [Manual installation](#manual-installation)
    3. [RP2040 Arduino SDK installation](#rp2040-arduino-sdk-installation)
7. [Usages](#usages)
    1. [Authentication](#authentication)
    2. [Firebase and Google Services](#firebase-and-google-services)
    3. [Async Queue](#async-queue)
        1. [Running Many Tasks Concurrency Using Different Async Clients (In Different SSL Clients)](#running-many-tasks-concurrency-using-different-async-clients-in-different-ssl-clients)
    4. [Async Client](#async-client)
    5. [Tasks Timeout](#tasks-timeout)
    6. [Async Result](#async-result)
        1. [App Events](#app-events)
        2. [Server Response and Event Data](#server-response-and-event-data)
        3. [Error Information](#error-information)
        4. [Debug Information](#debug-information)
    7. [App Initialization](#app-initialization)
        1. [CustomAuth (ID Token Authorization Using Service Account)](#customauth-id-token-authorization-using-service-account)
        2. [ServiceAuth (OAuth2.0 Access Token Authorization Using Service Account)](#serviceauth-oauth20-access-token-authorization-using-service-account)
        3. [UserAuth (User Sign-In Authentication)](#userauth-user-sign-in-authentication)
        4. [NoAuth (Non-Authentication)](#noauth-non-authentication)
        5. [CustomToken (ID Token Authorization Using Custom Token)](#customtoken-id-token-authorization-using-custom-token)
        6. [AccessToken (OAuth2.0 Access Token Authorization)](#accesstoken-oauth20-access-token-authorization)
        7. [IDToken (ID token Authorization)](#idtoken-id-token-authorization)
        8. [LegacyToken (Database Secret)](#legacytoken-database-secret)
    8. [Working with Filesystems and BLOB](#working-with-filesystems-and-blob)
    9. [Working with Networks](#working-with-networks)
    10. [Required Operation Flows](#required-operation-flows)
    11. [Basic Example](#basic-example)
    12. [Realtime Database Usage](#realtime-database-usage)
    13. [Google Cloud Firestore Database Usage](#google-cloud-firestore-database-usage)
    14. [Google Cloud Messaging Usage](#google-cloud-messaging-usage)
    15. [Firebase Storage Usage](#firebase-storage-usage)
    16. [Google Cloud Storage Usage](#google-cloud-storage-usage)
    17. [Google Cloud Functions Usage](#google-cloud-functions-usage)
    18. [Placeholders](#the-placeholder-represents-the-primitive-types-values-that-used-in-this-library)
8. [Project Preparation and Setup](#project-preparation-and-setup)
    1. [Authentication Getting Started](#authentication-getting-started)
    2. [Realtime Database Getting Started](#realtime-database-getting-started)
    3. [Google Cloud Firestore Database Getting Started](#google-cloud-firestore-database-getting-started)
    4. [Storage Getting Started](#storage-getting-started)
    5. [Google Cloud Functions Getting Started](#google-cloud-functions-getting-started)
    6. [Cloud Messaging Getting Started](#cloud-messaging-getting-started)
    7. [Service Account](#service-account)
        1. [Service Account Permissions](#service-account-permissions)
9. [Memory Options](#memory-options)
    1. [Memory Options for ESP8266](#memory-options-for-esp8266)
        1. [Arduino IDE](#arduino-ide)
        2. [PlatformIO IDE](#platformio-ide)
        3. [ESP8266 and SRAM/PSRAM Chip connection](#esp8266-and-srampsram-chip-connection)
    2. [Memory Options for ESP32](#memory-options-for-esp32)
        1. [Arduino IDE](#arduino-ide-1)
        2. [PlatformIO IDE](#platformio-ide-1)
10. [Library Build Options](#library-build-options)
    1. [Predefined Options](#predefined-options)
    2. [Optional Options](#optional-options)
11. [License](#license)


## Introduction

Async Firebase Client library for Arduino Documentation.

This library is the `REST APIs Firebase Client` that supports the following `Firebase Products` and `Google Cloud Products`.

- `Firebase Realtime Database`
- `Cloud Firestore Database`
- `Cloud Messaging`
- `Firebase Storage`
- `Google Cloud Functions`
- `Google Cloud Storage`

This [`FirebaseClient`](https://github.com/mobizt/FirebaseClient) library was cteated to solve the following issues in the old Firebase library.

- The confusion of different library names from the same developer and other developers in Library Manager.
- The different main header file name and class name issue.
- The long path issue due to long library name with spaces leads to `VisualMicro IDE` compilation error.
- The cental settings using only single config class can cause the usage confusion.
- The system time changes causes the auth token expiry period calculation error.
- The internal SSL Client and WiFiClient issue causes the session need to be closed at some interval.
- The `Realtime Database` stream event data can be missing due to the sync read operation.
- The async operation is not truely impremented.

> [!IMPORTANT]  
> If you are new to `Firebase Products`, please read the [Project Preparation and Setup](#project-preparation-and-setup) section first.

> [!IMPORTANT]  
> For ESP8266 device usage, you should read the [Memory Options for ESP8266](#memory-options-for-esp8266) section for heap selection.

## Frequently Asked Questions

For the FAQ (Frequently Asked Questions), please visit [here](/FAQ.md).

## Supported Devices.

 * ESP8266 MCUs based boards
 * ESP32 MCUs based boards
 * Arduino® MKR WiFi 1010
 * Arduino® MKR 1000 WIFI
 * Arduino® Nano 33 IoT
 * Arduino® MKR Vidor 4000
 * Arduino® UNO R4 WiFi (Renesas)
 * Arduino® Portenta C33
 * Arduino® Nano RP2040
 * Arduino® GIGA R1 WiFi
 * Raspberry Pi Pico (RP2040)
 * STM32 MCU based boards (minimum 256k Flash memory)
 * Teensy 3.1, 3.2, 3.5, 3.6, 4.0 and 4.1
 * LAN8720 Ethernet PHY
 * TLK110 Ethernet PHY
 * IP101 Ethernet PHY
 * ENC28J60 SPI Ethernet module
 * W5100 SPI Ethernet module
 * W5500 SPI Ethernet module
 * SIMCom Modules with [TinyGSM](https://github.com/vshymanskyy/TinyGSM)


 ## Dependencies


This library required the latest device's **platform Core SDK** to be installed.

> [!IMPORTANT]  
> Please make sure that the `Arduino IDE's Board Manager URL` is up to date before you update your device's `platform Core SDK`. Any issue due to using out dated `platform Core SDK` is your own risk and does not support by library.
>
> In case the library compile status showed here is passing and your code compilation is error, you should check your libary installation, `platform Core SDK` installation and the library usage. You cannot install library in the cloud storage virtual folder or sandbox e.g. `Microsoft's OneDrive`.

> [!IMPORTANT]
> If you are using `Arduino UNO R4 WiFi`, `Arduino Portenta C33`, `Arduino MKR WiFi 1010`, `Arduino NANO 33 IoT`, and `Arduino Nano RP2040`, please make sure you have already done the following.
> - Add the `Realtime Database host root certificate` to the board firmware. Plese see [Upload SSL root certificates](https://support.arduino.cc/hc/en-us/articles/360016119219-Upload-SSL-root-certificates) for how to.
> - Install the WiFiNINA library.
>

> [!IMPORTANT]
> If you are using `Arduino® MKR 1000 WIFI`, please make sure you have already done the following.
> - Add the `Realtime Database host root certificate` to the board firmware. 
> - Install the WiFi101 library and uninstall or remove the WiFiNINA library from the libraries folder if it was installed.
>

> [!WARNING]  
> Known problem in `PlatformIO IDE` compilation, if you are using `Arduino® UNO R4 WiFi` board and using the `Firebase Realtime Database`, your `Arduino® UNO R4 WiFi` board will stop with the error: `"Fault on interrupt or bare metal(no OS) environment"`.
> This is the error related to the `Realtime Database` server certificate issue in WiFiS3 library that compiled with `PlatformIO IDE` even you are already uploaded the SSL root certificates of `Realtime Database` server using the Arduino IDE Certificate Upload tool.
> This problem is only specific to `Firebase Realtime Database` in `Arduino® UNO R4 WiFi` which compiled using `PlatformIO IDE` only.  

For Arduino IDE, the ESP8266 Core SDK can be installed through **Boards Manager**. 

For PlatfoemIO IDE, the ESP8266 Core SDK can be installed through **PIO Home** > **Platforms** > **Espressif 8266 or Espressif 32**.

The RP2040 boards required [Arduino-Pico SDK](https://github.com/earlephilhower/arduino-pico) from Earle F. Philhower to be installed.

## Incompatability Between Old Firebase Library and This Firebase Library.

This library APIs are not compattible with the Firebase-ESP8266, Firebase-ESP32 and Firebase-ESP-Client libraries.

If you are using those libraries, the code should be rewritten.

In addition, some features are changed which included the following.

- ### FirebaseJson

There is no `JSON` library included in this `FirebaseClient` library. If you still prefer to use `FirebaseJson` functions as in the old library, you have to include it manually in your code.

The `FirebaseJson` library can be installed via the Arduino's Library Manager and PlatformIO's Library Manager or can be download and install from the [FirebaseJson Repository](https://github.com/mobizt/FirebaseJson).

- ### Realtime Database

The blob and file upload via set, push and update, the byte data will be converted to base64 encoded string and store to the database without the signature string (`file,` and `blob,`) prepended to the base64 string as in the old library.

The data on the database that read using the async get function which the blob and file config data assign to the function, will treat as base64 encoded string and will be decoded to byte array using base64 decoder.

Then get the data that contains signature string (`file,` and `blob,`) created by old library will lead to the error after base64 decoding.

Due to some pitfalls in the old library's `Multipath Stream` usage. User is only looking for the `JSON` parsing data without checking the actual received stream event data, and this library does not include the JSON parser, then this feature will not be implemented in this `FirebaseClient` library. 

- ### Cloud Messaging

The legaycy Firebase Cloud Messaging API was deprecated, only HTTPv1 is supported.

- ### Cloud Functions

 Deploying the Cloud Function from local flash or filesystem source in a single function is not available unless using several fnctions that are available to generate upload url, upload the source (zip file), deploy the function and set the permission.

- ### TCP KeepAlive

The `lwIP`'s TCP KeepAlive is not implemented in this `FirebaseClient` library because it can only be done by the network client (if it was supportd) e.g. `WiFiClient` in ESP32.

The TCP KeepAlive is currently not available in ESP8266 v3.1.2 at the time of this document writing and it will be available in the newer version after the pull request [#8940](https://github.com/esp8266/Arduino/pull/8940) was merged.

If you use the core SSL client e.g. `WiFiClientSecure` or `WiFiSSLClient`, sush feature is not available.

In the old Firebase library, this feature was done internally by the internal SSL client and `WiFiClient` integration.

If you want to use this feature and if you use ESP32, you can use `ESP_SSLClient` library that included in this library and set the `WiFiClient` as the client.

The following example code is for using `TCP KeepAlive` with `WiFiClient` and built-in `ESP_SSLClient` in ESP32.

```cpp

#include <lwip/sockets.h> // For lwIP TCP/IP sockets 

bool tcp_keep_alive_set = false;
int keepAlive = 1000; // Milliseconds
int keepIdle = 5; // Seconds
int keepInterval = 5; // Seconds
int keepCount = 1;

WiFiClient basic_client;

ESP_SSLClient ssl_client;

void setup()
{
    ssl_client.setClient(&basic_client);
    ssl_client.setInsecure();
}


void loop()
{
    // TCP KeepAlive should be set once after server was connected
    if (basic_client.connected())
    {
        if(!tcp_keep_alive_set)
        {
            tcp_keep_alive_set = true;
            basic_client.setSocketOption(IPPROTO_TCP, TCP_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
            basic_client.setSocketOption(IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle));
            basic_client.setSocketOption(IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
            basic_client.setSocketOption(IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
            
            // Or simpler functions in new ESP32 core
            // basic_client.setOption(TCP_KEEPALIVE, &keepAlive);
            // basic_client.setOption(TCP_KEEPIDLE, &keepIdle);
            // basic_client.setOption(TCP_KEEPINTVL, &keepInterval);
            // basic_client.setOption(TCP_KEEPCNT, &keepCount);
        }
    }
    else
    {
        tcp_keep_alive_set = false;
    }
}

```

- ### PSRAM

The PSRAM in this library was enabled by default but it was only used partly in tempolary buffer memory allocation.

- ### Server Authorization

The server certificate verification can be done with the SSL Client on user side.


- ### Filesystems

The filesystem features e.g. types (SD/Flash), initialization and operations should be done on user side.


## Installation


- ### Using Library Manager

At Arduino IDE, go to menu **Sketch** -> **Include Library** -> **Manage Libraries...**

In Library Manager Window, search **"firebase"** in the search form then select **"FirebaseClient"**. 

Click **"Install"** button.



For PlatformIO IDE, using the following command.

**pio lib install "FirebaseClient""**

Or at **PIO Home** -> **Library** -> **Registry** then search **FirebaseClient**.


- ### Manual installation

For Arduino IDE, download zip file from the repository (Github page) by select **Code** dropdown at the top of repository, select **Download ZIP** 

From Arduino IDE, select menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**.

Choose **FirebaseClient-main.zip** that previously downloaded.

Rename folder from **FirebaseClient-main** to **FirebaseClient**.

Go to menu **Files** -> **Examples** -> **FirebaseClient** and choose one from examples.


- ### RP2040 Arduino SDK installation

For Arduino IDE, the Arduino-Pico SDK can be installed from Boards Manager by searching pico and choose Raspberry Pi Pico/RP2040 to install.

For PlatformIO, the Arduino-Pico SDK can be installed via platformio.ini

```ini
[env:rpipicow]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = rpipicow
framework = arduino
board_build.core = earlephilhower
monitor_speed = 115200
board_build.filesystem_size = 1m
```

See this Arduino-Pico SDK [documentation](https://arduino-pico.readthedocs.io/en/latest/) for more information.


## Usages

Based on the async library design , there are no main generic `Firebase` class, central configuration class (`FirebaseConfig`) and all-in-one data containter class (`FirebaseData`) as in the old Firebase library. 

This library provides the managed classes that are used in different purposes i.e. the classes that used to hadle the sync and async tasks and to use as a container (data provider) for authentication credentials, networking and filesystems configurations, and async task result.

For the concept and basic usage understanding, you should read this documentation thoroughly. 

> [!IMPORTANT]  
> For new `Firebase` users, please read the [Project Preparation and Setup](#project-preparation-and-setup) section.

- ### Authentication

The authentication classes provide the authentication data for authentication and authorization using service account, sign-in credentials and auth tokens.

The REST API client (this library) will use the short-lived token for authorization.

This authorization token will be expired in 60 minutes or 3600 seconds.

The authorization token types that can be used for Firebase/Google APIs authorization are `ID token` and `access token` which will be called shorter as `auth tokens` in this library.

The following authentication/authorization classes generate and hold the `ID token` to be used in the authorization requests.

- The `UserAuth` class is for `User Sign-in authentication` which is the only authentication method that allows user to sign in for confirming the identity of a user and it provides the `ID token` to use in the authorization requests.

- The `IDToken` class is for `ID token authorization`, which the `ID token` that obtained from the user authentication process in other applications will be used for authorization request. 

- The `CustomAuth` class is for `ID token authorization using service account`. The `Service Account` will be used for user authentication and it also provides the `ID token` that uses in the authorization requests. This allows the client to acess the services on behalf of user with custom `UID`.

- The `CustomToken` class is for `ID token authorization using custom token`, which the custom claims signed `JWT` token obtained from the user authentication process in other applications will be used in the authorization requests.

The following authentication/authorization classes generate and hold the `access token` and `secret key` for Firebase/Google APIs privilege access.

- The `ServiceAuth` class is for `OAuth2.0 access token authorization using service account`. The `Service Account` will be used for user authentication and it also provides the `access token` that uses in the authorization requests.

- The `AcessToken` class is for `OAuth2.0 access token authorization`, which the `access token` from the user authentication process in other applications will be used in the authorization requests.

- The `LegacyToken` class that holds the `database secret` that used as the `secret key` for `Realtime Database` privilege access.

The following class provides non-authentication access.

- The `NoAuth` class is for non-authentication acesss.

The `User Sign-in authentication` requires the Email/Password Sign-in provider to be enabled in your project 

The `Users` tab in the `Authentication` page in the [`Firebase console`](https://console.firebase.google.com/) shows the list of users that are signed up or added.

![Email/Password Sign-in provider](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/user_auth_console.png)

The user management using this library are also [available](/examples/App/UserManagement).

The sucurity rules can be used for user accessing control.

Visit [Understand Firebase Realtime Database Security Rules](https://firebase.google.com/docs/database/security), [Get started with Cloud Firestore Security Rules](https://firebase.google.com/docs/firestore/security/get-started) and [Get started with Firebase Security Rules](https://firebase.google.com/docs/storage/security/get-started) to learn more about security rules.


In `OAuth2.0 access token authentication using service account` via `ServiceAuth` and `ID token authorization using service account` via `CustomAuth` involve the `JWT` token generation and RSA private key signing.

The valid time is needed in the JWT token generation process, the time status callback function that takes the user defined timestamp will be used in both `ServiceAuth`and `CustomAuth` classes.

The details for these authentication classes will be discussed later in the [App Initialization](#app-initialization) section.

This library used the internal millis timer to handle the token time to live. Then device time setting is not requierd in most authentication types. Only `ServiceAuth` and `CustomAuth` that required the valid time through the time status callback function.


- ### Firebase and Google Services

The Firebase and Google Services classes that are available in this library are included the following. 

- [RealtimeDatabase](examples/RealtimeDatabase/) class provides the `Realtime Database` functions.

- [Firestore::Databases](examples/FirestoreDatabase/Databases/) class provides the `Cloud Firestore Database` functions.

- [Firestore::Documents](examples/FirestoreDatabase/Documents/) class provides the `Cloud Firestore Document` functions.

- [Firestore::CollectionGroups::Indexes](examples/FirestoreDatabase/CollectionGroups/Indexes/) class provides the `Cloud Firestore CollectionGroups's Indexes` functions.

- [Messaging](examples/Messaging/) class provides the `Cloud Messaging` functions.

- [Storage](examples/Storage/) class provides the `Firebase Storage` functions.

- [CloudStorage](examples/CloudStorage/) class provides the `Google Cloud Storage` functions.

- [CloudFunctions](examples/CloudFunctions/) class provides the `Google Cloud Functions` functions.


- ### Async Queue


All sync and async tasks are managed using `FIFO queue` in async client. The task in the queue will refer to `slot` in this library. 
 
The memory used for a task that stores in the queue is approximately 1 k. Then in order to limit the memory usage in a async client, this library allows only 10 tasks can be stored in the queue at a time.

The maximum queue size can be set via the build flag `FIREBASE_ASYNC_QUEUE_LIMIT` or macro that defined in [src/Config.h](src/Config.h) or in your own defined config at [src/UserConfig.h](src/UserConfig.h).

The below image shows the ordering of tasks that are inserted or added to the queue. Only the first task in the queue will be executed.

![Async Task Queue](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/async_task_queue.png)

When the authentication task was required, it will insert to the first slot of the queue and all tasks are cancelled and removed from queue to reduce the menory usage unless the `SSE mode (HTTP Streaming)`task that stopped and waiting for restarting.

The authentication task has the highest priority and `SSE mode (HTTP Streaming)` task has the lowest priority in the queue.

![Async Task Queue Running](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/async_task_queue_running.png)

The sync task will be inserted to the first slot in the queue but above the authentication task.

When `SSE mode (HTTP Streaming)` task was currently stored in the queue, the new sync and async tasks will be inserted below it in the queue.

The new task can be cancelled in case the queue is full or the another `SSE mode (HTTP Streaming)` task is already stored in the queue. 

The error code `-118` (`FIREBASE_ERROR_OPERATION_CANCELLED`) or `"operation was cancelled"` will show in this case.
 
The running task will be removed from the queue when operation is finished or error occurred.

The `SSE mode (HTTP Streaming)` task will run continuously and repeatedly as long as the async task handler e.g. `<FirebaseServices>::loop()` and/or `FirebaseApp::loop()` (if it used the same async client) are running in the loop function.

The `SSE mode (HTTP Streaming)` is a kind of `infinite task` which the server connection was kept alive and waiting for the incoming event data.

When `SSE mode (HTTP Streaming)` task is currently running and new sync or async task is added to the queue, the `SSE mode (HTTP Streaming)` task will be stopped (but remains in the queue) as another task was inserted in to the first slot, the `SSE mode (HTTP Streaming)` task will be restart when another task is finished.

There is no notification error message like `"strean timed out"` in this library, when the `SSE mode (HTTP Streaming)` task was timed out because of network or any delay or blocking operation, it will reconnect automatically.

You cannot run more than one `infinite task` in the same async client's queue as one `infinite task` is never finished, and another `infinite task` is never started.

To run multiple `SSE mode (HTTP Streaming)` tasks, you have to run each task in different async client. Please note that `SSE mode (HTTP Streaming)` task consumes memory all the time while it is running. Running many `SSE mode (HTTP Streaming)` tasks may not possible because of out of memory especially in ESP8266 and SAMD devices.

> [!IMPORTANT]  
> The user blocking code and `delay` function that used in the same loop of async task handler will block the async tasks to run. Please avoid using `delay` function in the same loop of async task handler.

#### Running Many Tasks Concurrency Using Different Async Clients (In Different SSL Clients)

In Raspberry Pi Pico W, its `WiFiClientSecure` memory used for the transmit and receive buffers are adjustable (512 to 16384) and you have enough memory to run many tasks concurrency using different async clients.

In ESP32 device, its `WiFiClientSecure` memory usage cannot be adjusted, it requires at least 50 k per connection (37 k used for `mbedTLS` memory allocation) and only three `WiFiClientSecure`(s) can be defined.

Alternatively, for ESP32 device, you can use `ESP_SSLClient` that was included in this library. It works similar to ESP8266's `WiFiClientSecure` and the memory used for the transmit and receive buffers are adjustable (512 to 16384). See the [Stream Concurentcy example](/examples/RealtimeDatabase/Async/StreamConcurentcy/StreamConcurentcy.ino) for how to run many tasks concurrency.

The useful of using `ESP_SSLClient` is it uses `PSRAM` by default, you can use it in ESP32 and ESP8266 modules that have `PSRAM` or connected to external `PSRAM`.

In case of ESP8266 that connected to external `PSRAM`, you have enough RAM for running many tasks concurrency, and you can run [Stream Concurentcy example](/examples/RealtimeDatabase/Async/StreamConcurentcy/StreamConcurentcy.ino) without memory problem.

For how to use `PSRAM` in ESP32 and ESP8266 devices, see [Memory Options](#memory-options) section.

In case using ESP8266 without `PSRAM` and you want to reduce the memory usage, you can use `WiFiClientSecure` or `ESP_SSLClient` with minimum receive and transmit buffer size setting: 1024 for receive buffer and 512 for transmit buffer.

Note that, because the receive buffer size was set to minimum safe value, 1024, the large server response may not be able to handle. 


- ### Async Client

The SSL client is required to handle network and server connection as was set to the aync client class constructor.

The async client operates the SSL client via its pointer.

The async client also create the instances of async data and stored as a slot in its queue.

The async data created in the async client queue, holds the header and payload of the http request and response.


> [!WARNING]  
> The async client and its SSL Client that used for async tasks, need to be defined globally otherwise the dangling pointer problem will be occured.

The SSL Client is a kind of sync or blocking Client that takes time during SSL/TLS handshake.

The async SSL client can be used with the async client but it is currently experimental.

To stop and cancel the tasks, use `AsyncClientClass::stopAsync()` to stop only running task or `AsyncClientClass::stopAsync(true)` to stop all tasks.

In case `UID` is provided with the async task (see [Async Result](#async-result) section below), you can stop that task directly with `AsyncClientClass::stopAsync(<uid>)`.

You cannot stop the authentication task unless you can remove the functions; `FirebaseApp::loop()` and/or `FirebaseServicesApps::loop()`(if the same async client was used in `FirebaseServicesApps` and `FirebaseApp`) from the `loop` function.

> [!WARNING]  
> The numbers of async clients that can be used, the numbers of the tasks stored in the async client's queue will be limited which depends on the device free memory.

The async client (`AsyncClientClass`) takes two arguments in its constructor i.e. SSL Client and network config data (`network_config_data`). The network config data can be obtained from the networking classes via the static function called `getNetwork`.

This example shows how the async client is defined. 

```cpp
WiFiClientSecure ssl_client;

DefaultNetwork network;

AsyncClientClass aClient(ssl_client, getNetwork(network));
```

The `network_config_data` that obtained from network classes will be copied for use internally while the `SSL Client` pointer was used. Then `SSL Client` should be existed while using the `AsyncClientClass`. 

> [!WARNING]  
> To prevent dangling pointer issue, the `SSL Client` should be defined at the same usage scope as `AsyncClientClass`.

For the details of networking class, see [Working with Networks](#working-with-networks) section.

### Tasks Timeout

In case async task, the send timeout and read timeout are 30 seconds by default and it cannot be changed.

In case sync task, the timeouts can be set via `AsyncClientClass::setSyncSendTimeout` and `AsyncClientClass::setSyncReadTimeout` for send timeout and read timeout respectively.

- ### Async Result

This library provides the class object called async result (`AsyncResult`) which is used as a container that provides 4 types of information: `App Events` (`app_event_t`), `Server Response and Event Data`, `Debug Information` and `Error Information` and will be discussed later.

The information of `App Events` (`app_event_t`)'s authentication task can be obtained from `AsyncResult::appEvent()`

The information of `Server Response and Event Data` included the payload, `SSE mode (HTTP Streaming)` event-data, status and header data.

The `Error Information` (`FirebaseError`) can be obtained from `AsyncResult::error()`.

The `Debug Information` (`String`) can be obtained from `AsyncResult::debug()`.

There are two use cases of async result:

   1. User provided async result.

   The async result is defined by user and assigned with the async function.

Example:

```cpp
Database.get(<AsyncClientClass>, <path>, <options>, <AsyncResult>);
```

   2. From the instance of async data.

   The async result was taken from async data which created within the async task.

   This is the case where the async result callback function was set to the async function.   

Example:

```cpp
Database.get(<AsyncClientClass>, <path>, <options>, <AsyncResultCallback>);
```

In case 1, the async result (`AsyncResult`) shall be defined globally because it needs the containter to keep the result while running the async task.

In case 1, when the async result was used in the `loop` function to take or print information from it. It should follow the below recommendation for checking its status before processing the data to avoid processing or printing the same data.

> To check the `App Event` changes, you should use `AsyncResult::appEvent().code() > 0`.
>
> To check the `Server Response and Event Data` changes, you should use `AsyncResult::available()`.
>
> To check whether `Error` is occurred, you should use `AsyncResult::isError()`.
>
> To check when `Debug` information is available, you should use `AsyncResult::isDebug()`.


In case 2, the async client (`AsyncClientClass`) where the async data was created and insert in to its queue, shall be defined globally because the async data should be existed while running the async task.

In case 2, the async result which can be accessed via the async result callback function is dynamically created and it will be deconstructed when the async task is finished or stopped. 

Then you may need to copy the async result that provided in the async result callback function to use anywhere when the async result instance was deconstructed.

In case 2, you can set the `UID` (unique identifier) to identify the task. The `UID` is any string that user defined and it is useful to identify the tasks when the same `AsyncResultCallback` was set for various async tasks. 

You can get the `UID` from `AsyncResult` via `AsyncResult::uid()`. 

> [!NOTE]  
> You cannot set `UID` to the `AsyncResult` directly as it will be overwritten, then the `UID` from the `AsyncResult` that defined by user in case 1 will always be the default `UID`. 

> [!NOTE]  
> The async client used in authentication task should be defined globally as it runs asynchronously.

> [!CAUTION]
> Please avoid calling the codes or functions that consumed large memory inside the asyn callback function because they use stack memory then the wdt reset crash can be occurred in ESP8266 device because of stack overflow.
> Then in ESP8266 device, global defined `AsyncResult` as used in case 1 is recommended for async task.

- ### App Events

The authentication task information or `app_event_t` can be obtained from `app_event_t AsyncResult::appEvent()`.

The event codes (`firebase_auth_event_type enum`) return from `AsyncResult::appEvent().code()` are included the following.

- `auth_event_uninitialized` or 0, when authentication task is not initialized.

- `auth_event_initializing` or 1, when authentication task is begin.

- `auth_event_signup` or 2, when the user sign up process is begin.

- `auth_event_send_verify_email` or 3, when the user verification email sending process is begin.

- `auth_event_delete_user` or 4, when the user deletion process is begin.

- `auth_event_reset_password` or 5, when the user reset password process is begin.

- `auth_event_token_signing` or 6, when the `JWT` token signing process is begin.

- `auth_event_authenticating` or 7, when the google API authorization token request is begin.

- `auth_event_auth_request_sent` or 8, when the a authorization token request was sent.

- `auth_event_auth_response_received` or 9, when a authorization token was received.

- `auth_event_ready` or 10, when the authorization token is ready.

- `auth_event_error` or 11, when error was occurred in the authentication process.

The event strings return from `String AsyncResult::appEvent().message()` which give the event detail are included the following.

- `"undefined"` when authentication task is not initialized.

- `"initializing"` when authentication task is begin.

- `"sign up"` when the user sign up process is begin.

- `"send verification email"` when the user verification email sending process is begin.

- `"delete user"` when the user deletion process is begin.

- `"reset password"` when the user reset password process is begin.

- `"token signing"` when the `JWT` token signing process is begin.

- `"authenticating"` when the google API authorization token request is begin.

- `"auth request sent"` when the a authorization token request was sent.

- `"auth response received"` when a authorization token was received.

- `"ready"` when the authorization token is ready.

- `"error"` when error was occurred in the authentication process.

- ### Server Response and Event Data

The generic server response can be obtained from `AsyncResult` via the following member functions.

- `String AsyncResult::payload()` returns the copy of server response payload string.  

- `const char *AsyncResult::c_str() const` returns the pointer to the internal response payload string buffer. 

- `bool AsyncResult::available()` returns true if server response payload was received.

- `String AsyncResult::path()` returns the path of the resource of the request. 

- `String AsyncResult::etag()` returns the Etag of the server response headers.

If the size of payload string from the async reseut is large, to access the internal string buffer directly, use `AsyncResult::payload().c_str()` instead.

<br>

The specific `Realtime Database` server response payload and `SSE mode (HTTP Streaming)` event data (`RealtimeDatabaseResult`) can be obtained from  `AsyncResult::to<RealtimeDatabaseResult>()` which are included the following.

- `bool RealtimeDatabaseResult::isStream()` returns true if the result is from `SSE mode (HTTP Streaming)` task.

- `String RealtimeDatabaseResult::event()` returns the `SSE mode (HTTP Streaming)` event type strings include `put`, `patch`, `keep-alive`, `cancel` and `auth_revoked`.

- `String RealtimeDatabaseResult::dataPath()` returns the `SSE mode (HTTP Streaming)` event data path which is the relative path of the changed value in the database. The absolute path of the changed value can be obtained from the concatenation of `AsyncResult::path()` and `RealtimeDatabaseResult::dataPath()` e.g. `AsyncResult::path() + "/" + RealtimeDatabaseResult::dataPath()`.

- `realtime_database_data_type RealtimeDatabaseResult::type()` returns the `realtime_database_data_type` enum (see below) represents the type of `Realtime Database` response payload and event data (`HTTP Streaming`).

- `RealtimeDatabaseResult::name()` returns the name (random UID) of the node that will be creaated after from `RealtimeDatabase::Push`.

<br>

The `realtime_database_data_type` enums are included the following.

- `realtime_database_data_type_undefined` or -1.

- `realtime_database_data_type_null`  or 5.

- `realtime_database_data_type_integer` or 1.

- `realtime_database_data_type_float` or 2.

- `realtime_database_data_type_double` or 3.

- `realtime_database_data_type_boolean` or 4.

- `realtime_database_data_type_string` or 5.

- `realtime_database_data_type_json` or 6.

- `realtime_database_data_type_array` or 7.

<br>

The `Realtime Database` server response payload and `HTTP Streaming` event data can be converted to the values of type `T` via `RealtimeDatabaseResult::to<T>()` e.g. boolean, integer, float, double and string.

The following is the example for how to convert the payload to any value.

```cpp

// aResult is the AsyncResult object.
RealtimeDatabaseResult &databaseResult = aResult.to<RealtimeDatabaseResult>();

bool boolVal = databaseResult.to<bool>();

int intVal = databaseResult.to<int>();

float floatval = databaseResult.to<float>();

double doubleVal = databaseResult.to<double>();

String stringVal = databaseResult.to<String>();
```

- ### Error Information

The error information (`FirebaseError`) from the async result can be obtained from `FirebaseError AsyncResult::error()` are included the following.

  - `int AsyncResult::error().code()` returns the numeric error of two major sources: `TCP Client Error` and `Server Response Error`. The `TCP Client Error` was defined [here](/src/core/Error.h#L32-L64) and the `Server Response Error` was defined [here](/src/core/Error.h#L72-L87).

  - `String AsyncResult::error().message()` returns the error string based on the `AsyncResult::error().code()` which in case `Server Response Error`, the `unauthorized`, `precondition failed (ETag does not match)` and `HTTP Status xxx` can be returned. In case `TCP Client Error`, the [predefined messages](/src/core/Error.h#L141-L199) can be returned.


- ### Debug Information

The debug information (`String`) from the async result can be obtained from `String AsyncResult::debug()` which is currently availabele when starting and closing the server connection and information about network connection process.

### App Initialization

The Firebase app (`FirebaseApp`) is the class that used to handle the authentication task. 

The process of App intitialization:

- The `FirebaseApp` class accepts the user auth data (`user_auth_data`) which is the struct that holds the user sign-in credentials and tokens.

The user auth data that passes to the `FirebaseApp` class constructor can be obtained from the authentication/authorization classes via `getAuth` function.

The app token (`app_token_t`) that is the object that provided by the user auth data (`user_auth_data`), will provide the `auth tokens` that used in the authorization requests.

- User call `FirebaseApp::getApp<T>(<FirebaseServiceApp>)` to apply the auth data from app token to the Service apps before use.

The authentication/authorization classes also mentioned in the earlier section will be discussed here.

The following authentication/authorization classes generate and hold the `ID token` which used in the authorization requests.

- [UserAuth](examples/App/AppInitialization/UserAuth/UserAuth.ino)

- [CustomAuth](examples/App/AppInitialization/CustomAuth/CustomAuth.ino)

- [IDToken](examples/App/AppInitialization/TokenAuth/IDToken/IDToken.ino)

The following classes provide the privilege access to the Firebase/Google APIs.

 - [ServiceAuth](examples/App/AppInitialization/ServiceAuth/ServiceAuth.ino)
 
 - [AccessToken](examples/App/AppInitialization/TokenAuth/AccessToken/AccessToken.ino)

 - [LegacyToken](examples/App/AppInitialization/TokenAuth/LegacyToken/LegacyToken.ino)

 The following class provides for non-authentication acesss.
 
 - [NoAuth](examples/App/AppInitialization/NoAuth/NoAuth.ino)


The `getAuth` function is the function that provides the user auth data (`user_auth_data`) from the authentication/authorization classes.

> [!NOTE]  
> The user auth data that passes to `FirebaseApp` will be coppied to use internally, then changing the credentials in authentication/authorization classes after the `FirebaseApp` was initialized will not change the internal user auth data. You have to re-initiate the `FirebaseApp` again for apply the changes.

The user `UID` can be obtained from `FirebaseApp::getUid()`. Don't be confused with the task `UID` that described earlier.

The `auth token` (`ID token` and `access token`) and legacy token can be obtained from `FirebaseApp::getToken()`.

The refresh token (when using authentication/authorization classes that provided the `ID token`) can be obtained from `FirebaseApp::getRefreshToken()`.


- ### CustomAuth (ID Token Authorization Using Service Account)

The service account credentials and json file can be used for authentication. 

The [CustomAuth](examples/App/AppInitialization/CustomAuth/CustomAuth.ino) class parameters are included the following.

```cpp
CustomAuth::CustomAuth(<TimeStatusCallback>, <api_key>, <client_email>, <project_id>, <private_key>, <user_id>, <scope>, <claims>, <expire>)
```


```cpp
CustomAuth::CustomAuth(<TimeStatusCallback>, <file_config_data>, <api_key>, <user_id>, <scope>, <claims>, <expire>)
```

`<TimeStatusCallback>` The time status callback that provides the UNIX timestamp used to set the issue time and expiration time in the JWT token payload.

`<file_config_data>` The filesystem data (`file_config_data`) obtained from `FileConfig` class object of service account key file.

`<api_key>` The web API key of the project.

`<client_email>` The service account client Email.

`<project_id>` The service account project ID.

`<private_key>` The service account private key.

`<user_id>` The user ID.

`<scope>` The OAuth scopes.

`<claims>` The OAuth claims.

`<expire>`The expiry period in seconds (less than 3600), 3300 is the default value.

- ### ServiceAuth (OAuth2.0 Access Token Authorization Using Service Account)

This type of authentication is used when privilege (admin rights) access is needed.

The service account credentials and json file can be used for authentication. 

The [ServiceAuth](examples/App/AppInitialization/ServiceAuth/ServiceAuth.ino) class parameters are included the following.

```cpp
ServiceAuth::ServiceAuth(<TimeStatusCallback>, <client_email>, <project_id>, <private_key>, <expire>)
```

```cpp
ServiceAuth::ServiceAuth(<TimeStatusCallback>, <file_config_data>, <expire>)
```

`<TimeStatusCallback>` The time status callback that provides the UNIX timestamp used to set the issue time and expiration time in the JWT token payload.

`<file_config_data>` The filesystem data (`file_config_data`) obtained from `FileConfig` class object of service account key file.

`<client_email>` The service account client Email.

`<project_id>` The service account project ID.

`<private_key>` The service account private key.

`<expire>` The expiry period in seconds (less than 3600), 3300 is the default value.

> [!NOTE]  
> The refresh token is not available for OAuth 2.0 JWT Bearer Flow.

The auth token need to be re-created instead of refreshing.

- ### UserAuth (User Sign-In Authentication)

The user name and password credentials are used for authentication. You can save the credentials to file and load it with the constructor.

The [UserAuth](examples/App/AppInitialization/UserAuth/UserAuth.ino) class parameters are included the following.

```cpp
UserAuth::UserAuth(<api_key>, <user_email>, <user_password>, <expire>)
```

```cpp
UserAuth::UserAuth(<file_config_data>)
```

```cpp
UserAuth::save(<file_config_data>)
```

`<api_key>` The API key which can be obtained from Firebase console > Project Overview > Project settings.

`<user_email>` The Email of user in the project.

`<user_password>`The password of user in the project.

`<expire>`The expiry period in seconds (less than 3600), 3300 is the default value.

`<file_config_data>` The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `UserAuth` credentials will be saved to or read from.

- ### NoAuth (Non-Authentication)

The [NoAuth](examples/App/AppInitialization/NoAuth/NoAuth.ino) class allows you to access the Firebase/Google APIs without authorization token. There is no parameters in its class constructor.

This required read and write access in Firebase/Google APIs.

It should be used for testing only.

- ### CustomToken (ID Token Authorization Using Custom Token)

The API key and the custom token credentials are used for authorization. You can save the credentials to file and load it with the constructor.

The [CustomToken](examples/App/AppInitialization/TokenAuth/CustomToken/CustomToken.ino) class parameters are included the following.

```cpp
CustomToken::CustomToken(<api_key>, <custom_token>, <expire_in_seconds>)
```

```cpp
CustomToken::CustomToken(<file_config_data>)
```

```cpp
CustomToken::save(<file_config_data>)
```

`<api_key>` API key can be obtained from Firebase console > Project Overview > Project settings.

`<custom_token>` The custom token (jwt signed token).

`<expire_in_seconds>` Expiry period in seconds (less than 3600), 3300 is the default value.

`<file_config_data>` The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `CustomToken` credentials will be saved to or read from.

The expire period should be less than the remaining time to live of your signed `JWT` token.

> [!NOTE]  
> Only valid `RS256` signed `JWT` token can be used with Firebase Products and Google Cloud Product that provided in this library. 

Some Arduino boards that have built-in crypto chip e.g. [ATECC608A](https://www.microchip.com/en-us/product/atecc608a) do not support RSA-256 algorithm and its crypto library cannot use to create the signed JWT token used in this library.

You can pass the refresh token to the `<custom_token>` to refresh the token and use it last long. The refresh token is generally available from the ID token request.

The ID token is a short-lived token which will be expired in 1 hour.

- ### AccessToken (OAuth2.0 Access Token Authorization)

The access token was used for authorization. You can save the credentials to file and load it with the constructor.

The [AccessToken](examples/App/AppInitialization/TokenAuth/AccessToken/AccessToken.ino) class parameters are included the following.

```cpp
AccessToken::AccessToken(<access_token>, <expire_in_seconds>, <refresh_token>, <client_id>, <client_secret>)
```

```cpp
AccessToken::AccessToken(<file_config_data>)
```

```cpp
AccessToken::save(<file_config_data>)
```

`<access_token>` The access token.

`<expire_in_seconds>` Expire period in seconds

`<refresh_token>` Refresh token.

`<client_id>` Client ID.

`<client_secret>` Client secret.

`<file_config_data>` The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `AccessToken` credentials will be saved to or read from.

Normally `<refresh_token>` is not needed, if it is provided, the token will be refresh immediately when calling `FirebaseApp`'s  `initializeApp`.

The Client ID and Client Secret are OAuth 2.0 credentials that can be taken from the [developer console](https://console.developers.google.com/apis/credentials).

- ### IDToken (ID token Authorization)

The API key and ID token are used for authorization. You can save the credentials to file and load it with the constructor.

The [IDToken](examples/App/AppInitialization/TokenAuth/IDToken/IDToken.ino) class parameters are included the following.

```cpp
IDToken::IDToken(<api_key>, <ID_token>, <expire_in_seconds>, <refresh_token>)
```

```cpp
IDToken::IDToken(<file_config_data>)
```

```cpp
IDToken::save(<file_config_data>)
```

`<api_key>` API key can be obtained from Firebase console > Project Overview > Project settings.

`<ID_token>` The ID token.

`<expire_in_seconds>` Expire period in seconds.

`<refresh_token>` Refresh token.

`<file_config_data>` The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `IDToken` credentials will be saved to or read from.

- ### LegacyToken (Database Secret)

The database secret was used for authorization. You can save the credentials to file and load it with the constructor.

The [LegacyToken](examples/App/AppInitialization/TokenAuth/LegacyToken/LegacyToken.ino) class parameters is included the following.

```cpp
LegacyToken::LegacyToken(<database_secret>)
```

```cpp
LegacyToken::LegacyToken(<file_config_data>);
```

```cpp
LegacyToken::save(<file_config_data>);
```

`<database_secret>` The `Realtime Database` secret key.

`<file_config_data>` The filesystem data (`file_config_data`) obtained from `FileConfig` class object of file that the `LegacyToken` credentials will be saved to or read from.


- ### Working with Filesystems and BLOB

The file config class ([FileConfig](examples/App/AppInitialization/SaveAndLoad/)) will be used to hold the SD/Flash filesystems information and the file operation callback when file upload or download is required.

The function that requires file/BLOB for download and upload will accept the file config data (`file_config_data`) in its parameters.

The `file_config_data` can be obtained from the static functions called `getFile` and `getBlob`.

The `FileConfig` class parameters is included the following.

```cpp
FileConfig::FileConfig(<filename>, <file_callback>)
```

`<filename>` The full file name included its path.

The file name can be a name of source (input) file and target (output) file that used in upload and download.

`<file_callback>` The file callback required for file operations: read, write, append and remove.

The `<file_callback>` function parameters included the `File` reference returned from the file operation, filename and the `file_operating_mode` enum.

The `file_operating_mode` enums included the following.

- `file_mode_open_read`

- `file_mode_open_write`

- `file_mode_open_append`

- `file_mode_open_remove`

The following example shows how the `FileConfig` works with `SPIFFS` filesystem.

```cpp

#define MY_FS SPIFFS

void fileCallback(File &file, const char *filename, file_operating_mode mode)
{
    // FILE_OPEN_MODE_READ, FILE_OPEN_MODE_WRITE and FILE_OPEN_MODE_APPEND are defined in this library
    switch (mode)
    {
    case file_mode_open_read:
        file = MY_FS.open(filename, FILE_OPEN_MODE_READ);
        break;
    case file_mode_open_write:
        file = MY_FS.open(filename, FILE_OPEN_MODE_WRITE);
        break;
    case file_mode_open_append:
        file = MY_FS.open(filename, FILE_OPEN_MODE_APPEND);
        break;
    case file_mode_remove:
        MY_FS.remove(filename);
        break;
    default:
        break;
    }
}

FileConfig media_file("/media.mp4", fileCallback);

void download()
{
     storage.download(aClient, FirebaseStorage::Parent(STORAGE_BUCKET_ID, "media.mp4"), getFile(media_file), asyncCB);
}

```

The blob config class ([BlobConfig](examples/RealtimeDatabase/Extras/BLOB/BLOB.ino)) provides the in/out data for upload and download functions.

The `BlobConfig` class constructor parameters class parameters are included the following.

```cpp
BlobConfig::BlobConfig(<blob>, <blob_size>)
```

`<blob>` The BLOB data (byte array).

`<blob_size>` The size of data.

The data can be a source (input) for upload and target (output) for download.

The following example shows how the `BlobConfig` works.

```cpp
uint8_t source[2048];
uint8_t dest[2048];

BlobConfig upload_data(source, 2048);
BlobConfig download_data(dest, 2048);

void upload()
{
    Database.set(aClient, "/test/blob", getBlob(upload_data), asyncCB);
}

void download()
{
    Database.get(aClient, "/test/blob", getBlob(download_data), asyncCB);
}

```

When you don't use filesystems, you can exclude the related code in this library by removing the `ENABLE_FS` macro in [src/Config.h](/src/Config.h) or in your own defined config at [src/UserConfig.h](/src) or adding `DISABLE_FS` in the compiler build flags.


- ### Working with Networks

The `AsyncClientClass` object requires the network config data (`network_config_data`) that obtained from one of the following networking classes via the static function called `getNetwork`.

- [DefaultNetwork](examples/App/NetworkInterfaces/DefaultNetworks/DefaultNetwork/DefaultNetwork.ino) is used with the core WiFi enabled networking.

- [DefaultWiFiNetwork](examples/App/NetworkInterfaces/DefaultNetworks/DefaultWiFiNetwork/DefaultWiFiNetwork.ino) is used with the core WiFi Multi enabled networking or non-core WiFi networking.

- [DefaultEthernetNetwork](examples/App/NetworkInterfaces/DefaultNetworks/DefaultEthernetNetwork) is used with the core Ethernet enabled networking.

- [EthernetNetwork](examples/App/NetworkInterfaces/EthernetNetwork/EthernetNetwork.ino) is used with the non-core Ethernet networking.

- [GSMNetwork](examples/App/NetworkInterfaces/GSMNetwork/GSMNetwork.ino) is used with the non-core GSM networking.

- [GenericNetwork](examples/App/NetworkInterfaces/GenericNetwork/GenericNetwork.ino) is used with the non-core or user defined networking.

> [!WARNING]  
> In ESP32, [ADC2](https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html) (`GPIO 0`, `GPIO 2`, `GPIO 4`, `GPIO 12`, `GPIO 13`, `GPIO 14`, `GPIO 15`, `GPIO 25`, `GPIO 26` and `GPIO 27`) cannot be used while using WiFi.

The default network class can be used with WiFi capable MCUs e.g. ESP8266, ESP32 and Raspberry Pi Pico W.

The boolean parameter assigned with the default network class constructor is the option for how the network (WiFi) reconnection can be done automatically or manually.

The default WiFi network class provided the mean for connection with multiple WiFi credentials (WiFi Multi),

This default WiFi network class is suitable for device that has the built-in WiFi module (e.g. U-blox) or device that supports WiFi Multi in its core.

The default ethernet network class can be used with the Ethernet capable MCUs using core Ethernet drivers e.g. ESP8266 and ESP32 devices.

The other known networks (Ethernet and GSM) classes are also available.

The user defined or generic networks are supported by assigning the callbacks for network connection and the connection status in its class constructor.

- `DefaultNetwork`

The `DefaultNetwork` class parameters are included the following.

```cpp
DefaultNetwork::DefaultNetwork()
```

```cpp
DefaultNetwork::DefaultNetwork(<reconnect_option>)
```

`<reconnect_option>` The Boolean value set for enabling the WiFi reconnection when the WiFi is disconnected. It should be set with `false` when the WiFi reconnection was controlled by your code or third-party library e.g. WiFiManager.

When you define `DefaultNetwork` with no parameter, the WiFi reconnection will be enabled. 

> [!CAUTION]
> When the WiFi was manage for connection and reconnection by user code or third party library e.g. WiFiManager,
> you have to set the parameter, `<reconnect_option>` to false in the `DefaultNetwork` class constructor otherwise you are unable to  re-connect to the WiFi due to interferences operations.

> [!CAUTION]
> If you are using ESP32 device, and `<reconnect_option>` was enabled. The bugs in the outdated ESP32 Core WiFi will cause unknown network connection issue when library is trying to reconnect your WiFi using `WiFi.reconnect()`.
> You have to update the ESP32 Arduino Core SDK to the latest version or at least v2.x.x.

- `DefaultWiFiNetwork`

This `DefaultWiFiNetwork` class required some parameter for reconnection using WiFi credentials.

The class parameters are included the following.

```cpp
DefaultWiFiNetwork::DefaultWiFiNetwork(<FirebaseWiFi>, <reconnect_option>)
```

`<FirebaseWiFi>` The FirebaseWiFi class object that used for keeping the network credentials (WiFi APs and WiFi passwords).

`<reconnect_option>`  The bool option for network reconnection. It should set with `false` when the WiFi reconnection was controlled by your code or third-party library e.g. WiFiManager.

The `FirebaseWiFi` class holds the WiFi credentials list. The AP and password can be added to list with `addAP`.

The `FirebaseWiFi` object should be defined at the same usage scope of `DefaultWiFiNetwork` and `AsyncClientClass`.

The example for working with multiple WiFi APs.

```cpp
FirebaseWiFi wifimulti;

DefaultWiFiNetwork default_network(wifimulti, true /* reconnect network */);

void setup()
{
    wifimulti.addAP("wifi_ssid1", "password1");
    wifimulti.addAP("wifi_ssid2", "password2");
    wifimulti.addAP("wifi_ssid3", "password3");
}
```

- `DefaultEthernetNetwork`

The DefaultEthernetNetwork class parameters are included the following.

```cpp
DefaultEthernetNetwork::DefaultEthernetNetwork()
```

```cpp
DefaultEthernetNetwork::DefaultEthernetNetwork(<Firebase_SPI_ETH_Module>)
```

`<Firebase_SPI_ETH_Module>` The ESP8266 core SPI ethernet driver class that work with external SPI Ethernet modules that currently supported e.g. ENC28J60, Wiznet W5100 and Wiznet 5500. This `<Firebase_SPI_ETH_Module>` should be defined at the same usage scope of `AsyncClientCalss`.

 To use ESP8266 native lwIP Ethernet, the one of following macros, `ENABLE_ESP8266_ENC28J60_ETH`, `ENABLE_ESP8266_W5500_ETH` and `ENABLE_ESP8266_W5100_ETH` should be defined in [src/Config.h](/src/Config.h) or in your own defined config at [src/UserConfig.h](/src) or adding `ENABLE_ESP8266_ENC28J60_ETH`, `ENABLE_ESP8266_W5500_ETH` and `ENABLE_ESP8266_W5100_ETH` in the compiler build flags.

In `PlatformIO IDE`, please set the `lib_ldf_mode` option in `platformio.ini` as the following.

```ini
lib_ldf_mode = chain+
```

The following example shows how to use ENC28J60 Ethernet module with ESP8266
```cpp
#include <ENC28J60lwIP.h>

#define ETH_CS_PIN 16 // D0

ENC28J60lwIP eth(ETH_CS_PIN);

Firebase_SPI_ETH_Module spi_eth;

DefaultEthernetNetwork default_network(spi_eth);

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(default_network));

void setup()
{
      spi_eth.enc28j60 = &eth;
}
```

See [ESP8266 DefaultEthernetNetwork example](examples/App/NetworkInterfaces/DefaultNetworks/DefaultEthernetNetwork/ESP8266/ESP8266.ino) for using ESP8266 with its native lwIP Ethernet library.

For ESP32, to use the native ETH class, define the `DefaultEthernetNetwork` object with no parameter.

See [ESP32 DefaultEthernetNetwork example](examples/App/NetworkInterfaces/DefaultNetworks/DefaultEthernetNetwork/ESP32/ESP32.ino) for using ESP32 with its native Ethernet library.

- `EthernetNetwork`

By default the external Ethernet module can be used with the library when the macro `ENABLE_ETHERNET_NETWORK` was assigned and Ethernet library was included in the user sketch.

The user defined Ethernet class and header other than `Ethernet.h` and `Ethernet` can be used, see the [Library Build Options](#library-build-options) section.

The class parameters are included the following.

```cpp
EthernetNetwork::EthernetNetwork(<mac>, <cs_pin>, <reset_pin>, <Firebase_StaticIP>)
```
`<mac>` The mac address.

`<cs_pin>` The Ethernet module chip select/enable pin.

`<reset_pin>` The Ethernet module reset pin. Assign -1 if not used.

`<Firebase_StaticIP>` (Optional) The pointer to `Firebase_StaticIP` object that holds the static ip configuration.

The `Firebase_StaticIP` class constructor parameters for static IP are following.

```cpp
Firebase_StaticIP::Firebase_StaticIP(<local_ip>, <subnet>, <gateway>, <dns_server>, <optional>)
```

`<local_ip>` The static IP.

`<subnet>` The subnet IP.

`<gateway>` The default gateway IP.

`<dns_server>` The dns server IP.

`<optional>` The boolean option to force use static IP only (not use DHCP).

See [EthernetNetwork example](examples/App/NetworkInterfaces/EthernetNetwork/EthernetNetwork.ino)  for external Ethernet module usage.

- `GSMNetwork`

The `GSMNetwork` class can be used only with [TinyGSM](https://github.com/vshymanskyy/TinyGSM) library.

As [TinyGSM](https://github.com/vshymanskyy/TinyGSM) library requirement, one of GSM module macro should be defined in the sketch. 

For example, for SIM7600 module, the macro `TINY_GSM_MODEM_SIM7600` should be defined.

> [!IMPORTANT]  
> The GSM module macros e.g. `TINY_GSM_MODEM_SIM7600` should be defined in two locations.
> 1. In [src/Config.h](/src/Config.h) or in your own defined config at [src/UserConfig.h](/src) or adding `TINY_GSM_MODEM_SIM7600` in compiler build flags.
> 2. In your sketch.

The class parameters are included the following.

```cpp
GSMNetwork::GSMNetwork(<modem>, <gsm_pin>, <apn>, <user>, <password>)
```

`<modem>` The pointer to TinyGsm modem object. Modem should be initialized and/or set the mode before transfering the data.

`<gsm_pin>` The SIM pin.

`<apn>` The GPRS APN (Access Point Name).

`<user>` The GPRS user.

`<password>` The GPRS password.

The TinyGsm modem should be defined at the same usage scope of `GSMNetwork` and `AsyncClientClass`.

See [GSMNetwork example](examples/App/NetworkInterfaces/GSMNetwork/GSMNetwork.ino) for using [TinyGSM](https://github.com/vshymanskyy/TinyGSM) with this library.

- `GenericNetwork`

This type of network class works with all networking interfaces: Ethernet Module, WiFI Module, Bluetooth Module, NB-IoT module, and LoRa Module that have the internet access ability.

The class parameters are included the following.

```cpp
GenericNetwork::GenericNetwork(<net_connect_callback>, <network_status_callback>)
```

`<net_connect_callback>` The network connection callback function.

`<network_status_callback>` The network status callback function.

In the `<net_connect_callback>`, the complete operations for the carier (network) and internet connection should be performed and waits until the internet connection was established.

In the `<network_status_callback>` function, the `status` (Boolean variable) that provided in the function, should set with the network status.

See [GenericNetwork example](examples/App/NetworkInterfaces/GenericNetwork/GenericNetwork.ino) for using WiFi with `GenericNetwork` for demonstation.

- ### Required Operation Flows

When using this library, you should follow one of the following operation flows otherwise unexpected errors can be occurred.

![Operation Flows 1](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/operation_flows1.png)

![Operation Flows 2](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/operation_flows2.png)

This library does not run any background process in the FreeRTOS task or schedule task and timer ISR.

For maintaining the async tasks, you have to place the code for `Maintain Authentication and Async Operation Queue` in the infinite loop function e.g. main `loop()` function, timer or scheduler cyclically event's callback function or infinite loop in the FreeRTOS task (as in ESP32).

For ESP32's `FreeRTOS` task, the CPU Core 1 is recommend for safely operation. Although the async task is designed to run without blocking, the SSL/TLS handshake in SSL client is the blocking process then running the task in Core 0 may cause the wdt reset.

**Example** for `Maintain Authentication and Async Operation Queue` in ESP32's `FreeRTOS` task in lambda function usage style.

```cpp
void setup()
{
    /////////////////////////////////////
    // Network connection code here
    /////////////////////////////////////

    /////////////////////////////////////
    // Authentication code here
    /////////////////////////////////////

    auto loopTask = [](void *pvParameters)
    {
        for (;;)
        {
            /////////////////////////////////////
            // Maintain Authentication Queue
            /////////////////////////////////////
            app.loop();

            /////////////////////////////////////
            // Maintain App Async Queue
            /////////////////////////////////////
            Database.loop();

            if (app.ready())
            {
                // your other code here
                // For non-callback stream, you can check the information that provided from AsyncResult that assigned with the asyn get function with SSE mode (HTTP Streaming)
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    };

    xTaskCreatePinnedToCore(loopTask, "loopTask", 8000, NULL, 3, NULL, 1 /* must be core 1 for network task */);
}
```

- ### Basic Example


```cpp
#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_GIGA)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>) || defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#elif __has_include(<WiFiC3.h>) || defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif __has_include(<WiFi.h>)
#include <WiFi.h>
#endif

#include <FirebaseClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

void asyncCB(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (< 3600) */);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_C33) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

RealtimeDatabase Database;

unsigned long tmo = 0;

void setup()
{

    Serial.begin(115200);

    /////////////////////////////////////
    // Network connection
    /////////////////////////////////////
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    /////////////////////////////////////
    // Authentication
    /////////////////////////////////////

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(user_auth));

    
    /////////////////////////////////////
    // Waits for App authenticate
    /////////////////////////////////////
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;
    
    /////////////////////////////////////
    // Apply auth data to Service app
    /////////////////////////////////////
    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    /////////////////////////////////////
    // Call Service App
    /////////////////////////////////////
    if (app.ready())
    {
       // Set int
       Database.set<int>(aClient, "/test/int", 12345, asyncCB);
    }

}

void loop()
{
    /////////////////////////////////////
    // Maintain Authentication Queue
    /////////////////////////////////////
    app.loop();

    /////////////////////////////////////
    // Maintain App Async Queue
    /////////////////////////////////////
    Database.loop();

    /////////////////////////////////////
    // Call Service App
    /////////////////////////////////////
    if (app.ready() && millis() - tmo > 3000)
    {
      
       tmo = millis();
    
       // Get int
       Database.get(aClient, "/test/int", asyncCB);
    }

}

void asyncCB(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

```

The library provides the placeholder struct for boolean, integer, float with custom precision, double with custom precision and object represents JSON or map and Array to use as the input of the functions.

- `bool_t` is the placeholder class represents the `bool` value used in this libaray. The normal `bool` value is supported.

- `number_t` is the placeholder class represents the `int`, `float` and `double` values used in this libaray. The normal `int`, `float` and `double` values are supported unless the default decimal places will be used for `float` and `double` values.

- `string_t` is the placeholder class represents the string used in this libaray. The normal `String`, `const char*` are supported.

- `object_t` is the placeholder class represents the `JSON` object and `JSON Array` object used in this libaray.

There is no `JSON` serialization/deserialization class in this library unless the `JsonWriter` utility class to create the `JSON` object placeholder (`object_t`) which will be available for user and also used in the examples.


- ### Realtime Database Usage

    - [Examples](/examples/RealtimeDatabase).

    - [Class and Functions](/resources/docs/realtime_database.md).

    - [API Doc](https://firebase.google.com/docs/reference/rest/database).

- ### Google Cloud Firestore Database Usage

    - [Examples](/examples/FirestoreDatabase).

    - [Class and Functions](/resources/docs/firestore_database.md).

    - [API Doc](https://firebase.google.com/docs/firestore/reference/rest).

- ### Google Cloud Messaging Usage

    - [Examples](/examples/Messaging).

    - [Class and Functions](/resources/docs/messaging.md).

    - [API Doc](https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages).

- ### Firebase Storage Usage

    - [Examples](/examples/Storage).

    - [Class and Functions](/resources/docs/storage.md).

- ### Google Cloud Storage Usage

    - [Examples](/examples/CloudStorage).

    - [Class and Functions](/resources/docs/cloud_storage.md).

    - [API Doc](https://cloud.google.com/storage/docs/json_api/v1).

- ### Google Cloud Functions Usage

    - [Examples](/examples/CloudFunctions).

    - [Class and Functions](/resources/docs/cloud_functions.md).

    - [API Doc](https://cloud.google.com/functions/docs/reference/rest/v2/projects.locations.functions).

- ### The placeholder represents the primitive types values that used in this library 

    - [Class and Functions](/resources/docs/placeholders.md).

- ### JsonWriter

    - [Class and Functions](/resources/docs/json_writer.md).


> [!WARNING]
> This library included the `SSL Client` library called [`ESP_SSLClient`](https://github.com/mobizt/FirebaseClient/tree/main/src/client/SSLClient) to use in JWT token signing and the alternative use of the core SSL Client library e.g. `WiFIClientSecure` and `WiFiSSLClient` in some Arduino Client use cases which makes this library portable with no third-party library needed.
When this library was used together with my other library e.g. [ESP-Mail-Client](https://github.com/mobizt/ESP-Mail-Client) which comes with built-in `ESP_SSLClient` library, the Arduino IDE compilation error will be occurred.
You have to remove the folder `src/client/SSLClient` in the subsequent included library. 
For example if `ESP_Mail_Client.h` was included after `FirebaseClient.h`, the folder [`src/client/SSLClient`](https://github.com/mobizt/ESP-Mail-Client/tree/master/src/client/SSLClient) in the `ESP-Mail-Client` library installation folder should be removed.

The useful of using `ESP_SSLClient` library is it uses `PSRAM` by default (if it is available), you can use it in ESP32 and ESP8266 modules that have `PSRAM` or connected to external `PSRAM`.

For using `PSRAM`, see [Memory Options](#memory-options) section.


## Project Preparation and Setup

You have to setup the Firebase project to use `Firebase products`. Visit [Understand Firebase Projects](https://firebase.google.com/docs/projects/learn-more) to learn more about Firebase projects.

For new Firebase user, go to the [Google Firebase Console](https://console.firebase.google.com/), sign in to your Google account, and create a new project as the following images.

![Create a project](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/blank_account_create_a_project.png)

![Name a project](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/create_a_project_step1.png)

![Ignore the Google Analytics](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/create_a_project_step2.png)

Click the button `All products`.

![All products](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_all_products.png)

### Authentication Getting Started

When `custom token`, `ID token` authorization are used via `CustomAuth`, `UserAuth`, `CustomToken` and `IDToken`, the `Authentication` service is needed and must be set up otherwise the `HTTP status code 400` error will show in the async callback.

> [!NOTE]  
> The `Authentication` service is not required for `OAuth2.0 access token authentication using service account` authentication and legacy token (database secret) authorization.

To get started with `Authentication`,  choose `Authentication` and click `Get started` button.

![Authentication Get Started](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_authentication_get_started.png)

Under Sign-in providers, choose Email/Password.

![Authentication Select Email/Password Provider](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_authentication_get_started_step1.png)

Select enable check option in Email/Password section, and click `Save` button.

![Authentication Enable Email/Password](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_authentication_get_started_step2.png)

Then click at `Users` tab, click `Add user` button.

![Authentication Add User](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_authentication_get_started_step3.png)

Fill in the `Email` and `Password` and click `Add user` button.

Once the `Authentication` was setup, the `Web API Key` will be generated. See the `Project Settings` page for `Web API Key`. 

![Authentication Web API Key](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_authentication_get_started_step4.png)

The `Web API Key` is used in all `custom token`, `ID token` authentications that used via the `CustomAuth`, `UserAuth`, `CustomToken` and `IDToken` provider classes and it was assign to the `API_KEY` in the exampless.

At the `Authentication` page, under `Sign-in method` tab, other Sign-in providers can be added.

To add `Anonymous` sign-in provider, click at `Add new provider` button.

![Authentication Anonymous Provider](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_authentication_get_started_step5.png)

Select enable check option in Email/Password section, and click `Save` button.

![Authentication Enable Anonymous Provider](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_authentication_get_started_step6.png)

### Realtime Database Getting Started

To get started with `Firebase Realtime Database`, choose `Realtime Database` and click `Create Database`. 

![Create Database](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_rtdb_create_database.png)

Set up your `Database options` and `Security rules`.

![Set Database Location](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_rtdb_setup_database_step1.png)

![Set Database Security Rules](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_rtdb_setup_database_step2.png)

You can choose `Start in locked mode` or `Start in test mode` for `Security rules`.

Once the database was created, click on the `Rules` tab and change the `Security rules` as following to allow the basic authentication, click `Publish` button to apply the changes.

```yaml
{
  "rules": {
    ".read": "auth != null", 
    ".write": "auth != null"
  }
}
```
![Change Database Security Rules](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_rtdb_setup_database_step3.png)

The warning `Your security rules are not secure. Any authenticated user can steal, modify, or delete data in your database.` will be displayed due to insecure rules which you can change it for more secure later.

Visit [Understand Firebase Realtime Database Security Rules](https://firebase.google.com/docs/database/security) to learn more about security rules.

The reference url is the `DATABASE_URL` that defined and used in the `Realtime Database` examples can be obtained from the `Data` tab as the following.

![Realtime Database Reference URL](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_rtdb_reference_url.png)

The reference url or database url also can be taken from the `Service Account` key file, see [Service Account](#service-account) section.

#### Realtime Database Legacy Usage

The database secret is the secret key for privileged accessing the `Realtime Database` service.

The database secret is now currently deprecated. Alternatively, to use the `Realtime Database` with the same privileged access as database secret but secured, the `OAuth2.0 access token authentication using service account` via `ServiceAuth` provider class is recommended.  

To get the database secret, in the `Project Settings` page in the [`Google Firebase Console`](https://console.firebase.google.com/), under the `Service accounts` Tab click `Database secret`.

![Realtime Database Database Secret on Service Accounts Tab](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_rtdb_secret_key_step1.png)

The database secret for the databases are available in the `Secrets` column.

![Realtime Database Database Secret Table](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_rtdb_secret_key_step2.png)

### Google Cloud Firestore Database Getting Started

To get started with `Cloud Firestore Database`, choose `Cloud Firestore` and click `Create database`. 

![Create Firestore Database](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_firestore_create_database.png)

Set up your database `Name and Location` and `Security rules`.

> [!NOTE]  
> The first database name (ID) is `(default)` which cannot be changed.

![Set Name and Location](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_firestore_create_database_step1.png)

![Set Security Rules](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_firestore_create_database_step2.png)

Once the database was created, click on the `Rules` tab and change the `Security rules` as following to allow the basic authentication, click `Publish` button to apply the changes.

```yaml
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    match /{document=**} {
      allow read, write: if request.auth.uid != null;
    }
  }
}
```
![Change Security Rules](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_firestore_create_database_step3.png)

Visit [Get started with Cloud Firestore Security Rules](https://firebase.google.com/docs/firestore/security/get-started) to learn more about security rules.

### Storage Getting Started

To get started with `Storage`, choose `Storage` and click `Get started`. 

![Storage Get Started](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_storage_get_started.png)

Then set the `Secure Rules for Cloud Storage` and `Cloud Storage location`.

![Secure Rules for Cloud Storage](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_storage_get_started_step1.png)

![Set Cloud Storage location](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_storage_get_started_step2.png)

Once the storage bucket was created, click on the `Rules` tab and change the `Security rules` as following to allow the basic authentication, click `Publish` button to apply the changes.

```yaml
rules_version = '2';
service firebase.storage {
  match /b/{bucket}/o {
    match /{allPaths=**} {
      allow read, write: if request.auth.uid != null
    }
  }
}
```

![Change Security Rules](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_storage_get_started_step3.png)

Visit [Get started with Firebase Security Rules](https://firebase.google.com/docs/storage/security/get-started) to learn more about security rules.

The folder path is the `STORAGE_BUCKET_ID` that defined and used in the Storage examples can be obtained from the `Files` tab as the following.

![Storage folder path](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_storage_folder_path.png)

The folder path or the `STORAGE_BUCKET_ID` also can be taken from the `Service Account` key file, see [Service Account](#service-account) section.

### Google Cloud Functions Getting Started

To use the `Cloud Functions` for running your backend automate code, the billing plan for the project is needed to be upgraded to at least `Blaze` plan.

![Functions Get Started](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_functions_get_started.png)

![Functions Upgrade Billing Plan](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_functions_get_started_step1.png)

You can `Set a billing budget` at this step or skip it.

![Functions Upgrade Plan Purchase](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_functions_get_started_step2.png)

![Functions Upgrade Purchased](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_functions_get_started_step3.png)

Click the `Get started` button and follow the steps as show in the following images.

![Functions Install command line tools](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_functions_get_started_step4.png)

![Functions Finishing](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_functions_get_started_step5.png)

> [!IMPORTANT]  
> The `Cloud Build API` and `Cloud Run Admin API` must be enabled, click [here](https://console.developers.google.com/apis/library/cloudbuild.googleapis.com) to enable `Cloud Build API`and [here](https://console.cloud.google.com/apis/library/run.googleapis.com) to enable `Cloud Run Admin API`.


### Cloud Messaging Getting Started

![Cloud Messaging Get Started](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_messaging_get_started.png)

Create an app (iOS, Android, Web, or Unity app) for getting started.

The following steps showed the web app (Javascript) is created.

![Cloud Messaging Register App](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_messaging_get_started_step1.png)

Add the Firebase SDK and click `Continue to console` button.

![Cloud Messaging Add Firebase SDK](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/firebase_console_messaging_get_started_step2.png)

Follow the [Firebase Cloud Messaging Quickstart for Javascript](https://github.com/firebase/quickstart-js/tree/master/messaging) for the rest of steps.

> [!NOTE]  
> The script that provided in [Firebase Cloud Messaging Quickstart for Javascript](https://github.com/firebase/quickstart-js/tree/master/messaging) does not work at the present with the error `Failed to register a Service Worker`.

To workaround, change the script in `firebase-messaging-sw.js` as the following.

From

```js
importScripts('/__/firebase/init.js');
```

To

```js
firebase.initializeApp({
  apiKey: 'API_KEY', // Change this with your API Key
  authDomain: 'PROJECT_ID.firebaseapp.com', // Change this with your API Key
  databaseURL: 'https://PROJECT_ID.firebaseio.com', // Change this with your database url
  projectId: 'PROJECT_ID', // Change this with your Project ID
  storageBucket: 'PROJECT_ID.appspot.com', // Change this with your storage bucket
  messagingSenderId: 'SENDER_ID', // Change this with your messaging sender ID
  appId: 'APP_ID' // Change this with your app ID
});
```

The app (iOS, Android, Web and Unity) registration token or `DEVICE_TOKEN` is a unique token string that identifies each client app instance. The registration token is required for single device and device group messaging. Note that registration tokens must be kept secret ([ref](https://firebase.google.com/docs/cloud-messaging/concept-options#credentials))


### Service Account

A service account is a special kind of account (bot or application account) that belongs to app instead of to an individual end user. Service accounts enable server-to-server interactions between an app and a Google service.

#### Types of service accounts

In Google Cloud, there are several different types of service accounts:

- User-managed service accounts: Service accounts that you create and manage. These service accounts are often used as identities for workloads.

- Default service accounts: User-managed service accounts that are created automatically when you enable certain Google Cloud services e.g. `Firebase Admin SDK Service Account`. You are responsible for managing these service accounts.

- Google-managed service accounts: Google-created and Google-managed service accounts that allow services to access resources on your behalf.

A `Firebase Admin SDK Service Account` is created automatically when you create a Firebase project or add Firebase to a Google Cloud project and it is used to communicate with Firebase.

The `Service Account` credentials are required for `OAuth2.0 access token authentication using service account` and `ID token authorization using service account` via the `ServiceAuth` and `CustomAuth` provider classes.

The `Service Account` private key contains the `Service Account` credentials which used for initialize the Firebase mobile app.

To generate and download `Service Account` private key file, in the `Project Settings` page in the [`Google Firebase Console`](https://console.firebase.google.com/), click at `Service accounts` tab and `Generate new private key`. 

To use `Service Account` in your sketch, open the .json file that is already downloaded with the text editor.

```json
{
  "type": "service_account",
  "project_id": "...",
  "private_key_id": "...",
  "private_key": "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n",
  "client_email": "...",
  "client_id": "...",
  "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  "token_uri": "https://oauth2.googleapis.com/token",
  "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
  "client_x509_cert_url": "...",
  "universe_domain": "googleapis.com"
}
```

Copy the `project_id`, `client_email` and `private_key` from .json file and paste to these defines in the example.

```cpp
#define FIREBASE_PROJECT_ID "..." // Taken from "project_id" key in JSON file.
#define FIREBASE_CLIENT_EMAIL "..." // Taken from "client_email" key in JSON file.
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n"; // Taken from "private_key" key in JSON file.
```

Normally `Firebase Admin SDK Service Account` is used in Firebase APIs authentication/authorization.

To access `Google Cloud APIs` e.g. `Google Cloud Storage`, `Google Cloud Functions` services, you have to update the service account permissions.

#### Service Account Permissions

When the `Firebase Admin SDK Service Account` was used for Google Cloud APIs authentication/authorization, you have to update its permissive roles.

The `Basic roles` are highly permissive roles that used to grant principals broad access to Google Cloud resources.

We can add one of `Basic roles` i.e. `Editor` and/or `Owner`.

To add the Basic's `Owner` or `Editor` roles, go to the [Identity and Access Management console](https://console.cloud.google.com/iam-admin).

Then choose the project, and select the `VIEW BY PRINCIPALS` tab.

From the table displayed, look at the `firebase-adminsdk` in the `Name` column, then click on the pencil icon on the right side in this table row to `Edit principal`. 

![IAM Add Permission](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/iam_add_permission_step1.png)

Add the role `Editor` and/or `Owner` under the `Basic` roles and click `Save` button.

Wait a few minutes for the action to propagate after adding roles. 

![IAM Add Roles](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/iam_add_permission_step2.png)


## Memory Options

- ### Memory Options for ESP8266

When you update the ESP8266 Arduino Core SDK to v3.x.x, the memory for Heap and stack can be choosen from the IDE.

You can choose the Heap memory between internal and external memory chip from IDE e.g. Arduino IDE and PlatformIO on VSCode or Atom IDE.

- #### Arduino IDE


For ESP8266 devices that don't have external SRAM/PSRAM chip installed, choose the MMU **option 3**, 16KB cache + 48KB IRAM and 2nd Heap (shared).

![Arduino IDE config](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/esp8266_heap_option_arduino_ide.png)

For ESP8266 devices that have external 23LC1024 SRAM chip installed, choose the MMU **option 5**, 128K External 23LC1024.

![MMU VM 128K](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/esp8266_psram_option_arduino_ide.png)

For ESP8266 devices that have external ESP-PSRAM64 chip installed, choose the MMU **option 6**, 1M External 64 MBit PSRAM.


- #### PlatformIO IDE

The MMU options can be selected from build_flags in your project's platformio.ini file

For ESP8266 devices that don't not have external SRAM/PSRAM chip installed, add build flag as below.

```ini
[env:d1_mini]
platform = espressif8266
build_flags = -D PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED
board = d1_mini
framework = arduino
monitor_speed = 115200
```


For ESP8266 devices that have external 23LC1024 SRAM chip installed, add build flag as below.

```ini
[env:d1_mini]
platform = espressif8266
;128K External 23LC1024
build_flags = -D PIO_FRAMEWORK_ARDUINO_MMU_EXTERNAL_128K
board = d1_mini
framework = arduino
monitor_speed = 115200
```


For ESP8266 devices that have external ESP-PSRAM64 chip installed, add build flag as below.

```ini
[env:d1_mini]
platform = espressif8266
;1M External 64 MBit PSRAM
build_flags = -D PIO_FRAMEWORK_ARDUINO_MMU_EXTERNAL_1024K
board = d1_mini
framework = arduino
monitor_speed = 115200
```


- #### ESP8266 and SRAM/PSRAM Chip connection

Most ESP8266 modules don't have the built-in SRAM/PSRAM on board. External memory chip connection can be done via SPI port as below.

```
23LC1024/ESP-PSRAM64                ESP8266

CS (Pin 1)                          GPIO15
SCK (Pin 6)                         GPIO14
MOSI (Pin 5)                        GPIO13
MISO (Pin 2)                        GPIO12
/HOLD (Pin 7 on 23LC1024 only)      3V3
Vcc (Pin 8)                         3V3
Vcc (Pin 4)                         GND
```

Once the external Heap memory was selected in IDE, to allow the library to use the external memory, you can set it in [**Config.h**](src/Config.h) by define this macro.


```cpp
#define ENABLE_PSRAM
```

This macro was defined by default when you installed or update the library.



- ### Memory Options for ESP32

In ESP32 module that has PSRAM installed, you can enable it and set the library to use this external memory instead.

- #### Arduino IDE

To enable PSRAM in ESP32 module.

![Enable PSRAM in ESP32](https://raw.githubusercontent.com/mobizt/FirebaseClient/main/resources/images/esp32_psram_option_arduino_ide.png)


- #### PlatformIO IDE


In PlatformIO on VSCode or Atom IDE, add the following build_flags in your project's platformio.ini file.

```ini
build_flags = -DBOARD_HAS_PSRAM -mfix-esp32-psram-cache-issue
```

As in ESP8266, once the external Heap memory was enabled in IDE, to allow the library to use the external memory, you can set it in [**Config.h**](src/Config.h) by define this macro.

```cpp
#define ENABLE_PSRAM
```


## Library Build Options 

The library build options are defined as preprocessor macros (`#define name`).

Some options can be disabled.

- ### Predefined Options

The predefined options that are already set in [**Config.h**](src/Config.h) are following.

```cpp
ENABLE_DATABASE // For RTDB compilation
ENABLE_FIRESTORE // For Firestore compilation
ENABLE_FIRESTORE_QUERY // For Firestore Query feature compilation
ENABLE_MESSAGING // For Firebase Cloud Messaging compilation
ENABLE_STORAGE // For Firebase Storage compilation
ENABLE_CLOUD_STORAGE // For Google Cloud Storage compilation
ENABLE_FUNCTIONS // For Google Cloud Functions compilation
ENABLE_PSRAM // For enabling PSRAM support
ENABLE_OTA // For enabling OTA updates support via RTDB, Firebase Storage and Google Cloud Storage buckets
ENABLE_FS // For enabling Flash filesystem support

// For enabling authentication and token
ENABLE_SERVICE_AUTH
ENABLE_CUSTOM_AUTH
ENABLE_USER_AUTH
ENABLE_ACCESS_TOKEN
ENABLE_CUSTOM_TOKEN
ENABLE_ID_TOKEN
ENABLE_LEGACY_TOKEN

// For enabling non-sdk networking
ENABLE_ETHERNET_NETWORK
ENABLE_GSM_NETWORK
```

- ### Optional Options

The following options are not yet defined in [**Config.h**](src/Config.h) and can be assigned by user.

```cpp
FIREBASE_ETHERNET_MODULE_LIB `"EthernetLibrary.h"` // For the Ethernet library to work with external Ethernet module
FIREBASE_ETHERNET_MODULE_CLASS EthernetClass // For the Ethernet class object of Ethernet library to work with external Ethernet module
FIREBASE_ETHERNET_MODULE_TIMEOUT 2000 // For the time out in milliseconds to wait external Ethernet module to connect to network
ENABLE_ESP8266_ENC28J60_ETH //  For native core library ENC28J60 Ethernet module support in ESP8266
ENABLE_ESP8266_W5500_ETH // For native core library W5500 Ethernet module support in ESP8266
ENABLE_ESP8266_W5100_ETH // For native core library W5100 Ethernet module support in ESP8266
FIREBASE_DISABLE_ONBOARD_WIFI // For disabling on-board WiFI functionality in case external Client usage
FIREBASE_DISABLE_NATIVE_ETHERNET // For disabling native (sdk) Ethernet functionality in case external Client usage
ENABLE_ASYNC_TCP_CLIENT // For Async TCP Client usage
FIREBASE_ASYNC_QUEUE_LIMIT // For maximum async queue limit setting for an async client
FIREBASE_DEFAULT_DEBUG_PORT // For Firebase.printf debug port
```

You can assign the optional build options using one of the following methods.

- By creating user config file `UserConfig.h` in library installed folder and define these optional options.

- By adding compiler build flags with `-D name`.

In PlatformIO IDE, using `build_flags` in PlatformIO IDE's platformio.ini is more convenient 

```ini
build_flags = -D DISABLE_STORAGE
              -D FIREBASE_DISABLE_ONBOARD_WIFI
```

For external Ethernet module integation used with function `setEthernetClient`, both `FIREBASE_ETHERNET_MODULE_LIB` and `FIREBASE_ETHERNET_MODULE_CLASS` should be defined.

`FIREBASE_ETHERNET_MODULE_LIB` is the Ethernet library name with extension (.h) and should be inside `""` or `<>` e.g. `"Ethernet.h"`.

`FIREBASE_ETHERNET_MODULE_CLASS` is the name of static object defined from class e.g. `Ethernet`.

`FIREBASE_ETHERNET_MODULE_TIMEOUT` is the time out in milliseconds to wait network connection.

For disabling predefined options instead of editing the [**Config.h**](src/Config.h) or using `#undef` in `UserConfig.h`, you can define these build flags with these names or macros in `UserConfig.h`.

```cpp
DISABLE_DATABASE // For disabling RTDB support
DISABLE_FIRESTORE // For disabling Firestore support
DISABLE_FIRESTORE_QUERY // For Firestore Query feature compilation
DISABLE_MESSAGING // For disabling Firebase Cloud Messaging support
DISABLE_STORAGE // For disabling Firebase Storage support
ENABLE_CLOUD_STORAGE // For disabling Google Cloud Storage support
DISABLE_FUNCTIONS // For disabling Google Cloud Functions support
DISABLE_PSRAM // For disabling PSRAM support
DISABLE_OTA // For disabling OTA updates support
DISABLE_FS // For disabling filesystem support

// For disabling authentication and token
DISABLE_SERVICE_AUTH
DISABLE_CUSTOM_AUTH
DISABLE_USER_AUTH
DISABLE_ACCESS_TOKEN
DISABLE_CUSTOM_TOKEN
DISABLE_ID_TOKEN
DISABLE_LEGACY_TOKEN

FIREBASE_DISABLE_ALL_OPTIONS // For disabling all predefined build options above
```

> [!NOTE]  
> `UserConfig.h` for user config should be placed in the library installed folder inside the `src` folder.

This `UserConfig.h` will not change or overwrite when update the library.

## Frequently Asked Questions

For the FAQ (Frequently Asked Questions), please visit [here](/FAQ.md).

## License

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
