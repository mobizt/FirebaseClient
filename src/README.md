

## Examples Tree Structure

```yaml
├───App
│   ├───AppInitialization
│   │   ├───Async
│   │   │   ├───Callback
│   │   │   │   ├───CustomAuth
│   │   │   │   ├───CustomAuthFile
│   │   │   │   ├───NoAuth
│   │   │   │   ├───SaveAndLoad
│   │   │   │   ├───ServiceAuth
│   │   │   │   ├───ServiceAuthFile
│   │   │   │   ├───TokenAuth
│   │   │   │   │   ├───AccessToken
│   │   │   │   │   ├───CustomToken
│   │   │   │   │   ├───IDToken
│   │   │   │   │   └───LegacyToken
│   │   │   │   └───UserAuth
│   │   │   └───NoCallback
│   │   │       ├───CustomAuth
│   │   │       ├───NoAuth
│   │   │       ├───ServiceAuth
│   │   │       ├───TokenAuth
│   │   │       │   ├───AccessToken
│   │   │       │   ├───CustomToken
│   │   │       │   ├───IDToken
│   │   │       │   └───LegacyToken
│   │   │       └───UserAuth
│   │   └───Sync
│   │       ├───CustomAuth
│   │       ├───CustomAuthFile
│   │       ├───NoAuth
│   │       ├───SaveAndLoad
│   │       ├───ServiceAuth
│   │       ├───ServiceAuthFile
│   │       ├───TokenAuth
│   │       │   ├───AccessToken
│   │       │   ├───CustomToken
│   │       │   ├───IDToken
│   │       │   └───LegacyToken
│   │       └───UserAuth
│   ├───NetworkInterfaces
│   │   ├───Async
│   │   │   ├───Callback
│   │   │   │   ├───DefaultNetworks
│   │   │   │   │   ├───DefaultEthernetNetwork
│   │   │   │   │   │   ├───ESP32
│   │   │   │   │   │   └───ESP8266
│   │   │   │   │   ├───DefaultNetwork
│   │   │   │   │   └───DefaultWiFiNetwork
│   │   │   │   ├───EthernetNetwork
│   │   │   │   ├───GenericNetwork
│   │   │   │   └───GSMNetwork
│   │   │   └───NoCallback
│   │   │       ├───DefaultNetworks
│   │   │       │   ├───DefaultEthernetNetwork
│   │   │       │   │   ├───ESP32
│   │   │       │   │   └───ESP8266
│   │   │       │   ├───DefaultNetwork
│   │   │       │   └───DefaultWiFiNetwork
│   │   │       ├───EthernetNetwork
│   │   │       ├───GenericNetwork
│   │   │       └───GSMNetwork
│   │   └───Sync
│   │       ├───DefaultNetworks
│   │       │   ├───DefaultEthernetNetwork
│   │       │   │   ├───ESP32
│   │       │   │   └───ESP8266
│   │       │   ├───DefaultNetwork
│   │       │   └───DefaultWiFiNetwork
│   │       ├───EthernetNetwork
│   │       ├───GenericNetwork
│   │       └───GSMNetwork
│   └───UserManagement
│       ├───Async
│       │   ├───Callback
│       │   │   ├───Anonymous
│       │   │   ├───DeleteUser
│       │   │   ├───ResetPassword
│       │   │   ├───SignUp
│       │   │   └───Verify
│       │   └───NoCallback
│       │       ├───Anonymous
│       │       ├───DeleteUser
│       │       ├───ResetPassword
│       │       ├───SignUp
│       │       └───Verify
│       └───Sync
│           ├───Anonymous
│           ├───DeleteUser
│           ├───ResetPassword
│           ├───SignUp
│           └───Verify
├───AsyncTCP
├───CloudFunctions
│   ├───Async
│   │   ├───Callback
│   │   │   ├───Call
│   │   │   ├───Create
│   │   │   ├───Delete
│   │   │   ├───GenDownloadURL
│   │   │   ├───GenUploadURL
│   │   │   ├───Get
│   │   │   ├───GetIamPolicy
│   │   │   ├───List
│   │   │   ├───Patch
│   │   │   ├───SetIamPolicy
│   │   │   └───TestIamPermissions
│   │   └───NoCallback
│   │       ├───Call
│   │       ├───Create
│   │       ├───Delete
│   │       ├───GenDownloadURL
│   │       ├───GenUploadURL
│   │       ├───Get
│   │       ├───GetIamPolicy
│   │       ├───List
│   │       ├───Patch
│   │       ├───SetIamPolicy
│   │       └───TestIamPermissions
│   └───Sync
│       ├───Call
│       ├───Create
│       ├───Delete
│       ├───GenDownloadURL
│       ├───GenUploadURL
│       ├───Get
│       ├───GetIamPolicy
│       ├───List
│       ├───Patch
│       ├───SetIamPolicy
│       └───TestIamPermissions
├───CloudStorage
│   ├───Async
│   │   ├───Callback
│   │   │   ├───Delete
│   │   │   ├───Download
│   │   │   ├───GetMetadata
│   │   │   ├───List
│   │   │   ├───OTA
│   │   │   └───Upload
│   │   └───NoCallback
│   │       ├───Delete
│   │       ├───Download
│   │       ├───GetMetadata
│   │       ├───List
│   │       ├───OTA
│   │       └───Upload
│   └───Sync
│       ├───Delete
│       ├───Download
│       ├───GetMetadata
│       ├───List
│       ├───OTA
│       └───Upload
├───FirestoreDatabase
│   ├───CollectionGroups
│   │   └───Indexes
│   │       ├───Async
│   │       │   ├───Callback
│   │       │   │   ├───Create
│   │       │   │   ├───Delete
│   │       │   │   ├───Get
│   │       │   │   └───List
│   │       │   └───NoCallback
│   │       │       ├───Create
│   │       │       ├───Delete
│   │       │       ├───Get
│   │       │       └───List
│   │       └───Sync
│   │           ├───Create
│   │           ├───Delete
│   │           ├───Get
│   │           └───List
│   ├───Databases
│   │   ├───Async
│   │   │   ├───Callback
│   │   │   │   ├───Create
│   │   │   │   ├───Delete
│   │   │   │   ├───ExportDocuments
│   │   │   │   ├───Get
│   │   │   │   ├───ImportDocuments
│   │   │   │   ├───Indexes
│   │   │   │   │   ├───Create
│   │   │   │   │   ├───Delete
│   │   │   │   │   ├───Get
│   │   │   │   │   └───List
│   │   │   │   ├───List
│   │   │   │   └───Update
│   │   │   └───NoCallback
│   │   │       ├───Create
│   │   │       ├───Delete
│   │   │       ├───ExportDocuments
│   │   │       ├───Get
│   │   │       ├───ImportDocuments
│   │   │       ├───Indexes
│   │   │       │   ├───Create
│   │   │       │   ├───Delete
│   │   │       │   ├───Get
│   │   │       │   └───List
│   │   │       ├───List
│   │   │       └───Update
│   │   └───Sync
│   │       ├───Create
│   │       ├───Delete
│   │       ├───ExportDocuments
│   │       ├───Get
│   │       ├───ImportDocuments
│   │       ├───Indexes
│   │       │   ├───Create
│   │       │   ├───Delete
│   │       │   ├───Get
│   │       │   └───List
│   │       ├───List
│   │       └───Update
│   └───Documents
│       ├───Async
│       │   ├───Callback
│       │   │   ├───BatchGet
│       │   │   ├───BatchWrite
│       │   │   ├───Commit
│       │   │   │   ├───AppendArray
│       │   │   │   ├───AppendMapValue
│       │   │   │   ├───AppendMapValueArray
│       │   │   │   ├───AppendMapValueTimestamp
│       │   │   │   ├───IncrementFieldValue
│       │   │   │   └───SetUpdateDelete
│       │   │   ├───CreateDocument
│       │   │   ├───Delete
│       │   │   ├───Get
│       │   │   ├───List
│       │   │   ├───ListCollectionIds
│       │   │   ├───Patch
│       │   │   │   ├───AppendMapValue
│       │   │   │   └───UpdateDocument
│       │   │   └───RunQuery
│       │   └───NoCallback
│       │       ├───BatchGet
│       │       ├───BatchWrite
│       │       ├───Commit
│       │       │   ├───AppendArray
│       │       │   ├───AppendMapValue
│       │       │   ├───AppendMapValueArray
│       │       │   ├───AppendMapValueTimestamp
│       │       │   ├───IncrementFieldValue
│       │       │   └───SetUpdateDelete
│       │       ├───CreateDocument
│       │       ├───Delete
│       │       ├───Get
│       │       ├───List
│       │       ├───ListCollectionIds
│       │       ├───Patch
│       │       │   ├───AppendMapValue
│       │       │   └───UpdateDocument
│       │       └───RunQuery
│       └───Sync
│           ├───BatchGet
│           ├───BatchWrite
│           ├───Commit
│           │   ├───AppendArray
│           │   ├───AppendMapValue
│           │   ├───AppendMapValueArray
│           │   ├───AppendMapValueTimestamp
│           │   ├───IncrementFieldValue
│           │   └───SetUpdateDelete
│           ├───CreateDocument
│           ├───Delete
│           ├───Get
│           ├───List
│           ├───ListCollectionIds
│           ├───Patch
│           │   ├───AppendMapValue
│           │   └───UpdateDocument
│           └───RunQuery
├───Messaging
│   ├───Async
│   │   ├───Callback
│   │   │   └───Send
│   │   └───NoCallback
│   │       └───Send
│   └───Sync
│       └───Send
├───RealtimeDatabase
│   ├───Async
│   │   ├───Callback
│   │   │   ├───BLOB
│   │   │   ├───File
│   │   │   ├───Get
│   │   │   ├───OTA
│   │   │   ├───Push
│   │   │   ├───Remove
│   │   │   ├───Set
│   │   │   ├───Stream
│   │   │   ├───StreamConcurentcy
│   │   │   ├───StreamGSM
│   │   │   └───Update
│   │   └───NoCallback
│   │       ├───BLOB
│   │       ├───File
│   │       ├───Get
│   │       ├───OTA
│   │       ├───Push
│   │       ├───Remove
│   │       ├───Set
│   │       ├───Stream
│   │       ├───StreamConcurentcy
│   │       ├───StreamGSM
│   │       └───Update
│   └───Sync
│       ├───ETAG
│       ├───Existed
│       ├───FilteringData
│       ├───Get
│       ├───IndexingData
│       ├───Priority
│       ├───Push
│       ├───Remove
│       ├───SecurityRules
│       ├───Set
│       ├───Shallow
│       ├───Timestamp
│       └───Update
└───Storage
    ├───Async
    │   ├───Callback
    │   │   ├───Delete
    │   │   ├───Download
    │   │   ├───GetMetadata
    │   │   ├───List
    │   │   ├───OTA
    │   │   └───Upload
    │   └───NoCallback
    │       ├───Delete
    │       ├───Download
    │       ├───GetMetadata
    │       ├───List
    │       ├───OTA
    │       └───Upload
    └───Sync
        ├───Delete
        ├───Download
        ├───GetMetadata
        ├───List
        ├───OTA
        └───Upload

```

