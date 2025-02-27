# File

## Description

 The class represent File which containing source content.

```cpp
class File
```

## Constructors

1. ### 🔹 explicit File(const String &content, const String &name, const String &fingerprint = "")

A File class constructor in the Rules namespace.

```cpp
explicit File(const String &content, const String &name, const String &fingerprint = "")
```

**Params:**

- `content` - Rules language statements. Use \n for line breaks.
- `name` - File name.
- `fingerprint` - Fingerprint (e.g. github sha) associated with the File. A base64-encoded string.

## Functions

1. ### 🔹 void content(const String &content)

Rules language statements. Use \n for line breaks.

```cpp
void content(const String &content)
```

**Params:**

- `content` - Rules language statements. Use \n for line breaks.

2. ### 🔹 void name(const String &name)

File name.

```cpp
void name(const String &name)
```

**Params:**

- `name` - File name.


3. ### 🔹 void fingerprint(const String &fingerprint)

Fingerprint (e.g. github sha) associated with the File. A base64-encoded string.

```cpp
void fingerprint(const String &fingerprint)
```

**Params:**

- `fingerprint` - Base64 encoded string of File fingerprint.

# Source

## Description

 The class represent Source which is one or more File messages comprising a logical set of rules.

```cpp
class Source
```

## Constructors

1. ### 🔹 explicit Source(const File &file)

A File class constructor in the Rules namespace.

```cpp
explicit Source(const File &file)
```

**Params:**

- `file` - File containing source content.

## Functions

1. ### 🔹 void files(const File &file)

Append the File containing source content.

```cpp
void files(const File &file)
```

**Params:**

- `file` - File containing source content.

1. ### 🔹 void clear()

Clear all Files in the Source object.

```cpp
void clear()
```


# Ruleset

## Description

 The class represents Ruleset which is an immutable copy of Source with a globally unique identifier and a creation time.


```cpp
class Ruleset
```

## Constructors

1. ### 🔹 Ruleset(const Rules::Source &source, const String &attachment_point)

A Ruleset class constructor with source and attachment_point.

```cpp
Ruleset(const Rules::Source &source, const String &attachment_point)
```

**Params:**

- `source` - Source for the Ruleset.
- `attachment_point` - Intended resource to which this Ruleset should be released. May be left blank to signify the resource associated with the default release. Expected format: firestore.googleapis.com/projects/<project number>/databases/<uuid>


## Functions

1. ### 🔹 void source(const Rules::Source &source)

Source for the Ruleset.

```cpp
void source(const Rules::Source &source)
```

**Params:**

- `source` - Source for the Ruleset.

2. ### 🔹 void attachmentPoint(const String &attachment_point)

Intended resource to which this Ruleset should be released. May be left blank to signify the resource associated with the default release. Expected format: firestore.googleapis.com/projects/<project number>/databases/<uuid>

```cpp
void attachmentPoint(const String &attachment_point)
```

**Params:**

- `attachment_point` - Intended resource to which this Ruleset should be released.


# ListOptions

## Description

 The class represents the REST API request's query parameters.


```cpp
class ListOptions
```

## Functions

1. ### 🔹 ListOptions &pageSize(int value)

Page size to load. Maximum of 100. Defaults to 10.
Note: pageSize is just a hint and the service may choose to load less than pageSize due to the size of the output.
To traverse all of the releases, caller should iterate until the pageToken is empty.

```cpp
ListOptions &pageSize(int value)
```

**Params:**

- `value` -  Optional. The page size.

2. ### 🔹 ListOptions &pageToken(const String &value)

Next page token for loading the next batch of Ruleset instances.

```cpp
ListOptions &pageToken(const String &value)
```

**Params:**

- `value` - The page token.

3. ### 🔹 ListOptions &filter(const String value)

Ruleset/Release filter.

For Ruleset, the list method supports filters with restrictions on Ruleset.name.
Filters on Ruleset.create_time should use the date function which parses strings that conform to the RFC 3339 date/time specifications.
Example: create_time > date("2017-01-01T00:00:00Z") AND name=UUID-*


