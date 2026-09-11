#pragma once

#include <cstdint> // for uint32_t
#include <string>

#include "BuildInfo.h"
#include "OtaDependencies.h"

#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "dev"
#endif

struct OtaStatus
{
    std::string currentVersion;
    std::string availableVersion;
    bool updateAvailable;
    bool querySucceeded;
};

class OtaUpdater
{
public:
    OtaUpdater(
        const BuildInfo &buildInfo,
        const char *firmwareUrl,
        IVersionSource &versionSource,
        IFirmwareInstaller &firmwareInstaller,
        IClock &clock,
        uint32_t checkIntervalMs);

    void checkForUpdate(); // called periodically from the loop
    void checkForUpdateIfDue();
    void checkForUpdateNow();

    const char *getCurrentFirmwareVersion() const;
    OtaStatus getUpdateStatus();

private:
    bool fetchAvailableVersion(std::string &availableVersion);

    // dependency-injected; not owned
    const BuildInfo &_buildInfo;
    const char *_firmwareUrl;
    IVersionSource &_versionSource;
    IFirmwareInstaller &_firmwareInstaller;
    IClock &_clock;

    //    String _currentVersion = FIRMWARE_VERSION; //"1.0.0";   // TODO: inject from build
    uint32_t _checkIntervalMs;
    uint32_t _lastCheckMs = 0;
};