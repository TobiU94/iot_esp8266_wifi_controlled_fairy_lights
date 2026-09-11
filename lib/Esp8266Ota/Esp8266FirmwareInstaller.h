#pragma once
#include "OtaDependencies.h"

class Esp8266FirmwareInstaller : public IFirmwareInstaller
{
public:
    FirmwareUpdateResult install(const char *firmwareUrl) override;
};