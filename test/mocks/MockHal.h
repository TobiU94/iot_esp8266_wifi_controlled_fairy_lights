#pragma once
#include "IHal.h"

// This is our **spy** — it records what happened so tests can assert on it.
class MockHal : public IHal
{
public:
    uint8_t lastPin = 0;
    uint8_t lastValue = 0;
    uint8_t lastMode = 0;
    int pinModeCalls = 0;
    int digitalWriteCalls = 0;

    void pinMode(uint8_t pin, uint8_t mode) override
    {
        lastPin = pin;
        lastMode = mode;
        pinModeCalls++;
    }

    void digitalWrite(uint8_t pin, uint8_t value) override
    {
        lastPin = pin;
        lastValue = value;
        digitalWriteCalls++;
    }

    int digitalRead(uint8_t pin) override
    {
        return lastValue;
    }
};
