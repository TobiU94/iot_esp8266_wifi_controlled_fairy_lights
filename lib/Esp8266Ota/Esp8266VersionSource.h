#include "OtaDependencies.h"

class Esp8266VersionSource : public IVersionSource
{
public:
    explicit Esp8266VersionSource(const char *versionUrl);
    bool fetchAvailableVersion(std::string &availableVersion) override;

private:
    const char *_versionUrl;
};