/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FUNCTIONS_DATA_OPTIONS_H
#define FUNCTIONS_DATA_OPTIONS_H

#include <Arduino.h>
#include "./core/Utils/JSON.h"
#include "./core/Utils/ObjectWriter.h"
#include "./core/AsyncClient/AsyncClient.h"
#include "./core/Utils/URL.h"
#include "./functions/Policy.h"

// https://cloud.google.com/functions/docs/reference/rest/v2/projects.locations.functions
#if defined(ENABLE_FUNCTIONS)
namespace GoogleCloudFunctions
{
    enum google_cloud_functions_request_type
    {
        fn_undefined,
        fn_create,
        fn_delete,
        fn_call,
        fn_deploy,
        fn_download,
        fn_list,
        fn_get,
        fn_patch,
        fn_gen_downloadUrl,
        fn_gen_uploadUrl,
        fn_get_iam_policy,
        fn_set_iam_policy,
        fn_test_iam_permission
    };

    // Severity of the state message.
    enum Severity
    {
        SEVERITY_UNSPECIFIED, //	Not specified. Invalid severity.
        ERROR,                //	ERROR-level severity.
        WARNING,              //	WARNING-level severity.
        INFO                  //	INFO-level severity.
    };

    // The environment the function is hosted on.
    enum Environment
    {
        ENVIRONMENT_UNSPECIFIED, // Unspecified
        GEN_1,                   //	Gen 1
        GEN_2                    //	Gen 2
    };

    // Docker Registry to use for storing function Docker images.
    enum DockerRegistry
    {
        DOCKER_REGISTRY_UNSPECIFIED, //	Unspecified.
        CONTAINER_REGISTRY,          //	Docker images will be stored in multi-regional Container Registry repositories named gcf.
        ARTIFACT_REGISTRY            //	Docker images will be stored in regional Artifact Registry repositories. By default, GCF will create and use repositories named gcf-artifacts in every region in which a function is deployed. But the repository to use can also be specified by the user using the dockerRepository field.
    };

    // Describes the retry policy in case of function's execution failure. Retried execution is charged as any other execution.
    enum RetryPolicy
    {
        RETRY_POLICY_UNSPECIFIED,  //	Not specified.
        RETRY_POLICY_DO_NOT_RETRY, //	Do not retry.
        RETRY_POLICY_RETRY         // Retry on any failure, retry up to 7 days with an exponential backoff (capped at 10 seconds).
    };

    // This controls what traffic is diverted through the VPC Access Connector resource. By default PRIVATE_RANGES_ONLY will be used.
    enum VpcConnectorEgressSettings
    {
        VPC_CONNECTOR_EGRESS_SETTINGS_UNSPECIFIED, //	Unspecified.
        PRIVATE_RANGES_ONLY,                       //	Use the VPC Access Connector only for private IP space from RFC1918.
        ALL_TRAFFIC                                //	Force the use of VPC Access Connector for all egress traffic from the function.
    };

    // Available ingress settings. This controls what traffic can reach the function.
    // If unspecified, ALLOW_ALL will be used.
    enum IngressSettings
    {
        INGRESS_SETTINGS_UNSPECIFIED, //	Unspecified.
        ALLOW_ALL,                    //	Allow HTTP traffic from public and private sources.
        ALLOW_INTERNAL_ONLY,          //	Allow HTTP traffic from only private VPC sources.
        ALLOW_INTERNAL_AND_GCLB       //	Allow HTTP traffic from private VPC sources and through GCLB.
    };

    // Available security level settings.
    // This enforces security protocol on function URL.
    // Security level is only configurable for 1st Gen functions,
    // If unspecified, SECURE_OPTIONAL will be used. 2nd Gen functions are SECURE_ALWAYS ONLY.
    enum SecurityLevel
    {
        SECURITY_LEVEL_UNSPECIFIED, //	Unspecified.
        SECURE_ALWAYS,              //	Requests for a URL that match this handler that do not use HTTPS are automatically redirected to the HTTPS URL with the same path. Query parameters are reserved for the redirect.
        SECURE_OPTIONAL             //	Both HTTP and HTTPS requests with URLs that match the handler succeed without redirects. The application can examine the request to determine which protocol was used and respond accordingly.
    };

