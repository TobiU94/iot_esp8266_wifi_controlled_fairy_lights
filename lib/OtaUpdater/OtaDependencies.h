#pragma once

#include <string> // use std::string to avoid Arduino.h's String dtype; must compile natively

class IVersionSource
{
public:
    virtual ~IVersionSource() = default;
    virtual bool fetchAvailableVersion(std::string &availableVersion) = 0;
};

enum class FirmwareUpdateResult
{
    Failed,
    NoUpdate,
    Succeeded
};

class IFirmwareInstaller
{
public:
    virtual ~IFirmwareInstaller() = default;

    virtual FirmwareUpdateResult install(const char *firmwareUrl) = 0;
};

class IClock
{
public:
    virtual ~IClock() = default;

    virtual uint32_t millis() const = 0; // uint_32_t to later match ESP8266's millis() return type
};