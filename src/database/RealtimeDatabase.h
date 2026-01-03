/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef DATABASE_REALTIME_DATABASE_H
#define DATABASE_REALTIME_DATABASE_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./database/DataOptions.h"

using namespace firebase_ns;

#if defined(ENABLE_DATABASE)
class RealtimeDatabase : public RTDBResultImpl, AppBase
{
    friend class FirebaseApp;
    friend class AppBase;

public:
    explicit RealtimeDatabase(const String &url = "") { this->service_url = url; }

    RealtimeDatabase &operator=(const RealtimeDatabase &rhs)
    {
        this->service_url = rhs.service_url;
        this->app_token = rhs.app_token;
        return *this;
    }

    ~RealtimeDatabase() {}

    /**
     * Unbind or remove the FirebaseApp
     */
    void resetApp() { resetAppImpl(); }

    /**
     * Set the Firebase database URL
     * @param url The Firebase database URL.
     */
    void url(const String &url) { this->service_url = uut.getHost(url, nullptr); }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * String json = Database.get<String>(aClient, "/path/to/data");
     * ```
     * @param aClient The async client.
     * @param path The node path to get value.
     * @return value that casts from response payload.
     *
     */
    template <typename T = int>
    auto get(AsyncClientClass &aClient, const String &path) -> typename std::enable_if<!std::is_same<T, void>::value && !std::is_same<T, AsyncResult>::value, T>::type { return sendRequest(&aClient, path, reqns::http_get, slot_options_t(), nullptr, nullptr, aClient.getResult(), NULL)->rtdbResult.to<T>(); }

    /**
     * Get value at the node path.
     *
     * The DatabaseOptions related to the Conditional Requests and Query Parameters supported by
     * Firebase Realtime Database REST API are following.
     *
     * readTimeout, the timeout (number) in ms which limits how long the read takes on the server side.
     *
     * writeSizeLimit, the size of a write limit can be "tiny", "small", "meduim", "large" and "unlimited".
     *
     * shallow, the option (boolean) for shallowing (truncating) the JSON representation data data into true while JSON primitive values
     * (string, number and boolean) will not shallow.
     *
     * Filter, the options for complex data filtering which included the properties i.e., orderBy, startAt, endAt, limitToFirst, limitToLast, and equalTo.
     *
     * ### Example
     * ```cpp
     * DatabaseOptions options;
     * options.filter.orderBy("Data");
     * options.filter.startAt(105);
     * options.filter.endAt(120);
     * options.filter.limitToLast(8);
     *
     * String json = Database.get<String>(aClient, "/path/to/data", options);
     * ```
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param options The database options (DatabaseOptions).
     * @return value that casts from response payload.
     *
     */
    template <typename T = int>
    auto get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options) -> typename std::enable_if<!std::is_same<T, void>::value && !std::is_same<T, AsyncResult>::value, T>::type { return sendRequest(&aClient, path, reqns::http_get, slot_options_t(false, false, false, false, false, options.shallow), &options, nullptr, aClient.getResult(), NULL)->rtdbResult.to<T>(); }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * Database.get(aClient, "/path/to/data", aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param aResult The async result (AsyncResult)
     * @param sse The Server-sent events (Stream) mode
     *
     */
    void get(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, bool sse = false) { sendRequest(&aClient, path, reqns::http_get, slot_options_t(false, sse, true, false, false, false), nullptr, nullptr, &aResult, NULL); }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * Database.get(aClient, "/path/to/data", cb);
     * ```
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param cb The async result callback (AsyncResultCallback).
     * @param sse The Server-sent events (Stream) mode.
     * @param uid The user specified UID of async result (optional).
     *
     */
    void get(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, bool sse = false, const String &uid = "") { sendRequest(&aClient, path, reqns::http_get, slot_options_t(false, sse, true, false, false, false), nullptr, nullptr, nullptr, cb, uid); }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * DatabaseOptions options;
     * options.filter.orderBy("Data");
     * options.filter.startAt(105);
     * options.filter.endAt(120);
     * options.filter.limitToLast(8);
     *
     * Database.get<String>(aClient, "/path/to/data", options, aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param options The database options (DatabaseOptions).
     * @param aResult The async result (AsyncResult)
     * @return value that casts from response payload.
     *
     */
    void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResult &aResult) { sendRequest(&aClient, path, reqns::http_get, slot_options_t(false, false, true, false, false, false), &options, nullptr, &aResult, NULL); }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * DatabaseOptions options;
     * options.filter.orderBy("Data");
     * options.filter.startAt(105);
     * options.filter.endAt(120);
     * options.filter.limitToLast(8);
     *
     * Database.get<String>(aClient, "/path/to/data", options, cb);
     * ```
     *
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param options The database options (DatabaseOptions).
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResultCallback cb, const String &uid = "") { sendRequest(&aClient, path, reqns::http_get, slot_options_t(false, false, true, false, false, false), &options, nullptr, nullptr, cb, uid); }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * #include <FS.h>
     * File myFile; // Define the File object globally.
     * #defined FILESYSTEMS SPIFFS
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     myFile = FILESYSTEMS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     myFile = FILESYSTEMS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     myFile = FILESYSTEMS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     FILESYSTEMS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     *   file = myFile;
     * }
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * Database.get(aClient, "/path/to/data", getFile(fileConfig), aResult);
     * ```
     *
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileCallback.
     * The FileCallback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
     * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
     * @return boolean value indicates the operating status.
     */
    bool get(AsyncClientClass &aClient, const String &path, file_config_data &file) { return sendRequest(&aClient, path, reqns::http_get, slot_options_t(), nullptr, &file, nullptr, NULL)->error().code() == 0; }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * #include <FS.h>
     * File myFile; // Define the File object globally.
     * #defined FILESYSTEMS SPIFFS
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     myFile = FILESYSTEMS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     myFile = FILESYSTEMS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     myFile = FILESYSTEMS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     FILESYSTEMS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     *   file = myFile;
     * }
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * Database.get(aClient, "/path/to/data", getFile(fileConfig), aResult);
     * ```
     *
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileCallback.
     * The FileCallback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
     * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
     * @param aResult The async result (AsyncResult)
     */
    void get(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult) { sendRequest(&aClient, path, reqns::http_get, slot_options_t(false, false, true, false, false, false), nullptr, &file, &aResult, NULL); }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * #include <FS.h>
     * File myFile; // Define the File object globally.
     * #defined FILESYSTEMS SPIFFS
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     myFile = FILESYSTEMS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     myFile = FILESYSTEMS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     myFile = FILESYSTEMS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     FILESYSTEMS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     *   file = myFile;
     * }
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * Database.get(aClient, "/path/to/data", getFile(fileConfig), cb);
     * ```
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileCallback.
     * The FileCallback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
     * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     *
     */
    void get(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "") { sendRequest(&aClient, path, reqns::http_get, slot_options_t(false, false, true, false, false, false), nullptr, &file, nullptr, cb, uid); }

    /**
     * Checks if path exists in database
     *
     * ### Example
     * ```cpp
     * bool status = Database.exists(aClient, "/path/to/data");
     * ```
     * @param aClient The async client.
     * @param path The node path to check.
     * @return boolean value. Returns true if node exists.
     *
     */
    bool exists(AsyncClientClass &aClient, const String &path)
    {
        DatabaseOptions options;
        options.silent = true;
        return !getNullETagOption(&sendRequest(&aClient, path, reqns::http_get, slot_options_t(), &options, nullptr, aClient.getResult(), NULL)->rtdbResult);
    }

    // Obsoleted, use exists instead.
    bool existed(AsyncClientClass &aClient, const String &path) { return exists(aClient, path); }

    /**
     * Perform OTA update using a firmware file from the database.
     *
     * The data of node to download should be base64 encoded string of the firmware file.
     *
     * ### Example
     * ```cpp
     * Database.ota(aClient, "/path/to/data", aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to download.
     * @param command The OTA command (optional). 0 or U_FLASH for firmware, 100 or U_FLASHFS (ESP32) or 
     * U_FS (ESP8266 or Raspberry Pi Pico), 101 or U_SPIFFS (ESP32), 
     * 102 for U_FATFS (ESP32), 103 or U_LITTLEFS (ESP32).
     * @return boolean value indicates the operating status.
     *
     */
    bool ota(AsyncClientClass &aClient, const String &path, int command = 0) { return sendRequest(&aClient, path, reqns::http_get, slot_options_t(), nullptr, nullptr, nullptr, NULL, "", "", "", command)->error().code() == 0; }

    /**
     * Perform OTA update using a firmware file from the database.
     *
     * The data of node to download should be base64 encoded string of the firmware file.
     *
     * ### Example
     * ```cpp
     * Database.ota(aClient, "/path/to/data", aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to download.
     * @param aResult The async result (AsyncResult)
     * @param command The OTA command (optional). 0 or U_FLASH for firmware, 100 or U_FLASHFS (ESP32) or 
     * U_FS (ESP8266 or Raspberry Pi Pico), 101 or U_SPIFFS (ESP32), 
     * 102 for U_FATFS (ESP32), 103 or U_LITTLEFS (ESP32).
     *
     */
    void ota(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, int command = 0) { sendRequest(&aClient, path, reqns::http_get, slot_options_t(false, false, true, false, true, false), nullptr, nullptr, &aResult, NULL, "", "", "", command); }

    /**
     * Perform OTA update using a firmware file from the database.
     *
     * ### Example
     * ```cpp
     * Database.ota(aClient, "/path/to/data", aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to download.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     * @param command The OTA command (optional). 0 or U_FLASH for firmware, 100 or U_FLASHFS (ESP32) or 
     * U_FS (ESP8266 or Raspberry Pi Pico), 101 or U_SPIFFS (ESP32), 
     * 102 for U_FATFS (ESP32), 103 or U_LITTLEFS (ESP32).
     *
     * The data of node to download should be base64 encoded string of the firmware file.
     *
     */
    void ota(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "", int command = 0) { sendRequest(&aClient, path, reqns::http_get, slot_options_t(false, false, true, false, true, false), nullptr, nullptr, nullptr, cb, uid, "", "", command); }

    /**
     * Set value to database.
     * @param aClient The async client.
     * @param path The node path to set the value.
     * @param value The value to set.
     * @param matchingEtag. Optional. The Etag value for comparison with the existing server's Ethag value.
     * The operation will fail with HTTP code 412 Precondition Failed if the Etag does not match.
     * @return boolean value indicates the operating status.
     *
     * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
     *
     * The string_t is for string placeholder e.g. string_t("hello there").
     * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
     * The boolean_t is for boolean placeholder e.g. boolean_t(true).
     * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
     *
     * ### Example
     * ```cpp
     * bool status = Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));
     * ```
     */
    template <typename T = const char *>
    bool set(AsyncClientClass &aClient, const String &path, T value, const String &matchingEtag = "") { return storeAsync(aClient, path, value, reqns::http_put, false, aClient.getResult(), NULL, "", matchingEtag); }

    /**
     * Set value to database.
     * @param aClient The async client.
     * @param path The node path to set the value.
     * @param value The value to set.
     * @param matchingEtag Optional. The Etag value for comparison with the existing server's Ethag value.
     * The operation will fail with HTTP code 412 Precondition Failed if the Etag does not match.
     * @param aResult The async result (AsyncResult)
     *
     * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
     *
     * The string_t is for string placeholder e.g. string_t("hello there").
     * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
     * The boolean_t is for boolean placeholder e.g. boolean_t(true).
     * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
     *
     * ### Example
     * ```cpp
     * Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), aResult);
     * ```
     */
    template <typename T = const char *>
    void set(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult, const String &matchingEtag = "") { storeAsync(aClient, path, value, reqns::http_put, true, &aResult, NULL, "", matchingEtag); }

    /**
     * Set value to database.
     * @param aClient The async client.
     * @param path The node path to set the value.
     * @param value The value to set.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid Optional. The user specified UID of async result.
     * @param matchingEtag Optional. The Etag value for comparison with the existing server's Ethag value.
     * The operation will fail with HTTP code 412 Precondition Failed if the Etag does not match.
     *
     * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
     *
     * The string_t is for string placeholder e.g. string_t("hello there").
     * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
     * The boolean_t is for boolean placeholder e.g. boolean_t(true).
     * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
     *
     * ### Example
     * ```cpp
     * Database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), cb);
     * ```
     */
    template <typename T = const char *>
    void set(AsyncClientClass &aClient, const String &path, T value, AsyncResultCallback cb, const String &uid = "", const String &matchingEtag = "") { storeAsync(aClient, path, value, reqns::http_put, true, nullptr, cb, uid, matchingEtag); }

    /**
     * Set content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * #include <FS.h>
     * File myFile; // Define the File object globally.
     * #defined FILESYSTEMS SPIFFS
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     myFile = FILESYSTEMS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     myFile = FILESYSTEMS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     myFile = FILESYSTEMS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     FILESYSTEMS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     *   file = myFile;
     * }
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * Database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to set value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileCallback.
     * The FileCallback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
     * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
     * @param matchingEtag Optional. The Etag value for comparison with the existing server's Ethag value.
     * The operation will fail with HTTP code 412 Precondition Failed if the Etag does not match.
     * @return boolean value indicates the operating status.
     */
    bool set(AsyncClientClass &aClient, const String &path, file_config_data &file, const String &matchingEtag = "") { return sendRequest(&aClient, path, reqns::http_put, slot_options_t(), nullptr, &file, nullptr, nullptr, "", "", matchingEtag)->lastError.code() == 0; }

    /**
     * Set content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * #include <FS.h>
     * File myFile; // Define the File object globally.
     * #defined FILESYSTEMS SPIFFS
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     myFile = FILESYSTEMS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     myFile = FILESYSTEMS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     myFile = FILESYSTEMS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     FILESYSTEMS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     *   file = myFile;
     * }
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * Database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to set value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileCallback.
     * The FileCallback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
     * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
     * @param matchingEtag Optional. The Etag value for comparison with the existing server's Ethag value.
     * The operation will fail with HTTP code 412 Precondition Failed if the Etag does not match.
     * @param aResult The async result (AsyncResult)
     */
    void set(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult, const String &matchingEtag = "") { sendRequest(&aClient, path, reqns::http_put, slot_options_t(false, false, true, false, false, false), nullptr, &file, &aResult, nullptr, "", "", matchingEtag); }

    /**
     * Set content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * #include <FS.h>
     * File myFile; // Define the File object globally.
     * #defined FILESYSTEMS SPIFFS
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     myFile = FILESYSTEMS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     myFile = FILESYSTEMS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     myFile = FILESYSTEMS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     FILESYSTEMS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     *   file = myFile;
     * }
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * Database.set(aClient, "/path/to/data", getFile(fileConfig), cb);
     * ```
     * @param aClient The async client.
     * @param path The node path to set value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileCallback.
     * The FileCallback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
     * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid Optional. The user specified UID of async result.
     * @param matchingEtag Optional. The Etag value for comparison with the existing server's Ethag value.
     * The operation will fail with HTTP code 412 Precondition Failed if the Etag does not match.
     */
    void set(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "", const String &matchingEtag = "") { sendRequest(&aClient, path, reqns::http_put, slot_options_t(false, false, true, false, true, false), nullptr, &file, nullptr, cb, uid, "", matchingEtag); }

    /**
     * Push value to database.
     *
     * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
     *
     * The string_t is for string placeholder e.g. string_t("hello there").
     * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
     * The boolean_t is for boolean placeholder e.g. boolean_t(true).
     * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
     *
     * ### Example
     * ```cpp
     * String name = Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));
     * ```
     * @param aClient The async client.
     * @param path The node path to push the value.
     * @param value The value to push.
     * @return String name of new node that created.
     *
     */
    template <typename T = const char *>
    String push(AsyncClientClass &aClient, const String &path, T value) { return sendRequest(&aClient, path, reqns::http_post, slot_options_t(), nullptr, nullptr, aClient.getResult(), NULL, "", convert(value).c_str())->rtdbResult.name(); }

    /**
     * Push value to database.
     *
     * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
     *
     * The string_t is for string placeholder e.g. string_t("hello there").
     * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
     * The boolean_t is for boolean placeholder e.g. boolean_t(true).
     * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
     *
     * ### Example
     * ```cpp
     * Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to push the value.
     * @param value The value to push.
     * @param aResult The async result (AsyncResult).
     */
    template <typename T = const char *>
    void push(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult) { sendRequest(&aClient, path, reqns::http_post, slot_options_t(false, false, true, false, false, false), nullptr, nullptr, &aResult, NULL, "", convert(value).c_str()); }

    /**
     * Push value to database.
     *
     * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
     *
     * The string_t is for string placeholder e.g. string_t("hello there").
     * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
     * The boolean_t is for boolean placeholder e.g. boolean_t(true).
     * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
     *
     * ### Example
     * ```cpp
     * Database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), cb);
     * ```
     * @param aClient The async client.
     * @param path The node path to push the value.
     * @param value The value to push.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     */
    template <typename T = const char *>
    void push(AsyncClientClass &aClient, const String &path, T value, AsyncResultCallback cb, const String &uid = "") { sendRequest(&aClient, path, reqns::http_post, slot_options_t(false, false, true, false, false, false), nullptr, nullptr, nullptr, cb, uid, convert(value).c_str()); }

    /**
     * Push content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * #include <FS.h>
     * File myFile; // Define the File object globally.
     * #defined FILESYSTEMS SPIFFS
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     myFile = FILESYSTEMS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     myFile = FILESYSTEMS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     myFile = FILESYSTEMS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     FILESYSTEMS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     *   file = myFile;
     * }
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * Database.push(aClient, "/path/to/data", getFile(fileConfig), aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to push value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileCallback.
     * The FileCallback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
     * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
     * @return String name of new node that created.
     *
     */
    String push(AsyncClientClass &aClient, const String &path, file_config_data &file) { return sendRequest(&aClient, path, reqns::http_post, slot_options_t(), nullptr, &file, nullptr, nullptr)->rtdbResult.name(); }

    /**
     * Push content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * #include <FS.h>
     * File myFile; // Define the File object globally.
     * #defined FILESYSTEMS SPIFFS
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     myFile = FILESYSTEMS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     myFile = FILESYSTEMS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     myFile = FILESYSTEMS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     FILESYSTEMS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     *   file = myFile;
     * }
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * Database.push(aClient, "/path/to/data", getFile(fileConfig), aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to push value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileCallback.
     * The FileCallback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
     * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
     * @param aResult The async result (AsyncResult)
     *
     */
    void push(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult) { sendRequest(&aClient, path, reqns::http_post, slot_options_t(false, false, true, false, false, false), nullptr, &file, &aResult, nullptr); }

    /**
     * Push content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * #include <FS.h>
     * File myFile; // Define the File object globally.
     * #defined FILESYSTEMS SPIFFS
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     myFile = FILESYSTEMS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     myFile = FILESYSTEMS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     myFile = FILESYSTEMS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     FILESYSTEMS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     *   file = myFile;
     * }
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * Database.push(aClient, "/path/to/data", getFile(fileConfig), cb);
     * ```
     * @param aClient The async client.
     * @param path The node path to push value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileCallback.
     * The FileCallback function parameters included the File reference returned from file operation, filename for file operation and file_operating_mode.
     * The file_operating_mode included file_mode_open_read, file_mode_open_write, file_mode_open_append and file_mode_open_remove.
     *
     */
    void push(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "") { sendRequest(&aClient, path, reqns::http_post, slot_options_t(false, false, true, false, false, false), nullptr, &file, nullptr, cb, uid); }

    /**
     * Update (patch) JSON representation data to database.
     *
     * ### Example
     * ```cpp
     * bool status = Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"));
     * ```
     * @param aClient The async client.
     * @param path The node path to update.
     * @param value The JSON representation data (object_t) to update.
     * @return boolean value indicates the operating status.
     *
     */
    template <typename T = object_t>
    bool update(AsyncClientClass &aClient, const String &path, const T &value) { return storeAsync(aClient, path, value, reqns::http_patch, false, aClient.getResult(), NULL, "", ""); }

    /**
     * Update (patch) JSON representation data to database.
     *
     * ### Example
     * ```cpp
     * Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"), aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to update.
     * @param value The JSON representation data (object_t) to update.
     * @param aResult The async result (AsyncResult).
     */
    template <typename T = object_t>
    void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResult &aResult) { storeAsync(aClient, path, value, reqns::http_patch, true, &aResult, NULL, "", ""); }

    /**
     * Update (patch) JSON representation data to database.
     *
     * ### Example
     * ```cpp
     * Database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"), cb);
     * ```
     * @param aClient The async client.
     * @param path The node path to update.
     * @param value The JSON representation data (object_t) to update.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     */
    template <typename T = object_t>
    void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResultCallback cb, const String &uid = "") { storeAsync(aClient, path, value, reqns::http_patch, true, nullptr, cb, uid, ""); }

    /**
     * Remove node from database
     *
     * ### Example
     * ```cpp
     * bool status = Database.remove(aClient, "/path/to/data");
     * ```
     * @param aClient The async client.
     * @param path The node path to remove.
     * @param matchingEtag Optional. The Etag value for comparison with the existing server's Ethag value.
     * The operation will fail with HTTP code 412 Precondition Failed if the Etag does not match.
     * @return boolean value indicates the operating status.
     *
     */
    bool remove(AsyncClientClass &aClient, const String &path, const String &matchingEtag = "") { return getNullETagOption(&sendRequest(&aClient, path, reqns::http_delete, slot_options_t(), nullptr, nullptr, aClient.getResult(), nullptr, "", "", matchingEtag)->rtdbResult) && String(aClient.getResult()->rtdbResult.data()).indexOf("null") > -1; }

    /**
     * Remove node from database
     *
     * ### Example
     * ```cpp
     * bool status = Database.remove(aClient, "/path/to/data", aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to remove.
     * @param matchingEtag Optional. The Etag value for comparison with the existing server's Ethag value.
     * The operation will fail with HTTP code 412 Precondition Failed if the Etag does not match.
     * @param aResult The async result (AsyncResult).
     *
     */
    void remove(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, const String &matchingEtag = "") { sendRequest(&aClient, path, reqns::http_delete, slot_options_t(false, false, true, false, false, false), nullptr, nullptr, &aResult, nullptr, "", "", matchingEtag); }

    /**
     * Remove node from database
     *
     * ### Example
     * ```cpp
     * Database.remove(aClient, "/path/to/data", cb);
     * ```
     * @param aClient The async client.
     * @param path The node path to remove.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
     * @param matchingEtag Optional. The Etag value for comparison with the existing server's Ethag value.
     * The operation will fail with HTTP code 412 Precondition Failed if the Etag does not match.
     */
    void remove(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "", const String &matchingEtag = "") { sendRequest(&aClient, path, reqns::http_delete, slot_options_t(false, false, true, false, false, false), nullptr, nullptr, nullptr, cb, uid, "", matchingEtag); }

    /**
     * Filtering response payload for SSE mode (HTTP Streaming).
     * @param filter The event keywords for filtering.
     *
     * This is optional to allow specific events filtering.
     *
     * The following event keywords are supported.
     * get - To allow the http get response (first put event since stream connected).
     * put - To allow the put event.
     * patch - To allow the patch event.
     * keep-alive - To allow the keep-alive event.
     * cancel - To allow the cancel event.
     * auth_revoked - To allow the auth_revoked event.
     *
     * To clear all prevousely set filter to allow all Stream events, use RealtimeDatabase::setSSEFilters().
     */
    void setSSEFilters(const String &filter = "") { this->sse_events_filter = filter; }

