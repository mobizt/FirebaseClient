# The network config classes.


<br>

# DefaultNetwork

## Description

The base class that provides the network data or default network class for built-in WiFi boards.


```cpp
class DefaultNetwork
```


## Constructors

1. ### 🔹 DefaultNetwork(bool reconnect = true)

    Set the bool value to the placeholder.

    ```cpp
    DefaultNetwork(bool reconnect = true)
    ```

    **Params:**

    - `reconnect` - The boolean option for network reconnection which applied for WiFi network only.

## Functions

1. ### 🔹 void clear()
    
    Clear the internal network data.

    ```cpp
    void clear()
    ```

2. ### 🔹 network_config_data &get()
    
    Get the reference to the internal `network_config_data`.

    ```cpp
    network_config_data &get()
    ```

    **Returns:**

    - `network_config_data &` - The reference to internal `network_config_data`.


<br>

# GenericNetwork

## Description

The network class works for all networking interfaces e.g. Ethernet Module, WiFI Module, Bluetooth Module, NB-IoT module, and LoRa Module that have the internet access ability.


```cpp
class GenericNetwork
```


## Constructors

1. ### 🔹 GenericNetwork(NetworkConnectionCallback networkConnectionCB, NetworkStatusCallback networkStatusCB)

    In the networkConnectionCB function, the complete operations for the carier (network) and internet connection should be performed and waits until the internet connection was established.
    
    In the networkStatusCB function, the `status` (Boolean variable) that provided in the function, should set with the network status.
 
    See [GenericNetwork example](/examples/App/NetworkInterfaces/Async/Callback/GenericNetwork) for using WiFi with GenericNetwork for demonstation.

    ```cpp
    GenericNetwork(NetworkConnectionCallback networkConnectionCB, NetworkStatusCallback networkStatusCB)
    ```

    **Params:**

    - `networkConnectionCB` - The network connection callback function.
    - `networkStatusCB` - The network status callback function.

## Functions

1. ### 🔹 void clear()
    
    Clear the internal network data.

    ```cpp
    void clear()
    ```

2. ### 🔹 network_config_data &get()
    
    Get the reference to the internal `network_config_data`.

    ```cpp
    network_config_data &get()
    ```

    **Returns:**

    - `network_config_data &` - The reference to internal `network_config_data`.


<br>

# GSMNetwork

## Description

