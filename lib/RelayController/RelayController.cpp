// RelayController.cpp
#include "RelayController.h"

RelayController::RelayController(uint8_t pin) : _pin(pin), _isOn(false) {}

void RelayController::begin()
{
    pinMode(_pin, OUTPUT);
    turnOff();
}

void RelayController::turnOff()
{
    digitalWrite(_pin, LOW);
    _isOn = false;
}

void RelayController::turnOn()
{
    digitalWrite(_pin, HIGH);
    _isOn = true;
}

bool RelayController::isOn() const
{
    return _isOn;
}