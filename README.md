# FirebaseClient
Async Firebase Client library for Arduino.

This library supports Firebase Realtime database, Cloud Firestore database, Cloud Messaging, Firebase Storage, Google Cloud Storage and Google Cloud Functions.

The features can be configurable to add and exclude some unused features, see [Library Build Options](#library-build-options).


This library is the new Firebase Client library that supports both sync and async usages.

This library is [Firebase-ESP-Client](https://github.com/mobizt/Firebase-ESP-Client) library replacement.

## Contents

[1. Features](#features)

[2. Supported Devices](#supported-devices)

[3. Dependencies](#dependencies)

[4. Incompatability](#incompatability)

[5. Installation](#installation)

- [Using Library Manager](#using-library-manager)

- [Manual installation](#manual-installation)

- [RP2040 Arduino SDK installation](#rp2040-arduino-sdk-installation)

[6. Usages](#usages)

- [Indroduction](#indroduction)

- [Aurhentication](#authentication)

- [Firebase and Google Services](#firebase-and-google-services)

- [Working with filesystems and BLOB](#working-with-filesystems-and-blob)

- [Required Operation flows](#required-operation-flows)

- [Async Client](#async-client)

- [Networking Data](#networking-data)

- [Authentication and Network Classes](#authentication-and-network-classes)

- [File config and Blob config Classes](#file-config-and-blob-config-classes)

- [Firebase and Google Services Classes](#firebase-and-google-services)

- [Basic Example](#basic-example)

- [Realtime Database Usage](#realtime-database-usage)

- [Google Cloud Firestore Database Usage](#google-cloud-firestore-database-usage)

- [Google Cloud Messaging Usage](#google-cloud-messaging-usage)

- [Firebase Storage Usage](#firebase-storage-usage)

- [Google Cloud Storage Usage](#google-cloud-storage-usage)

- [Google Cloud Functions Usage](#google-cloud-functions-usage)

[7. Memory Options](#memory-options)

- [Memory Options for ESP8266](#memory-options-for-esp8266)

- [Arduino IDE](#arduino-ide)

- [PlatformIO IDE](#platformio-ide)

- [ESP8266 and SRAM/PSRAM Chip connection](#esp8266-and-srampsram-chip-connection)

- [Memory Options for ESP32](#memory-options-for-esp32)

- [Arduino IDE](#arduino-ide-1)

- [PlatformIO IDE](#platformio-ide-1)

[8. Library Build Options](#library-build-options)

- [Predefined Options](#predefined-options)

- [Optional Options](#optional-options)

[9. License](#license)

## Features

* Simple APIs.

* Supports synchronous and asynchronous operations.

* Supports most Arduino devices (except for AVR) with or without external neywork module.

* Supports most Firebase Services included Google Cloud Storage.

* Supports Firmware OTA updates.

* Supports user management.

* Faster and more reliable.

## Supported Devices.

 * ESP8266 MCUs based boards
 * ESP32 MCUs based boards
 * Arduino MKR WiFi 1010
 * Arduino MKR 1000 WIFI
 * Arduino Nano 33 IoT
 * Arduino MKR Vidor 4000
 * Raspberry Pi Pico (RP2040)
 * LAN8720 Ethernet PHY
 * TLK110 Ethernet PHY
 * IP101 Ethernet PHY
 * ENC28J60 SPI Ethernet module
 * W5100 SPI Ethernet module
 * W5500 SPI Ethernet module
 * SIMCom Modules with TinyGSMClient

 Note that this library does not support Arduino® UNO R4 WiFi due to the memory issue (related to stack and String class memory issue).


 ## Dependencies


This library required **Platform's Core SDK** to be installed.

For Arduino IDE, ESP8266 Core SDK can be installed through **Boards Manager**. 

For PlatfoemIO IDE, ESP8266 Core SDK can be installed through **PIO Home** > **Platforms** > **Espressif 8266 or Espressif 32**.

The RP2040 boards required Arduino-Pico SDK from Earle F. Philhower https://github.com/earlephilhower/arduino-pico


## Incompatability

This library APIs are not compattible with Firebase-ESP8266, Firebase-ESP32 and Firebase-ESP-Clientlibraries.

If you are using those libraries, the code should be rewritten.



## Installation


### Using Library Manager

At Arduino IDE, go to menu **Sketch** -> **Include Library** -> **Manage Libraries...**

In Library Manager Window, search **"firebase"** in the search form then select **"FirebaseClient"**. 

Click **"Install"** button.



For PlatformIO IDE, using the following command.

**pio lib install "FirebaseClient""**

Or at **PIO Home** -> **Library** -> **Registry** then search **FirebaseClient**.


### Manual installation

For Arduino IDE, download zip file from the repository (Github page) by select **Code** dropdown at the top of repository, select **Download ZIP** 

From Arduino IDE, select menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**.

Choose **FirebaseClient-main.zip** that previously downloaded.

Rename folder from **FirebaseClient-main** to **FirebaseClient**.

Go to menu **Files** -> **Examples** -> **FirebaseClient** and choose one from examples.


### RP2040 Arduino SDK installation

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

### Indroduction

There are classes or objects that used for required operations and data in this library e.g. authentication and auth data, netorking data, Firebase service apps and Firebase service apps data.

### Authentication

The authentication classes provide the custom token, access token, ID token authentications using credentials or tokens from other sources.

The ID token authentication required the Email/Password Sign-in provider to be enabled and the API key.

The users tab will show the users that can sign in.

![Email/Password Sign-in provider](/resources/images/user_auth_console.png)

 Copy the `Web API Key` in the console as shown in the picture below to use as API key in your code.

![API Key](/resources/images/api_key_console.png)

For the service account key file, chose the button `Generate new private key` in the console as shown in the picture below.

The database secret used in Realtime database can be optained from the same Service accounts tab, and database secret section.

![Service Account Key](/resources/images/sa_key_console.png)

Some Google Services, the sucurity rules can be used to control your authentication for more secure usage.

For the Realtime database security rules, see [this link](https://firebase.google.com/docs/database/security).

For Cloud Firestore database security rules, see [this link](https://firebase.google.com/docs/firestore/security/get-started).

Non-authentication (for testing only) and user management classes and functions also available.

### Firebase and Google Services

The Firebase and Google services Classes that are available are Realtime database, Cloud Firestore database, Cloud Messaging, Firebase Storage, Cloud Functions and Google Cloud Storage classes.

This library used internal millis timer to handle the token time to live. Then device time setting is not requierd in most authentication types.

In access and custom token authentications using service accout file (sa and custom auths), it related to JWT token creation and the token signing using RSA private key.  This process require the valid timestamp, then the time status setting callback will be required in the sa and custom auth class constructor.

### Working with filesystems and BLOB

The file config class will be used to hold the SD/Flash file and the file operation callback when file upload or download is required.

The blob config class will provide the in/out data payload.

When filesystems are not used, remove ENABLE_FS macro or adding DISABLE_FS in compiler flags.

### Required Operation flows

User have to follow the following operation flows otherwise unknown errors can be occurred.

`Network Connection -> Authentication (app intitialize included JWT token creation) -> Waits for App authenticate -> Apply auth data to Service app -> Call Service App (url may be required) -> Maintain Authentication and Async Operation Queue`

Or

`Network Connection -> Authentication (app intitialize included JWT token creation) -> Apply auth data to Service app -> Waits for App authenticate -> Call Service App (url may be required) -> Maintain Authentication and Async Operation Queue`


### Async Client

The authentication and Firebase service apps taks require the async client for server connection.

The async client holds the SSL client pointer that provided in its class constructor and used for server connection.

The async client also create the instances of async data and stored as slot data in a FIFO queue.

The async data created in the async client will hold the asyn http request header and payload, http response header and payload to be processed.

The authentication task has highest priority and its async data will be inserted at the first slot of async queue.

The order of lower priority slot data are sync task, async task and SSE task respectively.

Then when user uses async client for multiple tasks included Reltime database stream, sync and async operations, the stream will be interrupt (breaking the connection) because of the async client only connect to server via one TCP socket (Arduino Clinet) at a time.

The async client that used for authentication task and async task included SSE stream, need to be defined globally otherwise the dangling pointer problem will be occured.

The SSL Client is a kind of sync or blocking Client that takes time during establishing the SSL server connection (SSL handshake).

The async SSL client can be assigned to the async client class constructor but currently experimental.

### Networking Data

The network class object that holds the network interface data need to be assigned to the async client constructor.

The default network class can be used for WiFi capable MCUs e.g. ESP8266, ESP32 and Raspberry Pi Pico W.

The network (WiFI) reconnection or resume can be done automatically or manually by user can be configurable via the boolean parameter assigned with default network class constructor.

The default WiFi network class provided mean for multiple WiFi's ssid and passworrd connections (WiFi Multi),

This default WiFi network class is suitable for device that provided the built-in WiFi module for non-WiFI capable MCU that the WiFi reconnection requires the ssid and password and device that support WiFi Multi in its core.

The default ethernet network class can be used for Ethernet capable MCUs using core Ethernet drivers e.g. ESP8266 and ESP32.

The other known networks (Ethernet and GSM) class are also available.

The user defined or generic networks are supported by assigning the ntwork connection and status callacks in its class constructor.

### Authentication and Network Classes

Here is the list of the authentication and network classes that described above.

[FirebaseApp](examples/App/AppInitialization) is for authentication.

[CustomAuth](examples/App/AppInitialization/CustomAuth/CustomAuth.ino) is for the custom token authentication using the service account credentials.

[ServiceAuth](examples/App/AppInitialization/ServiceAuth/ServiceAuth.ino) is for the access token authentication using the service account credentials.

[UserAuth](examples/App/AppInitialization/UserAuth/UserAuth.ino) is for ID token authentication using email and password.

[NoAuth](examples/App/AppInitialization/NoAuth/NoAuth.ino) is for non-authentication.

[CustomToken](examples/App/AppInitialization/TokenAuth/CustomToken/CustomToken.ino) is for custom token authentication using user provided custom token.

[AccessToken](examples/App/AppInitialization/TokenAuth/AccessToken/AccessToken.ino) is for access token authentication using user provided access token.

[IDToken](examples/App/AppInitialization/TokenAuth/IDToken/IDToken.ino) is for ID token authentication using user provide ID token.

[LegacyToken](examples/App/AppInitialization/TokenAuth/LegacyToken/LegacyToken.ino) is for the secret key or known as database secret for accessing the Realtime database only. 

[UserAccount](examples/App/UserManagement/) is for user management e.g. [create user](examples/App/UserManagement/SignUp/SignUp.ino), [delete user](examples/App/UserManagement/DeleteUser/DeleteUser.ino), [verify](examples/App/UserManagement/Verify/Verify.ino), [reset password](examples/App/UserManagement/ResetPassword/ResetPassword.ino) and [anonymous sign-in](examples/App/UserManagement/Anonymous/Anonymous.ino).

[DefaultNetwork](examples/App/NetworkInterfaces/DefaultNetworks/DefaultNetwork/DefaultNetwork.ino) is for core WiFi enabled networking.

[DefaultEthernetNetwork](examples/App/NetworkInterfaces/DefaultNetworks/DefaultEthernetNetwork) is for core Ethernet enabled networking.

[DefaultWiFiNetwork](examples/App/NetworkInterfaces/DefaultNetworks/DefaultWiFiNetwork/DefaultWiFiNetwork.ino) is for core WiFi Multi enabled networking or non-core WiFi netowking.

[EthernetNetwork](examples/App/NetworkInterfaces/EthernetNetwork/EthernetNetwork.ino) is for non-core Ethernet networking.

[GSMNetwork](examples/App/NetworkInterfaces/GSMNetwork/GSMNetwork.ino) is for non-core GSM networking.

[GenericNetwork](examples/App/NetworkInterfaces/GenericNetwork/GenericNetwork.ino) is for non-core or user defined networking.

[AsyncClientClass](#) is for aync and sync operations that holds the TCP session, async data queue of http request and response.

[AsyncTCPConfig](examples/AsyncTCP/AsyncTCP.ino) is for async SSL client data.

### File config and Blob config Classes

[FileConfig](examples/App/AppInitialization/SaveAndLoad/) is for aync and sync operations that holds the TCP session, async data queue of http request and response.

[BlobConfig](examples/RealtimeDatabase/Extras/BLOB/BLOB.ino) is for aync and sync operations that holds the TCP session, async data queue of http request and response.


### Firebase and Google Services classes


[RealtimeDatabase](examples/RealtimeDatabase/) is for Realtime database operation.

[Firestore::Databases](examples/FirestoreDatabase/Databases/) is for Cloud Firestore databases operation.

[Firestore::Documents](examples/FirestoreDatabase/Documents/) is for Cloud Firestore documents operation.

[Firestore::CollectionGroups::Indexes](examples/FirestoreDatabase/CollectionGroups/Indexes/) is for Cloud Firestore CollectionGroups's Indexes operation.

[Messaging](examples/Messaging/) is for Cloud Messaging operation.

[Storage](examples/Storage/) is for Firebase Storage operation.

[CloudStorage](examples/CloudStorage/) is for Google Cloud Storage operation.

[CloudFunctions](examples/CloudFunctions/) is for Google Cloud Functions operation.


### Basic Example


```cpp
#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
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

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

#if __has_include(<WiFiClientSecure.h>)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif __has_include(<WiFiSSLClient.h>)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

RealtimeDatabase Database;

unsigned long to = 0;

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

#if __has_include(<WiFiClientSecure.h>)
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
    if (app.ready() && millis() - to > 3000)
    {
       // Get int
       Database.get(aClient, "/test/int", asyncCB);
    }

}

void asyncCB(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
    {
        Firebase.printf("Event msg: %s, code: %d\n", aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug msg: %s\n", aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error msg: %s, code: %d\n", aResult.error().message().c_str(), aResult.error().code());
    }
}

```
As the library is the Firebase (REST API) Client, but it also provides the extended functions to use in OTA update, filesystem download and upload as in the old library with cleaner and easy to read API and functions.

The request payload and URL query parameters are supported as class object with the same names and types that represent the inputs and qury parameters that are available from Google API documentation.

The function name or method is the same or identical to the Google API documentation unlesss some function names cannot be used due to prohibit keyword in C/C++, e.g. delete, visibility.

The library provides the placeholder struct for boolean, integer, float with custom precision, double with custom precision and object represents JSON or map and Array for input.

The response payload returning result (output) is straightforward as String.

It should be note that, the async result in the async callback can be lived only inside the callback and it decontructed when async operation is complete. To take the data out from the async result callback, the static or glocal variable should be used to copy the data.

If the size of payload string in async reseut is large, to copy the char array buffer directly, use `aResult.payload().c_str()` instead.

There is no JSON serialization/deserialization utilized or provided in this library.

## Realtime Database Usage

- [Examples](/examples/RealtimeDatabase).

- [Class and Functions](/resources/docs/realtime_database.md).

- [API Doc](https://firebase.google.com/docs/reference/rest/database).

## Google Cloud Firestore Database Usage

- [Examples](/examples/FirestoreDatabase).

- [Class and Functions](/resources/docs/firestore_database.md).

- [API Doc](https://firebase.google.com/docs/firestore/reference/rest).

## Google Cloud Messaging Usage

- [Examples](/examples/Messaging).

- [Class and Functions](/resources/docs/messaging.md).

- [API Doc](https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages).


## Firebase Storage Usage

- [Examples](/examples/Storage).

- [Class and Functions](/resources/docs/storage.md).

## Google Cloud Storage Usage

- [Examples](/examples/CloudStorage).

- [Class and Functions](/resources/docs/cloud_storage.md).

- [API Doc](https://cloud.google.com/storage/docs/json_api/v1).

## Google Cloud Functions Usage

- [Examples](/examples/CloudFunctions).

- [Class and Functions](/resources/docs/cloud_functions.md).

- [API Doc](https://cloud.google.com/functions/docs/reference/rest/v2/projects.locations.functions).


See [all examples](/examples) for complete usages.

See [function description](/src/README.md) for all available functions.



## Memory Options

### Memory Options for ESP8266

When you update the ESP8266 Arduino Core SDK to v3.x.x, the memory for Heap and stack can be configurable from IDE.

You can choose the Heap memory between internal and external memory chip from IDE e.g. Arduino IDE and PlatformIO on VSCode or Atom IDE.

#### Arduino IDE


For ESP8266 devices that don't have external SRAM/PSRAM chip installed, choose the MMU **option 3**, 16KB cache + 48KB IRAM and 2nd Heap (shared).

![Arduino IDE config](/resources/images/esp8266_heap_option_arduino_ide.png)

For ESP8266 devices that have external 23LC1024 SRAM chip installed, choose the MMU **option 5**, 128K External 23LC1024.

![MMU VM 128K](/resources/images/esp8266_psram_option_arduino_ide.png)

For ESP8266 devices that have external ESP-PSRAM64 chip installed, choose the MMU **option 6**, 1M External 64 MBit PSRAM.


#### PlatformIO IDE

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


#### ESP8266 and SRAM/PSRAM Chip connection

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



### Memory Options for ESP32

In ESP32 module that has PSRAM installed, you can enable it and set the library to use this external memory instead.

#### Arduino IDE

To enable PSRAM in ESP32 module.

![Enable PSRAM in ESP32](/resources/images/esp32_psram_option_arduino_ide.png)


#### PlatformIO IDE


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

### Predefined Options

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

### Optional Options

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
build_flags = -D DISABLE_FB_STORAGE
              -D EFIREBASE_DISABLE_ONBOARD_WIFI
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

Note that, `UserConfig.h` for user config should be placed in the library install folder inside src folder.

This `UserConfig.h` will not change or overwrite when update the library.



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