#if defined(FIREBASE_OTA_STORAGE)
    /**
     * Set Arduino OTA Storage.
     *  @param storage The Arduino OTAStorage class object.
     */
    void setOTAStorage(OTAStorage &storage) { ota_storage_addr = reinterpret_cast<uint32_t>(&storage); }
#endif

    /**
     * Perform the async task repeatedly (DEPRECATED).
     */
    void loop() { loopImpl(); }

private:
    String sse_events_filter;
    struct req_data
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path, uid, etag;
        reqns::http_request_method method = reqns::http_undefined;
        slot_options_t opt;
        DatabaseOptions *options = nullptr;
        file_config_data *file = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        bool isSSEFilter = false;
        int command = 0;
        req_data() {}
        req_data(AsyncClientClass *aClient, const String &path, reqns::http_request_method method, const slot_options_t &opt, DatabaseOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "", const String &etag = "", int command = 0)
        {
            this->aClient = aClient;
            this->path = path;
            this->method = method;
            this->opt = opt;
            this->options = options;
            this->file = file;
            this->aResult = aResult;
            this->cb = cb;
            this->uid = uid;
            this->etag = etag;
            isSSEFilter = aClient->sman.isSSEFilter();
            this->command = command;
        }
    };

    template <typename T = object_t>
    bool storeAsync(AsyncClientClass &aClient, const String &path, const T &value, reqns::http_request_method mode, bool async, AsyncResult *aResult, AsyncResultCallback cb, const String &uid, const String &etag)
    {
        ValueConverter vcon;
        String payload;
        vcon.getVal<T>(payload, value);
        DatabaseOptions options;
        if (!async && etag.length() == 0)
            options.silent = true;
        return sendRequest(&aClient, path, mode, slot_options_t(false, false, async, payload.indexOf("\".sv\"") > -1, false, false), &options, nullptr, aResult, cb, uid, payload.c_str(), etag)->lastError.code() == 0;
    }
    template <typename T = object_t>
    String convert(T value)
    {
        ValueConverter vcon;
        String payload;
        vcon.getVal<T>(payload, value);
        return payload.c_str();
    }

    AsyncResult *sendRequest(AsyncClientClass *aClient, const String &path, reqns::http_request_method method, slot_options_t opt, DatabaseOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "", const char *payload = "", const String &etag = "", int command = 0)
    {
        req_data areq(aClient, path, method, opt, options, file, aResult, cb, uid, etag, command);
        asyncRequest(areq, payload);
        return aClient->getResult();
    }

    void asyncRequest(req_data &request, const char *payload = "")
    {
        app_token_t *atoken = appToken();

        if (!atoken)
        {
            request.aClient->_lastError()->code();
            request.aClient->_lastError()->setLastError(FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED, "app was not assigned");
            return request.aClient->setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);
        }

        service_url.trim();
        if (service_url.length() == 0)
        {
            request.aClient->_lastError()->code();
            request.aClient->_lastError()->setLastError(FIREBASE_ERROR_INVALID_DATABASE_URL, "invalid database URL, please use RealtimeDatabase::url to set");
            return request.aClient->setClientError(request, FIREBASE_ERROR_INVALID_DATABASE_URL);
        }

        request.opt.app_token = atoken;
        request.opt.user_auth = user_auth;
        request.opt.auth_param = user_auth->getAuthDataType() != user_auth_data_no_token && user_auth->getAuthTokenType() != auth_access_token && user_auth->getAuthTokenType() != auth_sa_access_token;
        String extras;
        sut.printTo(extras, 100, ".json%s%s", request.opt.auth_param ? "?auth=" : "", request.opt.auth_param ? String(FIREBASE_AUTH_PLACEHOLDER).c_str() : ""); //
        addParams(request.opt.auth_param, extras, request.method, request.options, request.file);

        async_data *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return request.aClient->setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        request.aClient->newRequest(sData, service_url, request.path, extras, request.method, request.opt, request.uid, request.etag);

        if (request.file)
            sData->request.file_data.copy(*request.file);

        setFileStatus(sData, request);

        if (request.opt.ota)
        {
            sData->request.ota = true;
            sData->request.base64 = true;
            sData->aResult.download_data.ota = true;
            sData->request.ul_dl_task_running_addr = ul_dl_task_running_addr;
            sData->request.ota_storage_addr = ota_storage_addr;
            sData->request.command = request.command;
        }

        if (sData->download)
            sData->request.base64 = true;

        if (request.file && sData->upload)
        {
            sData->request.base64 = true;
            sData->request.setFileContentLength(0);

            if (sData->request.file_data.file_size == 0)
                return request.aClient->setClientError(request, FIREBASE_ERROR_FILE_READ);
        }
        else if (strlen(payload))
        {
            sData->request.val[reqns::payload] = payload;
            sData->request.setContentLengthFinal(strlen(payload));
        }

        if (request.cb)
            sData->cb = request.cb;

        request.aClient->addRemoveClientVec(reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(request.aClient->getResultList())));

        if (sData->sse && sse_events_filter.length() && !request.isSSEFilter)
            request.aClient->setSSEFilters(sse_events_filter);

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void addParams(bool hasParam, String &extras, reqns::http_request_method method, DatabaseOptions *options, bool isFile)
    {
        String params;
        if (options)
        {
            if (options->readTimeout > 0)
                sut.printTo(params, 20, "%stimeout=%dms", hasParam ? "&" : "?", options->readTimeout);

            if (options->writeSizeLimit.length())
                sut.printTo(params, 20, "%swriteSizeLimit=%s", params.length() || hasParam ? "&" : "?", options->writeSizeLimit.c_str());

            if (options->shallow)
                sut.printTo(params, 20, "%sshallow=true", params.length() || hasParam ? "&" : "?");
        }

        if ((options && options->silent) || ((method == reqns::http_put || method == reqns::http_post || method == reqns::http_patch) && isFile))
            sut.printTo(params, 20, "%sprint=silent", params.length() || hasParam ? "&" : "?");

        if (options && options->filter.complete)
        {
            options->filter.uri[0] = params.length() || hasParam ? '&' : '?';
            params += options->filter.uri;
        }

        if (params.length())
            extras += params;
    }

    void setFileStatus(async_data *sData, const req_data &request)
    {
        using namespace reqns;
        bool isFile = request.file && request.file->data && request.file->data_size > 0;
#if defined(ENABLE_FS)
        isFile |= request.file && request.file->filename.length() > 0;
#endif
        if (isFile || request.opt.ota)
        {
            sData->download = request.method == http_get;
            sData->upload = request.method == http_post || request.method == http_put || request.method == http_patch;
        }
    }

private:
    StringUtil sut;
};
#endif
#endif