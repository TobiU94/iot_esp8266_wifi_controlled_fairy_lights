#include <unity.h>
#include "RelayController.h"
#include "../mocks/MockHal.h"

void test_begin_configures_pin_as_output_and_turns_off()
{
    MockHal hal;
    RelayController relay(hal, 5); // 5 corresponds to D1 on ESP8266

    relay.begin();

    TEST_ASSERT_EQUAL(5, hal.lastPin); // hal is autonomous; it is only dependency injected into relay, but relay does not own it
    TEST_ASSERT_EQUAL(IHal::PIN_OUTPUT, hal.lastMode);
    TEST_ASSERT_EQUAL(IHal::PIN_LOW, hal.lastValue); // pinMode expected to go low via begin() -> turnOff() sequence
    TEST_ASSERT_FALSE(relay.isOn());                 // pinMode should be off (see above)
}

void test_turnOn_writes_high()
{
    MockHal hal;
    RelayController relay(hal, 5);

    relay.begin();
    relay.turnOn();

    TEST_ASSERT_EQUAL(IHal::PIN_HIGH, hal.lastValue);
    TEST_ASSERT_TRUE(relay.isOn());
}

void test_turnOff_writes_low()
{
    MockHal hal;
    RelayController relay(hal, 5);

    relay.begin();
    relay.turnOn();
    relay.turnOff();

    TEST_ASSERT_EQUAL(IHal::PIN_LOW, hal.lastValue);
    TEST_ASSERT_FALSE(relay.isOn());
}

int main(int, char **)
{
    UNITY_BEGIN();
    RUN_TEST(test_begin_configures_pin_as_output_and_turns_off);
    RUN_TEST(test_turnOn_writes_high);
    RUN_TEST(test_turnOff_writes_low);

    return UNITY_END();
}