    const struct firebase_ns::key_str_30 _DockerRegistry[DockerRegistry::ARTIFACT_REGISTRY + 1] PROGMEM = {"DOCKER_REGISTRY_UNSPECIFIED", "CONTAINER_REGISTRY", "RETRY_POLICY_RETRY"};
    const struct firebase_ns::key_str_50 _VpcConnectorEgressSettings[VpcConnectorEgressSettings::ALL_TRAFFIC + 1] PROGMEM = {"VPC_CONNECTOR_EGRESS_SETTINGS_UNSPECIFIED", "PRIVATE_RANGES_ONLY", "ALL_TRAFFIC"};
    const struct firebase_ns::key_str_30 _IngressSettings[IngressSettings::ALLOW_INTERNAL_AND_GCLB + 1] PROGMEM = {"INGRESS_SETTINGS_UNSPECIFIED", "ALLOW_ALL", "ALLOW_INTERNAL_ONLY", "ALLOW_INTERNAL_AND_GCLB"};
    const struct firebase_ns::key_str_30 _SecurityLevel[SecurityLevel::SECURE_OPTIONAL + 1] PROGMEM = {"SECURITY_LEVEL_UNSPECIFIED", "SECURE_ALWAYS", "SECURE_OPTIONAL"};
    const struct firebase_ns::key_str_30 _RetryPolicy[RetryPolicy::RETRY_POLICY_RETRY + 1] PROGMEM = {"RETRY_POLICY_UNSPECIFIED", "RETRY_POLICY_DO_NOT_RETRY", "RETRY_POLICY_RETRY"};
    const struct firebase_ns::key_str_30 _Environment[Environment::GEN_2 + 1] PROGMEM = {"ENVIRONMENT_UNSPECIFIED", "GEN_1", "GEN_2"};

    /**
     * Location of the source in a Google Cloud Source Repository.
     */
    struct RepoSource : public BaseO6
    {
    public:
        // ID of the project that owns the Cloud Source Repository. If omitted, the project ID requesting the build is assumed.
        RepoSource &projectId(const String &value) { return wr.set<RepoSource &, String>(*this, value, buf, bufSize, 1, __func__); }

        // Name of the Cloud Source Repository.
        RepoSource &repoName(const String &value) { return wr.set<RepoSource &, String>(*this, value, buf, bufSize, 2, __func__); }

        // Directory, relative to the source root, in which to run the build.
        // This must be a relative path. If a step's dir is specified and is an absolute path,
        // this value is ignored for that step's execution. eg. helloworld (no leading slash allowed)
        RepoSource &dir(const String &value) { return wr.set<RepoSource &, String>(*this, value, buf, bufSize, 3, __func__); }

        // Union field revision.
        // Regex matching branches to build.
        // The syntax of the regular expressions accepted is the syntax accepted by RE2
        // and described at https://github.com/google/re2/wiki/Syntax
        RepoSource &branchName(const String &value) { return wr.append<RepoSource &, String>(*this, value, buf, bufSize, 4, __func__); }

        // Union field revision.
        // Regex matching tags to build.
        // The syntax of the regular expressions accepted is the syntax accepted by RE2
        //  and described at https://github.com/google/re2/wiki/Syntax
        RepoSource &tagName(const String &value) { return wr.append<RepoSource &, String>(*this, value, buf, bufSize, 4, __func__); }

        // Union field revision.
        // Explicit commit SHA to build.
        RepoSource &commitSha(const String &value) { return wr.append<RepoSource &, String>(*this, value, buf, bufSize, 4, __func__); }
    };

    /**
     * Location of the source in an archive file in Google Cloud Storage.
     */
    struct StorageSource : public BaseO4
    {
    public:
        // Google Cloud Storage bucket containing the source (see Bucket Name Requirements, https://cloud.google.com/storage/docs/bucket-naming#requirements).
        StorageSource &bucket(const String &value) { return wr.set<StorageSource &, String>(*this, value, buf, bufSize, 1, __func__); }

