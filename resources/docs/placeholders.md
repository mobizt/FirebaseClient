# The placeholder represents the primitive types values that used in this library.


<br>

# boolean_t

## Description

The placeholder (container) class represents the Boolean value


```cpp
class boolean_t
```


## Constructors

1. ### 🔹 boolean_t(bool v)

    Set the bool value to the placeholder.

    ```cpp
    boolean_t(bool v)
    ```

    **Params:**

    - `v` - The bool value to set.

## Functions

1. ### 🔹 const char * c_str() const
    
    Return the pointer to internal string buffer.

    ```cpp
    const char * c_str() const
    ```

    **Returns:**

    - `const char *` - The pointer to internal string buffer.

2. ### 🔹 size_t printTo(Print &p)
    
    Prints data to the serial port as human-readable ASCII text.

    ```cpp
    size_t printTo(Print &p)
    ```

    **Params:**

    - `p` - The Print object.

    **Returns:**

    - `size_t` - The length of string that printed.


<br>

# number_t

## Description

The placeholder (container) class represents the number value


```cpp
class number_t
```


## Constructors

1. ### 🔹 number_t(TV v, int d)

    Set the number types T value to the placeholder.

    ```cpp
    number_t(TV v, int d)
    ```

    **Params:**

    - `v` - The number types T value to set.
    - `d` - The deceimal places of float and double value to set.

2. ### 🔹 number_t(T o)

    Set the number and object type T value to the placeholder.
    The constructor does not gauard for non-number value.

    ```cpp
    number_t(T o)
    ```

    **Params:**

    - `o` - The number type T value to set.

## Functions

1. ### 🔹 const char * c_str() const
    
    Return the pointer to internal string buffer.

    ```cpp
    const char * c_str() const
    ```

    **Returns:**

    - `const char *` - The pointer to internal string buffer.

2. ### 🔹 size_t printTo(Print &p)
    
    Prints data to the serial port as human-readable ASCII text.

    ```cpp
    size_t printTo(Print &p)
    ```

    **Params:**

    - `p` - The Print object.

    **Returns:**

    - `size_t` - The length of string that printed.


<br>

# string_t

## Description

The placeholder (container) class represents the string value


```cpp
class string_t
```


## Constructors

1. ### 🔹 string_t(T v)

    Set the primitive and object types T value to the placeholder.
    This supports number_t and boolean_t objects.

    ```cpp
    string_t(T v)
    ```

    **Params:**

    - `v` - The primitive and object types T value to set.

## Functions

1. ### 🔹 string_t &operator+=(const T &rval)
    
    Append the value to the placeholder.

    ```cpp
    string_t &operator+=(const T &rval)
    ```

    **Params:**

    - `v` - The primitive and object types T value to append.

    **Returns:**

    - `string_t &` - The self instance.

2. ### 🔹 const char * c_str() const
    
    Return the pointer to internal string buffer.

    ```cpp
    const char * c_str() const
    ```

    **Returns:**

    - `const char *` - The pointer to internal string buffer.

3. ### 🔹 size_t printTo(Print &p)
    
    Prints data to the serial port as human-readable ASCII text.

    ```cpp
    size_t printTo(Print &p)
    ```

    **Params:**

    - `p` - The Print object.

    **Returns:**

    - `size_t` - The length of string that printed.

4. ### 🔹 void clear()
    
    Clear internal buffer.

    ```cpp
    void clear()
    ```



<br>

# object_t

## Description

The placeholder (container) class represents the JSON value


```cpp
class object_t
```


## Constructors

1. ### 🔹 object_t(T v)

    Set the primitive and object types T value to the placeholder.
    This supports number_t, boolean_t and string_t objects.

    ```cpp
    object_t(T v)
    ```

    **Params:**

    - `v` - The primitive and object types T value to set.

## Functions

1. ### 🔹 object_t &operator+=(const T &rval)
    
    Append the value to the placeholder.

    ```cpp
    string_t &operator+=(const T &rval)
    ```

    **Params:**

    - `v` - The primitive and object types T value to append.

    **Returns:**

    - `object_t &` - The self instance.

2. ### 🔹void initObject()
    
    Initiate the placeholder as JSON object representation.

    ```cpp
    void initObject()
    ```

3. ### 🔹void initArray()
    
    Initiate the placeholder as JSON Array object representation.

    ```cpp
    void initArray()
    ```

4. ### 🔹 const char * c_str() const
    
    Return the pointer to internal string buffer.

    ```cpp
    const char * c_str() const
    ```

    **Returns:**

    - `const char *` - The pointer to internal string buffer.

5. ### 🔹 size_t printTo(Print &p)
    
    Prints data to the serial port as human-readable ASCII text.

    ```cpp
    size_t printTo(Print &p)
    ```

    **Params:**

    - `p` - The Print object.

    **Returns:**

    - `size_t` - The length of string that printed.

6. ### 🔹 void clear()
    
    Clear internal buffer.

    ```cpp
    void clear()
    ```