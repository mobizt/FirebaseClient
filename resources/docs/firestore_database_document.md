
# Document

## Description

Google Cloud Firestore database Document class


```cpp
class Document
```

## Constructors

1. ### 🔹 Document(const String &name = "")

A Firestore document constructor with document resource name.

```cpp
Document(const String &name = "")
```

**Params:**

- `name` - The resource name of the document.

2. ### 🔹 Document(const String &key, T value)

A Firestore document constructor with `Values::Value` object. See [Firestore Value class and functions](/resources/docs/firestore_database_values.md).

```cpp
Document(const String &key, T value)
```

**Params:**

- `key` - The key of an object.

- `value` - The value of an `Values::Value` object.

## Functions

1. ### 🔹 Document &add(const String &key, T value)

Add the `Values::Value` object to Firestore document. See [Firestore Value class and functions](/resources/docs/firestore_database_values.md).

```cpp
Document &add(const String &key, T value)
```

**Params:**

- `key` - The key of an `Values::Value` object.

- `value` - The value of an `Values::Value` object.

2. ### 🔹 void setName(const String &name)

Set the document resource name.

```cpp
void setName(const String &name)
```

**Params:**

- `name` - The resource name of the document.