        // Google Cloud Storage object containing the source.
        // This object must be a gzipped archive file (.tar.gz) containing source to build.
        StorageSource &object(const String &value) { return wr.set<StorageSource &, String>(*this, value, buf, bufSize, 2, __func__); }

        // Google Cloud Storage generation (int64 format) for the object. If the generation is omitted, the latest generation will be used.
        StorageSource &generation(const String &value) { return wr.set<StorageSource &, String>(*this, value, buf, bufSize, 3, __func__); }
    };

    /**
     * The location of the function source code.
     */
    struct Source : public BaseO1
    {
    public:
        // Union field source
        // If provided, get the source from this location in Google Cloud Storage.
        Source &storageSource(const StorageSource &value) { return wr.add<Source &, StorageSource>(*this, value, buf, __func__); }

        // Union field source
        // If provided, get the source from this location in a Cloud Source Repository.
        Source &repoSource(const RepoSource &value) { return wr.add<Source &, RepoSource>(*this, value, buf, __func__); }

        // Union field source
        // If provided, get the source from GitHub repository. This option is valid only for GCF 1st Gen function. Example: https://github.com///blob//
        Source &gitUri(const String &value) { return wr.add<Source &, String>(*this, value, buf, __func__); }
    };

    /**
     * Describes the Build step of the function that builds a container from the given source.
     */
    struct BuildConfig : public BaseO10
    {
    public:
        // The runtime in which to run the function. Required when deploying a new function, optional when updating an existing function. For a complete list of possible choices, see the gcloud command reference.
        BuildConfig &runtime(const String &value) { return wr.set<BuildConfig &, String>(*this, value, buf, bufSize, 1, __func__); }

        // The name of the function (as defined in source code) that will be executed. Defaults to the resource name suffix, if not specified. For backward compatibility, if function with given name is not found, then the system will try to use function named "function". For Node.js this is name of a function exported by the module specified in source_location.
        BuildConfig &entryPoint(const String &value) { return wr.set<BuildConfig &, String>(*this, value, buf, bufSize, 2, __func__); }

        // The location of the function source code.
        BuildConfig &source(const Source &value) { return wr.set<BuildConfig &, Source>(*this, value, buf, bufSize, 3, __func__); }

        // Name of the Cloud Build Custom Worker Pool that should be used to build the function. The format of this field is projects/{project}/locations/{region}/workerPools/{workerPool} where {project} and {region} are the project id and region respectively where the worker pool is defined and {workerPool} is the short name of the worker pool.
        BuildConfig &workerPool(const String &value) { return wr.set<BuildConfig &, String>(*this, value, buf, bufSize, 4, __func__); }

        // User-provided build-time environment variables for the function
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        BuildConfig &environmentVariables(const object_t &value) { return wr.set<BuildConfig &, object_t>(*this, value, buf, bufSize, 5, __func__); }

        // Docker Registry to use for this deployment. This configuration is only applicable to 1st Gen functions, 2nd Gen functions can only use Artifact Registry.
        // If dockerRepository field is specified, this field will be automatically set as ARTIFACT_REGISTRY. If unspecified, it currently defaults to CONTAINER_REGISTRY. This field may be overridden by the backend for eligible deployments.
        BuildConfig &dockerRegistry(DockerRegistry value) { return wr.set<BuildConfig &, const char *>(*this, _DockerRegistry[value].text, buf, bufSize, 6, __func__); }

        // Repository in Artifact Registry to which the function docker image will be pushed after it is built by Cloud Build. If specified by user, it is created and managed by user with a customer managed encryption key. Otherwise, GCF will create and use a repository named 'gcf-artifacts' for every deployed region.
        BuildConfig &dockerRepository(const String &value) { return wr.set<BuildConfig &, String>(*this, value, buf, bufSize, 7, __func__); }

        //[Preview] Service account to be used for building the container
        BuildConfig &serviceAccount(const String &value) { return wr.set<BuildConfig &, String>(*this, value, buf, bufSize, 8, __func__); }

