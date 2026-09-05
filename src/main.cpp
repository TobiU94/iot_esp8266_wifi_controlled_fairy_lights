#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "secrets.h" // provides WIFI_SSID, WIFI_PASSWORD
#include "RelayController.h"
#include "LightWebServer.h"
#include "Esp8266Hal.h"

MDNSResponder mdns;

Esp8266Hal hal;
RelayController relay(hal, D1);
LightWebServer webServer(relay);

void setup()
{
    relay.begin();

    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (mdns.begin("esp8266", WiFi.localIP()))
    {
        Serial.println("MDNS responder started");
    }

    webServer.begin();

    Serial.println("HTTP server started");
}

void loop()
{
    webServer.handleClient();
}