/**
 * 2025-01-27
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 K. Suwatchai (Mobizt)
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
#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/NetConfig.h"
#include "./core/AsyncResult/AppDebug.h"

#if defined(ENABLE_ASYNC_TCP_CLIENT)
#include "./core/AsyncTCPConfig.h"
#endif

#define FIREBASE_RECONNECTION_TIMEOUT_MSEC 5000

#define FIREBASE_SESSION_TIMEOUT_SEC 150

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
    tcpc_async
};

namespace firebase_ns
{
    class ConnBase
    {
    public:
        ConnBase() {}

    protected:
        void setDebugBase(app_debug_t &app_debug, const String &msg) { app_debug.setDebug(msg); }
    };
}

struct conn_handler : public firebase_ns::ConnBase
{
private:
    network_config_data net;
    tcp_client_type client_type;
    Client *client = nullptr;
    void *atcp_config = nullptr;
    firebase_ns::app_debug_t *app_debug = nullptr;
    bool connected = false;
    int netErrState = 0;

public:
    void newConn(tcp_client_type client_type, Client *client, void *atcp_config, firebase_ns::app_debug_t *app_debug)
    {
        this->client_type = client_type;
        this->client = client;
        this->atcp_config = atcp_config;
        this->app_debug = app_debug;
    }
    void setNetwork(network_config_data &net) { this->net.copy(net); }
    unsigned long networkLastSeen() { return this->net.disconnected_ms; }
    firebase_network_type getNetworkType() { return this->net.network_type; }
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
            else if (client_type == tcpc_async)
            {
#if defined(ENABLE_ASYNC_TCP_CLIENT)
                if (atcp_config && atcp_config->tcpStatus)
                    atcp_config->tcpStatus(this->connected);
#endif
            }
        }
        return this->connected;
    }

    function_return_type connect(const char *host, uint16_t port)
    {
        function_return_type ret = ret_failure;
        if (!isConnected() && client_type == tcpc_sync)
            ret = client->connect(host, port) > 0 ? ret_complete : ret_failure;
#if defined(ENABLE_ASYNC_TCP_CLIENT)
        else if (client_type == tcpc_async)
        {
            if (async_tcp_config && async_tcp_config->tcpStatus && async_tcp_config->tcpConnect)
            {
                bool status = false;
                if (async_tcp_config->tcpStatus)
                    async_tcp_config->tcpStatus(status);

                if (!status)
                {
                    if (async_tcp_config->tcpConnect)
                        async_tcp_config->tcpConnect(host, port);

                    if (async_tcp_config->tcpStatus)
                        async_tcp_config->tcpStatus(status);
                }

                ret = status ? ret_complete : ret_continue;
            }
        }
#endif
        connected = ret == ret_complete;
        return ret;
    }

    void stop()
    {

        if (client_type == tcpc_sync)
        {
            if (client)
                client->stop();
        }
        else
        {
#if defined(ENABLE_ASYNC_TCP_CLIENT)
            if (atcp_config && atcp_config->tcpStop)
                atcp_config->tcpStop();
#endif
        }

        this->connected = false;
    }

    void setDebug(const String &msg)
    {
        if (app_debug)
            setDebugBase(*app_debug, msg);
    }

    /**
     * Checking for valid IP.
     * @return true for valid.
     */
    bool validIP(IPAddress ip)
    {
        char buf[16];
        sprintf(buf, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
        return strcmp(buf, "0.0.0.0") != 0;
    }

    /**
     * Get the ethernet link status.
     * @return true for link up or false for link down.
     */
    bool ethLinkUp()
    {
        bool ret = false;

#if defined(ENABLE_ETHERNET_NETWORK)

#if defined(FIREBASE_LWIP_ETH_IS_AVAILABLE)

#if defined(ESP32)
        if (validIP(ETH.localIP()))
        {
            ETH.linkUp();
            ret = true;
        }
#elif defined(ESP8266) || defined(CORE_ARDUINO_PICO)

        if (!net.eth)
            return false;

#if defined(ESP8266) && defined(ESP8266_CORE_SDK_V3_X_X)

#if defined(INC_ENC28J60_LWIP)
        if (net.eth->enc28j60)
        {
            ret = net.eth->enc28j60->status() == WL_CONNECTED;
            goto ex;
        }
#endif
#if defined(INC_W5100_LWIP)
        if (net.eth->w5100)
        {
            ret = net.eth->w5100->status() == WL_CONNECTED;
            goto ex;
        }
#endif
#if defined(INC_W5500_LWIP)
        if (net.eth->w5500)
        {
            ret = net.eth->w5500->status() == WL_CONNECTED;
            goto ex;
        }
#endif

#elif defined(CORE_ARDUINO_PICO)

#endif

#endif

        return ret;

#if defined(INC_ENC28J60_LWIP) || defined(INC_W5100_LWIP) || defined(INC_W5500_LWIP)
    ex:
#endif

        // workaround for ESP8266 Ethernet
        delayMicroseconds(0);

        return ret;
#endif

#endif

        return ret;
    }

    function_return_type gprsConnect()
    {

#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE)
        TinyGsm *gsmModem = (TinyGsm *)net.gsm.modem;

        if (gsmModem)
        {
            if (net.gsm.conn_status == network_config_data::gsm_conn_status_idle)
            {
                net.gsm.conn_status = network_config_data::gsm_conn_status_waits_network;

                gprsDisconnect();

                // Unlock your SIM card with a PIN if needed
                if (net.gsm.pin.length() && gsmModem->getSimStatus() != 3)
                    gsmModem->simUnlock(net.gsm.pin.c_str());

#if defined(TINY_GSM_MODEM_XBEE)
                // The XBee must run the gprsConnect function BEFORE waiting for network!
                gsmModem->gprsConnect(_apn.c_str(), _user.c_str(), _password.c_str());
#endif

                setDebug(FPSTR("Waiting for network..."));
                return ret_continue;
            }
            else if (net.gsm.conn_status == network_config_data::gsm_conn_status_waits_network)
            {
                if (!gsmModem->waitForNetwork())
                {
                    setDebug(FPSTR("Network connection failed"));
                    netErrState = 1;
                    net.network_status = false;
                    net.gsm.conn_status = network_config_data::gsm_conn_status_idle;
                    return ret_failure;
                }

                net.gsm.conn_status = network_config_data::gsm_conn_status_waits_gprs;

                setDebug(FPSTR("Network connected"));

                if (gsmModem->isNetworkConnected())
                {
                    if (netErrState == 0)
                    {
                        String debug = FPSTR("Connecting to ");
                        debug += net.gsm.apn.c_str();
                        setDebug(debug);
                    }
                }

                return ret_continue;
            }
            else if (net.gsm.conn_status == network_config_data::gsm_conn_status_waits_gprs)
            {
                if (gsmModem->isNetworkConnected())
                {

                    net.network_status = gsmModem->gprsConnect(net.gsm.apn.c_str(), net.gsm.user.c_str(), net.gsm.password.c_str()) && gsmModem->isGprsConnected();

                    if (netErrState == 0)
                        setDebug(net.network_status ? FPSTR("GPRS/EPS connected") : FPSTR("GPRS/EPS connection failed"));

                    if (!net.network_status)
                        netErrState = 1;
                }
            }

            net.gsm.conn_status = network_config_data::gsm_conn_status_idle;

            return net.network_status ? ret_complete : ret_failure;
        }

#endif
        return ret_failure;
    }

    bool gprsConnected()
    {
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE)
        TinyGsm *gsmModem = (TinyGsm *)net.gsm.modem;
        net.network_status = gsmModem && gsmModem->isGprsConnected();
#endif
        return net.network_status;
    }

    bool gprsDisconnect()
    {
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE)
        TinyGsm *gsmModem = (TinyGsm *)net.gsm.modem;
        net.network_status = gsmModem && gsmModem->gprsDisconnect();