        // An identifier for Firebase function sources. Disclaimer: This field is only supported for Firebase function deployments.
        BuildConfig &sourceToken(const String &value) { return wr.set<BuildConfig &, String>(*this, value, buf, bufSize, 9, __func__); }
    };
    /**
     * Configuration for a single version.
     */
    struct SecretVersion : public BaseO4
    {
    public:
        // Version of the secret (version number or the string 'latest'). It is preferable to use latest version with secret volumes as secret value changes are reflected immediately.
        SecretVersion &version(const String &value) { return wr.set<SecretVersion &, String>(*this, value, buf, bufSize, 1, __func__); }

        // Relative path of the file under the mount path where the secret value for this version will be fetched and made available. For example, setting the mountPath as '/etc/secrets' and path as secret_foo would mount the secret value file at /etc/secrets/secret_foo.
        SecretVersion &path(const String &value) { return wr.set<SecretVersion &, String>(*this, value, buf, bufSize, 2, __func__); }
    };
    /**
     * Configuration for a secret volume. It has the information necessary to fetch the secret value from secret manager and make it available as files mounted at the requested paths within the application container.
     */
    struct SecretVolume : public BaseO6
    {
    public:
        // The path within the container to mount the secret volume. For example, setting the mountPath as /etc/secrets would mount the secret value files under the /etc/secrets directory. This directory will also be completely shadowed and unavailable to mount any other secrets. Recommended mount path: /etc/secrets
        SecretVolume &mountPath(const String &value) { return wr.set<SecretVolume &, String>(*this, value, buf, bufSize, 1, __func__); }

        // Project identifier (preferably project number but can also be the project ID) of the project that contains the secret. If not set, it is assumed that the secret is in the same project as the function.
        SecretVolume &projectId(const String &value) { return wr.set<SecretVolume &, String>(*this, value, buf, bufSize, 2, __func__); }

        // Name of the secret in secret manager (not the full resource name).
        SecretVolume &secret(const String &value) { return wr.set<SecretVolume &, String>(*this, value, buf, bufSize, 3, __func__); }

        // List of secret versions to mount for this secret. If empty, the latest version of the secret will be made available in a file named after the secret under the mount point.
        // The item or value will be added to the array or list.
        // To add more items, call this function again.
        SecretVolume &versions(const SecretVersion &value) { return wr.append<SecretVolume &, SecretVersion>(*this, value, buf, bufSize, 4, __func__); }
    };
    /**
     * Configuration for a secret environment variable. It has the information necessary to fetch the secret value from secret manager and expose it as an environment variable.
     */
    struct SecretEnvVar : public BaseO6
    {
    public:
        // Name of the environment variable.
        SecretEnvVar &key(const String &value) { return wr.set<SecretEnvVar &, String>(*this, value, buf, bufSize, 1, __func__); }

        // Project identifier (preferably project number but can also be the project ID) of the project that contains the secret. If not set, it is assumed that the secret is in the same project as the function.
        SecretEnvVar &projectId(const String &value) { return wr.set<SecretEnvVar &, String>(*this, value, buf, bufSize, 2, __func__); }

        // Name of the secret in secret manager (not the full resource name).
        SecretEnvVar &secret(const String &value) { return wr.set<SecretEnvVar &, String>(*this, value, buf, bufSize, 3, __func__); }

        // Version of the secret (version number or the string 'latest'). It is recommended to use a numeric version for secret environment variables as any updates to the secret value is not reflected until new instances start.
        SecretEnvVar &version(const String &value) { return wr.set<SecretEnvVar &, String>(*this, value, buf, bufSize, 4, __func__); }
    };

    /**
     * Describes the Service being deployed. Currently Supported : Cloud Run (fully managed).
     */
    struct ServiceConfig : public BaseO16
    {

    public:
        // The function execution timeout. Execution is considered failed and can be terminated if the function is not completed at the end of the timeout period. Defaults to 60 seconds.
        ServiceConfig &timeoutSeconds(uint32_t value) { return wr.set<ServiceConfig &, uint32_t>(*this, value, buf, bufSize, 1, __func__); }

        // The amount of memory available for a function. Defaults to 256M. Supported units are k, M, G, Mi, Gi. If no unit is supplied the value is interpreted as bytes. See https://github.com/kubernetes/kubernetes/blob/master/staging/src/k8s.io/apimachinery/pkg/api/resource/quantity.go a full description.
        ServiceConfig &availableMemory(const String &value) { return wr.set<ServiceConfig &, String>(*this, value, buf, bufSize, 2, __func__); }

