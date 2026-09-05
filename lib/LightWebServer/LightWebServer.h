#pragma once
#include <ESP8266WebServer.h>
#include "RelayController.h"

class LightWebServer
{
public:
    LightWebServer(RelayController &relay);

    void begin();
    void handleClient();

private:
    void handleRoot();
    void handleNotFound();

    RelayController &_relay;
    ESP8266WebServer _server;

    static constexpr const char *HEADER_MESSAGE = "Hello from ESP8266! \n/on: to turn LED ON \n/off: to turn LED OFF \n";
};