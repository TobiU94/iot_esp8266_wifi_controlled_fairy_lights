#pragma once
#include <stdint.h> // grants access to **fixed-width integer types** (`uint8_t`, `uint16_t`, `uint32_t`), ensuring predictable register sizes, bit manipulation, and peripheral memory mapping.

class IHal
{
public:
    static constexpr uint8_t PIN_LOW = 0;
    static constexpr uint8_t PIN_HIGH = 1;
    static constexpr uint8_t PIN_OUTPUT = 1;

    virtual ~IHal() = default;

    virtual void pinMode(uint8_t pin, uint8_t mode) = 0;
    virtual void digitalWrite(uint8_t pin, uint8_t value) = 0;
    virtual int digitalRead(uint8_t pin) = 0;
};