        // The number of CPUs used in a single container instance. Default value is calculated from available memory. Supports the same values as Cloud Run, see https://cloud.google.com/run/docs/reference/rest/v1/Container#resourcerequirements Example: "1" indicates 1 vCPU
        ServiceConfig &availableCpu(const String &value) { return wr.set<ServiceConfig &, String>(*this, value, buf, bufSize, 3, __func__); }

        // Environment variables that shall be available during function execution.
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        ServiceConfig &environmentVariables(const object_t &value) { return wr.set<ServiceConfig &, object_t>(*this, value, buf, bufSize, 4, __func__); }

        // The limit on the maximum number of function instances that may coexist at a given time.
        // In some cases, such as rapid traffic surges, Cloud Functions may, for a short period of time, create more instances than the specified max instances limit. If your function cannot tolerate this temporary behavior, you may want to factor in a safety margin and set a lower max instances value than your function can tolerate.
        //  See the Max Instances (https://cloud.google.com/functions/docs/max-instances) Guide for more details.
        ServiceConfig &maxInstanceCount(uint32_t value) { return wr.set<ServiceConfig &, uint32_t>(*this, value, buf, bufSize, 5, __func__); }

        // The limit on the minimum number of function instances that may coexist at a given time.
        // Function instances are kept in idle state for a short period after they finished executing the request to reduce cold start time for subsequent requests. Setting a minimum instance count will ensure that the given number of instances are kept running in idle state always. This can help with cold start times when jump in incoming request count occurs after the idle instance would have been stopped in the default case.
        ServiceConfig &minInstanceCount(uint32_t value) { return wr.set<ServiceConfig &, uint32_t>(*this, value, buf, bufSize, 6, __func__); }

        // The Serverless VPC Access connector that this cloud function can connect to. The format of this field is projects/*/locations/*/connectors/*.
        ServiceConfig &vpcConnector(const String &value) { return wr.set<ServiceConfig &, String>(*this, value, buf, bufSize, 7, __func__); }

        // The egress settings for the connector, controlling what traffic is diverted through it.
        ServiceConfig &vpcConnectorEgressSettings(VpcConnectorEgressSettings value) { return wr.set<ServiceConfig &, const char *>(*this, _VpcConnectorEgressSettings[value].text, buf, bufSize, 8, __func__); }

        // The ingress settings for the function, controlling what traffic can reach it.
        ServiceConfig &ingressSettings(IngressSettings value) { return wr.set<ServiceConfig &, const char *>(*this, _IngressSettings[value].text, buf, bufSize, 9, __func__); }

        // The email of the service's service account. If empty, defaults to {project_number}-compute@developer.gserviceaccount.com.
        ServiceConfig &serviceAccountEmail(const String &value) { return wr.set<ServiceConfig &, String>(*this, value, buf, bufSize, 10, __func__); }

        // Whether 100% of traffic is routed to the latest revision. On functions.create and functions.patch, when set to true, the revision being deployed will serve 100% of traffic, ignoring any traffic split settings, if any. On functions.get, true will be returned if the latest revision is serving 100% of traffic.
        ServiceConfig &allTrafficOnLatestRevision(bool value) { return wr.set<ServiceConfig &, bool>(*this, value, buf, bufSize, 11, __func__); }

        // Secret environment variables configuration.
        // The item or value will be added to the array or list.
        // To add more items, call this function again.
        ServiceConfig &secretEnvironmentVariables(const SecretEnvVar &value) { return wr.append<ServiceConfig &, SecretEnvVar>(*this, value, buf, bufSize, 12, __func__); }

        // Secret volumes configuration.
        // The item or value will be added to the array or list.
        // To add more items, call this function again.
        ServiceConfig &secretVolumes(const SecretVolume &value) { return wr.append<ServiceConfig &, SecretVolume>(*this, value, buf, bufSize, 13, __func__); }

