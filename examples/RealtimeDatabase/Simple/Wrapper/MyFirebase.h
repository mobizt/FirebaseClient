#ifndef MY_FIREBASE_H
#define MY_FIREBASE_H

#include <Arduino.h>
#include <FirebaseClient.h>

/**
 * The simple, reliable and efficient Firebase wrapper class example.
 *
 * Powered by FirebaseClient library.
 */
class MyFirebase
{
public:
    MyFirebase()
    {
        for (int i = 0; i < max_stream_client_num; i++)
            stream_client[i] = nullptr;
    }

    void begin(Client &client, const String &databaseUrl)
    {
        NoAuth no_auth;
        beginInternal(client, getNetwork(net), getAuth(no_auth), databaseUrl);
    }

    void userBegin(Client &client, const String &databaseUrl, const String &apiKey, const String &email, const String &password)
    {
        UserAuth user_auth(apiKey, email, password);
        beginInternal(client, getNetwork(net), getAuth(user_auth), databaseUrl);
    }

    void serviceBegin(Client &client, const String &databaseUrl, uint32_t timestamp, const String &clientEmail, const String &projectId, const String &privateKey)
    {
        ServiceAuth service_auth(timestamp, clientEmail, projectId, privateKey, 3000);
        beginInternal(client, getNetwork(net), getAuth(service_auth), databaseUrl);
    }

