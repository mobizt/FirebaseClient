
# Value

## Description

A message that can hold any of the supported value types.

This class is in the `Values` namespace.


```cpp
class Value
```

## Constructors

1. ### 🔹 Value()

The default constructor.

```cpp
Value()
```

**Params:**

- `name` - The resource name of the document.

2. ### 🔹 Value(T value)

The constructor with the object value.

See the below supported objects classes.

```cpp
Value(T value)
```

**Params:**

- `value` - The object value.


# NullValue

## Description

A null value.

This class is in the `Values` namespace.

```cpp
class NullValue
```

## Constructors

1. ### 🔹 NullValue()

The default constructor.

```cpp
NullValue()
```


# StringValue

## Description

A string value.

This class is in the `Values` namespace.

```cpp
class StringValue
```

## Constructors

1. ### 🔹 StringValue(const String &value)

The constructor with string value.

```cpp
StringValue(const String &value)
```

**Params:**

- `value` - The string value.


# BooleanValue

## Description

A boolean value.

This class is in the `Values` namespace.

```cpp
class BooleanValue
```

## Constructors

1. ### 🔹 BooleanValue(bool value)

The constructor with boolean value.

```cpp
BooleanValue(bool value)
```

**Params:**

- `value` - The boolean value.


# IntegerValue

## Description

A 64-bit signed integer value.

This class is in the `Values` namespace.

```cpp
class IntegerValue
```

## Constructors

1. ### 🔹 IntegerValue(int value)

The constructor with a 64-bit signed integer value.

```cpp
IntegerValue(int64_t value)
```

**Params:**

- `value` - The 64-bit signed integer value.



# DoubleValue

## Description

A double value.

This class is in the `Values` namespace.

```cpp
class DoubleValue
```

## Constructors

1. ### 🔹 DoubleValue(double value)

The constructor with double value.

This constructor is obsoleted.

```cpp
DoubleValue(double value)
```

**Params:**

- `value` - The double value.

2. ### 🔹 DoubleValue(const number_t &value)

The constructor with `number_t` represents the double value.

```cpp
DoubleValue(const number_t &value)
```

**Params:**

- `value` - The `number_t` represents the double value.

# TimestampValue

## Description

A timestampValue value.

This class is in the `Values` namespace.

```cpp
class TimestampValue
```

## Constructors

1. ### 🔹 TimestampValue(const String &value)

The constructor with timestamp value string.

Precise only to microseconds. When stored, any additional precision is rounded down.

```cpp
TimestampValue(const String &value)
```

**Params:**

- `value` - The timestamp value string.


# BytesValue

## Description

A bytes value.

This class is in the `Values` namespace.

```cpp
class BytesValue
```

## Constructors

1. ### 🔹 BytesValue(const String &value)

The constructor with timestamp (base64-encoded) value string.

Must not exceed 1 MiB - 89 bytes. Only the first 1,500 bytes are considered by queries.

```cpp
BytesValue(const String &value)
```

**Params:**

- `value` - The bytes (base64-encoded) value string.



# ReferenceValue

## Description

A reference to a document.

This class is in the `Values` namespace.

```cpp
class ReferenceValue
```

## Constructors

1. ### 🔹 ReferenceValue(const String &value)

The constructor with resource name string.

```cpp
ReferenceValue(const String &value)
```

**Params:**

- `value` - The resource name of document.


# GeoPointValue

## Description

A geo point representing a point on the surface of Earth.

This class is in the `Values` namespace.

```cpp
class GeoPointValue
```

## Constructors

1. ### 🔹 GeoPointValue(double lat, double lng)

The constructor with latitude and longitude.

This constructor is obsoleted.

```cpp
GeoPointValue(double lat, double lng)
```

**Params:**

- `lat` - The latitude.

- `lng` - The longitude.

2. ### 🔹 GeoPointValue(const number_t &lat, const number_t &lng)

The constructor with latitude and longitude.

This constructor is obsoleted.

```cpp
GeoPointValue(const number_t &lat, const number_t &lng)
```

**Params:**

- `lat` - The `number_t` represents the latitude value.

- `lng` - The `number_t` represents the latitude value.


# ArrayValue

## Description

An array value.

This class is in the `Values` namespace.

```cpp
class ArrayValue
```

## Constructors

1. ### 🔹 ArrayValue(T value)

The constructor with object.

Cannot directly contain another array value, though can contain an map which contains another array.

```cpp
ArrayValue(T value)
```

**Params:**

- `value` - The object except for array value.


# MapValue

## Description

A map value.

This class is in the `Values` namespace.

```cpp
class MapValue
```

## Constructors

1. ### 🔹 MapValue(const String &key, T value)

The constructor with key and object value.

```cpp
MapValue(const String &key, T value)
```

**Params:**

- `key` - The key.

- `value` - The object value.

## Functions

1. ### 🔹 MapValue &add(const String &key, T value)

Add key and value pair to a map.

```cpp
MapValue &add(const String &key, T value)
```

**Params:**

- `key` - The key.

- `value` - The object value.
