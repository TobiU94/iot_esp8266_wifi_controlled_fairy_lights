#include <limits>
#include <ESP8266HTTPClient.h>
#include "LightWebServer.h"
#include "RelayController.h"
#include "OtaUpdater.h"
#include "ota_config.h"
#include "BuildInfo.h"

LightWebServer::LightWebServer(RelayController &relay, OtaUpdater &otaUpdater, const BuildInfo &buildInfo)
    : _relay(relay), _otaUpdater(otaUpdater), _buildInfo(buildInfo), _server(80) {}

void LightWebServer::begin()
{
    _server.onNotFound([this]()
                       { handleNotFound(); });
    _server.on("/", [this]()
               { handleRoot(); });
    _server.on("/on", [this]()
               { 
        _server.send(HTTP_CODE_OK, "text/plain", "LIGHTS ON");
        _relay.turnOn(); });
    _server.on("/off", [this]()
               {
                   _server.send(HTTP_CODE_OK, "text/plain", "LIGHTS OFF");
                   _relay.turnOff(); });

    _server.on("/ota/version", [this]()
               { _server.send(HTTP_CODE_OK, "text/plain", _otaUpdater.getCurrentFirmwareVersion()); });

    _server.on("/ota/status", [this]()
               {
            const OtaStatus status = _otaUpdater.getUpdateStatus();

            if (!status.querySucceeded)
            {
                _server.send(HTTP_CODE_SERVICE_UNAVAILABLE, "text/plain", "Unable to query firmware version");
                return ;
            }

            // 1. Single source of truth for the response format
            static constexpr char jsonFormat[] =
                "{\n"
                "  \"current\": \"%s\",\n"
                "  \"available\": \"%s\",\n"
                "  \"updateAvailable\": %s\n"
                "}";

            // 2. Compute exact stack buffer required dynamically based on runtime string lengths
            //    Template size minus 7 specifier chars ("%s", "%s", "%s") + 5 ("false") + null terminator
            const size_t bufferSize = sizeof(jsonFormat) - 7 + status.currentVersion.length() + status.availableVersion.length() + 5 // Max length of boolean string ("false")
                                      + 1;                                                                                           // Null terminator

            char jsonBuffer[bufferSize];

            snprintf(jsonBuffer, sizeof(jsonBuffer), jsonFormat,
                     status.currentVersion.c_str(),
                     status.availableVersion.c_str(),
                     status.updateAvailable ? "true" : "false");

            _server.send(HTTP_CODE_OK, "application/json", jsonBuffer); });

    _server.on("/ota/config", [this]()
               { 
                // Auto-calculate exact stack space required at compile time
                // 1. Single source of truth for the JSON format template
                static constexpr char jsonFormat[] =
                    "{\n"
                    "  \"url\": {\n"
                    "    \"version_txt\": \"%s\",\n"
                    "    \"firmware_bin\": \"%s\"\n"
                    "  },\n"
                    "  \"check_interval_ms\": %lu\n"
                    "}";

                // 2. Generic max character length of the integer type (e.g. 10 digits for uint32_t, 20 for uint64_t)
                using IntervalType = decltype(OtaConfig::CHECK_INTERVAL_MS);
                constexpr size_t maxIntDigits = std::numeric_limits<IntervalType>::digits10 + 1;

                // 3. Calculate buffer size: template length minus 5 specifier chars ("%s", "%s", "%lu")
                //    plus the dynamic lengths of the values + 1 null terminator
                const size_t bufferSize = sizeof(jsonFormat) - 5 + strlen(OtaConfig::VERSION_URL) + strlen(OtaConfig::FIRMWARE_URL) + maxIntDigits + 1;
                char jsonBuffer[bufferSize];

                snprintf(jsonBuffer, sizeof(jsonBuffer), jsonFormat,
                         OtaConfig::VERSION_URL,
                         OtaConfig::FIRMWARE_URL,
                         OtaConfig::CHECK_INTERVAL_MS);

                _server.send(HTTP_CODE_OK, "application/json", jsonBuffer); });

    _server.on("/build/info", [this]()
               {
                   static constexpr char jsonFormat[] =
                       "{\n"
                       "  \"firmwareVersion\": \"%s\",\n"
                       "  \"gitCommitHash\": \"%s\",\n"
                       "  \"buildTimestamp\": \"%s\"\n"
                       "}";

                   const size_t bufferSize = sizeof(jsonFormat) - 6 + strlen(_buildInfo.firmwareVersion) + strlen(_buildInfo.gitCommitHash) + strlen(_buildInfo.buildTimestamp) + 1;

                   char jsonBuffer[bufferSize];

                   snprintf(jsonBuffer, sizeof(jsonBuffer), jsonFormat,
                            _buildInfo.firmwareVersion,
                            _buildInfo.gitCommitHash,
                            _buildInfo.buildTimestamp);
                    _server.send(HTTP_CODE_OK, "application/json", jsonBuffer); });

    _server.on("/ota/check-now", [this]()
               {
        _server.send(
            HTTP_CODE_ACCEPTED,
            "text/plain",
            "Manual OTA update check started");
        _otaUpdater.checkForUpdateNow(); });
    _server.begin();
}

void LightWebServer::handleClient()
{
    _server.handleClient();
}

void LightWebServer::handleRoot()
{
    _server.send(200, "text/plain", HEADER_MESSAGE);
}

void LightWebServer::handleNotFound()
{
    char buffer[256];
    int offset = snprintf(buffer, sizeof(buffer),
                          "File not found\n\nURI: %s\nMethod: %s\nArguments: %d\n",
                          _server.uri().c_str(),
                          _server.method() == HTTP_GET ? "GET" : "POST",
                          _server.args());

    for (uint8_t i = 0; (i < _server.args()) && (offset < (int)sizeof(buffer)); ++i)
    {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                           " %s: %s\n",
                           _server.argName(i).c_str(),
                           _server.arg(i).c_str());
    }

    _server.send(404, "text/plain", buffer);
}