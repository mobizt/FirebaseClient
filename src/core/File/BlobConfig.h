/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_FILE_BLOB_CONFIG_DATA
#define CORE_FILE_BLOB_CONFIG_DATA

#include "./core/File/FileConfigData.h"

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
#endif