#endif
        return !net.network_status;
    }

    function_return_type ethernetConnect()
    {

#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE) && defined(ENABLE_ETHERNET_NETWORK)

        if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_idle && net.ethernet.ethernet_cs_pin > -1)
            FIREBASE_ETHERNET_MODULE_CLASS_IMPL.init(net.ethernet.ethernet_cs_pin);

        if (net.ethernet.ethernet_reset_pin > -1)
        {
            if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_idle)
            {
                setDebug(FPSTR("Resetting Ethernet Board..."));

                pinMode(net.ethernet.ethernet_reset_pin, OUTPUT);
                digitalWrite(net.ethernet.ethernet_reset_pin, HIGH);

                net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_rst_pin_unselected;
                net.ethernet.stobe_ms = millis();
            }
            else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_rst_pin_unselected && millis() - net.ethernet.stobe_ms > 200)
            {
                digitalWrite(net.ethernet.ethernet_reset_pin, LOW);
                net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_rst_pin_selected;
                net.ethernet.stobe_ms = millis();
            }
            else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_rst_pin_selected && millis() - net.ethernet.stobe_ms > 50)
            {
                digitalWrite(net.ethernet.ethernet_reset_pin, HIGH);
                net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_rst_pin_released;
                net.ethernet.stobe_ms = millis();
            }
            else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_rst_pin_released && millis() - net.ethernet.stobe_ms > 200)
            {
                net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_begin;
            }
        }
        else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_idle)
            net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_begin;

        if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_begin)
        {

            net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_waits;
            setDebug(FPSTR("Starting Ethernet connection..."));

            if (validIP(net.ethernet.static_ip.ipAddress))
            {
                if (net.ethernet.static_ip.optional == false)
                    FIREBASE_ETHERNET_MODULE_CLASS_IMPL.begin(net.ethernet.ethernet_mac, net.ethernet.static_ip.ipAddress, net.ethernet.static_ip.dnsServer, net.ethernet.static_ip.defaultGateway, net.ethernet.static_ip.netMask);
                else if (!FIREBASE_ETHERNET_MODULE_CLASS_IMPL.begin(net.ethernet.ethernet_mac))
                {
                    FIREBASE_ETHERNET_MODULE_CLASS_IMPL.begin(net.ethernet.ethernet_mac, net.ethernet.static_ip.ipAddress, net.ethernet.static_ip.dnsServer, net.ethernet.static_ip.defaultGateway, net.ethernet.static_ip.netMask);
                }
            }
            else
                FIREBASE_ETHERNET_MODULE_CLASS_IMPL.begin(net.ethernet.ethernet_mac);

            net.eth_timer.feed(FIREBASE_ETHERNET_MODULE_TIMEOUT / 1000);
        }
        else if (net.ethernet.conn_satatus == network_config_data::ethernet_conn_status_waits)
        {

            if (FIREBASE_ETHERNET_MODULE_CLASS_IMPL.linkStatus() != LinkON && net.eth_timer.remaining() > 0)
                return ret_continue;

            net.ethernet.conn_satatus = network_config_data::ethernet_conn_status_idle;

            bool ret = ethernetConnected();

            if (ret)
            {
                String msg = FPSTR("Connected, IP: ");
                msg += FIREBASE_ETHERNET_MODULE_CLASS_IMPL.localIP().toString();
                setDebug(msg);
            }
            else
            {
                setDebug(FPSTR("Can't connect to network"));
            }

            return ret ? ret_complete : ret_failure;
        }