        // Sets the maximum number of concurrent requests that each instance can receive. Defaults to 1.
        ServiceConfig &maxInstanceRequestConcurrency(uint32_t value) { return wr.set<ServiceConfig &, uint32_t>(*this, value, buf, bufSize, 14, __func__); }

        // Security level configure whether the function only accepts https. This configuration is only applicable to 1st Gen functions with Http trigger. By default https is optional for 1st Gen functions; 2nd Gen functions are https ONLY.
        ServiceConfig &securityLevel(SecurityLevel value) { return wr.set<ServiceConfig &, const char *>(*this, _SecurityLevel[value].text, buf, bufSize, 15, __func__); }
    };
    /**
     * Filters events based on exact matches on the CloudEvents attributes.
     */
    struct EventFilter : public BaseO4
    {
    public:
        // Required. The name of a CloudEvents attribute.
        EventFilter &attribute(const String &value) { return wr.set<EventFilter &, String>(*this, value, buf, bufSize, 1, __func__); }

        // Required. The value for the attribute.
        EventFilter &value(const String &value) { return wr.set<EventFilter &, String>(*this, value, buf, bufSize, 2, __func__); }

        // Optional. The operator used for matching the events with the value of the filter. If not specified, only events that have an exact key-value pair specified in the filter are matched. The only allowed value is match-path-pattern.
        EventFilter &Operator(const String &value) { return wr.set<EventFilter &, String>(*this, value, buf, bufSize, 3, __func__); }
    };
    /**
     * Describes EventTrigger, used to request events to be sent from another service.
     */
    struct EventTrigger : public BaseO10
    {
    public:
        // The region that the trigger will be in. The trigger will only receive events originating in this region. It can be the same region as the function, a different region or multi-region, or the global region. If not provided, defaults to the same region as the function.
        EventTrigger &triggerRegion(const String &value) { return wr.set<EventTrigger &, String>(*this, value, buf, bufSize, 1, __func__); }

        // Required. The type of event to observe. For example: google.cloud.audit.log.v1.written or google.cloud.pubsub.topic.v1.messagePublished.
        EventTrigger &eventType(const String &value) { return wr.set<EventTrigger &, String>(*this, value, buf, bufSize, 2, __func__); }

        // Criteria used to filter events.
        // The item or value will be added to the array or list.
        // To add more items, call this function again.
        EventTrigger &eventFilters(const EventFilter &value) { return wr.append<EventTrigger &, EventFilter>(*this, value, buf, bufSize, 3, __func__); }

        // Optional. The name of a Pub/Sub topic in the same project that will be used as the transport topic for the event delivery. Format: projects/{project}/topics/{topic}.
        // This is only valid for events of type google.cloud.pubsub.topic.v1.messagePublished. The topic provided here will not be deleted at function deletion.
        EventTrigger &pubsubTopic(const String &value) { return wr.set<EventTrigger &, String>(*this, value, buf, bufSize, 4, __func__); }

        // Optional. The email of the trigger's service account. The service account must have permission to invoke Cloud Run services, the permission is run.routes.invoke. If empty, defaults to the Compute Engine default service account: {project_number}-compute@developer.gserviceaccount.com.
        EventTrigger &serviceAccountEmail(const String &value) { return wr.set<EventTrigger &, String>(*this, value, buf, bufSize, 5, __func__); }

        // Optional. If unset, then defaults to ignoring failures (i.e. not retrying them).
        EventTrigger &retryPolicy(RetryPolicy value) { return wr.set<EventTrigger &, const char *>(*this, _RetryPolicy[value].text, buf, bufSize, 6, __func__); }

        // Optional. The name of the channel associated with the trigger in projects/{project}/locations/{location}/channels/{channel} format. You must provide a channel to receive events from Eventarc SaaS partners.
        EventTrigger &channel(const String &value) { return wr.set<EventTrigger &, String>(*this, value, buf, bufSize, 7, __func__); }

