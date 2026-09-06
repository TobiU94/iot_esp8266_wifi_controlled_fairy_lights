#pragma once
#include <ESP8266WebServer.h>
#include "RelayController.h"
#include "OtaUpdater.h"

class LightWebServer
{
public:
    LightWebServer(RelayController &relay, OtaUpdater &otaUpdater);

    void begin();
    void handleClient();

private:
    void handleRoot();
    void handleNotFound();

    RelayController &_relay;
    OtaUpdater &_otaUpdater;
    ESP8266WebServer _server;

    static constexpr const char *HEADER_MESSAGE = "Hello from ESP8266! \n/on: to turn LED ON \n/off: to turn LED OFF \n/ota/version: currently running firmware version";
};