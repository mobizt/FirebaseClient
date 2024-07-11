# The File and BLOB config classes.


<br>

# FileConfig

## Description

The File config class.


```cpp
class FileConfig
```


## Constructors

1. ### 🔹 FileConfig(const String &filename = "", FileConfigCallback cb = NULL)


    ```cpp
    FileConfig(const String &filename = "", FileConfigCallback cb = NULL)
    ```

    **Params:**

    - `filename` - The file name of file to be read and write.

    - `cb` - The FileConfigCallback function that accepts File object, file name and `file_operating_mode` to be processed.

    The `file_operating_mode` enums are included `file_mode_open_read`, `file_mode_open_write`, `file_mode_open_append`, and `file_mode_remove`.

## Functions

1. ### 🔹 void setFile(const String &filename, FileConfigCallback cb)

    Set the File config.

    ```cpp
    void setFile(const String &filename, FileConfigCallback cb)
    ```

    **Params:**

    - `filename` - The file name of file to be read and write.

    - `cb` - The FileConfigCallback function that accepts File object, file name and `file_operating_mode` to be processed.

    The `file_operating_mode` enums are included `file_mode_open_read`, `file_mode_open_write`, `file_mode_open_append`, and `file_mode_remove`.

2. ### 🔹 void clear()
    
    Clear the internal data.

    ```cpp
    void clear()
    ```

3. ### 🔹 FileConfig &setFilename(const String &filename)
    
    Set the file name.

    ```cpp
    FileConfig &setFilename(const String &filename)
    ```

    **Params:**

    - `filename` - The file name of file to be read and write.

    **Returns:**

    - `FileConfig &` - The reference to the `FileConfig` itself.

4. ### 🔹 FileConfig &setFilename(const String &filename)
    
    Set the file name.

    ```cpp
    FileConfig &setCallback(FileConfigCallback cb)
    ```

    **Params:**

    - `cb` - The FileConfigCallback function that accepts File object, file name and file_operating_mode to be processed.

    The `file_operating_mode` enums are included `file_mode_open_read`, `file_mode_open_write`, `file_mode_open_append`, and `file_mode_remove`.

    **Returns:**

    - `FileConfig &` - The reference to the `FileConfig` itself.


5. ### 🔹 file_config_data &get()
    
    Get the reference to the internal `file_config_data`.

    ```cpp
    file_config_data &get()
    ```

    **Returns:**

    - `file_config_data &` - The reference to `file_config_data`.


<br>

# BlobConfig

## Description

The BLOB config class.


```cpp
class BlobConfig
```


## Constructors

1. ### 🔹 BlobConfig(uint8_t *data = nullptr, size_t size = 0)

    ```cpp
    BlobConfig(uint8_t *data = nullptr, size_t size = 0)
    ```

    **Params:**

    - `data` - The pointer to the uint8_t data array.
    - `size` - The size of data in bytes.

## Functions

1. ### 🔹 void setBlob(uint8_t *data, size_t size)

    Set the BLOB config.

    ```cpp
    void setBlob(uint8_t *data, size_t size)
    ```

    **Params:**

    - `data` - The pointer to the uint8_t data array.
    - `size` - The size of data in bytes.

2. ### 🔹 void clear()
    
    Clear the internal data.

    ```cpp
    void clear()
    ```

3. ### 🔹 uint8_t *blob() const
    
    Get the pointer to the internal BLOB data.

    ```cpp
    uint8_t *blob() const
    ```

    **Returns:**

    - `uint8_t *` -  The pointer to uint8_t data array.

4. ### 🔹 size_t size()
    
    Get the size of data.

    ```cpp
    size_t size()
    ```

    **Returns:**

    - `size_t` -  The size of data in bytes.

5. ### 🔹 file_config_data &getData()
    
    Get the reference to the internal file_config_data.

    ```cpp
    file_config_data &getData()
    ```

    **Returns:**

    - `file_config_data &` -  The reference to the internal `file_config_data`.


## Static Functions

1. ### 🔹 static file_config_data &getFile(FileConfig &file)

    The static function to get the reference of `file_config_data` from `FileConfig`.

    ```cpp
    static file_config_data &getFile(FileConfig &file)
    ```

    **Params:**

    - `file` -  The `FileConfig` object.

    **Returns:**

    - `file_config_data &` -  The reference to the internal `file_config_data`.

2. ### 🔹 static file_config_data &getBlob(BlobConfig &blob)

    The static function to get the reference of `file_config_data` from `BlobConfig`.

    ```cpp
    static file_config_data &getBlob(BlobConfig &blob)
    ```

    **Params:**

    - `blob` -  The `BlobConfig` object.

    **Returns:**

    - `file_config_data &` -  The reference to the internal `file_config_data`.