For Release, the list method supports filters with restrictions on the Release.name, and Release.ruleset_name.
Example 1: A filter of 'name=prod*' might return Releases with names within 'projects/foo' prefixed with 'prod':
Name -> Ruleset Name:
projects/foo/releases/prod -> projects/foo/rulesets/uuid1234
projects/foo/releases/prod/v1 -> projects/foo/rulesets/uuid1234
projects/foo/releases/prod/v2 -> projects/foo/rulesets/uuid8888
Example 2: A filter of name=prod* rulesetName=uuid1234 would return only Release instances for 'projects/foo' with names prefixed with 'prod' referring to the same Ruleset name of 'uuid1234':
Name -> Ruleset Name:
projects/foo/releases/prod -> projects/foo/rulesets/1234
projects/foo/releases/prod/v1 -> projects/foo/rulesets/1234

In the examples, the filter parameters refer to the search filters are relative to the project. Fully qualified prefixed may also be used.
   

```cpp
ListOptions &filter(const String value)
```

**Params:**

- `value` - Optional. The filter.


# Release

## Description

 Release is a named reference to a Ruleset. Once a Release refers to a Ruleset, rules-enabled services will be able to enforce the Ruleset.

```cpp
class Release
```
## Constructors

1. ### 🔹 explicit Release(const String &releaseId, const String &rulesetId)

The Release class constructor with release ID and ruleset ID.

```cpp
explicit Release(const String &releaseId, const String &rulesetId)
```
The release Id is the part of the release name i.e. the release name structure `"projects/projectId/releaseId"`.

Release name/id should reflect the developer's deployment practices. For example, the release Id may include the environment name, application name, application version, or any other name meaningful to the developer.

Once a Release refers to a Ruleset, the rules can be enforced by Firebase Rules-enabled services.

More than one Release may be 'live' concurrently. Consider the following three Release names for projects/foo and the Ruleset to which they refer.

`Release Name` -> `Ruleset Name`:
`projects/foo/releases/prod` -> `projects/foo/rulesets/uuid123`
`projects/foo/releases/prod/beta` -> `projects/foo/rulesets/uuid123`
`projects/foo/releases/prod/v23` -> `projects/foo/rulesets/uuid456`

The relationships reflect a Ruleset rollout in progress. The prod and prod/beta releases refer to the same Ruleset. However, prod/v23 refers to a new Ruleset. The Ruleset reference for a Release may be updated using the releases.patch method.


**Params:**

- `releaseId` - The release ID.
- `rulesetId` - The ruleset ID.

## Functions

1. ### 🔹 void releaseId(const String &releaseId)

The release Id is the part of the release name i.e. the release name structure `"projects/projectId/releaseId"`.

Release name/id should reflect the developer's deployment practices. For example, the release Id may include the environment name, application name, application version, or any other name meaningful to the developer.

Once a Release refers to a Ruleset, the rules can be enforced by Firebase Rules-enabled services.

More than one Release may be 'live' concurrently. Consider the following three Release names for projects/foo and the Ruleset to which they refer.

`Release Name` -> `Ruleset Name`:
`projects/foo/releases/prod` -> `projects/foo/rulesets/uuid123`
`projects/foo/releases/prod/beta` -> `projects/foo/rulesets/uuid123`
`projects/foo/releases/prod/v23` -> `projects/foo/rulesets/uuid456`

The relationships reflect a Ruleset rollout in progress. The prod and prod/beta releases refer to the same Ruleset. However, prod/v23 refers to a new Ruleset. The Ruleset reference for a Release may be updated using the releases.patch method.

```cpp
void releaseId(const String &releaseId)
```

**Params:**

- `releaseId` -  The release ID.

2. ### 🔹 void rulesetId(const String &rulesetId)

The ruleset ID.

```cpp
void rulesetId(const String &rulesetId)
```

**Params:**

- `rulesetId` -  The ruleset ID.


# RuleSets

## Description

 The class to hadle ruleset operations.


```cpp
class RuleSets
```

## Functions

1. ### 🔹 void resetApp()

Unbind or remove FirebaseApp.


```cpp
void resetApp()
```


2. ## 🔹  void loop()

Perform the async task repeatedly (DEPRECATED).

```cpp
void loop()
```


3. ## 🔹  void create(AsyncClientClass &aClient, const String &projectId, const Ruleset &ruleSet, AsyncResultCallback cb, const String &uid = "")

Create a Ruleset from Source.

The Ruleset is given a unique generated name which is returned to the caller. Source containing syntactic or semantics errors will result in an error response indicating the first error encountered.

