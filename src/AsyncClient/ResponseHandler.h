#ifndef ASYNC_RESPONSE_HANDLER_H
#define ASYNC_RESPONSE_HANDLER_H
#include <Arduino.h>
#include <Client.h>

#define FIREBASE_TCP_READ_TIMEOUT 30 * 1000;

struct async_response_handler_t
{
public:
    enum response_type
    {
        RES_UNDEFINED = -1,
        RES_ERROR = 0,
        RES_OK = 1,
    };
    enum chunk_phase
    {
        READ_CHUNK_SIZE = 0,
        READ_CHUNK_DATA = 1
    };

    struct response_flags
    {
    public:
        bool header_remaining = false;
        bool payload_remaining = false;
        bool keep_alive = false;
        bool sse = false;
        bool chunks = false;
        bool payload_available = false;

        void reset()
        {
            header_remaining = false;
            payload_remaining = false;
            keep_alive = false;
            sse = false;
            chunks = false;
            payload_available = false;
        }
    };

    struct chunk_info_t
    {
        chunk_phase phase = READ_CHUNK_SIZE;
        int chunkSize = 0;
        int dataLen = 0;
    };

    struct auth_error_t
    {
        String string;
        response_type resp_type = RES_UNDEFINED;
        int resp_code = 0;
    };

    int httpCode = 0;
    response_flags flags;
    size_t payloadLen = 0;
    size_t payloadRead = 0;
    auth_error_t error;
    unsigned long dataTime = 0;
    uint8_t *toFill = nullptr;
    uint16_t toFillLen = 0;
    uint16_t toFillIndex = 0;
    String header, payload, location, etag;
    chunk_info_t chunkInfo;
    unsigned long last_response_ms = 0;
    unsigned long response_tmo = FIREBASE_TCP_READ_TIMEOUT;

    ~async_response_handler_t()
    {
        if (toFill)
            free(toFill);
        toFill = nullptr;
        toFillLen = 0;
        toFillIndex = 0;
    }
};

#endif