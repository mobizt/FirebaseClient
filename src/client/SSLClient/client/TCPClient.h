/*
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef BSSL_TCP_CLIENT_H
#define BSSL_TCP_CLIENT_H

#if defined(USE_LIB_SSL_ENGINE) || defined(USE_EMBED_SSL_ENGINE)

#include <string>

class BSSL_TCPClient : public Client
{

protected:
    bool _use_insecure;

public:
    // The default class constructor
    BSSL_TCPClient()
    {
        setClient(nullptr);
        _ssl_client.setTimeout(120000);
        _use_insecure = false;
    }

    // The class deconstructor
    ~BSSL_TCPClient() { setClient(nullptr); }

    /**
     * Set the client.
     * @param client The pointer to Client interface.
     * @param enableSSL The ssl option; true for enable, false for disable.
     *
     * Due to the client pointer is assigned, to avoid dangling pointer,
     * client should be existed as long as it was used for transportation.
     */
    void setClient(Client *client, bool enableSSL = true)
    {
        _basic_client = client;
        _ssl_client.setClient(_basic_client, enableSSL);
    }

    /**
     * Set debug level.
     * @param level The debug level or esp_ssl_client_debug_level.
     * esp_ssl_debug_none = 0
     * esp_ssl_debug_error = 1
     * esp_ssl_debug_warn = 2
     * esp_ssl_debug_info = 3
     * esp_ssl_debug_dump = 4
     */
    void setDebugLevel(int level) { _ssl_client.setDebugLevel(level); }

    /**
     * Connect to server.
     * @param ip The server IP to connect.
     * @param port The server port to connecte.
     * @return 1 for success or 0 for error.
     */
    int connect(IPAddress ip, uint16_t port) override { return connect(ip, port, 0); }

    /**
     * Connect to server.
     * @param ip The server IP to connect.
     * @param port The server port to connect.
     * @param timeout The connection time out in miiliseconds.
     * @return 1 for success or 0 for error.
     */
    int connect(IPAddress ip, uint16_t port, int32_t timeout)
    {
        _port = port;

        if (timeout > 0)
        {
            _timeout_ms = timeout;
            if (_basic_client)
                _basic_client->setTimeout(_timeout_ms);
            _ssl_client.setTimeout(_timeout_ms);
        }

        return _ssl_client.connect(ip, port);
    }

    /**
     * Connect to server.
     * @param host The server host name.
     * @param port The server port to connect.
     * @return 1 for success or 0 for error.
     */
    int connect(const char *host, uint16_t port) override { return connect(host, port, 0); }

    /**
     * Connect to server.
     * @param host The server host name.
     * @param port The server port to connect.
     * @param timeout The connection time out in miiliseconds.
     * @return 1 for success or 0 for error.
     */
    int connect(const char *host, uint16_t port, int32_t timeout)
    {

        _host = host;
        _port = port;

        if (timeout > 0)
        {
            _timeout_ms = timeout;
            if (_basic_client)
                _basic_client->setTimeout(_timeout_ms);
            _ssl_client.setTimeout(_timeout_ms);
        }

        return _ssl_client.connect(host, port);
    }

    /**
     * Get TCP connection status.
     * @return 1 for connected or 0 for not connected.
     */
    uint8_t connected() override { return _ssl_client.connected(); }

    /**
     * Validate the last Client connection with these host and port.
     * @param host The server host name.
     * @param port The server port to connect.
     * The Client connection will be closed when the provided host or port is not match with that of last connection.
     */
    void validate(const char *host, uint16_t port) { _ssl_client.validate(host, port); }

    /**
     * Validate the last Client connection with these IP and port.
     * @param ip The server IP to connect.
     * @param port The server port to connect.
     * The Client connection will be closed when the provided IP or port is not match with that of last connection.
     */
    void validate(IPAddress ip, uint16_t port) { _ssl_client.validate(ip, port); }

    /**
     * Get available data size to read.
     * @return The avaiable data size.
     * @note Get available data directly via lwIP for non-secure mode or via mbedTLS for secure mode.
     */
    int available() override { return _ssl_client.available(); }

    /**
     * The TCP data read function.
     * @return A byte data that was successfully read or -1 for error.
     * @note Get data directly via lwIP for non-secure mode or via mbedTLS to deccrypt data for secure mode.
     */
    int read() override
    {
        uint8_t data = -1;
        int res = read(&data, 1);
        if (res < 0)
            return res;
        return data;
    }

    /**
     * The TCP data read function.
     * @param buf The data buffer.
     * @param size The length of data that read.
     * @return The size of data that was successfully read or 0 for error.
     * @note Get data directly via lwIP for non-secure mode or via mbedTLS to deccrypt data for secure mode.
     */
    int read(uint8_t *buf, size_t size) override
    {
        if (!_ssl_client.connected())
            return 0;
        return _ssl_client.read(buf, size);
    }

    /**
     * The TCP data send function.
     * @param data The data to send.
     * @return The size of data that was successfully sent or 0 for error.
     */
    int send(const char *data) { return write(reinterpret_cast<const uint8_t *>(data), strlen(data)); }

    /**
     * The TCP data print function.
     * @param data The data to print.
     * @return The size of data that was successfully print or 0 for error.
     */
    int print(const char *data) { return send(data); }

    /**
     * The TCP data print function.
     * @param data The data to print.
     * @return The size of data that was successfully print or 0 for error.
     */
    int print(const String &data) { return print(data.c_str()); }

    /**
     * The TCP data print function.
     * @param data The data to print.
     * @return The size of data that was successfully print or 0 for error.
     */
    int print(int data)
    {
        char buf[64];
        memset(buf, 0, 64);
        sprintf(buf, "%d", data);
        int ret = send(buf);
        return ret;
    }

    /**
     * The TCP data print with new line function.
     * @param data The data to print.
     * @return The size of data that was successfully print or 0 for error.
     */
    int println(const char *data)
    {
        int len = send(data);
        if (len < 0)
            return len;
        int sz = send("\r\n");
        if (sz < 0)
            return sz;
        return len + sz;
    }

    /**
     * The TCP data print with new line function.
     * @param data The data to print.
     * @return The size of data that was successfully print or 0 for error.
     */
    int println(const String &data) { return println(data.c_str()); }

    /**
     * The TCP data print with new line function.
     * @param data The data to print.
     * @return The size of data that was successfully print or 0 for error.
     */
    int println(int data)
    {
        char buf[64];
        memset(buf, 0, 64);
        sprintf(buf, "%d\r\n", data);
        int ret = send(buf);
        return ret;
    }

    /**
     * The TCP data write function.
     * @param buf The data to write.
     * @param size The length of data to write.
     * @return The size of data that was successfully written or 0 for error.
     * @note Send data directly via lwIP for non-secure mode or via mbedTLS to encrypt for secure mode.
     */
    size_t write(const uint8_t *buf, size_t size) override
    {
        if (!_ssl_client.connected())
            return 0;
        return _ssl_client.write(buf, size);
    }

    /**
     * The TCP data write function.
     * @param data The byte of data to write.
     * @return The size of data that was successfully written (1) or 0 for error.
     * @note Send data directly via lwIP for non-secure mode or via mbedTLS to encrypt for secure mode.
     */
    size_t write(uint8_t data) override { return write(&data, 1); }

    /**
     * The TCP data write function.
     * @param buf The PGM data to write.
     * @param size The length of data to write.
     * @return The size of data that was successfully written or 0 for error.
     */
    size_t write_P(PGM_P buf, size_t size) { return _ssl_client.write_P(buf, size); }

    /**
     * The TCP data write function.
     * @param buf The string data to write.
     * @return The size of data that was successfully written or 0 for error.
     */
    size_t write(const char *buf) { return write(reinterpret_cast<const uint8_t *>(buf), strlen(buf)); }

    /**
     * The TCP data write function.
     * @param stream The stream data to write.
     * @return The size of data that was successfully written or 0 for error.
     */
    size_t write(Stream &stream) { return _ssl_client.write(stream); }

    /**
     * Read one byte from Stream with time out.
     * @return The byte of data that was successfully read or -1 for timed out.
     */
    int peek() override { return _ssl_client.peek(); }

    /**
     * Disable certificate verification and ignore the authentication.
     */
    void setInsecure()
    {
        _ssl_client.setInsecure();
        _use_insecure = true;
    }

    /**
     * Enable/disable the SSL layer transport.
     * @param enable The enable option; true for enable, false to disable.
     */
    void enableSSL(bool enable) { _ssl_client.enableSSL(enable); }

    /**
     * Upgrade the current connection by setting up the SSL and perform the SSL handshake.
     *
     * @return operating result.
     */
    bool connectSSL()
    {
        if (!_ssl_client.connectSSL(_host.c_str(), _port))
        {
            stop();
            return 0;
        }
        return 1;
    }

    /**
     * Upgrade the current connection by setting up the SSL and perform the SSL handshake.
     * @param host The host to connect (unused).
     * @param port The port to connect (unused).
     * @return operating result.
     */
    bool connectSSL(const String host, uint16_t port)
    {
        (void)host;
        (void)port;
        return connectSSL();
    }

    /**
     * Stop the TCP connection and release resources.
     */
    void stop() override { _ssl_client.stop(); }

    /**
     * Set the TCP connection timeout in seconds.
     * @param seconds The TCP timeout in seconds.
     */
    int setTimeout(uint32_t seconds)
    {
        _timeout_ms = seconds * 1000;
        _ssl_client.setTimeout(_timeout_ms);
        return 1;
    }

    /**
     * Get the TCP connection timeout in seconds.
     * @return The TCP timeout in seconds.
     */
    int getTimeout() { return _ssl_client.getTimeout() / 1000; }

    /**
     * Set the SSL handshake timeout in seconds.
     * @param handshake_timeout The SSL handshake timeout in seconds.
     */
    void setHandshakeTimeout(unsigned long handshake_timeout)
    {
        _handshake_timeout = handshake_timeout * 1000;
        _ssl_client.setHandshakeTimeout(_handshake_timeout);
    }

    /**
     * Set the TCP session timeout in seconds.
     *
     * @param seconds The TCP session timeout in seconds.
     *
     * The minimum session timeout value is 60 seconds.
     * Set 0 to disable session timed out.
     *
     * If There is no data to send (write) within this period,
     * the current connection will be closed and reconnect.
     *
     * This requires when ESP32 WiFiClient was used.
     */
    void setSessionTimeout(uint32_t seconds)
    {
        if (seconds > 0 && seconds < BSSL_SSL_CLIENT_MIN_SESSION_TIMEOUT_SEC)
            seconds = BSSL_SSL_CLIENT_MIN_SESSION_TIMEOUT_SEC;
        _tcp_session_timeout = seconds;
        _ssl_client.setSessionTimeout(seconds);
    }

    /**
     * Wait for all receive buffer data read.
     */
    void flush() override
    {
        if (!_basic_client)
            return;

        while (available() > 0)
            read();
    }

    /**
     *  Sets the requested buffer size for transmit and receive
     *  @param recv The receive buffer size.
     *  @param xmit The transmit buffer size.
     */
    void setBufferSizes(int recv, int xmit) { _ssl_client.setBufferSizes(recv, xmit); }

    operator bool() override { return connected(); }

    int availableForWrite() override
    {
        {
            return _ssl_client.availableForWrite();
        };
    }

    void setSession(BearSSL_Session *session) { _ssl_client.setSession(session); };

    void setKnownKey(const PublicKey *pk, unsigned usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN) { _ssl_client.setKnownKey(pk, usages); }

    /**
     * Verify certificate's SHA256 fingerprint.
     *
     * @param fingerprint The certificate's SHA256 fingerprint data to compare with server certificate's SHA256 fingerprint.
     * @return verification result.
     */
    bool setFingerprint(const uint8_t fingerprint[20]) { return _ssl_client.setFingerprint(fingerprint); }

    bool setFingerprint(const char *fpStr) { return _ssl_client.setFingerprint(fpStr); }

    void allowSelfSignedCerts() { _ssl_client.allowSelfSignedCerts(); }

    void setTrustAnchors(const X509List *ta) { _ssl_client.setTrustAnchors(ta); }

    void setX509Time(time_t now) { _ssl_client.setX509Time(now); }

    void setClientRSACert(const X509List *cert, const PrivateKey *sk) { _ssl_client.setClientRSACert(cert, sk); }

    void setClientECCert(const X509List *cert, const PrivateKey *sk, unsigned allowed_usages, unsigned cert_issuer_key_type) { _ssl_client.setClientECCert(cert, sk, allowed_usages, cert_issuer_key_type); }

    int getMFLNStatus() { return _ssl_client.getMFLNStatus(); };

    int getLastSSLError(char *dest = NULL, size_t len = 0) { return _ssl_client.getLastSSLError(dest, len); }
