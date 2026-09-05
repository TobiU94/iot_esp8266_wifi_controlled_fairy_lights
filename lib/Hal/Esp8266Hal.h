#pragma once
#include "IHal.h"
#include <Arduino.h>

class Esp8266Hal : public IHal
{
public:
    // Thin wrapper — zero logic, just forwards to Arduino.
    void pinMode(uint8_t pin, uint8_t mode) override { ::pinMode(pin, mode); }
    void digitalWrite(uint8_t pin, uint8_t value) override { ::digitalWrite(pin, value); }
    int digitalRead(uint8_t pin) override { return ::digitalRead(pin); }
};