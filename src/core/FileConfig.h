/**
 * Created July 30, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
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
#ifndef CORE_FILE_CONFIG_H
#define CORE_FILE_CONFIG_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/Memory.h"

#if defined(ENABLE_FS)
#include <FS.h>
#define FILEOBJ File
#else

#endif

#define FIREBASE_CHUNK_SIZE 2048

enum file_operating_mode
{
    file_mode_open_read,
    file_mode_open_write,
    file_mode_open_append,
    file_mode_remove
};

struct firebase_blob_writer
{
public:
    firebase_blob_writer() {}
    void init(uint8_t *data, size_t size)
    {
        this->data = data;
        this->size = size;
        index = 0;
    }

    size_t curIndex() const { return index; }

    size_t write(const uint8_t *buf, size_t size)
    {
        uint32_t towrite = size;
        if (index + towrite > this->size)
            towrite = this->size - index;
        if (towrite > 0)
        {
            memcpy(data + index, buf, towrite);
            index += towrite;
        }
        return towrite;
    }

private:
    size_t size = 0;
    uint8_t *data = nullptr;
    uint32_t index = 0;
};

#if defined(ENABLE_FS)
typedef void (*FileConfigCallback)(FILEOBJ &file, const char *filename, file_operating_mode mode);
#endif

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
    int index = 0;
    int size = 0;
    int read = 0;
    bool enable = false;
    String location;
    int len = 0;
    resume_state state = resume_state_undefined;

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
        location.remove(0, location.length());
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

struct file_config_data
{
    enum file_operating_status
    {
        file_status_closed,
        file_status_opened
    };
#if defined(ENABLE_FS)
    FILEOBJ file;
#endif
    String filename;
    size_t file_size = 0;

#if defined(ENABLE_CLOUD_STORAGE)
    file_upload_resumable_data resumable;
#endif
#if defined(ENABLE_FS)
    FileConfigCallback cb = NULL;
#endif
    file_operating_status file_status = file_status_closed;
    uint8_t *data = nullptr;
    size_t data_pos = 0;
    size_t data_size = 0;
    bool internal_data = false;
    firebase_blob_writer outB;
    bool initialized = false;

    void clearInternalData()
    {
        if (internal_data && data && data_size > 0)
        {
            delete data;
            data = nullptr;
        }
        data_size = 0;
        internal_data = false;
    }

    void initBlobWriter(size_t size)
    {
        clearInternalData();
        data = new uint8_t[size];
        if (data)
        {
            data_size = size;
            internal_data = true;
            outB.init(data, size);
        }
    }

public:
    file_config_data &operator=(const file_config_data &rhs)
    {
        copy(rhs);
        return *this;
    }

    void copy(const file_config_data &rhs)
    {
#if defined(ENABLE_FS)
        this->file = rhs.file;
#endif
        this->filename = rhs.filename;
        this->file_size = rhs.file_size;
        this->file_status = rhs.file_status;
        this->data_pos = rhs.data_pos;
        this->data_size = rhs.data_size;
        this->data = rhs.data;
#if defined(ENABLE_FS)
        this->cb = rhs.cb;
#endif
        if (!rhs.internal_data && rhs.data && rhs.data_size > 0)
        {
            clearInternalData();
            this->data = rhs.data;
            this->data_size = rhs.data_size;
            this->internal_data = false;
        }

        this->initialized = rhs.initialized;
    }

    void clear()
    {
        clearInternalData();
        filename.remove(0, filename.length());
#if defined(ENABLE_FS)
        cb = NULL;
#endif
        data = nullptr;
        data_size = 0;
        internal_data = false;
        initialized = false;
    }
};

#if defined(ENABLE_FS)

class FileConfig
{

public:
    /**
     * File config class.
     *
     * @param filename The file name of file to be read and write.
     * @param cb The FileConfigCallback function that accepts File object, file name and file_operating_mode to be processed.
     *
     * The file_operating_mode enums are included file_mode_open_read, file_mode_open_write, file_mode_open_append, and file_mode_remove.
     */
    explicit FileConfig(const String &filename = "", FileConfigCallback cb = NULL) { setFile(filename, cb); }

    ~FileConfig() {}

    /**
     * Set the File config.
     *
     * @param filename The file name of file to be read and write.
     * @param cb The FileConfigCallback function that accepts File object, file name and file_operating_mode to be processed.
     *
     * The file_operating_mode enums are included file_mode_open_read, file_mode_open_write, file_mode_open_append, and file_mode_remove.
     */
    void setFile(const String &filename, FileConfigCallback cb)
    {
        clear();
        if (cb && filename.length())
        {
            setFilename(filename);
            setCallback(cb);
            data.initialized = true;
        }
    }

    /**
     * Clear the internal data.
     */
    void clear() { data.clear(); }

    /**
     * Set the file name.
     *
     * @param filename The file name of file to be read and write.
     */
    FileConfig &setFilename(const String &filename)
    {
        if (filename.length() > 0 && filename[0] != '/')
            data.filename += '/';
        data.filename += filename;
        if (data.cb && data.filename.length())
            data.initialized = true;
        return *this;
    }

    /**
     * Set the callback.
     *
     * @param cb The FileConfigCallback function that accepts File object, file name and file_operating_mode to be processed.
     *
     * The file_operating_mode enums are included file_mode_open_read, file_mode_open_write, file_mode_open_append, and file_mode_remove.
     */
    FileConfig &setCallback(FileConfigCallback cb)
    {
        data.cb = cb;
        if (data.cb && data.filename.length())
            data.initialized = true;
        return *this;
    }

    /**
     * Get the reference to the internal file_config_data.
     * @return file_config_data & The reference to file_config_data.
     */
    file_config_data &get() { return data; }