```cpp
void create(AsyncClientClass &aClient, const String &projectId, const Ruleset &ruleSet, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `ruleSet` - The ruleset object.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

4. ## 🔹  void create(AsyncClientClass &aClient, const String &projectId, const Ruleset &ruleSet, AsyncResult &aResult)

Create a Ruleset from Source.

The Ruleset is given a unique generated name which is returned to the caller. Source containing syntactic or semantics errors will result in an error response indicating the first error encountered.

```cpp
void create(AsyncClientClass &aClient, const String &projectId, const Ruleset &ruleSet, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `ruleSet` - The ruleset object.
- `aResult` - The async result (AsyncResult).

5. ## 🔹  String create(AsyncClientClass &aClient, const String &projectId, const Ruleset &ruleSet)

Create a Ruleset from Source.

The Ruleset is given a unique generated name which is returned to the caller. Source containing syntactic or semantics errors will result in an error response indicating the first error encountered.

```cpp
String create(AsyncClientClass &aClient, const String &projectId, const Ruleset &ruleSet)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `ruleSet` - The ruleset object.

**Returns:**

- `String` - The response payload.

6. ## 🔹  void remove(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResultCallback cb, const String &uid = "")

Create a Ruleset from Source.

The Ruleset is given a unique generated name which is returned to the caller. Source containing syntactic or semantics errors will result in an error response indicating the first error encountered.

```cpp
void remove(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `rulesetId` - The ruleset ID.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

7. ## 🔹  void remove(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResult &aResult)

Create a Ruleset from Source.

The Ruleset is given a unique generated name which is returned to the caller. Source containing syntactic or semantics errors will result in an error response indicating the first error encountered.

```cpp
void remove(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `rulesetId` - The ruleset ID.
- `aResult` - The async result (AsyncResult).

8. ## 🔹  String remove(AsyncClientClass &aClient, const String &projectId, const String &rulesetId)

Create a Ruleset from Source.

The Ruleset is given a unique generated name which is returned to the caller. Source containing syntactic or semantics errors will result in an error response indicating the first error encountered.

```cpp
String remove(AsyncClientClass &aClient, const String &projectId, const String &rulesetId)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `rulesetId` - The ruleset ID.

**Returns:**

- `String` - The response payload.

9. ## 🔹  void get(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResultCallback cb, const String &uid = "")

Get a Ruleset by name including the full Source contents.

```cpp
void get(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `rulesetId` - The ruleset ID.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

10. ## 🔹  void get(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResult &aResult)

Get a Ruleset by name including the full Source contents.

```cpp
void get(AsyncClientClass &aClient, const String &projectId, const String &rulesetId, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `rulesetId` - The ruleset ID.
- `aResult` - The async result (AsyncResult).


11. ## 🔹  String get(AsyncClientClass &aClient, const String &projectId, const String &rulesetId)

Get a Ruleset by name including the full Source contents.

```cpp
String get(AsyncClientClass &aClient, const String &projectId, const String &rulesetId)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `rulesetId` - The ruleset ID.

**Returns:**

- `String` - The response payload.

12. ## 🔹  void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "")

List Ruleset metadata only and optionally filter the results by Ruleset name.

The full Source contents of a Ruleset may be retrieved with Rulesets::get.

```cpp
void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `listOptions` - The ListOptions object that represents the request query parameters e.g. `filter`, `pageSize` and `pageToken`.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).


13. ## 🔹  void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResult &aResult)

List Ruleset metadata only and optionally filter the results by Ruleset name.

The full Source contents of a Ruleset may be retrieved with Rulesets::get.

```cpp
void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `listOptions` - The ListOptions object that represents the request query parameters e.g. `filter`, `pageSize` and `pageToken`.
- `aResult` - The async result (AsyncResult).


14. ## 🔹  String list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions)

List Ruleset metadata only and optionally filter the results by Ruleset name.

The full Source contents of a Ruleset may be retrieved with Rulesets::get.

```cpp
String list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `listOptions` - The ListOptions object that represents the request query parameters e.g. `filter`, `pageSize` and `pageToken`.

**Returns:**

- `String` - The response payload.

15. ## 🔹  String getSha1(String payload)

Get SHA1 string

```cpp
String getSha1(String payload)
```

**Params:**

- `payload` - The string to calculate SHA1.

**Returns:**

- `String` - The SHA1 string.

# Releases

## Description

The class to perform a Release operations. Release is a named reference to a Ruleset. Once a Release refers to a Ruleset, rules-enabled services will be able to enforce the Ruleset.

The JSON representation of Release.
```
{
  "name": string,
  "rulesetName": string,
  "createTime": string,
  "updateTime": string
}
```

```cpp
class Releases
```

## Functions

1. ### 🔹 void resetApp()

Unbind or remove FirebaseApp.


```cpp
void resetApp()
```


1. ## 🔹  void loop()

Perform the async task repeatedly (DEPRECATED).

```cpp
void loop()
```

3. ## 🔹  void create(AsyncClientClass &aClient, const String &projectId, const Release &release, AsyncResultCallback cb, const String &uid = "")

Create a Ruleset from Source.

The Ruleset is given a unique generated name which is returned to the caller. Source containing syntactic or semantics errors will result in an error response indicating the first error encountered.

```cpp
void create(AsyncClientClass &aClient, const String &projectId, const Release &release, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `release` - The Release object.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

4. ## 🔹  void create(AsyncClientClass &aClient, const String &projectId, const Release &release, AsyncResult &aResult)

Create a Ruleset from Source.

The Ruleset is given a unique generated name which is returned to the caller. Source containing syntactic or semantics errors will result in an error response indicating the first error encountered.

```cpp
void create(AsyncClientClass &aClient, const String &projectId, const Release &release, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `release` - The Release object.
- `aResult` - The async result (AsyncResult).

5. ## 🔹  String create(AsyncClientClass &aClient, const String &projectId, const Release &release)

Create a Ruleset from Source.

The Ruleset is given a unique generated name which is returned to the caller. Source containing syntactic or semantics errors will result in an error response indicating the first error encountered.

```cpp
String create(AsyncClientClass &aClient, const String &projectId, const Release &release)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `release` - The Release object.

**Returns:**

- `String` - The response payload.

6. ## 🔹  void remove(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResultCallback cb, const String &uid = "")

Delete a Release by resource name.

```cpp
void remove(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `releaseId` - The release ID.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

7. ## 🔹  void remove(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResult &aResult)

Delete a Release by resource name.

```cpp
void remove(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `releaseId` - The release ID.
- `aResult` - The async result (AsyncResult).

8. ## 🔹  String remove(AsyncClientClass &aClient, const String &projectId, const String &releaseId)

Delete a Release by resource name.

```cpp
String remove(AsyncClientClass &aClient, const String &projectId, const String &releaseId)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `releaseId` - The release ID.

**Returns:**

- `String` - The response payload.

9. ## 🔹  void get(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResultCallback cb, const String &uid = "")

Get a Release by name.

```cpp
void get(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `releaseId` - The release ID.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

10. ## 🔹  void get(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResult &aResult)

Get a Release by name.

```cpp
void get(AsyncClientClass &aClient, const String &projectId, const String &releaseId, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `releaseId` - The release ID.
- `aResult` - The async result (AsyncResult).

11. ## 🔹  String get(AsyncClientClass &aClient, const String &projectId, const String &releaseId)

Get a Release by name.

```cpp
String get(AsyncClientClass &aClient, const String &projectId, const String &releaseId)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `releaseId` - The release ID.

**Returns:**

- `String` - The response payload.

12. ## 🔹  void getExecutable(AsyncClientClass &aClient, const String &projectId, const String &releaseId, Rules::ReleaseExecutableVersion executableVersion, AsyncResultCallback cb, const String &uid = "")

Get the Release executable to use when enforcing rules.

```cpp
void getExecutable(AsyncClientClass &aClient, const String &projectId, const String &releaseId, Rules::ReleaseExecutableVersion executableVersion, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `releaseId` - The release ID.
- `executableVersion` - ReleaseExecutableVersion enum.

- `RELEASE_EXECUTABLE_VERSION_UNSPECIFIED`  Executable format unspecified. Defaults to FIREBASE_RULES_EXECUTABLE_V1
- `FIREBASE_RULES_EXECUTABLE_V1`    Firebase Rules syntax 'rules2' executable versions: Custom AST for use with Java clients.
- `FIREBASE_RULES_EXECUTABLE_V2`	CEL-based executable for use with C++ clients.

The requested runtime executable version. Defaults to `FIREBASE_RULES_EXECUTABLE_V1`.

- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

13. ## 🔹  void getExecutable(AsyncClientClass &aClient, const String &projectId, const String &releaseId, Rules::ReleaseExecutableVersion executableVersion, AsyncResult &aResult)

Get the Release executable to use when enforcing rules.

```cpp
void getExecutable(AsyncClientClass &aClient, const String &projectId, const String &releaseId, Rules::ReleaseExecutableVersion executableVersion, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `releaseId` - The release ID.
- `executableVersion` - ReleaseExecutableVersion enum.

- `RELEASE_EXECUTABLE_VERSION_UNSPECIFIED`  Executable format unspecified. Defaults to FIREBASE_RULES_EXECUTABLE_V1
- `FIREBASE_RULES_EXECUTABLE_V1`    Firebase Rules syntax 'rules2' executable versions: Custom AST for use with Java clients.
- `FIREBASE_RULES_EXECUTABLE_V2`	CEL-based executable for use with C++ clients.

The requested runtime executable version. Defaults to `FIREBASE_RULES_EXECUTABLE_V1`.

- `aResult` - The async result (AsyncResult).

14. ## 🔹  String getExecutable(AsyncClientClass &aClient, const String &projectId, const String &releaseId, Rules::ReleaseExecutableVersion executableVersion)

Get the Release executable to use when enforcing rules.

```cpp
String getExecutable(AsyncClientClass &aClient, const String &projectId, const String &releaseId, Rules::ReleaseExecutableVersion executableVersion)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `releaseId` - The release ID.
- `executableVersion` - ReleaseExecutableVersion enum.

- `RELEASE_EXECUTABLE_VERSION_UNSPECIFIED`  Executable format unspecified. Defaults to FIREBASE_RULES_EXECUTABLE_V1
- `FIREBASE_RULES_EXECUTABLE_V1`    Firebase Rules syntax 'rules2' executable versions: Custom AST for use with Java clients.
- `FIREBASE_RULES_EXECUTABLE_V2`	CEL-based executable for use with C++ clients.

The requested runtime executable version. Defaults to `FIREBASE_RULES_EXECUTABLE_V1`.

**Returns:**

- `String` - The response payload.

15. ## 🔹  void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "")

List the Release values for a project. This list may optionally be filtered by Release name, Ruleset name, TestSuite name, or any combination thereof.

```cpp
void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `listOptions` - The ListOptions object that represents the request query parameters e.g. `filter`, `pageSize` and `pageToken`.
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

16. ## 🔹  void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResult &aResult)

List the Release values for a project. This list may optionally be filtered by Release name, Ruleset name, TestSuite name, or any combination thereof.

```cpp
void list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `listOptions` - The ListOptions object that represents the request query parameters e.g. `filter`, `pageSize` and `pageToken`.
- `aResult` - The async result (AsyncResult).

17. ## 🔹  String list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions)

List the Release values for a project. This list may optionally be filtered by Release name, Ruleset name, TestSuite name, or any combination thereof.

```cpp
String list(AsyncClientClass &aClient, const String &projectId, const ListOptions &listOptions)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `listOptions` - The ListOptions object that represents the request query parameters e.g. `filter`, `pageSize` and `pageToken`.

**Returns:**

- `String` - The response payload.

18. ## 🔹  void patch(AsyncClientClass &aClient, const String &projectId, const Release &release, const String &updateMask, AsyncResultCallback cb, const String &uid = "")

Update a Release via PATCH.

Only updates to rulesetName will be honored. Release rename is not supported. To create a Release use the releases.create method.

```cpp
void patch(AsyncClientClass &aClient, const String &projectId, const Release &release, const String &updateMask, AsyncResultCallback cb, const String &uid = "")
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `release` - The Release object to update.
- `updateMask` - Specifies which fields to update. 
This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".
- `cb` - The async result callback (AsyncResultCallback).
- `uid` - The user specified UID of async result (optional).

19. ## 🔹  void patch(AsyncClientClass &aClient, const String &projectId, const Release &release, const String &updateMask, AsyncResult &aResult)

Update a Release via PATCH.

Only updates to rulesetName will be honored. Release rename is not supported. To create a Release use the releases.create method.

```cpp
void patch(AsyncClientClass &aClient, const String &projectId, const Release &release, const String &updateMask, AsyncResult &aResult)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `release` - The Release object to update.
- `updateMask` - Specifies which fields to update. 
This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".
- `aResult` - The async result (AsyncResult).

20. ## 🔹  String patch(AsyncClientClass &aClient, const String &projectId, const Release &release, const String &updateMask)

Update a Release via PATCH.

Only updates to rulesetName will be honored. Release rename is not supported. To create a Release use the releases.create method.

```cpp
String patch(AsyncClientClass &aClient, const String &projectId, const Release &release, const String &updateMask)
```

**Params:**

- `aClient` - The async client.
- `projectId` - The project ID.
- `release` - The Release object to update.
- `updateMask` - Specifies which fields to update. 
This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".

**Returns:**

- `String` - The response payload.