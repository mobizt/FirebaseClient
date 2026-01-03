/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef RULES_DATA_OPTIONS_H
#define RULES_DATA_OPTIONS_H

#if defined(ENABLE_RULESETS)

#include <Arduino.h>

namespace Rules
{
    enum ReleaseExecutableVersion
    {
        RELEASE_EXECUTABLE_VERSION_UNSPECIFIED, // Executable format unspecified. Defaults to FIREBASE_RULES_EXECUTABLE_V1
        FIREBASE_RULES_EXECUTABLE_V1,           // Firebase Rules syntax 'rules2' executable versions: Custom AST for use with Java clients.
        FIREBASE_RULES_EXECUTABLE_V2            // CEL-based executable for use with C++ clients.
    };

     const struct firebase_ns::key_str_40 _ReleaseExecutableVersion[ReleaseExecutableVersion::FIREBASE_RULES_EXECUTABLE_V2 + 1] PROGMEM = {"RELEASE_EXECUTABLE_VERSION_UNSPECIFIED", "FIREBASE_RULES_EXECUTABLE_V1", "FIREBASE_RULES_EXECUTABLE_V2"};
   
}

enum firebase_rulesets_request_type
{
    rs_undefined,
    rs_create,
    rs_get,
    rs_delete,
    rs_list
};

enum firebase_releases_request_type
{
    rl_undefined,
    rl_create,
    rl_get,
    rl_get_executable,
    rl_delete,
    rl_list,
    rl_patch
};

// Ref https://firebase.google.com/docs/reference/rules/rest/v1/projects.rulesets/list
// https://firebase.google.com/docs/reference/rules/rest/v1/projects.releases/list

/**
 * This class used in RuleSets::list and Releases::list functions represents the query parametes.
 */
class ListOptions : public BaseO6
{
private:
    int pagesize = 10;
    DocumentMask msk;
    ObjectWriter owriter;
    URLUtil uut;

    ListOptions &set()
    {
        bool hasParam = false;
        owriter.clear(buf[0]);

        if (pagesize > 0)
            uut.addParam(buf[0], "pageSize", String(pagesize), hasParam);
        if (buf[1].length() > 0)
            uut.addParam(buf[0], "pageToken", buf[1], hasParam);
        if (buf[2].length() > 0)
            uut.addParam(buf[0], "filter", buf[2], hasParam);
        return *this;
    }

public:
    ListOptions() {}

    // Optional. Page size to load. Maximum of 100. Defaults to 10.
    // Note: pageSize is just a hint and the service may choose to load less than pageSize due to the size of the output.
    // To traverse all of the releases, caller should iterate until the pageToken is empty.
    ListOptions &pageSize(int value)
    {
        pagesize = value;
        return set();
    }

    // Optional. A Next page token for loading the next batch of Ruleset/Release instances.
    ListOptions &pageToken(const String &value)
    {
        buf[1] = value;
        return set();
    }

    // Optional. Ruleset/Release filter.
    // For Ruleset, the list method supports filters with restrictions on Ruleset.name.
    // Filters on Ruleset.create_time should use the date function which parses strings that conform to the RFC 3339 date/time specifications.
    // Example: create_time > date("2017-01-01T00:00:00Z") AND name=UUID-*
    // For Release, the list method supports filters with restrictions on the Release.name, and Release.ruleset_name.
    // Example 1: A filter of 'name=prod*' might return Releases with names within 'projects/foo' prefixed with 'prod':
    // Name -> Ruleset Name:
    // projects/foo/releases/prod -> projects/foo/rulesets/uuid1234
    // projects/foo/releases/prod/v1 -> projects/foo/rulesets/uuid1234
    // projects/foo/releases/prod/v2 -> projects/foo/rulesets/uuid8888
    // Example 2: A filter of name=prod* rulesetName=uuid1234 would return only Release instances for 'projects/foo' with names prefixed with 'prod' referring to the same Ruleset name of 'uuid1234':
    // Name -> Ruleset Name:
    // projects/foo/releases/prod -> projects/foo/rulesets/1234
    // projects/foo/releases/prod/v1 -> projects/foo/rulesets/1234
    // In the examples, the filter parameters refer to the search filters are relative to the project. Fully qualified prefixed may also be used.
    ListOptions &filter(const String value)
    {
        URLUtil uut;
        buf[2] = uut.encode(value);
        return set();
    }
};

class RulesetOptions
{
    friend class RuleSets;

public:
    String projectId, rulesetId, payload, pageToken, filter, extras;
    uint8_t pageSize = 10;
    firebase_rulesets_request_type requestType = rs_undefined;
};

class ReleaseOptions
{
    friend class Releases;

public:
    String projectId, releaseId, payload, pageToken, filter, extras;
    uint8_t pageSize = 10;
    firebase_releases_request_type requestType = rl_undefined;
};
#endif
#endif