#if defined(ENABLE_FS)
    void setCertStore(CertStoreBase *certStore) { _ssl_client.setCertStore(certStore); }
#endif
    bool setCiphers(const uint16_t *cipherAry, int cipherCount) { return _ssl_client.setCiphers(cipherAry, cipherCount); }

    bool setCiphers(const std::vector<uint16_t> &list) { return _ssl_client.setCiphers(list); }

    bool setCiphersLessSecure() { return _ssl_client.setCiphersLessSecure(); }

    bool setSSLVersion(uint32_t min = BR_TLS10, uint32_t max = BR_TLS12) { return _ssl_client.setSSLVersion(min, max); }

    bool probeMaxFragmentLength(IPAddress ip, uint16_t port, uint16_t len) { return _ssl_client.probeMaxFragmentLength(ip, port, len); }

    bool probeMaxFragmentLength(const char *hostname, uint16_t port, uint16_t len) { return _ssl_client.probeMaxFragmentLength(hostname, port, len); }

    bool probeMaxFragmentLength(const String &host, uint16_t port, uint16_t len) { return _ssl_client.probeMaxFragmentLength(host, port, len); }

    bool hasPeekBufferAPI() const EMBED_SSL_ENGINE_BASE_OVERRIDE { return true; }

    size_t peekAvailable() EMBED_SSL_ENGINE_BASE_OVERRIDE { return _ssl_client.peekAvailable(); }

    const char *peekBuffer() EMBED_SSL_ENGINE_BASE_OVERRIDE { return _ssl_client.peekBuffer(); }

    void peekConsume(size_t consume) EMBED_SSL_ENGINE_BASE_OVERRIDE { return _ssl_client.peekConsume(consume); }

    /**
     * Set the Root CA or CA certificate.
     * @param rootCA The Root CA or CA certificate.
     */
    void setCACert(const char *rootCA) { _ssl_client.setCACert(rootCA); }

    void setCertificate(const char *client_ca) { return _ssl_client.setCertificate(client_ca); }

    void setPrivateKey(const char *private_key) { return _ssl_client.setPrivateKey(private_key); }

    /**
     * Read and set CA cert from file (Stream).
     * @param stream The Stream interface.
     * @param size The size of data to read.
     * @return The operating result.
     */
    bool loadCACert(Stream &stream, size_t size)
    {
        const char *dest = mStreamLoad(stream, size);
        bool ret = false;
        if (dest)
        {
            setCACert(dest);
            ret = true;
        }
        return ret;
    }

    bool loadCertificate(Stream &stream, size_t size) { return _ssl_client.loadCertificate(stream, size); }

    bool loadPrivateKey(Stream &stream, size_t size) { return _ssl_client.loadPrivateKey(stream, size); }

    void clearAuthenticationSettings(){_ssl_client.clearAuthenticationSettings();}

    int connect(IPAddress ip, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key) { return _ssl_client.connect(ip, port, rootCABuff, cli_cert, cli_key); }

    int connect(const char *host, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key) { return _ssl_client.connect(host, port, rootCABuff, cli_cert, cli_key); }

    BSSL_TCPClient &operator=(const BSSL_TCPClient &other)
    {
        stop();
        setClient(other._basic_client);
        _use_insecure = other._use_insecure;
        _timeout_ms = other._timeout_ms;
        _handshake_timeout = other._handshake_timeout;
        _tcp_session_timeout = other._tcp_session_timeout;
        _ssl_client.setTimeout(_timeout_ms);
        _ssl_client.setHandshakeTimeout(_handshake_timeout);
        _ssl_client.setSessionTimeout(_tcp_session_timeout);
        if (_use_insecure)
            _ssl_client.setInsecure();
        return *this;
    }

    bool operator==(const bool value) { return bool() == value; }

    bool operator!=(const bool value) { return bool() != value; }

    bool operator==(const BSSL_TCPClient &rhs) { return _basic_client == rhs._basic_client && _port == rhs._port && _host == rhs._host; }

    bool operator!=(const BSSL_TCPClient &rhs) { return !this->operator==(rhs); };

private:
    String _host;
    uint16_t _port;
    BSSL_SSLClient _ssl_client;
    Client *_basic_client = nullptr;
    // Renameing from _timeout which also defined in parent's Stream class.
    unsigned long _timeout_ms = 15000;
    unsigned long _handshake_timeout = 60000;
    unsigned long _tcp_session_timeout = 0;

    char *mStreamLoad(Stream &stream, size_t size)
    {
        char *dest = reinterpret_cast<char *>(esp_sslclient_malloc(size + 1));
        if (!dest)
        {
            return nullptr;
        }
        if (size != stream.readBytes(dest, size))
        {
            esp_sslclient_free(&dest);
            return nullptr;
        }
        dest[size] = '\0';
        return dest;
    }
};

#endif

#endif