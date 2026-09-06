#include "OtaUpdater.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "ota_config.h"

void OtaUpdater::begin(const char *versionUrl, const char *firmwareUrl)
{
    _versionUrl = versionUrl;
    _firmwareUrl = firmwareUrl;
}

void OtaUpdater::checkForUpdateIfDue()
{
    unsigned long now = millis();
    if (now - _lastCheckMs > OtaConfig::CHECK_INTERVAL_MS)
    {
        _lastCheckMs = now;
        checkForUpdate();
    }
}

void OtaUpdater::checkForUpdate()
{
    // 1. Fetch version.txt from server
    WiFiClientSecure client;

    // skip certificate validation for now; TODO: do clean certificate validation once  functionality proven
    client.setInsecure();

    HTTPClient http;

    http.begin(client, _versionUrl);

    int httpCode = http.GET();

    if (httpCode == 200) // http code 200 -> request successfully processed
    {
        String newVersion = http.getString();
        newVersion.trim();

        if (newVersion != _currentVersion)
        {
            Serial.println("New firmware: " + newVersion);

            // 2. Download and flash
            t_httpUpdate_return ret = ESPhttpUpdate.update(client, _firmwareUrl);

            switch (ret)
            {
            case HTTP_UPDATE_FAILED:
                Serial.printf("Update failed: %s\n", ESPhttpUpdate.getLastErrorString().c_str());
                break;
            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("Firmware up-to-date - No updated needed");
                break;
            case HTTP_UPDATE_OK:
                Serial.println("Update OK - rebooting...");
                break; // ESP auto-restarts
            }
        }
    }
    http.end();
}