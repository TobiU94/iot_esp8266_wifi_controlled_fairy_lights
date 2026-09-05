// RelayController.cpp
#include "RelayController.h"

RelayController::RelayController(IHal &hal, uint8_t pin) : _hal(hal), _pin(pin), _isOn(false) {}

void RelayController::begin()
{
    _hal.pinMode(_pin, IHal::OUTPUT);
    turnOff();
}

void RelayController::turnOff()
{
    _hal.digitalWrite(_pin, IHal::LOW);
    _isOn = false;
}

void RelayController::turnOn()
{
    _hal.digitalWrite(_pin, IHal::HIGH);
    _isOn = true;
}

bool RelayController::isOn() const
{
    return _isOn;
}