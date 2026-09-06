#pragma once
#include <Arduino.h>

class OtaUpdater
{
public:
    void begin(const char *versionUrl, const char *firmwareUrl);
    void checkForUpdate(); // called periodically from the loop
    void checkForUpdateIfDue();

private:
    const char *_versionUrl = nullptr;  // set by .begin()
    const char *_firmwareUrl = nullptr; // set by .begin()
    String _currentVersion = "1.0.0";   // TODO: inject from build
    unsigned long _lastCheckMs = 0;
};