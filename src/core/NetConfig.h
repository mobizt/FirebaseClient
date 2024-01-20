/**
 * Created November 9, 2023
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 K. Suwatchai (Mobizt)
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
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/Network.h"
#include "Memory.h"

struct network_config_data
{
    friend class DefaultNetwork;
    friend class GenericNetwork;
    friend class EthernetNetwork;
    friend class GSMNetwork;
    friend class DefaultEthernetNetwork;
    friend class DefaultWiFiNetwork;
    friend class AsyncClient;

private:
    struct generic_data
    {
        NetworkConnectionCallback net_con_cb = NULL;
        NetworkStatusCallback net_status_cb = NULL;

    public:
        void copy(generic_data &rhs)
        {
            this->net_con_cb = rhs.net_con_cb;
            this->net_status_cb = rhs.net_status_cb;
        }
        void clear()
        {
            net_con_cb = NULL;
            net_status_cb = NULL;
        }
    };

    struct gsm_data
    {
        String pin, apn, user, password;
        void *modem = nullptr;

    public:
        void copy(gsm_data &rhs)
        {
            this->modem = rhs.modem;
            this->pin = rhs.pin;
            this->apn = rhs.apn;
            this->user = rhs.user;
            this->password = rhs.password;
        }
        void clear()
        {
            modem = nullptr;
            pin.clear();
            apn.clear();
            user.clear();
            password.clear();
        }
    };

    struct ethernet_data
    {
        int ethernet_reset_pin = -1;
        int ethernet_cs_pin = -1;
        uint8_t *ethernet_mac = nullptr;
        Firebase_StaticIP *static_ip = nullptr;

    public:
        void copy(ethernet_data &rhs)
        {
            this->ethernet_reset_pin = rhs.ethernet_reset_pin;
            this->ethernet_cs_pin = rhs.ethernet_cs_pin;
            this->ethernet_mac = rhs.ethernet_mac;
            this->static_ip = rhs.static_ip;
        }
        void clear()
        {
            ethernet_mac = nullptr;
            ethernet_cs_pin = -1;
            ethernet_reset_pin = -1;
            static_ip = nullptr;
        }
    };

    firebase_network_data_type network_data_type = firebase_network_data_undefined;

    generic_data generic;
    gsm_data gsm;
    ethernet_data ethernet;
    bool initialized = false;
    bool network_status = false;
    firebase_wifi *wifi = nullptr;
    SPI_ETH_Module *eth = NULL;
    unsigned long net_reconnect_ms =0;
    unsigned long net_reconnect_timeout = 10000;

public:
    ~network_config_data() { clear(); }
    network_config_data &operator=(network_config_data rhs)
    {
        copy(rhs);
        return *this;
    }

    void copy(network_config_data &rhs)
    {
        this->network_data_type = rhs.network_data_type;
        this->initialized = rhs.initialized;
        generic.copy(rhs.generic);
        gsm.copy(rhs.gsm);
        ethernet.copy(rhs.ethernet);
    }

    void clear()
    {
        generic.clear();
        gsm.clear();
        ethernet.clear();
        network_data_type = firebase_network_data_undefined;
        initialized = false;
        network_status = false;
    }
};

class DefaultNetwork
{

public:
    DefaultNetwork()
    {
        init();
        network_data.network_data_type = firebase_network_data_default_network;
    }
    ~DefaultNetwork() { clear(); }
    void clear()
    {
        network_data.clear();
    }
    network_config_data &get() { return network_data; }

protected:
    network_config_data network_data;

    void init()
    {
        clear();
        network_data.initialized = true;
    }
};

class GenericNetwork : public DefaultNetwork
{
public:
    GenericNetwork(NetworkConnectionCallback networkConnectionCB, NetworkStatusCallback networkStatusCB)
    {
        init();
        network_data.generic.net_con_cb = networkConnectionCB;
        network_data.generic.net_status_cb = networkStatusCB;
        network_data.network_data_type = firebase_network_data_generic_network;
    }
    ~GenericNetwork() { clear(); }
};

class GSMNetwork : public DefaultNetwork
{
public:
    template <typename T1 = const char *, typename T2 = const char *, typename T3 = const char *, typename T4 = const char *>
    GSMNetwork(void *modem, T1 pin, T2 apn, T3 user, T4 password)
    {
        init();
        network_data.gsm.modem = modem;
        network_data.gsm.pin = toStringPtr(pin);
        network_data.gsm.apn = toStringPtr(apn);
        network_data.gsm.user = toStringPtr(user);
        network_data.gsm.password = toStringPtr(password);
        network_data.network_data_type = firebase_network_data_gsm_network;
    }
    ~GSMNetwork() { clear(); }
};

class EthernetNetwork : public DefaultNetwork
{

public:
    EthernetNetwork(uint8_t macAddress[6], int csPin, int resetPin, Firebase_StaticIP *staticIP = nullptr)
    {
        init();
        network_data.ethernet.ethernet_mac = macAddress;
        network_data.ethernet.ethernet_cs_pin = csPin;
        network_data.ethernet.ethernet_reset_pin = resetPin;
        network_data.ethernet.static_ip = staticIP;
        network_data.network_data_type = firebase_network_data_ethernet_network;
    }
    ~EthernetNetwork() { clear(); }
};

class DefaultEthernetNetwork : public DefaultNetwork
{

public:
    DefaultEthernetNetwork(SPI_ETH_Module *eth)
    {
        init();
        network_data.eth = eth;
        network_data.network_data_type = firebase_network_data_default_network;
    }
    ~DefaultEthernetNetwork() { clear(); }
};


class DefaultWiFiNetwork : public DefaultNetwork
{

public:
    DefaultWiFiNetwork(firebase_wifi *wifi)
    {
        init();
        network_data.wifi = wifi;
        network_data.network_data_type = firebase_network_data_default_network;
    }
    ~DefaultWiFiNetwork() { clear(); }
};

template <typename T>
static network_config_data &getNetwork(T &net) { return net.get(); }

#endif