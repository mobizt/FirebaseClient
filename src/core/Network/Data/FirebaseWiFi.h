#ifndef CORE_NETWORK_DATA_FIREBASE_WIFI_H
#define CORE_NETWORK_DATA_FIREBASE_WIFI_H

#include <Arduino.h>
#include "./FirebaseConfig.h"

struct FirebaseWiFi_credential_t
{
    String ssid, password;
};

class FirebaseWiFi
{
    friend class conn_handler;

public:
    FirebaseWiFi() {}
    ~FirebaseWiFi()
    {
        clearAP();
        clearMulti();
    };
    void addAP(const String &ssid, const String &password)
    {
        FirebaseWiFi_credential_t data;
        data.ssid = ssid;
        data.password = password;
        credentials.push_back(data);
    }
    void clearAP() { credentials.clear(); }
    size_t size() const { return credentials.size(); }
    FirebaseWiFi_credential_t operator[](size_t index) { return credentials[index]; }

private:
    std::vector<FirebaseWiFi_credential_t> credentials;
#if defined(FIREBASE_HAS_WIFIMULTI)
#if defined(ESP8266)
    ESP8266WiFiMulti *multi = nullptr;
#else
    WiFiMulti *multi = nullptr;
#endif
#endif
    void reconnect()
    {
        if (credentials.size())
        {
            disconnect();
            connect();
        }
    }

    void connect()
    {
#if defined(FIREBASE_HAS_WIFIMULTI)
        clearMulti();
#if defined(ESP8266)
        multi = new ESP8266WiFiMulti();
#else
        multi = new WiFiMulti();
#endif
        for (size_t i = 0; i < credentials.size(); i++)
            multi->addAP(credentials[i].ssid.c_str(), credentials[i].password.c_str());

        if (credentials.size() > 0)
            multi->run();

#elif defined(FIREBASE_WIFI_IS_AVAILABLE)
        WiFi.begin((CONST_STRING_CAST)credentials[0].ssid.c_str(), credentials[0].password.c_str());
#endif
    }

    void disconnect()
    {
#if defined(FIREBASE_WIFI_IS_AVAILABLE)
        WiFi.disconnect();
#endif
    }

    void clearMulti()
    {
#if defined(FIREBASE_HAS_WIFIMULTI)
        if (multi)
            delete multi;
        multi = nullptr;
#endif
    }
};
#endif