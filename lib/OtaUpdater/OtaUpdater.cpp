#include "OtaUpdater.h"

OtaUpdater::OtaUpdater(
    const BuildInfo &buildInfo,
    const char *firmwareUrl,
    IVersionSource &versionSource,
    IFirmwareInstaller &firmwareInstaller,
    IClock &clock,
    uint32_t checkIntervalMs)
    : _buildInfo(buildInfo),
      _firmwareUrl(firmwareUrl),
      _versionSource(versionSource),
      _firmwareInstaller(firmwareInstaller),
      _clock(clock),
      _checkIntervalMs(checkIntervalMs) {};

void OtaUpdater::checkForUpdateIfDue()
{
    uint32_t now = _clock.millis();
    if (now - _lastCheckMs > _checkIntervalMs)
    {
        _lastCheckMs = now;

        checkForUpdate();
    }
}

void OtaUpdater::checkForUpdateNow()
{
    checkForUpdate();
}

void OtaUpdater::checkForUpdate()
{

    std::string availableVersion;

    // Case 0: fetch unsuccessful
    if (!fetchAvailableVersion(availableVersion))
    {
        return;
    }

    // Case 1: board already has latest released version -> do nothing
    if (availableVersion == _buildInfo.firmwareVersion)
    {
        return;
    }

    // Case 2: board does not yet have latest released version -> trigger firmware installation
    _firmwareInstaller.install(_firmwareUrl);
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

bool OtaUpdater::fetchAvailableVersion(std::string &availableVersion)
{
    return _versionSource.fetchAvailableVersion(availableVersion);
}