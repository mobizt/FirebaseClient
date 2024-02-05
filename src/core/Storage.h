/**
 * Created February 5, 2024
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
#ifndef CORE_STORAGE_H
#define CORE_STORAGE_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/Memory.h"

#if defined(ENABLE_FS)
#include <FS.h>
#define FILEOBJ File
#else
class dmFile : public Stream
{
public:
    operator bool() { return 0; }
    size_t write(uint8_t) { return 0; }
    size_t write(const uint8_t *buf, size_t size) { return 0; }
    int available() { return 0; }
    int read() { return 0; }
    int peek() { return 0; }
    void flush() {}
    int read(uint8_t *buf, size_t size) { return 0; }
    void close() {}
    size_t position() { return 0; }
    size_t size() { return 0; }
};
#define FILEOBJ dmFile
#endif

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

typedef void (*FileConfigCallback)(FILEOBJ &file, const char *filename, file_operating_mode mode);

struct file_config_data
{
    enum file_operating_status
    {
        file_status_closed,
        file_status_opened
    };

    FILEOBJ file;
    String filename;
    size_t file_size = 0;
    FileConfigCallback cb = NULL;
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
            Memory mem;
            mem.release(&data);
        }
        data_size = 0;
        internal_data = false;
    }

    void initBlobWriter(size_t size)
    {
        clearInternalData();
        Memory mem;
        data = reinterpret_cast<uint8_t *>(mem.alloc(size));
        if (data)
        {
            data_size = size;
            internal_data = true;
            outB.init(data, size);
        }
    }

public:
    file_config_data &operator=(file_config_data rhs)
    {
        copy(rhs);
        return *this;
    }

    void copy(file_config_data rhs)
    {
        this->file = rhs.file;
        this->filename = rhs.filename;
        this->cb = rhs.cb;

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
        filename.clear();
        cb = NULL;
        data = nullptr;
        data_size = 0;
        internal_data = false;
        initialized = false;
    }
};

class FileConfig
{

public:
    FileConfig(const String &filename, FileConfigCallback cb)
    {
        clear();
        if (filename.length() > 0 && filename[0] != '/')
            data.filename += '/';
        data.filename += filename;

        data.cb = cb;
        data.initialized = true;
    }
    ~FileConfig() {}
    void clear() { data.clear(); }

    file_config_data &get() { return data; }

private:
    file_config_data data;
};

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
    BlobConfig(uint8_t *data = nullptr, size_t size = 0)
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
    ~BlobConfig() {}
    void clear() { data.clear(); }

    uint8_t *blob() const { return data.data; }
    size_t size() const { return data.data_size; }

    file_config_data &getData() { return data; }

private:
    file_config_data data;
};

template <typename T>
static file_config_data &getFile(T &file) { return file.get(); }

template <typename T>
static file_config_data &getBlob(T &blob) { return blob.getData(); }

#endif