#ifndef CORE_FILE_RESUMABLE_UPLOAD_H
#define CORE_FILE_RESUMABLE_UPLOAD_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/Utils/StringUtil.h"

#if defined(ENABLE_CLOUD_STORAGE)
struct file_upload_resumable_data
{
private:
    enum resume_state
    {
        resume_state_undefined,
        resume_state_send_header,
        resume_state_send_payload,
        resume_state_read_response
    };
    
    int index = 0, size = 0, read = 0, len = 0;
    bool enable = false;
    String location;
    resume_state state = resume_state_undefined;
    StringUtil sut;

public:
    file_upload_resumable_data() {}
    bool isEnabled() { return enable; }
    void setSize(size_t size)
    {
        this->size = size;
        enable = size > 0;
    }
    void getRange()
    {
        int minChunkSize = 256 * 1024; // required by google
        read = size - index <= minChunkSize ? size - index : minChunkSize;
    }
    void updateRange()
    {
        index += read;
        getRange();
        len = read;
    }
    int getChunkSize(int size, int payloadIndex, int dataIndex)
    {
        int chunkSize = size - dataIndex < FIREBASE_CHUNK_SIZE ? size - dataIndex : FIREBASE_CHUNK_SIZE;

        if (payloadIndex + chunkSize > index + read)
            chunkSize = index + read - payloadIndex;

        return chunkSize;
    }
    void updateState(uint32_t &payloadIndex)
    {
        if (location.length() > 0)
        {
            if (state == resume_state_send_header)
            {
                state = resume_state_send_payload;
                payloadIndex = index;
            }
            else if (state == resume_state_send_payload)
            {
                state = resume_state_undefined;
                clear();
            }
        }
    }
    bool isUpload() { return state == resume_state_send_payload; }
    void setHeaderState()
    {
        if (location.length())
            state = resume_state_send_header;
    }
    int isComplete(int size, int payloadIndex)
    {
        if (payloadIndex < index + read)
            return 1;
        else if (payloadIndex < size)
        {
            state = resume_state_read_response;
            return 2;
        }
        return 0;
    }
    void getHeader(String &header, const String &host, const String &ext)
    {
        header = FPSTR("PUT ");
        header += ext;
        header += FPSTR(" HTTP/1.1\r\nHost: ");
        header += host;
        header += FPSTR("\r\nConnection: keep-alive\r\nContent-Length: ");
        header += len;
        header += FPSTR("\r\nContent-Range: bytes ");
        header += index;
        header += '-';
        header += index + read - 1;
        header += '/';
        header += size;
        header += FPSTR("\r\n\r\n");
    }
    void clear()
    {
        sut.clear(location);
        index = 0;
        size = 0;
        read = 0;
        enable = false;
        len = 0;
    }
    String &getLocationRef() { return location; }
    String getLocation() { return location.c_str(); }
};
#endif
#endif