private:
    file_config_data data;
};

#endif

class DefaultFileConfig
{

public:
    template <typename T = const char *>
    DefaultFileConfig() {}
    ~DefaultFileConfig() {}
    file_config_data &get() { return data; }

private:
    file_config_data data;
};

class BlobConfig
{

public:
    /**
     * BLOB config class.
     *
     * @param data The pointer to the uint8_t data array.
     * @param size The size of data in bytes.
     */
    explicit BlobConfig(uint8_t *data = nullptr, size_t size = 0) { setBlob(data, size); }

    ~BlobConfig() {}

    /**
     * Set the BLOB config.
     *
     * @param data The pointer to the uint8_t data array.
     * @param size The size of data in bytes.
     */
    void setBlob(uint8_t *data, size_t size)
    {
        clear();
        if (data && size > 0)
        {
            this->data.data = data;
            this->data.data_size = size;
            this->data.internal_data = false;
        }

        this->data.initialized = true;
    }

    /**
     * Clear the internal data.
     */
    void clear() { data.clear(); }

    /**
     * Get the pointer to the internal BLOB data.
     *
     * @return uint8_t* The pointer to uint8_t data array.
     */
    uint8_t *blob() const { return data.data; }

    /**
     * Get the data size.
     *
     * @return size_t The size of data in bytes.
     */
    size_t size() const { return data.data_size; }

    /**
     * Get the reference to the internal file_config_data.
     *
     * @return file_config_data & The reference to the internal file_config_data.
     */
    file_config_data &getData() { return data; }

private:
    file_config_data data;
};

/**
 * The static function to get the reference of file_config_data from FileConfig.
 *
 * @return file_config_data & The reference to the internal file_config_data.
 */
template <typename T>
static file_config_data &getFile(T &file) { return file.get(); }

/**
 * The static function to get the reference of file_config_data from BlobConfig.
 *
 * @return file_config_data & The reference to the internal file_config_data.
 */
template <typename T>
static file_config_data &getBlob(T &blob) { return blob.getData(); }

#endif