#endif

        return ret_continue;
    }

    bool ethernetConnected()
    {
#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE)
#if defined(ENABLE_ETHERNET_NETWORK)
        if (net.ethernet.conn_satatus != network_config_data::ethernet_conn_status_waits)
            net.network_status = FIREBASE_ETHERNET_MODULE_CLASS_IMPL.linkStatus() == LinkON && validIP(FIREBASE_ETHERNET_MODULE_CLASS_IMPL.localIP());
#else
        net.network_status = FIREBASE_ETHERNET_MODULE_CLASS_IMPL.linkStatus() == LinkON && validIP(FIREBASE_ETHERNET_MODULE_CLASS_IMPL.localIP());
#endif
#endif
        return net.network_status;
    }

    function_return_type netConnect()
    {
        if (!getNetworkStatus())
        {
            bool recon = net.reconnect;

            if (net.wifi && net.net_timer.feedCount() == 0)
                recon = true;

            // Self network connection controls.
            bool self_connect = net.network_type == firebase_network_gsm || net.network_type == firebase_network_ethernet;

            if (!self_connect && net.net_timer.remaining() == 0)
                net.net_timer.feed(FIREBASE_NET_RECONNECT_TIMEOUT_SEC);

            if (recon && (self_connect || net.net_timer.remaining() == 0))
            {

                if (net.network_type == firebase_network_generic)
                {
                    if (generic_network_owner_addr > 0 && generic_network_owner_addr != reinterpret_cast<uint32_t>(this))
                        return ret_continue;

                    if (generic_network_owner_addr == 0)
                        generic_network_owner_addr = reinterpret_cast<uint32_t>(this);

                    setDebug(FPSTR("Reconnecting to network..."));

                    if (net.generic.net_con_cb)
                        net.generic.net_con_cb();

                    generic_network_owner_addr = 0;
                }
                else if (net.network_type == firebase_network_gsm)
                {
                    if (gsm_network_owner_addr > 0 && gsm_network_owner_addr != reinterpret_cast<uint32_t>(this))
                        return ret_continue;

                    if (gsm_network_owner_addr == 0)
                        gsm_network_owner_addr = reinterpret_cast<uint32_t>(this);

                    if (gprsConnect() == ret_continue)
                        return ret_continue;

                    gsm_network_owner_addr = 0;
                }
                else if (net.network_type == firebase_network_ethernet)
                {
                    if (ethernet_network_owner_addr > 0 && ethernet_network_owner_addr != reinterpret_cast<uint32_t>(this))
                        return ret_continue;

                    if (ethernet_network_owner_addr == 0)
                        ethernet_network_owner_addr = reinterpret_cast<uint32_t>(this);

                    if (ethernetConnect() == ret_continue)
                        return ret_continue;

                    ethernet_network_owner_addr = 0;
                }
                else if (net.network_type == firebase_network_default)
                {

                    if (wifi_reconnection_ms == 0 || (wifi_reconnection_ms > 0 && millis() - wifi_reconnection_ms > FIREBASE_NET_RECONNECT_TIMEOUT_SEC * 1000))
                    {
                        wifi_reconnection_ms = millis();

                        setDebug(FPSTR("Reconnecting to WiFi network..."));

#if defined(FIREBASE_WIFI_IS_AVAILABLE)
#if defined(ESP32) || defined(ESP8266)

                        if (net.wifi && net.wifi->credentials.size())
                            net.wifi->reconnect();
                        else
                            WiFi.reconnect();
#else
                        if (net.wifi && net.wifi->credentials.size())
                            net.wifi->reconnect();
#endif
#endif
                    }
                }
            }
        }

        return getNetworkStatus() ? ret_complete : ret_failure;
    }

    bool getNetworkStatus()
    {
        bool net_status = net.network_status;

        // We will not invoke the network status request when device has built-in WiFi or Ethernet and it is connected.
        if (net.network_type == firebase_network_gsm)
        {
            net.network_status = gprsConnected();
        }
        else if (net.network_type == firebase_network_ethernet)
        {
            net.network_status = ethernetConnected();
        }
        // also check the native network before calling external cb
        else if (net.network_type == firebase_network_default || WIFI_CONNECTED || ethLinkUp() || PPP_CONNECTED)
            net.network_status = WIFI_CONNECTED || ethLinkUp() || PPP_CONNECTED;
        else if (net.network_type == firebase_network_generic)
        {
            if (!net.generic.net_status_cb)
                netErrState = 1;
            else
                net.generic.net_status_cb(net.network_status);
        }
        else
            net.network_status = false;

        if (net_status && !net.network_status)
            net.disconnected_ms = millis() - 10;

        return net.network_status;
    }
};

#endif