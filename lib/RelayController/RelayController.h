#pragma once
#include <Arduino.h>

class RelayController
{
public:
    explicit RelayController(uint8_t pin);

    void begin();
    void turnOn();
    void turnOff();

    bool isOn() const;

private:
    uint8_t _pin;
    bool _isOn;
};