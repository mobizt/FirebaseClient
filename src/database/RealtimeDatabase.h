/**
 * Created October 2, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef ASYNC_DATABASE_H
#define ASYNC_DATABASE_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
#include "./database/DataOptions.h"

using namespace firebase;

#if defined(ENABLE_DATABASE)

class RealtimeDatabase : public RTDBResultBase
{
    friend class FirebaseApp;
    friend class AppBase;

public:
    std::vector<uint32_t> cVec; // AsyncClient vector

    explicit RealtimeDatabase(const String &url = "")
    {
        this->service_url = url;
    };

    RealtimeDatabase &operator=(const RealtimeDatabase &rhs)
    {
        this->service_url = rhs.service_url;
        this->app_token = rhs.app_token;
        return *this;
    }

    ~RealtimeDatabase()
    {
    }

    /**
     * Set the Firebase database URL
     * @param url The Firebase database URL.
     */
    void url(const String &url)
    {
        this->service_url = url;
        if (this->service_url.length())
        {
            if (this->service_url.indexOf("://") > -1)
                this->service_url.remove(0, this->service_url.indexOf("://") + 3);

            if (this->service_url.length() && this->service_url[this->service_url.length() - 1] == '/')
                this->service_url.remove(this->service_url.length() - 1, 1);
        }
    }

    /**
     * Unbind or remove the FirebaseApp
     */
    void resetApp()
    {
        this->app_addr = 0;
        this->app_token = nullptr;
        this->avec_addr = 0; // AsyncClient vector (list) address
        this->ul_dl_task_running_addr = 0;
    }

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
    auto get(AsyncClientClass &aClient, const String &path) -> typename std::enable_if<!std::is_same<T, void>::value && !std::is_same<T, AsyncResult>::value, T>::type
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(), nullptr, nullptr, aClient.getResult(), NULL);
        asyncRequest(aReq);
        return aClient.getResult()->rtdbResult.to<T>();
    }

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
     * silent, the option (boolean) to mute the return reponse payload.
     *
     * classicRequest, the option (boolean) to use HTTP POST for PUT (set) and DELETE (remove).
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
    auto get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options) -> typename std::enable_if<!std::is_same<T, void>::value && !std::is_same<T, AsyncResult>::value, T>::type
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, false, false, false, false, options.shallow), &options, nullptr, aClient.getResult(), NULL);
        asyncRequest(aReq);
        return aClient.getResult()->rtdbResult.to<T>();
    }

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
    void get(AsyncClientClass &aClient, const String &path, AsyncResult &aResult, bool sse = false)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, sse, true, false, false, false), nullptr, nullptr, &aResult, NULL);
        asyncRequest(aReq);
    }

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
    void get(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, bool sse = false, const String &uid = "")
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, sse, true, false, false, false), nullptr, nullptr, nullptr, cb, uid);
        asyncRequest(aReq);
    }

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
    void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, false, true, false, false, false), &options, nullptr, &aResult, NULL);
        asyncRequest(aReq);
    }

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
    void get(AsyncClientClass &aClient, const String &path, DatabaseOptions &options, AsyncResultCallback cb, const String &uid = "")
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, false, true, false, false, false), &options, nullptr, nullptr, cb, uid);
        asyncRequest(aReq);
    }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     file = SPIFFS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     file = SPIFFS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     file = SPIFFS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     SPIFFS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
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
    void get(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, false, true, false, false, false), nullptr, &file, &aResult, NULL);
        asyncRequest(aReq);
    }

    /**
     * Get value at the node path.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     file = SPIFFS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     file = SPIFFS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     file = SPIFFS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     SPIFFS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
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
    void get(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "")
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(), nullptr, &file, nullptr, cb, uid);
        asyncRequest(aReq);
    }

    /**
     * Checks if path exists in database
     *
     * ### Example
     * ```cpp
     * bool status = Database.existed(aClient, "/path/to/data");
     * ```
     * @param aClient The async client.
     * @param path The node path to check.
     * @return boolean value indicates the operating status.
     *
     */
    bool existed(AsyncClientClass &aClient, const String &path)
    {
        DatabaseOptions options;
        options.silent = true;
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(), &options, nullptr, aClient.getResult(), NULL);
        asyncRequest(aReq);
        return !getNullETagOption(&aClient.getResult()->rtdbResult);
    }

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
     *
     */
    void ota(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, false, true, false, true, false), nullptr, nullptr, &aResult, NULL);
        asyncRequest(aReq);
    }

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
     *
     * The data of node to download should be base64 encoded string of the firmware file.
     *
     */
    void ota(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, slot_options_t(false, false, true, false, true, false), nullptr, nullptr, nullptr, cb, uid);
        asyncRequest(aReq);
    }

    /**
     * Set value to database.
     * @param aClient The async client.
     * @param path The node path to set the value.
     * @param value The value to set.
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
    bool set(AsyncClientClass &aClient, const String &path, T value)
    {
        return storeAsync(aClient, path, value, async_request_handler_t::http_put, false, aClient.getResult(), NULL, "");
    }

    /**
     * Set value to database.
     * @param aClient The async client.
     * @param path The node path to set the value.
     * @param value The value to set.
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
    void set(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_put, true, &aResult, NULL, "");
    }

    /**
     * Set value to database.
     * @param aClient The async client.
     * @param path The node path to set the value.
     * @param value The value to set.
     * @param cb The async result callback (AsyncResultCallback).
     * @param uid The user specified UID of async result (optional).
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
    void set(AsyncClientClass &aClient, const String &path, T value, AsyncResultCallback cb, const String &uid = "")
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_put, true, nullptr, cb, uid);
    }

    /**
     * Set content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     file = SPIFFS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     file = SPIFFS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     file = SPIFFS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     SPIFFS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     * }
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
     * @param aResult The async result (AsyncResult)
     */
    void set(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_put, slot_options_t(false, false, true, false, false, false), nullptr, &file, &aResult, nullptr);
        asyncRequest(aReq);
    }

    /**
     * Set content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     file = SPIFFS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     file = SPIFFS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     file = SPIFFS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     SPIFFS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     * }
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
     * @param uid The user specified UID of async result (optional).
     */
    void set(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "")
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_put, slot_options_t(false, false, true, false, true, false), nullptr, &file, nullptr, cb, uid);
        asyncRequest(aReq);
    }

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
    String push(AsyncClientClass &aClient, const String &path, T value)
    {
        ValueConverter vcon;
        String payload;
        vcon.getVal<T>(payload, value);
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(), nullptr, nullptr, aClient.getResult(), NULL);
        asyncRequest(aReq, payload.c_str());
        return aClient.getResult()->rtdbResult.name();
    }

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
    void push(AsyncClientClass &aClient, const String &path, T value, AsyncResult &aResult)
    {
        ValueConverter vcon;
        String payload;
        vcon.getVal<T>(payload, value);
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, true, false, false, false), nullptr, nullptr, &aResult, NULL);
        asyncRequest(aReq, payload.c_str());
    }

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
    void push(AsyncClientClass &aClient, const String &path, T value, AsyncResultCallback cb, const String &uid = "")
    {
        ValueConverter vcon;
        String payload;
        vcon.getVal<T>(payload, value);
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, true, false, false, false), nullptr, nullptr, nullptr, cb, uid);
        asyncRequest(aReq, payload.c_str());
    }

    /**
     * Push content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     file = SPIFFS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     file = SPIFFS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     file = SPIFFS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     SPIFFS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     * }
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
    void push(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, true, false, false, false), nullptr, &file, &aResult, nullptr);
        asyncRequest(aReq);
    }

    /**
     * Push content from file to database.
     *
     * ### Example
     * ```cpp
     * // Example of FileCallback when SPIFFS is used as filesystem.
     *
     * void fileCallback(File &file, const char *filename, file_operating_mode mode)
     * {
     *   switch (mode)
     *   {
     *     case file_mode_open_read:
     *     file = SPIFFS.open(filename, "r");
     *     break;
     *   case file_mode_open_write:
     *     file = SPIFFS.open(filename, "w");
     *     break;
     *   case file_mode_open_append:
     *     file = SPIFFS.open(filename, "a");
     *     break;
     *   case file_mode_remove:
     *     SPIFFS.remove(filename);
     *     break;
     *   default:
     *     break;
     *   }
     * }
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
    void push(AsyncClientClass &aClient, const String &path, file_config_data &file, AsyncResultCallback cb, const String &uid = "")
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, slot_options_t(false, false, true, false, false, false), nullptr, &file, nullptr, cb, uid);
        asyncRequest(aReq);
    }

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
    bool update(AsyncClientClass &aClient, const String &path, const T &value)
    {
        return storeAsync(aClient, path, value, async_request_handler_t::http_patch, false, aClient.getResult(), NULL, "");
    }

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
    void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResult &aResult)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_patch, true, &aResult, NULL, "");
    }

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
    void update(AsyncClientClass &aClient, const String &path, const T &value, AsyncResultCallback cb, const String &uid = "")
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_patch, true, nullptr, cb, uid);
    }

    /**
     * Remove node from database
     *
     * ### Example
     * ```cpp
     * bool status = Database.remove(aClient, "/path/to/data");
     * ```
     * @param aClient The async client.
     * @param path The node path to remove.
     * @return boolean value indicates the operating status.
     *
     */
    bool remove(AsyncClientClass &aClient, const String &path)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_delete, slot_options_t(), nullptr, nullptr, aClient.getResult(), nullptr);
        asyncRequest(aReq);
        return getNullETagOption(&aClient.getResult()->rtdbResult) && String(aClient.getResult()->rtdbResult.data()).indexOf("null") > -1;
    }

    /**
     * Remove node from database
     *
     * ### Example
     * ```cpp
     * bool status = Database.remove(aClient, "/path/to/data", aResult);
     * ```
     * @param aClient The async client.
     * @param path The node path to remove.
     * @param aResult The async result (AsyncResult).
     *
     */
    void remove(AsyncClientClass &aClient, const String &path, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_delete, slot_options_t(false, false, true, false, false, false), nullptr, nullptr, &aResult, nullptr);
        asyncRequest(aReq);
    }

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
     */
    void remove(AsyncClientClass &aClient, const String &path, AsyncResultCallback cb, const String &uid = "")
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_delete, slot_options_t(false, false, true, false, false, false), nullptr, nullptr, nullptr, cb, uid);
        asyncRequest(aReq);
    }

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
    void setSSEFilters(const String &filter = "")
    {
        this->sse_events_filter = filter;
    }

