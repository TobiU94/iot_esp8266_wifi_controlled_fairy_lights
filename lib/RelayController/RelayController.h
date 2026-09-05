#pragma once
#include <stdint.h>
#include "IHal.h" // using IHal.h interface to make RelayController platfrom independent

class RelayController
{
public:
    RelayController(IHal &hal, uint8_t pin); // no 'explicit' needed: 2 args, compiler will not do any dynamic type casting

    void begin();
    void turnOn();
    void turnOff();

    bool isOn() const;

private:
    IHal &_hal; // reference: can't be null, doesn't own it
    uint8_t _pin;
    bool _isOn;
};