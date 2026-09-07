#include <ESP8266HTTPClient.h>
#include "LightWebServer.h"
#include "RelayController.h"
#include "OtaUpdater.h"

LightWebServer::LightWebServer(RelayController &relay, OtaUpdater &otaUpdater)
    : _relay(relay), _otaUpdater(otaUpdater), _server(80) {}

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

            String response = "{\"current\":\"";
            response += status.currentVersion;
            response += "\",\"available\":\"";
            response += status.availableVersion;
            response += "\",\"updateAvailable\":";
            response += status.updateAvailable ? "true" : "false";
            response += "}";

            _server.send(HTTP_CODE_OK, "application/json", response); });
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