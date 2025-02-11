#if !defined(FIREBASE_CLIENT_CONFIG_H) && !defined(FIREBASE_CONFIG_H)
#define FIREBASE_CLIENT_CONFIG_H
#define FIREBASE_CONFIG_H

/** 📌 Predefined Build Options
 * ⛔ Use following build flag to disable all predefined options.
 * -D FIREBASE_DISABLE_ALL_OPTIONS
 */

/**📍 For RTDB compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_DATABASE
 */
#define ENABLE_DATABASE

/**📍 For Firestore compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_FIRESTORE
 */
#define ENABLE_FIRESTORE

/**📍 For Firestore Query feature compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_FIRESTORE_QUERY
 */
#define ENABLE_FIRESTORE_QUERY

/**📍 For Firebase Cloud Messaging compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_MESSAGING
 */
#define ENABLE_MESSAGING

/**📍 For Firebase Storage compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_STORAGE
 */
#define ENABLE_STORAGE

/**📍 For Google Cloud Storage compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_CLOUD_STORAGE
 */
#define ENABLE_CLOUD_STORAGE

/**📍 For Google Cloud Functions compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_FUNCTIONS
 */
#define ENABLE_FUNCTIONS

/**📍 For enabling PSRAM support
 * ⛔ Use following build flag to disable.
 * -D DISABLE_PSRAM
 */
#define ENABLE_PSRAM

/**📍 For enabling OTA updates support via RTDB, Firebase Storage and Google Cloud Storage buckets
 * ⛔ Use following build flag to disable.
 * -D DISABLE_OTA
 */
#define ENABLE_OTA

/**📍 For enabling filesystem
 * ⛔ Use following build flag to disable.
 * -D DISABLE_FS
 */
#define ENABLE_FS

/**📍 For enabling authentication and token
 * ⛔ Use following build flag to disable.
 * -D DISABLE_SERVICE_AUTH
 * -D DISABLE_CUSTOM_AUTH
 * -D DISABLE_USER_AUTH
 * -D DISABLE_ACCESS_TOKEN
 * -D DISABLE_CUSTOM_TOKEN
 * -D DISABLE_ID_TOKEN
 * -D DISABLE_LEGACY_TOKEN
 */
#define ENABLE_SERVICE_AUTH
#define ENABLE_CUSTOM_AUTH
#define ENABLE_USER_AUTH
#define ENABLE_ACCESS_TOKEN
#define ENABLE_CUSTOM_TOKEN
#define ENABLE_ID_TOKEN
#define ENABLE_LEGACY_TOKEN

#define ENABLE_ETHERNET_NETWORK
#define ENABLE_GSM_NETWORK

/** 🔖 Optional Build Options
 *
 * 🏷️ For external Ethernet module support.
 * - Should define both library name and class object name.
 * - FIREBASE_ETHERNET_MODULE_LIB is the Ethernet library name with extension (.h) and
 *   should be inside "" or <> e.g. "Ethernet.h".
 * - FIREBASE_ETHERNET_MODULE_CLASS is the name of static object defined from class e.g. Ethernet.
 * - FIREBASE_ETHERNET_MODULE_TIMEOUT is the time out in milliseconds to wait network connection.
 *
 * #define FIREBASE_ETHERNET_MODULE_LIB "EthernetLib.h"
 * #define FIREBASE_ETHERNET_MODULE_CLASS EthernetClass
 * #define FIREBASE_ETHERNET_MODULE_TIMEOUT 2000
 *
 * 🏷️ For native core library ENC28J60 Ethernet module support in ESP8266.
 * #define ENABLE_ESP8266_ENC28J60_ETH
 *
 * 🏷️ For native core library W5500 Ethernet module support in ESP8266.
 * #define ENABLE_ESP8266_W5500_ETH
 *
 * 🏷️ For native core library W5100 Ethernet module support in ESP8266.
 * #define ENABLE_ESP8266_W5100_ETH
 *
 * 🏷️ For disabling on-board WiFI functionality in case external Client usage.
 * #define FIREBASE_DISABLE_ONBOARD_WIFI
 *
 * 🏷️ For disabling native (sdk) Ethernet functionality in case external Client usage.
 * #define FIREBASE_DISABLE_NATIVE_ETHERNET
 *
 * 🏷️ For disabling native ESP32 (sdk) PPP functionality.
 * #define FIREBASE_DISABLE_NATIVE_PPP
 *
 * 🏷️ For Async TCP Client usage.
 * #define ENABLE_ASYNC_TCP_CLIENT
 *
 * 🏷️ For maximum async queue limit setting for an async client
 * #define FIREBASE_ASYNC_QUEUE_LIMIT 10
 *
 * 🏷️ For Firebase.printf debug port.
 * #define FIREBASE_PRINTF_PORT Serial
 *
 * * 🏷️ For Firebase.printf buffer size.
 * The default printf buffer size is 1024 for ESP8266 and SAMD otherwise 4096.
 * #define FIREBASE_PRINTF_BUFFER 2048
 */

#if __has_include("UserConfig.h")
#include "UserConfig.h"
#endif

#include "core/Options.h"

/**
 * Included from the following files.
 *
 * core/AsyncClient/ConnectionHandler.h
 * core/AsyncClient/RequestHandler.h
 * core/AsyncResult/ResultBase.h
 * core/Auth/AuthBase.h
 * core/Auth/UserAuthData.h
 * core/File/DefaultFileConfig.h
 * core/File/FileConfig.h
 * core/File/FileConfigData.h
 * core/File/ResumableUpload.h
 * core/JWT/JWT.h
 * core/JWT/JWT.cpp
 * core/Network/Data/FirebaseWiFi.h
 * core/Network/Data/NetworkConfigData.h
 * core/Network/Data/SPIEthernet.h
 * core/Network/Data/StaticData.h
 * core/Network/NetConfig.h
 * core/Network/NetworkOptions.h
 * core/Updater/OTAUpdater.h
 * core/Utils/Base64.h
 * core/Utils/JSON.h
 * core/Utils/Memory.h
 * core/Utils/ObjectWriter.h
 * core/Utils/OTA.h
 * core/Utils/StringUtil.h
 * core/Utils/Timer.h
 * core/Utils/URL.h
 * core/FirebaseApp.h
 * database/DatabaseFilter.h
 * database/DataOptions.h
 * firestore/DataOptions.h
 * firestore/Query.cpp
 * firestore/Query.h
 * firestore/Values.h
 * functions/DataOptions.h
 * functions/Policy.h
 * messaging/DataOptions.h
 * storage/DataOptions.h
 * cloud_storage/DataOptions.h
 */

#endif