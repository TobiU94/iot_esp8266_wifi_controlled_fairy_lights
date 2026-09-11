#include "Esp8266FirmwareInstaller.h"

#include <Arduino.h>
#include <ESP8266httpUpdate.h>

FirmwareUpdateResult Esp8266FirmwareInstaller::install(const char *firmwareUrl)
{
    WiFiClientSecure client;
    // skip certificate validation for now; TODO: do clean certificate validation once  functionality proven
    client.setInsecure();

    ESPhttpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    const t_httpUpdate_return result = ESPhttpUpdate.update(client, firmwareUrl);

    switch (result)
    {
    case HTTP_UPDATE_FAILED:
        Serial.printf("Update failed: %s\n", ESPhttpUpdate.getLastErrorString().c_str());
        return FirmwareUpdateResult::Failed;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("Firmware up-to-date - No updated needed");
        return FirmwareUpdateResult::NoUpdate;
    case HTTP_UPDATE_OK:
        Serial.println("Update OK - rebooting...");
        return FirmwareUpdateResult::Succeeded;
    }

    return FirmwareUpdateResult::Failed;
};