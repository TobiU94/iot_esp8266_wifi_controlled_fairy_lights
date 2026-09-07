#pragma once
#include <Arduino.h>

#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "dev"
#endif

struct OtaStatus
{
    String currentVersion;
    String availableVersion;
    bool updateAvailable;
    bool querySucceeded;
};

class OtaUpdater
{
public:
    void begin(const char *versionUrl, const char *firmwareUrl);

    void checkForUpdate(); // called periodically from the loop
    void checkForUpdateIfDue();
    void checkForUpdateNow();

    const String &getCurrentFirmwareVersion() const;
    OtaStatus getUpdateStatus();

private:
    bool fetchAvailableVersion(String &availableVersion);

    const char *_versionUrl = nullptr;         // set by .begin()
    const char *_firmwareUrl = nullptr;        // set by .begin()
    String _currentVersion = FIRMWARE_VERSION; //"1.0.0";   // TODO: inject from build
    unsigned long _lastCheckMs = 0;
};