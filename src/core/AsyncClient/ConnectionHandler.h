#ifndef CORE_ASYNC_CLIENT_CONNECTION_HANDLER_H
#define CORE_ASYNC_CLIENT_CONNECTION_HANDLER_H

#include <Arduino.h>
#include <Client.h>
#include "./core/AsyncResult/AppLog.h"

namespace firebase_ns
{
    enum function_return_type
    {
        ret_undefined = -2,
        ret_failure = -1,
        ret_continue = 0,
        ret_complete = 1,
        ret_retry = 2
    };

    enum tcp_client_type
    {
        tcpc_none,
        tcpc_sync,
    };

    class ConnBase : public app_log_t
    {
    public:
        ConnBase() {}

    protected:
        void debugPushBackBase(app_log_t &debug_log, const String &msg) { debug_log.push_back(-1, msg); }
    };
}

using namespace firebase_ns;

struct conn_handler : public ConnBase
{
private:
    tcp_client_type client_type = tcpc_sync;
    Client *client = nullptr;
    app_log_t *debug_log = nullptr;
    bool connected = false, client_changed = false;
    int netErrState = 0;

public:
    bool sse = false, async = false;
    String host;
    uint16_t port = 443;

    conn_handler() {}

    void newConn(tcp_client_type client_type, Client *client, app_log_t *debug_log)
    {
        this->client_type = client_type;
        this->client = client;
        this->debug_log = debug_log;
    }

    void setClientChange() { client_changed = true; }

    bool isConnected()
    {
        // Re-check the client status only when the connected flag was set.
        // This prevents calling client's connected() if it was not yet connected.
        // This also prevents ESP32 Arduino Core v3.x.x's WiFiClientSecure connected() method
        // (error) warning when it was trying to read 0 byte from, and set the socket option to
        // the unopen socket.
        if (this->connected)
        {
            if (client_type == tcpc_sync)
                this->connected = client && client->connected();
        }
        return this->connected;
    }

    function_return_type connect(const char *host, uint16_t port)
    {
        function_return_type ret = ret_failure;
        if (!isConnected() && client_type == tcpc_sync)
            ret = client->connect(host, port) > 0 ? ret_complete : ret_failure;
        connected = ret == ret_complete;

        if (connected)
        {
            this->host = host;
            this->port = port;
        }

        return ret;
    }

    void stop()
    {
        if (client_type == tcpc_sync)
        {
            if (client)
                client->stop();
        }
        reset();
    }

    void reset()
    {
        host.remove(0, host.length());
        port = 0;
        this->connected = false;
        client_changed = false;
    }

    bool isChanged() { return client_changed; }

    void setDebug(const String &msg)
    {
        if (debug_log)
            debugPushBackBase(*debug_log, msg);
    }
};

#endif