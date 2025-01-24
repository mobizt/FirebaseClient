#ifndef MY_FIREBASE_H
#define MY_FIREBASE_H

#include <Arduino.h>
#include <FirebaseClient.h>

/**
 * The example custom Realtime Database class that wraps everything inside. 
*/
class MyFirebase
{
private:
    static const int max_stream_client_num = 2;
    int current_stream_index = 0;
    Client *client = nullptr, *stream_client[max_stream_client_num];
    DefaultNetwork net;
    AsyncClientClass aClient, streamClient[max_stream_client_num];

    FirebaseApp app;
    RealtimeDatabase Database;
    AsyncResult authResult, streamResult;
    bool is_authenticated = false, is_enabled = true, is_jwt = false;
    bool is_event = false, is_debug = false, is_error = false, is_payload = false, is_stream = false;
    String task_id, payload_str, stream_payload, stream_event, stream_data_path, debug_str, event_str, err_str;
    int event_code = 0, err_code = 0, stream_data_type = 0;

    void clearData()
    {
        is_stream = false;
        is_debug = false;
        is_event = false;
        is_error = false;
        is_payload = false;
        stream_data_type = false;
        debug_str.remove(0, debug_str.length());
        payload_str.remove(0, payload_str.length());
        event_str.remove(0, event_str.length());
        err_str.remove(0, err_str.length());
        stream_payload.remove(0, stream_payload.length());
        stream_event.remove(0, stream_event.length());
        stream_data_path.remove(0, stream_data_path.length());
        task_id.remove(0, task_id.length());
    }

    bool available(AsyncResult &res)
    {
        // Once AsyncResult::isXXX was called, the information should be kept in
        // the temporary buffer because that information is gone
        // in the next run.

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
                stream_payload = RTDB.to<const char *>();
                stream_data_type = RTDB.type();
                stream_data_path = RTDB.dataPath();
                stream_event = RTDB.event();
            }
        }

        if (event_status || debug_status || error_status || payload_status)
            task_id = res.uid();

        return is_event || is_debug || is_error || is_payload;
    }

public:
    MyFirebase()
    {
        for (int i = 0; i < max_stream_client_num; i++)
            stream_client[i] = nullptr;
    }

    void begin(Client &client, const String &databaseUrl)
    {
        NoAuth no_auth;
        is_jwt = false;
        is_enabled = true;
        this->client = &client;
        aClient.setNetwork(client, getNetwork(net));
        Database.url(databaseUrl);
        initializeApp(aClient, app, getAuth(no_auth), authResult);
        app.getApp<RealtimeDatabase>(Database);
    }

    void userBegin(Client &client, const String &databaseUrl, const String &apiKey, const String &email, const String &password)
    {
        UserAuth user_auth(apiKey, email, password);
        is_jwt = false;
        is_enabled = true;
        this->client = &client;
        aClient.setNetwork(client, getNetwork(net));
        Database.url(databaseUrl);
        initializeApp(aClient, app, getAuth(user_auth), authResult);
        app.getApp<RealtimeDatabase>(Database);
    }

    void legacyBegin(Client &client, const String &databaseUrl, const String &databaseSecret)
    {
        LegacyToken legacy_token(databaseSecret);
        is_jwt = false;
        is_enabled = true;
        this->client = &client;
        aClient.setNetwork(client, getNetwork(net));
        Database.url(databaseUrl);
        initializeApp(aClient, app, getAuth(legacy_token), authResult);
        app.getApp<RealtimeDatabase>(Database);
    }

    void serviceBegin(Client &client, const String &databaseUrl, uint32_t timestamp, const String &clientEmail, const String &projectId, const String &privateKey)
    {
        ServiceAuth sa_auth(timestamp, clientEmail, projectId, privateKey, 3000);
        is_jwt = true;
        is_enabled = true;
        this->client = &client;
        aClient.setNetwork(client, getNetwork(net));
        Database.url(databaseUrl);
        initializeApp(aClient, app, getAuth(sa_auth), authResult);
        app.getApp<RealtimeDatabase>(Database);
    }

    void customBegin(Client &client, const String &databaseUrl, uint32_t timestamp, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &scope = "", const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
    {
        CustomAuth custom_auth(timestamp, apiKey, clientEmail, projectId, privateKey, uid, scope, claims, 3000);
        is_jwt = true;
        is_enabled = true;
        this->client = &client;
        aClient.setNetwork(client, getNetwork(net));
        Database.url(databaseUrl);
        initializeApp(aClient, app, getAuth(custom_auth), authResult);
        app.getApp<RealtimeDatabase>(Database);
    }

    bool ready()
    {
        is_authenticated = is_enabled && app.isInitialized() && app.ready();

        if (is_enabled && is_jwt)
            JWT.loop(app.getAuth());

        // Required for async tasks (auth and stream).
        app.loop();
        Database.loop();

        return is_authenticated;
    }

    bool available()
    {
        clearData();
        bool res = available(authResult);
        res |= available(streamResult);
        return res;
    }

    bool isStream() { return is_stream; }
    bool isEvent() { return is_event; }
    bool isDebug() { return is_debug; }
    bool isError() { return is_error; }
    bool isPayload() { return is_payload; }

    String taskId() { return task_id; }
    String eventString() { return event_str; }
    int eventCode() { return event_code; }
    String debugString() { return debug_str; }
    String errorString() { return err_str; }
    int errorCode() { return err_code; }
    String payload() { return payload_str; }

    String StreamData() { return stream_payload; }
    String StreamDataPath() { return stream_data_path; }
    String StreamEvent() { return stream_event; }
    int StreamDataType() { return stream_data_type; }

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

        Database.get(streamClient[current_stream_index], path, streamResult, true);
        current_stream_index++;

        if (current_stream_index > max_stream_client_num)
            current_stream_index = 0;
    }

    void pause(bool enable)
    {
        is_enabled = enable;

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
};

#endif