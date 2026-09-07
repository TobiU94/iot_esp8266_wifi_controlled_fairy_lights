#pragma once
#include <Arduino.h>

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

    const String &getCurrentFirmwareVersion() const;
    OtaStatus getUpdateStatus();

private:
    bool fetchAvailableVersion(String &availableVersion);

    const char *_versionUrl = nullptr;  // set by .begin()
    const char *_firmwareUrl = nullptr; // set by .begin()
    String _currentVersion = "31";      //"1.0.0";   // TODO: inject from build
    unsigned long _lastCheckMs = 0;
};