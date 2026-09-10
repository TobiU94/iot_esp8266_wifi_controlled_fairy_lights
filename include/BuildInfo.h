#pragma once

// Fallback macros if not injected by the build system / IDE
#ifndef BUILD_VERSION
#define BUILD_VERSION "0.0.0-dev"
#endif

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "SoMe__RaNdOm__HaSh"
#endif

#ifndef BUILD_TIMESTAMP
#define BUILD_TIMESTAMP __DATE__ " " __TIME__
#endif

struct BuildInfo
{
    const char *firmwareVersion = BUILD_VERSION;
    const char *gitCommitHash = GIT_COMMIT_HASH;
    const char *buildTimestamp = BUILD_TIMESTAMP;

    constexpr BuildInfo() = default;
    constexpr BuildInfo(const char *firmwareVersion, const char *gitCommitHash, const char *buildTimestamp)
        : firmwareVersion(firmwareVersion), gitCommitHash(gitCommitHash), buildTimestamp(buildTimestamp) {};
};