    void customBegin(Client &client, const String &databaseUrl, uint32_t timestamp, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &scope = "", const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
    {
        CustomAuth custom_auth(timestamp, apiKey, clientEmail, projectId, privateKey, uid, scope, claims, 3000);
        beginInternal(client, getNetwork(net), getAuth(custom_auth), databaseUrl);
    }

    void accessBegin(Client &client, const String &databaseUrl, const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL, const String &refresh = "", const String &client_id = "", const String &client_secret = "")
    {
        AccessToken access_token(token, expire, refresh, client_id, client_secret);
        beginInternal(client, getNetwork(net), getAuth(access_token), databaseUrl);
    }

    void idBegin(Client &client, const String &databaseUrl, const String &api_key, const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL, const String &refresh = "")
    {
        IDToken id_token(api_key, token, expire, refresh);
        beginInternal(client, getNetwork(net), getAuth(id_token), databaseUrl);
    }

    void legacyBegin(Client &client, const String &databaseUrl, const String &databaseSecret)
    {
        LegacyToken legacy_token(databaseSecret);
        beginInternal(client, getNetwork(net), getAuth(legacy_token), databaseUrl);
    }

    bool ready()
    {
        if (app.isInitialized())
        {
            JWT.loop(app.getAuth());
            app.loop();
            Database.loop();
        }

        return app.isInitialized() && app.ready();
    }

    bool available()
    {
        clearData();
        bool res = availableInternal(authResult);
        res |= availableInternal(streamResult);
        return res;
    }

    bool isStream() const { return is_stream; }
    bool isEvent() const { return is_event; }
    bool isDebug() const { return is_debug; }
    bool isError() const { return is_error; }
    bool isPayload() const { return is_payload; }

    String taskId() const { return task_id; }
    String eventString() const { return event_str; }
    int eventCode() const { return event_code; }
    String debugString() const { return debug_str; }
    String errorString() const { return err_str; }
    int errorCode() { return err_code; }
    String payload() const { return payload_str; }

    // The Stream payload might contain many events data due to
    // the events are constantly changing.
    uint32_t StreamEventCount() { return stream_payload.size(); }
    String StreamData(uint32_t index = 0) const { return stream_payload[index < stream_payload.size() ? index : 0]; }
    String StreamDataPath(uint32_t index = 0) const { return stream_data_path[index < stream_data_path.size() ? index : 0]; }
    String StreamEvent(uint32_t index = 0) const { return stream_event[index < stream_event.size() ? index : 0]; }
    int StreamDataType(uint32_t index = 0) const { return stream_data_type[index < stream_data_type.size() ? index : 0]; }

    String getString(const String &path) { return Database.get<String>(aClient, path); }
    int getInt(const String &path) { return Database.get<int>(aClient, path); }
    int getBool(const String &path) { return Database.get<bool>(aClient, path); }
    float getFloat(const String &path) { return Database.get<float>(aClient, path); }
    int getDouble(const String &path) { return Database.get<double>(aClient, path); }

    bool setString(const String &path, const String &data) { return Database.set<String>(aClient, path, data); }
    bool setInt(const String &path, int data) { return Database.set<int>(aClient, path, data); }
    bool setBool(const String &path, bool data) { return Database.set<bool>(aClient, path, data); }
    bool setFloat(const String &path, float data, int decimalPlaces = 2) { return Database.set<number_t>(aClient, path, number_t(data, decimalPlaces)); }
    bool setDouble(const String &path, double data, int decimalPlaces = 4) { return Database.set<number_t>(aClient, path, number_t(data, decimalPlaces)); }
    bool setJson(const String &path, const String &json) { return Database.set<object_t>(aClient, path, object_t(json)); }

    String pushString(const String &path, const String &data) { return Database.push<String>(aClient, path, data); }
    String pushInt(const String &path, int data) { return Database.push<int>(aClient, path, data); }
    String pushBool(const String &path, bool data) { return Database.push<bool>(aClient, path, data); }
    String pushFloat(const String &path, float data, int decimalPlaces = 2) { return Database.push<number_t>(aClient, path, number_t(data, decimalPlaces)); }
    String pushDouble(const String &path, double data, int decimalPlaces = 4) { return Database.push<number_t>(aClient, path, number_t(data, decimalPlaces)); }
    String pushJson(const String &path, const String &json) { return Database.push<object_t>(aClient, path, object_t(json)); }

    bool updateNode(const String &path, const String &json) { return Database.update<object_t>(aClient, path, object_t(json)); }

    bool exists(const String &path) { return Database.existed(aClient, path); }

    bool remove(const String &path) { return Database.remove(aClient, path); }

    void beginStream(Client &stream_client, const String &path, const String &filter = "")
    {
        this->stream_client[current_stream_index] = &stream_client;

        streamClient[current_stream_index].setNetwork(stream_client, getNetwork(net));

        Database.setSSEFilters(filter);

        // All results will store in one streamResult, the data of one Stream can be replaced by another Stream.
        // To avoid this issue, use StreamCallback or define the AsyncResults for each Stream.
        Database.get(streamClient[current_stream_index], path, streamResult, true);
        current_stream_index++;

        if (current_stream_index > max_stream_client_num - 1)
            current_stream_index = 0;
    }

    void pause(bool enable)
    {
        if (enable)
            app.getApp<RealtimeDatabase>(Database);
        else
        {
            Database.resetApp();
            aClient.stopAsync(true);

            for (int i = 0; i < max_stream_client_num; i++)
                streamClient[i].stopAsync(true);

            if (client)
                client->stop();

            for (int i = 0; i < max_stream_client_num; i++)
            {
                if (stream_client[i])
                    stream_client[i]->stop();
            }
        }
    }

private:
    static const int max_stream_client_num = 2;
    Client *client = nullptr, *stream_client[max_stream_client_num];
    AsyncClientClass aClient, streamClient[max_stream_client_num];

    DefaultNetwork net;
    FirebaseApp app;
    RealtimeDatabase Database;
    AsyncResult authResult, streamResult;

    bool is_event = false, is_debug = false, is_error = false, is_payload = false, is_stream = false;
    String task_id, payload_str, debug_str, event_str, err_str;
    std::vector<String> stream_payload, stream_event, stream_data_path;
    std::vector<int> stream_data_type;
    int event_code = 0, err_code = 0, current_stream_index = 0;

    void beginInternal(Client &client, network_config_data &net, user_auth_data &auth, const String &databaseUrl)
    {
        this->client = &client;
        aClient.setNetwork(client, net);
        Database.url(databaseUrl);
        initializeApp(aClient, app, auth, authResult);
        app.getApp<RealtimeDatabase>(Database);
    }

    bool availableInternal(AsyncResult &res)
    {
        // Once AsyncResult::isXXX was called, the information should be kept in
        // the temporary buffer because that information is gone
        // in the next loop running.

        bool event_status = res.isEvent();
        if (event_status)
        {
            event_str = res.appEvent().message();
            event_code = res.appEvent().code();
        }

        bool debug_status = res.isDebug();
        if (debug_status)
            debug_str = res.debug();

        bool error_status = res.isError();
        if (error_status)
        {
            err_str = res.error().message();
            err_code = res.error().code();
        }

        bool payload_status = res.available();
        if (payload_status)
            payload_str = res.c_str();

        is_event |= event_status;
        is_debug |= debug_status;
        is_error |= error_status;
        is_payload |= payload_status;

        if (payload_status)
        {
            RealtimeDatabaseResult &RTDB = res.to<RealtimeDatabaseResult>();
            is_stream |= RTDB.isStream();

            if (is_stream)
            {
                for (uint32_t i = 0; i < RTDB.eventCount(); i++)
                {
                    stream_payload.push_back(RTDB.to<const char *>(i));
                    stream_data_type.push_back(RTDB.type(i));
                    stream_data_path.push_back(RTDB.dataPath(i));
                    stream_event.push_back(RTDB.event(i));
                }
            }
        }

        if (event_status || debug_status || error_status || payload_status)
            task_id = res.uid();

        return is_event || is_debug || is_error || is_payload;
    }

    void clearData()
    {
        is_stream = false;
        is_debug = false;
        is_event = false;
        is_error = false;
        is_payload = false;
        debug_str.remove(0, debug_str.length());
        payload_str.remove(0, payload_str.length());
        event_str.remove(0, event_str.length());
        err_str.remove(0, err_str.length());
        stream_payload.clear();
        stream_data_type.clear();
        stream_data_path.clear();
        stream_event.clear();
        task_id.remove(0, task_id.length());
    }
};

#endif