#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "secrets.h" // provides WIFI_SSID, WIFI_PASSWORD

const int relayPin = D1;
const int led = 2;

MDNSResponder mdns;
ESP8266WebServer server(80);

void handleRoot()
{
    server.send(200, "text/plain",
                "hello from esp8266!) \n/on: to turn LED ON \n/off: to turn LED OFF \n");
}

void handleNotFound()
{
    digitalWrite(led, 1);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
    digitalWrite(led, 0);
}

void setup()
{
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);
    pinMode(led, OUTPUT);
    digitalWrite(led, 0);

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

    server.onNotFound(handleNotFound);
    server.on("/", handleRoot);
    server.on("/on", []()
              {
        server.send(200, "text/plain", "LIGHTS ON");
        digitalWrite(relayPin, HIGH); });
    server.on("/off", []()
              {
        server.send(200, "text/plain", "LIGHTS OFF");
        digitalWrite(relayPin, LOW); });

    server.begin();
    Serial.println("HTTP server started");

    pinMode(BUILTIN_LED, OUTPUT);
}

void loop()
{
    server.handleClient();
}