/**
 * Created January 29, 2024
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
using namespace std;

using namespace firebase;

class Database
{
    friend class FirebaseApp;

public:
    Database(const String &url = "")
    {
        this->dbUrl = url;
    };

    Database &operator=(Database &rhs)
    {
        this->dbUrl = rhs.dbUrl;
        return *this;
    }

    ~Database()
    {
    }

    void setApp(uint32_t app_addr, app_token_t *app_token)
    {
        this->app_addr = app_addr;
        this->app_token = app_token;
    }

    app_token_t *appToken()
    {
        List list;
        return list.existed(firebase_app_list, app_addr) ? app_token : nullptr;
    }

    /**
     * Get value at the node path.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @return value that casts from response payload.
     *
     * Example:
     *
     * String json = database.get<String>(aClient, "/path/to/data");
     */
    template <typename T1 = int>
    auto get(AsyncClient &aClient, const String &path) -> typename enable_if<!is_same<T1, void>::value && !is_same<T1, AsyncResult>::value, T1>::type
    {
        AsyncResult result;
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), nullptr, nullptr, &result, NULL);
        asyncRequest(aReq);
        return result.database.to<T1>();
    }

    /**
     * Get value at the node path.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param options The database options (DataOptions).
     * @return value that casts from response payload.
     *
     * The DataOptions related to the Conditional Requests and Query Parameters supported by
     * Firebase Realtime Database REST API are following.
     *
     * readTimeout, the timeout (number) in ms which limits how long the read takes on the server side.
     *
     * writeSizeLimit, the size of a write limit can be "tiny", "small", "meduim", "large" and "unlimited".
     *
     * shallow, the option (boolean) for shallowing (truncating) the JSON object data into true while JSON primitive values
     * (string, number and boolean) will not shallow.
     *
     * silent, the option (boolean) to mute the return reponse payload.
     *
     * classicRequest, the option (boolean) to use HTTP POST for PUT (set) and DELETE (remove).
     *
     * customHeaders, the custom header which is currently supported only X-Firebase-ETag header.
     *
     * ETAG, the ETAG value for The if-match condition request header for the data you want to update.
     *
     * Filter, the options for complex data filtering which included the properties i.e., orderBy, startAt, endAt, limitToFirst, limitToLast, and equalTo.
     *
     * Example:
     *
     * DataOptions options;
     * options.filter.orderBy("Data");
     * options.filter.startAt(105);
     * options.filter.endAt(120);
     * options.filter.limitToLast(8);
     *
     * String json = database.get<String>(aClient, "/path/to/data", options);
     */
    template <typename T1 = int>
    auto get(AsyncClient &aClient, const String &path, DataOptions &options) -> typename enable_if<!is_same<T1, void>::value && !is_same<T1, AsyncResult>::value, T1>::type
    {
        AsyncResult result;
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), &options, nullptr, &result, NULL);
        asyncRequest(aReq);
        return result.database.to<T1>();
    }

    /**
     * Get value at the node path.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param aResult The async result (AsyncResult)
     * @param sse The Server-sent events (Stream) mode
     *
     * Example:
     *
     * database.get(aClient, "/path/to/data", aResult);
     */
    void get(AsyncClient &aClient, const String &path, AsyncResult &aResult, bool sse = false)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, sse, true, false, false), nullptr, nullptr, &aResult, NULL);
        asyncRequest(aReq);
    }

    /**
     * Get value at the node path.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param cb The async result callback (AsyncResultCallback)
     * @param sse The Server-sent events (Stream) mode
     *
     * Example:
     *
     * database.get(aClient, "/path/to/data", cb);
     */
    void get(AsyncClient &aClient, const String &path, AsyncResultCallback cb, bool sse = false)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, sse, true, false, false), nullptr, nullptr, nullptr, cb);
        asyncRequest(aReq);
    }

    /**
     * Get value at the node path.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param options The database options (DataOptions).
     * @param aResult The async result (AsyncResult)
     * @return value that casts from response payload.
     *
     * Example:
     *
     * DataOptions options;
     * options.filter.orderBy("Data");
     * options.filter.startAt(105);
     * options.filter.endAt(120);
     * options.filter.limitToLast(8);
     *
     * database.get<String>(aClient, "/path/to/data", options, aResult);
     */
    void get(AsyncClient &aClient, const String &path, DataOptions &options, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, false, true, false, false), &options, nullptr, &aResult, NULL);
        asyncRequest(aReq);
    }

    /**
     * Get value at the node path.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param options The database options (DataOptions).
     * @param cb The async result callback (AsyncResultCallback)
     * @return value that casts from response payload.
     *
     * Example:
     *
     * DataOptions options;
     * options.filter.orderBy("Data");
     * options.filter.startAt(105);
     * options.filter.endAt(120);
     * options.filter.limitToLast(8);
     *
     * database.get<String>(aClient, "/path/to/data", options, cb);
     */
    void get(AsyncClient &aClient, const String &path, DataOptions &options, AsyncResultCallback cb)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, false, true, false, false), &options, nullptr, nullptr, cb);
        asyncRequest(aReq);
    }

    /**
     * Get value at the node path.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileConfigCallback.
     *
     * The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
     *
     * Example of FileConfigCallback when SPIFFS used as filesystem.
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
     * @param aResult The async result (AsyncResult)
     *
     * Example:
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * database.get(aClient, "/path/to/data", getFile(fileConfig), aResult);
     *
     */
    void get(AsyncClient &aClient, const String &path, file_config_data file, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), nullptr, &file, &aResult, NULL);
        asyncRequest(aReq);
    }

    /**
     * Get value at the node path.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileConfigCallback.
     *
     * The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
     *
     * Example of FileConfigCallback when SPIFFS used as filesystem.
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
     * @param cb The async result callback (AsyncResultCallback)
     *
     * Example:
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * database.get(aClient, "/path/to/data", getFile(fileConfig), cb);
     *
     */
    void get(AsyncClient &aClient, const String &path, file_config_data file, AsyncResultCallback cb)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), nullptr, &file, nullptr, cb);
        asyncRequest(aReq);
    }

    /**
     * Checks if path exists in database
     * @param aClient The async client.
     * @param path The node path to check.
     * @return boolean value indicates the operating status.
     *
     * Example:
     *
     * bool status = database.existed(aClient, "/path/to/data");
     */
    bool existed(AsyncClient &aClient, const String &path)
    {
        AsyncResult result;
        DataOptions options;
        options.silent = true;
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(), &options, nullptr, &result, NULL);
        asyncRequest(aReq);
        return !result.database.null_etag;
    }

    /**
     * Perform OTA update using a firmware file from the database.
     * @param aClient The async client.
     * @param path The node path to download.
     * @param aResult The async result (AsyncResult)
     *
     * The data of node to download should be base64 encoded string of the firmware file.
     *
     * Example:
     *
     * database.ota(aClient, "/path/to/data", aResult);
     */
    void ota(AsyncClient &aClient, const String &path, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, false, false, false, true), nullptr, nullptr, &aResult, NULL);
        asyncRequest(aReq);
    }

    /**
     * Perform OTA update using a firmware file from the database.
     * @param aClient The async client.
     * @param path The node path to download.
     * @param cb The async result callback (AsyncResultCallback)
     *
     * The data of node to download should be base64 encoded string of the firmware file.
     *
     * Example:
     *
     * database.ota(aClient, "/path/to/data", aResult);
     */
    void ota(AsyncClient &aClient, const String &path, AsyncResultCallback cb)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_get, AsyncClient::slot_options_t(false, false, false, false, true), nullptr, nullptr, nullptr, cb);
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
     * Example:
     *
     * bool status = database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));
     */
    template <typename T = const char *>
    bool set(AsyncClient &aClient, const String &path, T value)
    {
        AsyncResult result;
        return storeAsync(aClient, path, value, async_request_handler_t::http_put, false, &result, NULL);
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
     * Example:
     *
     * database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), aResult);
     */
    template <typename T = const char *>
    void set(AsyncClient &aClient, const String &path, T value, AsyncResult &aResult)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_put, true, &aResult, NULL);
    }

    /**
     * Set value to database.
     * @param aClient The async client.
     * @param path The node path to set the value.
     * @param value The value to set.
     * @param cb The async result callback (AsyncResultCallback)
     *
     * The value type can be primitive types, Arduino String, string_t, number_t, boolean_t and object_t.
     *
     * The string_t is for string placeholder e.g. string_t("hello there").
     * The number_t is for number (integer, float, double) placeholder with decimal places e.g. number_t(123.45678, 2).
     * The boolean_t is for boolean placeholder e.g. boolean_t(true).
     * The object_t is for JSON and JSON Array objects placeholder e.g. object_t("{\"name\":\"Jack\"}") or object_t("[123,true,\"hello\"]").
     *
     * Example:
     *
     * database.set<number_t>(aClient, "/path/to/data", number_t(123.456789, 3), cb);
     */
    template <typename T = const char *>
    void set(AsyncClient &aClient, const String &path, T value, AsyncResultCallback cb)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_put, true, nullptr, cb);
    }

    /**
     * Set content from file to database.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileConfigCallback.
     *
     * The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
     *
     * Example of FileConfigCallback when SPIFFS used as filesystem.
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
     * @param aResult The async result (AsyncResult)
     *
     * Example:
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);
     *
     */
    void set(AsyncClient &aClient, const String &path, file_config_data file, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_put, AsyncClient::slot_options_t(false, false, true, false, true), nullptr, &file, &aResult, nullptr);
        asyncRequest(aReq);
    }

    /**
     * Set content from file to database.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileConfigCallback.
     *
     * The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
     *
     * Example of FileConfigCallback when SPIFFS used as filesystem.
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
     * @param cb The async result callback (AsyncResultCallback)
     *
     * Example:
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * database.set(aClient, "/path/to/data", getFile(fileConfig), cb);
     *
     */
    void set(AsyncClient &aClient, const String &path, file_config_data file, AsyncResultCallback cb)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_put, AsyncClient::slot_options_t(false, false, true, false, true), nullptr, &file, nullptr, cb);
        asyncRequest(aReq);
    }

    /**
     * Push value to database.
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
     * Example:
     *
     * bool status = database.push<number_t>(aClient, "/path/to/data", number_t(123.456789, 3));
     */
    template <typename T = const char *>
    String push(AsyncClient &aClient, const String &path, T value)
    {
        ValueConverter vcon;
        AsyncResult result;
        String payload;
        vcon.getVal<T>(payload, value);
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClient::slot_options_t(), nullptr, nullptr, &result, NULL);
        asyncRequest(aReq, payload.c_str());
        return result.database.name();
    }

    /**
     * Push content from file to database.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileConfigCallback.
     *
     * The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
     *
     * Example of FileConfigCallback when SPIFFS used as filesystem.
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
     * @param aResult The async result (AsyncResult)
     *
     * Example:
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * database.set(aClient, "/path/to/data", getFile(fileConfig), aResult);
     *
     */
    void push(AsyncClient &aClient, const String &path, file_config_data file, AsyncResult &aResult)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClient::slot_options_t(false, false, true, false, true), nullptr, &file, &aResult, nullptr);
        asyncRequest(aReq);
    }

    /**
     * Push content from file to database.
     * @param aClient The async client.
     * @param path The node path to get value.
     * @param file The filesystem data (file_config_data) obtained from FileConfig class object.
     * The FileConfig object constructor should be included filename and FileConfigCallback.
     *
     * The FileConfigCallback function contains the parameters i.e., File reference returned from file operation, filename for file operation and file_operating_mode.
     *
     * Example of FileConfigCallback when SPIFFS used as filesystem.
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
     * @param cb The async result callback (AsyncResultCallback)
     *
     * Example:
     *
     * FileConfig fileConfig("/example.txt", fileCallback);
     *
     * database.set(aClient, "/path/to/data", getFile(fileConfig), cb);
     *
     */
    void push(AsyncClient &aClient, const String &path, file_config_data file, AsyncResultCallback cb)
    {
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_post, AsyncClient::slot_options_t(false, false, true, false, true), nullptr, &file, nullptr, cb);
        asyncRequest(aReq);
    }

    /**
     * Update (patch) JSON object to database.
     * @param aClient The async client.
     * @param path The node path to update.
     * @param value The JSON object (object_t) to update.
     * @return boolean value indicates the operating status.
     *
     * Example:
     *
     * bool status = database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"));
     */
    template <typename T = object_t>
    bool update(AsyncClient &aClient, const String &path, const T &value)
    {
        AsyncResult result;
        return storeAsync(aClient, path, value, async_request_handler_t::http_patch, false, &result, NULL);
    }

    /**
     * Update (patch) JSON object to database.
     * @param aClient The async client.
     * @param path The node path to update.
     * @param value The JSON object (object_t) to update.
     * @param aResult The async result (AsyncResult)
     *
     * Example:
     *
     * bool status = database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"), aResult);
     */
    template <typename T = object_t>
    void update(AsyncClient &aClient, const String &path, const T &value, AsyncResult &aResult)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_patch, true, &aResult, NULL);
    }

    /**
     * Update (patch) JSON object to database.
     * @param aClient The async client.
     * @param path The node path to update.
     * @param value The JSON object (object_t) to update.
     * @param cb The async result callback (AsyncResultCallback)
     *
     * Example:
     *
     * bool status = database.update<object_t>(aClient, "/path/to/data", object_t("{\"name\":\"Jack\"}"), cb);
     */
    template <typename T = object_t>
    void update(AsyncClient &aClient, const String &path, const T &value, AsyncResultCallback cb)
    {
        storeAsync(aClient, path, value, async_request_handler_t::http_patch, true, nullptr, cb);
    }

    /**
     * Remove node from database
     * @param aClient The async client.
     * @param path The node path to remove.
     * @return boolean value indicates the operating status.
     *
     * Example:
     *
     * bool status = database.remove(aClient, "/path/to/data");
     */
    bool remove(AsyncClient &aClient, const String &path)
    {
        AsyncResult result;
        async_request_data_t aReq(&aClient, path, async_request_handler_t::http_delete, AsyncClient::slot_options_t(), nullptr, nullptr, &result, nullptr);
        asyncRequest(aReq);
        return result.database.null_etag && result.database.data().indexOf("null") > -1;
    }

    /**
     * Set the database URL
     * @param url The database URL.
     */
    void url(const String &url)
    {
        this->dbUrl = url;
    }

    /**
     * Perform the async task repeatedly.
     * Should be places in main loop function.
     */
    void loop()
    {
        for (size_t clientSlot = 0; clientSlot < firebase_client_list.size(); clientSlot++)
        {
            AsyncClient *aClient = reinterpret_cast<AsyncClient *>(firebase_client_list[clientSlot]);
            if (aClient)
            {
                for (size_t slot = 0; slot < aClient->slotCount(); slot++)
                {
                    aClient->process(true);
                }
                aClient->handleRemove();
            }
        }
    }