        // Optional. The hostname of the service that 1st Gen function should be observed.
        // If no string is provided, the default service implementing the API will be used. For example, storage.googleapis.com is the default for all event types in the google.storage namespace.
        // The field is only applicable to 1st Gen functions.
        EventTrigger &service(const String &value) { return wr.set<EventTrigger &, String>(*this, value, buf, bufSize, 8, __func__); }
    };
    /**
     * Describes a Cloud Function that contains user computation executed in response to an event. It encapsulates function and trigger configurations.
     */
    struct Function : public BaseO10
    {
    public:
        // A user-defined name of the function. Function names must be unique globally and match pattern projects/*/locations/*/functions/*
        Function &name(const String &value) { return wr.set<Function &, String>(*this, value, buf, bufSize, 1, __func__); }

        // User-provided description of a function.
        Function &description(const String &value) { return wr.set<Function &, String>(*this, value, buf, bufSize, 2, __func__); }

        // Describes the Build step of the function that builds a container from the given source.
        Function &buildConfig(const BuildConfig &value) { return wr.set<Function &, BuildConfig>(*this, value, buf, bufSize, 3, __func__); }

        // Describes the Service being deployed. Currently deploys services to Cloud Run (fully managed).
        Function &serviceConfig(const ServiceConfig &value) { return wr.set<Function &, ServiceConfig>(*this, value, buf, bufSize, 4, __func__); }

        // An Eventarc trigger managed by Google Cloud Functions that fires events in response to a condition in another service.
        Function &eventTrigger(const EventTrigger &value) { return wr.set<Function &, EventTrigger>(*this, value, buf, bufSize, 5, __func__); }

        // Labels associated with this Cloud Function.
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        Function &labels(const object_t &value) { return wr.set<Function &, object_t>(*this, value, buf, bufSize, 6, __func__); }

        // Describe whether the function is 1st Gen or 2nd Gen.
        Function &environment(Environment value) { return wr.set<Function &, const char *>(*this, _Environment[value].text, buf, bufSize, 7, __func__); }

        //[Preview] Resource name of a KMS crypto key (managed by the user) used to encrypt/decrypt function resources.
        // It must match the pattern projects/{project}/locations/{location}/keyRings/{key_ring}/cryptoKeys/{crypto_key}.
        Function &kmsKeyName(const String &value) { return wr.set<Function &, String>(*this, value, buf, bufSize, 8, __func__); }
    };

    struct UploadURLOptions : public BaseO4
    {
    public:
        // [Preview] Resource name of a KMS crypto key (managed by the user) used to encrypt/decrypt function source code objects in intermediate Cloud Storage buckets. When you generate an upload url and upload your source code, it gets copied to an intermediate Cloud Storage bucket. The source code is then copied to a versioned directory in the sources bucket in the consumer project during the function deployment.
        // It must match the pattern projects/{project}/locations/{location}/keyRings/{key_ring}/cryptoKeys/{crypto_key}.
        UploadURLOptions &kmsKeyName(const String &value) { return wr.set<UploadURLOptions &, String>(*this, value, buf, bufSize, 1, __func__); }

        // The function environment the generated upload url will be used for. The upload url for 2nd Gen functions can also be used for 1st gen functions, but not vice versa. If not specified, 2nd generation-style upload URLs are generated.
        UploadURLOptions &environment(Environment value) { return wr.set<UploadURLOptions &, const char *>(*this, _Environment[value].text, buf, bufSize, 2, __func__); }
    };

    /**
     * Cloud Functions List Query parameters
     */
    struct ListOptions : public BaseO6
    {
    protected:
        ObjectWriter owriter;
        StringUtil sut;

        ListOptions &setBuf()
        {
            owriter.clear(buf[0]);
            for (size_t i = 1; i < bufSize; i++)
            {
                if (buf[i].length())
                {
                    if (buf[0].length())
                        buf[0] += '&';
                    buf[0] += buf[i];
                }
            }
            return *this;
        }

    public:
        // Maximum number of functions to return per call. The largest allowed pageSize is 1,000, if the pageSize is omitted or specified as greater than 1,000 then it will be replaced as 1,000. The size of the list response can be less than specified when used with filters.
        ListOptions &pageSize(uint64_t value)
        {
            buf[1] = "pageSize=" + sut.numString(value);
            return setBuf();
        }