The GSM network class works for [TinyGSM](https://github.com/vshymanskyy/TinyGSM) library only.


```cpp
class GSMNetwork
```


## Constructors

1. ### 🔹 GSMNetwork(TinyGsm *modem, const String &pin, const String &apn, const String &user, const String &password)

    This required two places of the GSM module macro definitions, one in your sketch and another in `/src/Config.h` or in your own defined config at `src/UserConfig.h` or adding `TINY_GSM_MODEM_XXXXXXX` in compiler build flags.
    
    The TinyGsm modem should be defined at the same usage scope of `GSMNetwork` and `AsyncClientClass`.

    See [GSMNetwork example](/examples/App/NetworkInterfaces/Async/Callback/GSMNetwork/) for using TinyGSM with this library.

    ```cpp
    GSMNetwork(TinyGsm *modem, const String &pin, const String &apn, const String &user, const String &password)
    ```

    **Params:**

    - `modem` - The pointer to TinyGsm modem object. Modem should be initialized and/or set the mode before transfering the data.
    - `pin` - The SIM pin.
    - `apn` - The GPRS APN (Access Point Name).
    - `user` - The GPRS user.
    - `password` - The GPRS password.

## Functions

1. ### 🔹 void clear()
    
    Clear the internal network data.

    ```cpp
    void clear()
    ```

2. ### 🔹 network_config_data &get()
    
    Get the reference to the internal `network_config_data`.

    ```cpp
    network_config_data &get()
    ```

    **Returns:**

    - `network_config_data &` - The reference to internal `network_config_data`.



<br>

# EthernetNetwork

## Description

The Ethernet network class for generic Ethernet modules.


```cpp
class EthernetNetwork
```


## Constructors

1. ### 🔹 EthernetNetwork(uint8_t macAddress[6], int csPin, int resetPin)

    By default the external Ethernet module can be used with the library when the macro `ENABLE_ETHERNET_NETWORK` was defined and Ethernet library was included in the user sketch.
    
    The user defined Ethernet class and header other than `Ethernet.h` and `Ethernet` can be used, see [Library Build Options](https://github.com/mobizt/FirebaseClient?tab=readme-ov-file#library-build-options) for how to.

    ```cpp
    EthernetNetwork(uint8_t macAddress[6], int csPin, int resetPin)
    ```

    **Params:**

    - `macAddress` - The mac address.
    - `csPin` - The Ethernet module chip select/enable pin.
    - `resetPin` - The Ethernet module reset pin. Assign -1 if not used.
    - `staticIP` - (Optional) The pointer to Firebase_StaticIP object that holds the static ip configuration.

2. ### 🔹 EthernetNetwork(uint8_t macAddress[6], int csPin, int resetPin, const Firebase_StaticIP &staticIP)

    By default the external Ethernet module can be used with the library when the macro `ENABLE_ETHERNET_NETWORK` was defined and Ethernet library was included in the user sketch.
    
    The user defined Ethernet class and header other than `Ethernet.h` and `Ethernet` can be used, see [Library Build Options](https://github.com/mobizt/FirebaseClient?tab=readme-ov-file#library-build-options) for how to.

    ```cpp
    EthernetNetwork(uint8_t macAddress[6], int csPin, int resetPin, const Firebase_StaticIP &staticIP)
    ```

    **Params:**

    - `macAddress` - The mac address.
    - `csPin` - The Ethernet module chip select/enable pin.
    - `resetPin` - The Ethernet module reset pin. Assign -1 if not used.
    - `staticIP` - The `Firebase_StaticIP` object that holds the static ip configuration. See [Firebase_StaticIP](#firebase_staticip) for more detail.

## Functions

1. ### 🔹 void clear()
    
    Clear the internal network data.

    ```cpp
    void clear()
    ```

2. ### 🔹 network_config_data &get()
    
    Get the reference to the internal `network_config_data`.

    ```cpp
    network_config_data &get()
    ```

    **Returns:**

    - `network_config_data &` - The reference to internal `network_config_data`.


<br>

# DefaultEthernetNetwork

## Description

The default Ethernet class for ESP8266 with lwIP Ethernet.


```cpp
class DefaultEthernetNetwork
```


## Constructors
    
1. ### 🔹 DefaultEthernetNetwork(Firebase_SPI_ETH_Module &eth)

    This eth should be defined at the same usage scope of AsyncClientCalss.

    To use ESP8266 native lwIP Ethernet, the one of following macros, `ENABLE_ESP8266_ENC28J60_ETH`, `ENABLE_ESP8266_W5500_ETH` and `ENABLE_ESP8266_W5100_ETH` should be defined in src/Config.h or in your own defined config at src/UserConfig.h or adding `ENABLE_ESP8266_ENC28J60_ETH`, `ENABLE_ESP8266_W5500_ETH` and `ENABLE_ESP8266_W5100_ETH` in the compiler build flags.
    
    Use `Firebase_SPI_ETH_Module::enc28j60`, `Firebase_SPI_ETH_Module::w5100` and `Firebase_SPI_ETH_Module::w5500` to assign the pointer to `ENC28J60lwIP`, `Wiznet5100lwIP` and `Wiznet5500lwIP` classes objects respectively.
    
    In PlatformIO IDE, please set the `lib_ldf_mode = chain+` option in platformio.ini.
    
    See [ESP8266 DefaultEthernetNetwork example](/examples/App/NetworkInterfaces/Async/Callback/DefaultNetworks/DefaultEthernetNetwork/ESP8266/) for using ESP8266 with its native lwIP Ethernet library.

    ```cpp
    DefaultEthernetNetwork(Firebase_SPI_ETH_Module &eth)
    ```

    **Params:**

    - `eth` - The ESP8266 core SPI ethernet driver class that work with external SPI Ethernet modules that currently supported e.g. ENC28J60, Wiznet W5100 and Wiznet 5500.


    2. ### 🔹 DefaultEthernetNetwork()

    The default Ethernet class for ESP32 with native Ethernet boards.
    
    See [ESP32 DefaultEthernetNetwork example](/examples/App/NetworkInterfaces/Async/Callback/DefaultNetworks/DefaultEthernetNetwork/ESP32/) for using ESP32 with its native lwIP Ethernet library.

    ```cpp
    DefaultEthernetNetwork(Firebase_SPI_ETH_Module &eth)
    ```

## Functions

1. ### 🔹 void clear()
    
    Clear the internal network data.

    ```cpp
    void clear()
    ```

2. ### 🔹 network_config_data &get()
    
    Get the reference to the internal `network_config_data`.

    ```cpp
    network_config_data &get()
    ```

    **Returns:**

    - `network_config_data &` - The reference to internal `network_config_data`.

<br>


# DefaultWiFiNetwork

## Description

The WiFi network class with multiple APs supports.


```cpp
class DefaultWiFiNetwork
```


## Constructors

1. ### 🔹 DefaultWiFiNetwork(FirebaseWiFi &wifi, bool reconnect = true)

    The FirebaseWiFi class holds the WiFi credentials list. The AP and password can be added to list with `FirebaseWiFi::addAP`.
    
    The FirebaseWiFi object should be defined at the same usage scope of `DefaultWiFiNetwork` and `AsyncClientClass`.

    ```cpp
    DefaultWiFiNetwork(FirebaseWiFi &wifi, bool reconnect = true)
    ```

    **Params:**

    - `wifi` - The FirebaseWiFi class object that used for keeping the network credentials (WiFi APs and WiFi passwords).
    - `reconnect` - The bool option for network reconnection. It should set with `false` when the WiFi reconnection was controlled by your code or third-party library e.g. WiFiManager.


## Functions

1. ### 🔹 void clear()
    
    Clear the internal network data.

    ```cpp
    void clear()
    ```

2. ### 🔹 network_config_data &get()
    
    Get the reference to the internal `network_config_data`.

    ```cpp
    network_config_data &get()
    ```

    **Returns:**

    - `network_config_data &` - The reference to internal `network_config_data`.


<br>


# Firebase_StaticIP

## Description

The Firebase Client Static address provider class.


```cpp
class Firebase_StaticIP
```


## Constructors

1. ### 🔹 Firebase_StaticIP(IPAddress ipAddress, IPAddress netMask, IPAddress defaultGateway, IPAddress dnsServer, bool optional)

    
    ```cpp
    Firebase_StaticIP(IPAddress ipAddress, IPAddress netMask, IPAddress defaultGateway, IPAddress dnsServer, bool optional)
    ```

    **Params:**

    - `ipAddress` - The static IP.
    - `netMask` - The subnet IP.
    - `defaultGateway` - The default gateway IP.
    - `dnsServer` - The dns server IP.
    - `optional` - The boolean option to force use static IP only (not use DHCP).