private:
    String dbUrl;
    uint32_t app_addr = 0;
    app_token_t *app_token = nullptr;

    struct async_request_data_t
    {
    public:
        AsyncClient *aClient = nullptr;
        String path;
        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;
        AsyncClient::slot_options_t opt;
        DataOptions *options = nullptr;
        file_config_data *file = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        async_request_data_t() {}
        async_request_data_t(AsyncClient *aClient, const String &path, async_request_handler_t::http_request_method method, AsyncClient::slot_options_t opt, DataOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb)
        {
            this->aClient = aClient;
            this->path = path;
            this->method = method;
            this->opt = opt;
            this->options = options;
            this->file = file;
            this->aResult = aResult;
            this->cb = cb;
        }
    };

    template <typename T = object_t>
    bool storeAsync(AsyncClient &aClient, const String &path, const T &value, async_request_handler_t::http_request_method mode, bool async, AsyncResult *aResult, AsyncResultCallback cb)
    {
        ValueConverter vcon;
        String payload;
        vcon.getVal<object_t>(payload, value);
        DataOptions options;
        if (!async)
            options.silent = true;
        async_request_data_t aReq(&aClient, path, mode, AsyncClient::slot_options_t(false, false, async, false, false), &options, nullptr, aResult, cb);
        asyncRequest(aReq, payload.c_str());
        if (!async)
            return aResult->lastError.code() == 0;
        return true;
    }

    void asyncRequest(async_request_data_t &request, const char *payload = "")
    {
        app_token_t *app_token = appToken();

        if (!app_token)
        {
            request.aResult->error_available = true;
            if (request.aResult)
                request.aResult->lastError.setClientError(FIREBASE_ERROR_APP_WAS_NOT_ASSIGNED);

            if (request.cb)
                request.cb(*request.aResult);
            return;
        }

        FirebaseApp *app = reinterpret_cast<FirebaseApp *>(app_addr);

        if (app && app->isExpired())
        {

            request.aResult->error_available = true;

            if (request.aResult)
                request.aResult->lastError.setClientError(FIREBASE_ERROR_UNAUTHENTICATE);

            if (request.cb)
                request.cb(*request.aResult);
            return;
        }

        request.opt.app_token = app_token;
        bool auth_param = app_token->auth_data_type != user_auth_data_no_token && app_token->auth_type != auth_access_token && app_token->auth_type != auth_sa_access_token;
        String extras = auth_param ? ".json?auth=" + app_token->token : ".json";

        addParams(auth_param, extras, request.method, request.options, request.file);
        AsyncClient::async_data_item_t *sData = request.aClient->newSlot(firebase_client_list, dbUrl, request.path, extras, request.method, request.opt);
        if (request.file)
        {
            sData->request.file_data.copy(*request.file);
            sData->request.base64 = true;
            request.aClient->setFileContentLength(sData);
        }
        else if (strlen(payload))
        {
            sData->request.payload = payload;
            request.aClient->setContentLength(sData, strlen(payload));
        }

        if (request.opt.ota)
        {
            sData->request.ota = true;
            sData->request.base64 = true;
        }

        if (request.cb)
            sData->cb = request.cb;
        if (request.aResult)
        {
            sData->refResult = request.aResult;
            sData->ref_result_addr = request.aResult->addr;
        }

        request.aClient->process(sData->async);
        request.aClient->handleRemove();
    }

    void addParams(bool hasQueryParams, String &extras, async_request_handler_t::http_request_method method, DataOptions *options, bool isFile)
    {
        URLHelper uh;

        if (options)
        {
            if (options->readTimeout > 0)
                uh.addParam(extras, "timeout", String(options->readTimeout) + "ms", hasQueryParams);

            uh.addParam(extras, "writeSizeLimit", options->writeSizeLimit, hasQueryParams);

            if (options->shallow)
                uh.addParam(extras, "shallow", "true", hasQueryParams, true);
        }

        if ((options && options->silent) || ((method == async_request_handler_t::http_put || method == async_request_handler_t::http_post || method == async_request_handler_t::http_patch) && isFile))
            uh.addParam(extras, "print", "silent", hasQueryParams, true);

        if (options && options->filter.complete)
            extras += options->filter.uri;
    }
};

#endif