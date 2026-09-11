#include "OtaDependencies.h"

class Esp8266Clock : public IClock
{
public:
    uint32_t millis() const override;
};