#if defined(FIREBASE_OTA_STORAGE)
    /**
     * Set Arduino OTA Storage.
     *  @param storage The Arduino OTAStorage class object.
     */
    void setOTAStorage(OTAStorage &storage) { ota_storage_addr = reinterpret_cast<uint32_t>(&storage); }
#endif

    /**
     * Perform the async task repeatedly.
     * Should be placed in main loop function.
     */
    void loop()
    {
        app_token_t *aToken = appToken();
        for (size_t i = 0; i < cVec.size(); i++)
        {
            AsyncClientClass *client = reinterpret_cast<AsyncClientClass *>(cVec[i]);
            if (client)
            {
                // Store the auth time in all async clients.
                // The auth time will be used to reconnect the Stream when auth changed.
                if (aToken && aToken->auth_ts > 0 && aToken->authenticated)
                    client->setAuthTs(aToken->auth_ts);
                client->process(true);
                client->handleRemove();
            }
        }
    }

private:
    String service_url;
    String sse_events_filter;

    // FirebaseApp address and FirebaseApp vector address
    uint32_t app_addr = 0, avec_addr = 0;
    uint32_t ul_dl_task_running_addr = 0;
    uint32_t ota_storage_addr = 0;
    app_token_t *app_token = nullptr;

    struct async_request_data_t
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path;
        String uid;
        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;
        slot_options_t opt;
        DatabaseOptions *options = nullptr;
        file_config_data *file = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        async_request_data_t() {}
        async_request_data_t(AsyncClientClass *aClient, const String &path, async_request_handler_t::http_request_method method, slot_options_t opt, DatabaseOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
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
        }
    };

    void setApp(uint32_t app_addr, app_token_t *app_token, uint32_t avec_addr, uint32_t ul_dl_task_running_addr)
    {
        this->app_addr = app_addr;
        this->app_token = app_token;
        this->avec_addr = avec_addr; // AsyncClient vector (list) address
        this->ul_dl_task_running_addr = ul_dl_task_running_addr;
    }

    app_token_t *appToken()
    {
        if (avec_addr > 0)
        {
            const std::vector<uint32_t> *aVec = reinterpret_cast<std::vector<uint32_t> *>(avec_addr);
            List vec;
            return vec.existed(*aVec, app_addr) ? app_token : nullptr;
        }
        return nullptr;
    }

    template <typename T = object_t>
    bool storeAsync(AsyncClientClass &aClient, const String &path, const T &value, async_request_handler_t::http_request_method mode, bool async, AsyncResult *aResult, AsyncResultCallback cb, const String &uid)
    {
        ValueConverter vcon;
        String payload;
        vcon.getVal<T>(payload, value);
        DatabaseOptions options;
        if (!async && aClient.reqEtag.length() == 0)
            options.silent = true;
        async_request_data_t aReq(&aClient, path, mode, slot_options_t(false, false, async, payload.indexOf("\".sv\"") > -1, false, false), &options, nullptr, aResult, cb, uid);
        asyncRequest(aReq, payload.c_str());
        if (!async)
            return aResult->lastError.code() == 0;
        return true;
    }

    void asyncRequest(async_request_data_t &request, const char *payload = "")
    {
        app_token_t *atoken = appToken();

        if (!atoken)
            return setClientError(request, FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

        request.opt.app_token = atoken;
        request.opt.auth_param = atoken->auth_data_type != user_auth_data_no_token && atoken->auth_type != auth_access_token && atoken->auth_type != auth_sa_access_token;
        String extras = request.opt.auth_param ? ".json?auth=" + String(FIREBASE_AUTH_PLACEHOLDER) : ".json";

        addParams(request.opt.auth_param, extras, request.method, request.options, request.file);

        async_data_item_t *sData = request.aClient->createSlot(request.opt);

        if (!sData)
            return setClientError(request, FIREBASE_ERROR_OPERATION_CANCELLED);

        request.aClient->newRequest(sData, service_url, request.path, extras, request.method, request.opt, request.uid);

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
        }

        if (request.file && sData->upload)
        {
            sData->request.base64 = true;
            request.aClient->setFileContentLength(sData, 0);

            if (sData->request.file_data.file_size == 0)
                return setClientError(request, FIREBASE_ERROR_FILE_READ);
        }
        else if (strlen(payload))
        {
            sData->request.val[req_hndlr_ns::payload] = payload;
            request.aClient->setContentLength(sData, strlen(payload));
        }

        if (request.cb)
            sData->cb = request.cb;

        request.aClient->addRemoveClientVec(reinterpret_cast<uint32_t>(&(cVec)), true);

        if (request.aResult)
            sData->setRefResult(request.aResult, reinterpret_cast<uint32_t>(&(request.aClient->rVec)));

        if (sData->sse && sse_events_filter.length())
            request.aClient->sse_events_filter = sse_events_filter;

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void addParams(bool hasQueryParams, String &extras, async_request_handler_t::http_request_method method, DatabaseOptions *options, bool isFile)
    {
        URLUtil uut;
        bool hasParam = hasQueryParams;

        if (options)
        {
            if (options->readTimeout > 0)
                uut.addParam(extras, "timeout", String(options->readTimeout) + "ms", hasQueryParams);

            uut.addParam(extras, "writeSizeLimit", options->writeSizeLimit, hasQueryParams);

            if (options->shallow)
                uut.addParam(extras, "shallow", "true", hasQueryParams, true);
        }

        if ((options && options->silent) || ((method == async_request_handler_t::http_put || method == async_request_handler_t::http_post || method == async_request_handler_t::http_patch) && isFile))
            uut.addParam(extras, "print", "silent", hasQueryParams, true);

        if (options && options->filter.complete)
        {
            options->filter.uri[0] = !hasParam ? '?' : '&';
            extras += options->filter.uri;
        }
    }

    void setClientError(async_request_data_t &request, int code)
    {
        AsyncResult *aResult = request.aResult;

        if (!aResult)
            aResult = new AsyncResult();

        aResult->lastError.setClientError(code);

        if (request.cb)
            request.cb(*aResult);

        if (!request.aResult)
        {
            delete aResult;
            aResult = nullptr;
        }
    }

    void setFileStatus(async_data_item_t *sData, const async_request_data_t &request)
    {
        if ((request.file && request.file->filename.length()) || request.opt.ota)
        {
            sData->download = request.method == async_request_handler_t::http_get;
            sData->upload = request.method == async_request_handler_t::http_post ||
                            request.method == async_request_handler_t::http_put ||
                            request.method == async_request_handler_t::http_patch;
        }
    }
};

#endif

#endif