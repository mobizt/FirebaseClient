/**
 * Created March 18, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef CLOUD_FUNCTIONS_DATA_OPTIONS_H
#define CLOUD_FUNCTIONS_DATA_OPTIONS_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/JSON.h"
#include "./core/ObjectWriter.h"
#include "./core/AsyncClient/AsyncClient.h"
#include "./core/URL.h"

// https://cloud.google.com/functions/docs/reference/rest/v2/projects.locations.functions

#if defined(ENABLE_FUNCTIONS)

namespace GoogleCloudFunctions
{
    enum google_cloud_functions_request_type
    {
        google_cloud_functions_request_type_undefined,
        google_cloud_functions_request_type_create,
        google_cloud_functions_request_type_delete,
        google_cloud_functions_request_type_call,
        google_cloud_functions_request_type_deploy,
        google_cloud_functions_request_type_download,
        google_cloud_functions_request_type_list,
        google_cloud_functions_request_type_patch,
        google_cloud_functions_request_type_set_iam_policy
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
    /**
     * Location of the source in a Google Cloud Source Repository.
     */
    struct RepoSource : public Printable
    {
    protected:
        size_t bufSize = 7;
        String buf[7];
        ObjectWriter owriter;
        URLHelper uh;

        RepoSource &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // ID of the project that owns the Cloud Source Repository. If omitted, the project ID requesting the build is assumed.
        RepoSource &projectId(const String &value) { return setObject(buf[1], FPSTR("projectId"), value, true, true); }
        // Name of the Cloud Source Repository.
        RepoSource &repoName(const String &value) { return setObject(buf[2], FPSTR("repoName"), value, true, true); }
        // Directory, relative to the source root, in which to run the build.
        // This must be a relative path. If a step's dir is specified and is an absolute path,
        // this value is ignored for that step's execution. eg. helloworld (no leading slash allowed)
        RepoSource &dir(const String &value) { return setObject(buf[3], FPSTR("dir"), value, true, true); }
        // Regex matching branches to build.
        // The syntax of the regular expressions accepted is the syntax accepted by RE2
        // and described at https://github.com/google/re2/wiki/Syntax
        RepoSource &branchName(const String &value)
        {
            // Union field revision.
            if (buf[5].length() == 0 && buf[6].length() == 0)
                return setObject(buf[4], FPSTR("branchName"), value.c_str(), false, true);
            return *this;
        }
        // Regex matching tags to build.
        // The syntax of the regular expressions accepted is the syntax accepted by RE2
        //  and described at https://github.com/google/re2/wiki/Syntax
        RepoSource &tagName(const String &value)
        {
            // Union field revision.
            if (buf[4].length() == 0 && buf[6].length() == 0)
                return setObject(buf[5], FPSTR("tagName"), value.c_str(), false, true);
            return *this;
        }
        // Explicit commit SHA to build.
        RepoSource &commitSha(const String &value)
        {
            // Union field revision.
            if (buf[4].length() == 0 && buf[5].length() == 0)
                return setObject(buf[6], FPSTR("commitSha"), value.c_str(), false, true);
            return *this;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Location of the source in an archive file in Google Cloud Storage.
     */
    struct StorageSource : public Printable
    {
    protected:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        URLHelper uh;

        StorageSource &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Google Cloud Storage bucket containing the source (see Bucket Name Requirements, https://cloud.google.com/storage/docs/bucket-naming#requirements).
        StorageSource &bucket(const String &value) { return setObject(buf[1], FPSTR("bucket"), value, true, true); }
        // Google Cloud Storage object containing the source.
        // This object must be a gzipped archive file (.tar.gz) containing source to build.
        StorageSource &object(const String &value) { return setObject(buf[2], FPSTR("object"), value, true, true); }
        // Google Cloud Storage generation (int64 format) for the object. If the generation is omitted, the latest generation will be used.
        StorageSource &generation(const String &value) { return setObject(buf[3], FPSTR("generation"), value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * The location of the function source code.
     */
    struct Source : public Printable
    {
    protected:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        URLHelper uh;

        Source &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // If provided, get the source from this location in Google Cloud Storage.
        Source &storageSource(const StorageSource &value)
        {
            // Union field source
            if (buf[2].length() == 0 && buf[3].length() == 0)
                return setObject(buf[1], FPSTR("storageSource"), value.c_str(), false, true);
            return *this;
        }
        // If provided, get the source from this location in a Cloud Source Repository.
        Source &repoSource(const RepoSource &value)
        {
            // Union field source
            if (buf[1].length() == 0 && buf[3].length() == 0)
                return setObject(buf[2], FPSTR("repoSource"), value.c_str(), false, true);
            return *this;
        }
        // If provided, get the source from GitHub repository. This option is valid only for GCF 1st Gen function. Example: https://github.com///blob//
        Source &gitUri(const String &value)
        {
            // Union field source
            if (buf[1].length() == 0 && buf[2].length() == 0)
                return setObject(buf[3], FPSTR("gitUri"), value, true, true);
            return *this;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Describes the Build step of the function that builds a container from the given source.
     */
    struct BuildConfig : public Printable
    {
    protected:
        size_t bufSize = 10;
        String buf[10];
        ObjectWriter owriter;
        URLHelper uh;

        BuildConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // The runtime in which to run the function. Required when deploying a new function, optional when updating an existing function. For a complete list of possible choices, see the gcloud command reference.
        BuildConfig &runtime(const String &value) { return setObject(buf[1], FPSTR("runtime"), value, true, true); }
        // The name of the function (as defined in source code) that will be executed. Defaults to the resource name suffix, if not specified. For backward compatibility, if function with given name is not found, then the system will try to use function named "function". For Node.js this is name of a function exported by the module specified in source_location.
        BuildConfig &entryPoint(const String &value) { return setObject(buf[2], FPSTR("entryPoint"), value, true, true); }
        // The location of the function source code.
        BuildConfig &source( Source value) { return setObject(buf[3], FPSTR("source"), value.c_str(), false, true); }
        // Name of the Cloud Build Custom Worker Pool that should be used to build the function. The format of this field is projects/{project}/locations/{region}/workerPools/{workerPool} where {project} and {region} are the project id and region respectively where the worker pool is defined and {workerPool} is the short name of the worker pool.
        BuildConfig &workerPool(const String &value) { return setObject(buf[4], FPSTR("workerPool"), value, true, true); }
        // User-provided build-time environment variables for the function
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        BuildConfig &environmentVariables(const object_t &value) { return setObject(buf[5], FPSTR("environmentVariables"), value.c_str(), false, true); }
        // Docker Registry to use for this deployment. This configuration is only applicable to 1st Gen functions, 2nd Gen functions can only use Artifact Registry.
        // If dockerRepository field is specified, this field will be automatically set as ARTIFACT_REGISTRY. If unspecified, it currently defaults to CONTAINER_REGISTRY. This field may be overridden by the backend for eligible deployments.
        BuildConfig &dockerRegistry(DockerRegistry value)
        {
            if (value == DOCKER_REGISTRY_UNSPECIFIED)
                return setObject(buf[6], "dockerRegistry", "DOCKER_REGISTRY_UNSPECIFIED", true, true);
            else if (value == CONTAINER_REGISTRY)
                return setObject(buf[6], "dockerRegistry", "CONTAINER_REGISTRY", true, true);
            else if (value == ARTIFACT_REGISTRY)
                return setObject(buf[6], "dockerRegistry", "ARTIFACT_REGISTRY", true, true);
            return *this;
        }
        // Repository in Artifact Registry to which the function docker image will be pushed after it is built by Cloud Build. If specified by user, it is created and managed by user with a customer managed encryption key. Otherwise, GCF will create and use a repository named 'gcf-artifacts' for every deployed region.
        BuildConfig &dockerRepository(const String &value) { return setObject(buf[7], FPSTR("dockerRepository"), value, true, true); }
        //[Preview] Service account to be used for building the container
        BuildConfig &serviceAccount(const String &value) { return setObject(buf[8], FPSTR("serviceAccount"), value, true, true); }
        // An identifier for Firebase function sources. Disclaimer: This field is only supported for Firebase function deployments.
        BuildConfig &sourceToken(const String &value) { return setObject(buf[9], FPSTR("sourceToken"), value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Configuration for a single version.
     */
    struct SecretVersion : public Printable
    {
    protected:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;
        URLHelper uh;

        SecretVersion &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Version of the secret (version number or the string 'latest'). It is preferable to use latest version with secret volumes as secret value changes are reflected immediately.
        SecretVersion &version(const String &value) { return setObject(buf[1], FPSTR("version"), value, true, true); }
        // Relative path of the file under the mount path where the secret value for this version will be fetched and made available. For example, setting the mountPath as '/etc/secrets' and path as secret_foo would mount the secret value file at /etc/secrets/secret_foo.
        SecretVersion &path(const String &value) { return setObject(buf[2], FPSTR("path"), value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Configuration for a secret volume. It has the information necessary to fetch the secret value from secret manager and make it available as files mounted at the requested paths within the application container.
     */
    struct SecretVolume : public Printable
    {
    protected:
        size_t bufSize = 5;
        String buf[5];
        ObjectWriter owriter;
        URLHelper uh;

        SecretVolume &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // The path within the container to mount the secret volume. For example, setting the mountPath as /etc/secrets would mount the secret value files under the /etc/secrets directory. This directory will also be completely shadowed and unavailable to mount any other secrets. Recommended mount path: /etc/secrets
        SecretVolume &mountPath(const String &value) { return setObject(buf[1], FPSTR("mountPath"), value, true, true); }
        // Project identifier (preferably project number but can also be the project ID) of the project that contains the secret. If not set, it is assumed that the secret is in the same project as the function.
        SecretVolume &projectId(const String &value) { return setObject(buf[2], FPSTR("projectId"), value, true, true); }
        // Name of the secret in secret manager (not the full resource name).
        SecretVolume &secret(const String &value) { return setObject(buf[3], FPSTR("secret"), value, true, true); }
        // List of secret versions to mount for this secret. If empty, the latest version of the secret will be made available in a file named after the secret under the mount point.
        SecretVolume &versions(const SecretVersion &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[4], FPSTR("versions"), value.c_str(), false);
            return *this;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Configuration for a secret environment variable. It has the information necessary to fetch the secret value from secret manager and expose it as an environment variable.
     */
    struct SecretEnvVar : public Printable
    {
    protected:
        size_t bufSize = 5;
        String buf[5];
        ObjectWriter owriter;
        URLHelper uh;

        SecretEnvVar &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Name of the environment variable.
        SecretEnvVar &key(const String &value) { return setObject(buf[1], FPSTR("key"), value, true, true); }
        // Project identifier (preferably project number but can also be the project ID) of the project that contains the secret. If not set, it is assumed that the secret is in the same project as the function.
        SecretEnvVar &projectId(const String &value) { return setObject(buf[2], FPSTR("projectId"), value, true, true); }
        // Name of the secret in secret manager (not the full resource name).
        SecretEnvVar &secret(const String &value) { return setObject(buf[3], FPSTR("secret"), value, true, true); }
        // Version of the secret (version number or the string 'latest'). It is recommended to use a numeric version for secret environment variables as any updates to the secret value is not reflected until new instances start.
        SecretEnvVar &version(const String &value) { return setObject(buf[4], FPSTR("version"), value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * Describes the Service being deployed. Currently Supported : Cloud Run (fully managed).
     */
    struct ServiceConfig : public Printable
    {
    protected:
        size_t bufSize = 16;
        String buf[16];
        ObjectWriter owriter;
        URLHelper uh;

        ServiceConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // The function execution timeout. Execution is considered failed and can be terminated if the function is not completed at the end of the timeout period. Defaults to 60 seconds.
        ServiceConfig &timeoutSeconds(uint32_t value) { return setObject(buf[1], FPSTR("timeoutSeconds"), String(value), false, true); }
        // The amount of memory available for a function. Defaults to 256M. Supported units are k, M, G, Mi, Gi. If no unit is supplied the value is interpreted as bytes. See https://github.com/kubernetes/kubernetes/blob/master/staging/src/k8s.io/apimachinery/pkg/api/resource/quantity.go a full description.
        ServiceConfig &availableMemory(const String &value) { return setObject(buf[2], FPSTR("availableMemory"), value, true, true); }
        // The number of CPUs used in a single container instance. Default value is calculated from available memory. Supports the same values as Cloud Run, see https://cloud.google.com/run/docs/reference/rest/v1/Container#resourcerequirements Example: "1" indicates 1 vCPU
        ServiceConfig &availableCpu(const String &value) { return setObject(buf[3], FPSTR("availableCpu"), value, true, true); }
        // Environment variables that shall be available during function execution.
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        ServiceConfig &environmentVariables(const object_t &value) { return setObject(buf[4], FPSTR("environmentVariables"), value.c_str(), false, true); }
        // The limit on the maximum number of function instances that may coexist at a given time.
        // In some cases, such as rapid traffic surges, Cloud Functions may, for a short period of time, create more instances than the specified max instances limit. If your function cannot tolerate this temporary behavior, you may want to factor in a safety margin and set a lower max instances value than your function can tolerate.
        //  See the Max Instances (https://cloud.google.com/functions/docs/max-instances) Guide for more details.
        ServiceConfig &maxInstanceCount(uint32_t value) { return setObject(buf[5], FPSTR("maxInstanceCount"), String(value), false, true); }
        // The limit on the minimum number of function instances that may coexist at a given time.
        // Function instances are kept in idle state for a short period after they finished executing the request to reduce cold start time for subsequent requests. Setting a minimum instance count will ensure that the given number of instances are kept running in idle state always. This can help with cold start times when jump in incoming request count occurs after the idle instance would have been stopped in the default case.
        ServiceConfig &minInstanceCount(uint32_t value) { return setObject(buf[6], FPSTR("minInstanceCount"), String(value), false, true); }
        // The Serverless VPC Access connector that this cloud function can connect to. The format of this field is projects/*/locations/*/connectors/*.
        ServiceConfig &vpcConnector(const String &value) { return setObject(buf[7], FPSTR("vpcConnector"), value, true, true); }
        // The egress settings for the connector, controlling what traffic is diverted through it.
        ServiceConfig &vpcConnectorEgressSettings(VpcConnectorEgressSettings value)
        {
            if (value == VPC_CONNECTOR_EGRESS_SETTINGS_UNSPECIFIED)
                return setObject(buf[8], "vpcConnectorEgressSettings", "VPC_CONNECTOR_EGRESS_SETTINGS_UNSPECIFIED", true, true);
            else if (value == PRIVATE_RANGES_ONLY)
                return setObject(buf[8], "vpcConnectorEgressSettings", "PRIVATE_RANGES_ONLY", true, true);
            else if (value == ALL_TRAFFIC)
                return setObject(buf[8], "vpcConnectorEgressSettings", "ALL_TRAFFIC", true, true);
            return *this;
        }
        // The ingress settings for the function, controlling what traffic can reach it.
        ServiceConfig &ingressSettings(IngressSettings value)
        {
            if (value == INGRESS_SETTINGS_UNSPECIFIED)
                return setObject(buf[9], "ingressSettings", "INGRESS_SETTINGS_UNSPECIFIED", true, true);
            else if (value == ALLOW_ALL)
                return setObject(buf[9], "ingressSettings", "ALLOW_ALL", true, true);
            else if (value == ALLOW_INTERNAL_ONLY)
                return setObject(buf[9], "ingressSettings", "ALLOW_INTERNAL_ONLY", true, true);
            else if (value == ALLOW_INTERNAL_AND_GCLB)
                return setObject(buf[9], "ingressSettings", "ALLOW_INTERNAL_AND_GCLB", true, true);
            return *this;
        }
        // The email of the service's service account. If empty, defaults to {project_number}-compute@developer.gserviceaccount.com.
        ServiceConfig &serviceAccountEmail(const String &value) { return setObject(buf[10], FPSTR("serviceAccountEmail"), value, true, true); }
        // Whether 100% of traffic is routed to the latest revision. On functions.create and functions.patch, when set to true, the revision being deployed will serve 100% of traffic, ignoring any traffic split settings, if any. On functions.get, true will be returned if the latest revision is serving 100% of traffic.
        ServiceConfig &allTrafficOnLatestRevision(bool value) { return setObject(buf[11], FPSTR("allTrafficOnLatestRevision"), owriter.getBoolStr(value), false, true); }
        // Secret environment variables configuration.
        ServiceConfig &secretEnvironmentVariables(const SecretEnvVar &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[12], FPSTR("secretEnvironmentVariables"), value.c_str(), false);
            return *this;
        }
        // Secret volumes configuration.
        ServiceConfig &secretVolumes(const SecretVolume &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[13], FPSTR("secretVolumes"), value.c_str(), false);
            return *this;
        }
        // Sets the maximum number of concurrent requests that each instance can receive. Defaults to 1.
        ServiceConfig &maxInstanceRequestConcurrency(uint32_t value) { return setObject(buf[14], FPSTR("maxInstanceRequestConcurrency"), String(value), false, true); }
        // Security level configure whether the function only accepts https. This configuration is only applicable to 1st Gen functions with Http trigger. By default https is optional for 1st Gen functions; 2nd Gen functions are https ONLY.
        ServiceConfig &securityLevel(SecurityLevel value)
        {
            if (value == SECURITY_LEVEL_UNSPECIFIED)
                return setObject(buf[15], "securityLevel", "SECURITY_LEVEL_UNSPECIFIED", true, true);
            else if (value == SECURE_ALWAYS)
                return setObject(buf[15], "securityLevel", "SECURE_ALWAYS", true, true);
            else if (value == SECURE_OPTIONAL)
                return setObject(buf[15], "securityLevel", "SECURE_OPTIONAL", true, true);
            return *this;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Filters events based on exact matches on the CloudEvents attributes.
     */
    struct EventFilter : public Printable
    {
    protected:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        URLHelper uh;

        EventFilter &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Required. The name of a CloudEvents attribute.
        EventFilter &attribute(const String &value) { return setObject(buf[1], FPSTR("attribute"), value, true, true); }
        // Required. The value for the attribute.
        EventFilter &value(const String &value) { return setObject(buf[2], FPSTR("value"), value, true, true); }
        // Optional. The operator used for matching the events with the value of the filter. If not specified, only events that have an exact key-value pair specified in the filter are matched. The only allowed value is match-path-pattern.
        EventFilter &Operator(const String &value) { return setObject(buf[3], FPSTR("operator"), value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Describes EventTrigger, used to request events to be sent from another service.
     */
    struct EventTrigger : public Printable
    {
    protected:
        size_t bufSize = 9;
        String buf[9];
        ObjectWriter owriter;
        URLHelper uh;

        EventTrigger &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // The region that the trigger will be in. The trigger will only receive events originating in this region. It can be the same region as the function, a different region or multi-region, or the global region. If not provided, defaults to the same region as the function.
        EventTrigger &triggerRegion(const String &value) { return setObject(buf[1], FPSTR("triggerRegion"), value, true, true); }
        // Required. The type of event to observe. For example: google.cloud.audit.log.v1.written or google.cloud.pubsub.topic.v1.messagePublished.
        EventTrigger &eventType(const String &value) { return setObject(buf[2], FPSTR("eventType"), value, true, true); }
        // Criteria used to filter events.
        EventTrigger &eventFilters(const EventFilter &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[3], FPSTR("eventFilters"), value.c_str(), false);
            return *this;
        }
        // Optional. The name of a Pub/Sub topic in the same project that will be used as the transport topic for the event delivery. Format: projects/{project}/topics/{topic}.
        // This is only valid for events of type google.cloud.pubsub.topic.v1.messagePublished. The topic provided here will not be deleted at function deletion.
        EventTrigger &pubsubTopic(const String &value) { return setObject(buf[4], FPSTR("pubsubTopic"), value, true, true); }
        // Optional. The email of the trigger's service account. The service account must have permission to invoke Cloud Run services, the permission is run.routes.invoke. If empty, defaults to the Compute Engine default service account: {project_number}-compute@developer.gserviceaccount.com.
        EventTrigger &serviceAccountEmail(const String &value) { return setObject(buf[5], FPSTR("serviceAccountEmail"), value, true, true); }
        // Optional. If unset, then defaults to ignoring failures (i.e. not retrying them).
        EventTrigger &retryPolicy(RetryPolicy value)
        {
            if (value == RETRY_POLICY_UNSPECIFIED)
                return setObject(buf[6], "retryPolicy", "RETRY_POLICY_UNSPECIFIED", true, true);
            else if (value == RETRY_POLICY_DO_NOT_RETRY)
                return setObject(buf[6], "retryPolicy", "RETRY_POLICY_DO_NOT_RETRY", true, true);
            else if (value == RETRY_POLICY_RETRY)
                return setObject(buf[6], "retryPolicy", "RETRY_POLICY_RETRY", true, true);
            return *this;
        }
        // Optional. The name of the channel associated with the trigger in projects/{project}/locations/{location}/channels/{channel} format. You must provide a channel to receive events from Eventarc SaaS partners.
        EventTrigger &channel(const String &value) { return setObject(buf[7], FPSTR("channel"), value, true, true); }
        // Optional. The hostname of the service that 1st Gen function should be observed.
        // If no string is provided, the default service implementing the API will be used. For example, storage.googleapis.com is the default for all event types in the google.storage namespace.
        // The field is only applicable to 1st Gen functions.
        EventTrigger &service(const String &value) { return setObject(buf[8], FPSTR("service"), value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Describes a Cloud Function that contains user computation executed in response to an event. It encapsulates function and trigger configurations.
     */
    struct Function : public Printable
    {

    protected:
        size_t bufSize = 9;
        String buf[9];
        ObjectWriter owriter;
        URLHelper uh;

        Function &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // A user-defined name of the function. Function names must be unique globally and match pattern projects/*/locations/*/functions/*
        Function &name(const String &value) { return setObject(buf[1], FPSTR("name"), value, true, true); }
        // User-provided description of a function.
        Function &description(const String &value) { return setObject(buf[2], FPSTR("description"), value, true, true); }
        // Describes the Build step of the function that builds a container from the given source.
        Function &buildConfig(const BuildConfig &value) { return setObject(buf[3], FPSTR("buildConfig"), value.c_str(), false, true); }
        // Describes the Service being deployed. Currently deploys services to Cloud Run (fully managed).
        Function &serviceConfig(const ServiceConfig &value) { return setObject(buf[4], FPSTR("serviceConfig"), value.c_str(), false, true); }
        // An Eventarc trigger managed by Google Cloud Functions that fires events in response to a condition in another service.
        Function &eventTrigger(const EventTrigger &value) { return setObject(buf[5], FPSTR("eventTrigger"), value.c_str(), false, true); }
        // Labels associated with this Cloud Function.
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        Function &labels(const object_t &value) { return setObject(buf[6], FPSTR("labels"), value.c_str(), false, true); }
        // Describe whether the function is 1st Gen or 2nd Gen.
        Function &environment(Environment value)
        {
            if (value == ENVIRONMENT_UNSPECIFIED)
                return setObject(buf[7], "environment", "ENVIRONMENT_UNSPECIFIED", true, true);
            else if (value == GEN_1)
                return setObject(buf[7], "environment", "GEN_1", true, true);
            else if (value == GEN_2)
                return setObject(buf[7], "environment", "GEN_2", true, true);
            return *this;
        }
        //[Preview] Resource name of a KMS crypto key (managed by the user) used to encrypt/decrypt function resources.
        // It must match the pattern projects/{project}/locations/{location}/keyRings/{key_ring}/cryptoKeys/{crypto_key}.
        Function &kmsKeyName(const String &value) { return setObject(buf[8], FPSTR("kmsKeyName"), value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    class Parent
    {

    private:
        String projectId, locationId, bucketId;

    public:
        Parent() {}
        Parent(const String &projectId, const String &locationId, const String &bucketId)
        {
            this->projectId = projectId;
            this->locationId = locationId;
            this->bucketId = bucketId;

            if (this->bucketId.length() && this->bucketId.indexOf("://") > -1)
                this->bucketId.remove(0, this->bucketId.indexOf("://") + 3);

            if (this->bucketId.length())
            {
                if (this->bucketId.indexOf("://") > -1)
                    this->bucketId.remove(0, this->bucketId.indexOf("://") + 3);

                if (this->bucketId.length() && this->bucketId[this->bucketId.length() - 1] == '/')
                    this->bucketId.remove(this->bucketId.length() - 1, 1);
            }
        }
        String getProjectId() const { return projectId; }
        String getLocationId() const { return locationId; }
        String getBucketId() const { return bucketId; }
    };

    class DataOptions
    {
        friend class Messaging;

    public:
        String payload, extras;
        GoogleCloudFunctions::Parent parent;
        google_cloud_functions_request_type requestType = google_cloud_functions_request_type_undefined;
        unsigned long requestTime = 0;
        void copy(DataOptions &rhs)
        {
            this->extras = rhs.extras;
            this->payload = rhs.payload;
        }

    private:
    };

    struct async_request_data_t
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path;
        String uid;
        String mime;
        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;
        slot_options_t opt;
        DataOptions *options = nullptr;
        file_config_data *file = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        async_request_data_t() {}
        async_request_data_t(AsyncClientClass *aClient, const String &path, async_request_handler_t::http_request_method method, slot_options_t opt, DataOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
        {
            this->aClient = aClient;
            this->path = path;
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