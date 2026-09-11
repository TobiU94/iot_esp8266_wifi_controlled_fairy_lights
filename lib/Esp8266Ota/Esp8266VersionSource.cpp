#include "Esp8266VersionSource.h"
#include <string>

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

Esp8266VersionSource::Esp8266VersionSource(const char *versionUrl)
    : _versionUrl(versionUrl) {};

bool Esp8266VersionSource::fetchAvailableVersion(std::string &availableVersion)
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

    String response = http.getString();
    response.trim();

    availableVersion = response.c_str();

    http.end();
    return true;
}