#include "OtaUpdater.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "ota_config.h"
#include "BuildInfo.h"

OtaUpdater::OtaUpdater(const BuildInfo &buildInfo, const char *versionUrl, const char *firmwareUrl)
    : _buildInfo(buildInfo), _versionUrl(versionUrl), _firmwareUrl(firmwareUrl) {};

// void OtaUpdater::begin(const char *versionUrl, const char *firmwareUrl)
// {
//     _versionUrl = versionUrl;
//     _firmwareUrl = firmwareUrl;
// }

void OtaUpdater::checkForUpdateIfDue()
{
    unsigned long now = millis();
    if (now - _lastCheckMs > OtaConfig::CHECK_INTERVAL_MS)
    {
        _lastCheckMs = now;

        Serial.println("Checking for firmware update...");
        checkForUpdate();
    }
}

void OtaUpdater::checkForUpdateNow()
{
    Serial.println("Manual OTA update check requested.");
    checkForUpdate();
}

void OtaUpdater::checkForUpdate()
{

    String availableVersion;

    // Case 0: fetch unsuccessful
    if (!fetchAvailableVersion(availableVersion))
    {
        Serial.println("Unable to query available firmware version.");
        return;
    }

    // Case 1: board already has latest released version
    if (availableVersion == _buildInfo.firmwareVersion)
    {
        Serial.println("Firmware is up to date.");
        return;
    }

    // Case 2: board does not yet have latest released version
    Serial.println("New firmware available: " + availableVersion);

    // 1. Fetch version.txt from server
    WiFiClientSecure client;
    // skip certificate validation for now; TODO: do clean certificate validation once  functionality proven
    client.setInsecure();

    ESPhttpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    const t_httpUpdate_return result = ESPhttpUpdate.update(client, _firmwareUrl);

    switch (result)
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

const char *OtaUpdater::getCurrentFirmwareVersion() const
{
    return _buildInfo.firmwareVersion;
}

OtaStatus OtaUpdater::getUpdateStatus()
{
    // makes use of fetchAvailableVersion

    OtaStatus status{
        _buildInfo.firmwareVersion,
        "",
        false,
        false};

    // checks whether query is successful, and if so in-place modifies status.availableVersion
    status.querySucceeded = fetchAvailableVersion(status.availableVersion);

    if (status.querySucceeded)
    {
        status.updateAvailable = (status.availableVersion != status.currentVersion);
    }

    return status;
}

bool OtaUpdater::fetchAvailableVersion(String &availableVersion)
{
    WiFiClientSecure client;
    client.setInsecure(); // TODO: ultimately replace by safe connection

    HTTPClient http;
    http.begin(client, _versionUrl);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setRedirectLimit(3);

    const int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK)
    {
        Serial.printf("Version request failed: HTTP %d\n", httpCode);
        http.end(); // ensure clean shutdown of HTTPClient, even if request failed
        return false;
    }

    availableVersion = http.getString();
    availableVersion.trim();

    // clean shutdown of HTTPClient
    http.end();
    return true;
}