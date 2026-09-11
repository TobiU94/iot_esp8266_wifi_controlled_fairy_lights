#include "Esp8266Clock.h"
#include <Arduino.h>

uint32_t Esp8266Clock::millis() const
{
    return ::millis();
};