## Firebase Client Class and Static Functions Usage

- [Class and Functions](/resources/docs/firebase_client.md).

## Async Client Class Usage

- [Class and Functions](/resources/docs/async_client.md).

## Firebase App Usage

- [Class and Functions](/resources/docs/firebase_app.md).

## Async Result Usage

- [Class and Functions](/resources/docs/async_result.md).

## Realtime Database Usage

- [Examples](/examples/RealtimeDatabase).

- [Class and Functions](/resources/docs/realtime_database.md).

- [API Doc](https://firebase.google.com/docs/reference/rest/database).

## Realtime Database Result Usage

- [Class and Functions](/resources/docs/realtime_database_result.md).

## Google Cloud Firestore Database Usage

- [Examples](/examples/FirestoreDatabase).

- [Class and Functions](/resources/docs/firestore_database.md).

- [API Doc](https://firebase.google.com/docs/firestore/reference/rest).

## Google Cloud Messaging Usage

- [Examples](/examples/Messaging).

- [Class and Functions](/resources/docs/messaging.md).

- [API Doc](https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages).


## Firebase Storage Usage

- [Examples](/examples/Storage).

- [Class and Functions](/resources/docs/storage.md).

## Google Cloud Storage Usage

- [Examples](/examples/CloudStorage).

- [Class and Functions](/resources/docs/cloud_storage.md).

- [API Doc](https://cloud.google.com/storage/docs/json_api/v1).

## Google Cloud Functions Usage

- [Examples](/examples/CloudFunctions).

- [Class and Functions](/resources/docs/cloud_functions.md).

- [API Doc](https://cloud.google.com/functions/docs/reference/rest/v2/projects.locations.functions).

## The placeholder represents the primitive types values that used in this library 

- [Class and Functions](/resources/docs/placeholders.md).

## JsonWriter

- [Class and Functions](/resources/docs/json_writer.md).
