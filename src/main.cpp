#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include "RelayController.h"
#include "LightWebServer.h"
#include "Esp8266Hal.h"
#include "OtaUpdater.h"
#include "ota_config.h"

MDNSResponder mdns;

Esp8266Hal hal;
RelayController relay(hal, D1);
OtaUpdater otaUpdater;
LightWebServer webServer(relay, otaUpdater);

void initializeNetwork()
{
    // WiFiManager: tries saved credentials first;
    // if none/failed, starts AP "FairyLight-Setup" so the user can configures
    // his WiFi via captive portal.
    // Credentials persist in flash across reboots.
    Serial.begin(115200);
    WiFiManager wifiManager;

    bool connected = wifiManager.autoConnect("FairyLight-Setup");

    if (!connected)
    {
        Serial.println("Failed to connect and timed out - restarting...");
        delay(3000);
        ESP.restart();
    }

    // upon successful connection
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // mDNS exposes the board at http://esp8266.local
    if (mdns.begin("esp8266", WiFi.localIP()))
    {
        Serial.println("MDNS responder started");
    }
}

void setup()
{

    initializeNetwork();

    relay.begin();
    // bring up OTA firmware update service
    otaUpdater.begin(OtaConfig::VERSION_URL, OtaConfig::FIRMWARE_URL);

    webServer.begin();
    Serial.println("HTTP server started");
}

void loop()
{
    webServer.handleClient();
    mdns.update(); // required to keep mdns responsive

    // Check for OTA updated periodically
    otaUpdater.checkForUpdateIfDue();
}