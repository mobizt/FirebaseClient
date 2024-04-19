
# JsonWriter

## Description

The object_t placeholder companion utility class for writing JSON


```cpp
class JsonWriter
```


## Constructors

1. ### 🔹 JsonWriter()

    The JsonWriter constructor.

## Functions

1. ### 🔹 void create(object_t &obj, const String &path, T value)
    
    Create a JSON object. 

    When the path contains "/", the nested JSON object will be created.

    The value supports primitive types values, number_t, boolean_t, string_t objects and object_t.

    ```cpp
    void create(object_t &obj, const String &path, T value)
    ```

    **Params:**

    - `obj` - The object_t to strore the value.
    
    - `path` - The JSON path or key.

    - `value` - The value to set. 

2. ### 🔹 void join(object_t &obj, int nunArgs, ...)
    
    Put multiple object_t(s) in to a target object_t.

    ### Example
    ```cpp
    
    object_t json, obj1, obj2, obj3, obj4;
    JsonWriter writer;

    writer.create(obj1, "int/value", 9999);
    writer.create(obj2, "string/value", string_t("hello"));
    writer.create(obj3, "float/value", number_t(123.456, 2));
    writer.join(obj4, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);
    writer.create(json, "node/list", obj4);

    object_t arr;
    arr.initArray(); // initialize to be used as array
    writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

    Serial.println(json);

    Serial.println(arr);

    ```

    ```cpp
    void join(object_t &obj, int nunArgs, ...)
    ```

    **Params:**

    - `obj` - The target object_t to store the object_t(s).

    - `nunArgs` - The numbers of object_t to store.

    - `...` - The object_t(s) list.

