
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
│   │   │       ├───CustomAuthFile
│   │   │       ├───NoAuth
│   │   │       ├───SaveAndLoad
│   │   │       ├───ServiceAuth
│   │   │       ├───ServiceAuthFile
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
│   │   │   ├───CustomPushID
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
│   │       ├───CustomPushID
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
│   ├───Simple
│   │   ├───SimpleDatabaseSecret
│   │   ├───SimpleNoAuth
│   │   ├───StreamDatabaseSecret
│   │   └───StreamNoAuth
│   └───Sync
│       ├───CustomPushID
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