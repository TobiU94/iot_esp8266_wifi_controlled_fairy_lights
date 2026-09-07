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

    static constexpr const char *HEADER_MESSAGE =
        "Hello from ESP8266 WiFi Fairy Lights! \n"
        "GET    /               - show this help\n"
        "GET    /on             - turn lights on\n"
        "GET    /off            - turn lights off\n"
        "GET    /ota/version    - show installed firmware version\n"
        "GET    /ota/status     - compare installed and available versions\n";
};