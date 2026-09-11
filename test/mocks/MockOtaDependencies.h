#pragma once

#include <cstdint>
#include <string>
#include "OtaDependencies.h"

// controlls the result of retrieving version.txt
class MockVersionSource : public IVersionSource
{
public:
    bool fetchSuccess = true;
    std::string availableVersion; // must be public, so we can manipulate it
    int fetchCalls = 0;           // our sensor to track inner workings

    bool fetchAvailableVersion(std::string &result) override
    {
        ++fetchCalls;

        if (!fetchSuccess)
        {
            return false;
        }

        result = availableVersion;
        return true;
    }
};

// records whether installation was requested
class MockFirmwareInstaller : public IFirmwareInstaller
{
public:
    FirmwareUpdateResult result = FirmwareUpdateResult::Succeeded;

    const char *lastFirmwareUrl = nullptr;
    int installCalls = 0;

    FirmwareUpdateResult install(const char *firmwareUrl) override
    {
        ++installCalls;
        lastFirmwareUrl = firmwareUrl;
        return result;
    }
};

class MockClock : public IClock
{
public:
    uint32_t currentMs = 0;

    uint32_t millis() const
    {
        return currentMs;
    }
};