        // The value returned by the last ListFunctionsResponse; indicates that this is a continuation of a prior functions.list call, and that the system should return the next page of data.
        ListOptions &pageToken(const String &value)
        {
            buf[2] = "pageToken=" + value;
            return setBuf();
        }

        // The filter for Functions that match the filter expression, following the syntax outlined in https://google.aip.dev/160.
        ListOptions &filter(const String &value)
        {
            buf[3] = "filter=" + value;
            return setBuf();
        }

        // The sorting order of the resources returned. Value should be a comma separated list of fields. The default sorting oder is ascending. See https://google.aip.dev/132#ordering.
        ListOptions &orderBy(const String &value)
        {
            buf[4] = "orderBy=" + value;
            return setBuf();
        }
    };

    /**
     * Encapsulates settings provided to GetIamPolicy.
     */
    struct GetPolicyOptions : public BaseO1
    {
    protected:
        ObjectWriter owriter;

    public:
        // Optional. The maximum policy version that will be used to format the policy.
        // Valid values are 0, 1, and 3. Requests specifying an invalid value will be rejected.
        // Requests for policies with any conditional role bindings must specify version 3. Policies with no conditional role bindings may specify any valid value or leave the field unset.
        // The policy in the response might use the policy version that you specified, or it might use a lower policy version. For example, if you specify version 3, but the policy has no conditional role bindings, the response uses version 1.
        GetPolicyOptions &requestedPolicyVersion(uint32_t value)
        {
            buf = "options.requestedPolicyVersion=" + String(value);
            return *this;
        }
    };

    /**
     * Class that provides the Policy and updateMask information for use in set IAM policy functiion.
     */
    struct SetPolicyOptions : public BaseO4
    {
    public:
        // REQUIRED: The complete policy to be applied to the resource. The size of the policy is limited to a few 10s of KB. An empty policy is a valid policy but certain Google Cloud services (such as Projects) might reject them.
        SetPolicyOptions &policy(const IAMPolicy::Policy &value) { return wr.set<SetPolicyOptions &, IAMPolicy::Policy>(*this, value, buf, bufSize, 1, __func__); }

        // OPTIONAL: A FieldMask specifying which fields of the policy to modify. Only the fields in the mask will be modified. If no mask is provided, the following default mask is used:
        // paths: "bindings, etag"
        // This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".
        SetPolicyOptions &updateMask(const String &value) { return wr.set<SetPolicyOptions &, String>(*this, value, buf, bufSize, 2, __func__); }
    };

    /**
     * Class that provides the permissions list for testing.
     */
    struct Permissions : public BaseO2
    {
    public:
        Permissions &permissions(const String &value) { return wr.set<Permissions &, String>(*this, value, buf, bufSize, 1, __func__); }
        // Obsoleted, use permissions instead.
        Permissions &add(const String &value) { return permissions(value); }
    };

    class Parent
    {

    private:
        String projectId, locationId, bucketId, functionId;

    public:
        Parent() {}
        explicit Parent(const String &projectId, const String &locationId)
        {
            this->projectId = projectId;
            this->locationId = locationId;
        }
        String getProjectId() const { return projectId; }
        String getLocationId() const { return locationId; }
        String getFunctionId() const { return functionId; }
        void setFunctionId(const String &functionId) { this->functionId = functionId; }
    };

    class DataOptions
    {
    public:
        String payload, extras;
        GoogleCloudFunctions::Parent parent;
        google_cloud_functions_request_type requestType = fn_undefined;
        unsigned long requestTime = 0;
        void copy(const DataOptions &rhs)
        {
            this->extras = rhs.extras;
            this->payload = rhs.payload;
        }

    private:
    };

    struct req_data
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path, uid, mime;
        reqns::http_request_method method = reqns::http_undefined;
        slot_options_t opt;
        DataOptions *options = nullptr;
        file_config_data *file = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        req_data() {}
        explicit req_data(AsyncClientClass *aClient, reqns::http_request_method method, slot_options_t opt, DataOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
        {
            this->aClient = aClient;
            this->method = method;
            this->opt = opt;
            this->options = options;
            this->file = file;
            this->aResult = aResult;
            this->cb = cb;
            this->uid = uid;
        }
    };
}
#endif
#endif