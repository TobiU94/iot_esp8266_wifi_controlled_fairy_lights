#include <unity.h>
#include "BuildInfo.h"
#include "OtaUpdater.h"
#include "../mocks/MockOtaDependencies.h"

namespace
{
    const char *FIRMWARE_URL = "https://example.test/firmware.bin";
    constexpr uint32_t CHECK_INTERVAL_MS = 100;

    struct OtaUpdaterFixture
    {
        BuildInfo buildInfo{"1.2.3", "a1b2c3", "2026-01-01T12:00:00"};
        MockVersionSource versionSource;
        MockFirmwareInstaller firmwareInstaller;
        MockClock clock;

        OtaUpdater updater;

        OtaUpdaterFixture() : updater(
                                  buildInfo,
                                  FIRMWARE_URL,
                                  versionSource,
                                  firmwareInstaller,
                                  clock,
                                  CHECK_INTERVAL_MS) {};
    };
}

// TEST 0:
void test_current_version_comes_from_build_info()
{
    OtaUpdaterFixture fixture;

    TEST_ASSERT_EQUAL_STRING(
        "1.2.3",
        fixture.updater.getCurrentFirmwareVersion());
}

// TEST 1:
void test_check_for_updates_does_not_install_if_version_fetch_fails()
{
    OtaUpdaterFixture fixture;

    fixture.versionSource.fetchSuccess = false;

    fixture.updater.checkForUpdate();

    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.versionSource.fetchCalls);

    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.firmwareInstaller.installCalls);
}

// TEST 2
void test_check_for_update_does_not_install_if_versions_match()
{
    OtaUpdaterFixture fixture;

    // set availableVersion in MockVersionSource to match fixture.updater's buildinfo
    fixture.versionSource.availableVersion = "1.2.3";

    // calling checkForUpdates without triggering an update
    // 1.) increments fetchCalls in MockVersionSource
    // 2.) does NOT trigger an install, i.e., does NOT increment installCalls in MockFirmwareInstaller
    fixture.updater.checkForUpdate();

    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.versionSource.fetchCalls);

    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.firmwareInstaller.installCalls);
}

// TEST 3:
void test_check_for_update_triggers_install_if_version_mismatch()
{
    OtaUpdaterFixture fixture;

    // set availableVersion in MockVersionSource to NOT match fixture.updater's buildinfo
    fixture.versionSource.availableVersion = "1.2.4";

    // calling checkForUpdates triggering an update
    // 1.) increments fetchCalls in MockVersionSource
    // 2.) DOES trigger an install, i.e., installCalls in MockFirmwareInstaller is incremented
    fixture.updater.checkForUpdate();

    // versionSource executed fetchAvailableVersion
    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.versionSource.fetchCalls);

    // firmwareInstaller.install() was called
    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.firmwareInstaller.installCalls);

    // firmware URL was passed down into firmwareInstaller
    TEST_ASSERT_EQUAL_STRING(
        FIRMWARE_URL,
        fixture.firmwareInstaller.lastFirmwareUrl);
}

// TEST 4:
// this test verifies what OtaUpdater.getUpdateStatus() reports if
// retrieving the remote version fails (-> fetchAvailableVersion() return false)
void test_update_status_reports_fetch_failure()
{
    OtaUpdaterFixture fixture;

    // set to mock .fetchAvailbleVersion() querying unsuccessfully (see Check 0)
    fixture.versionSource.fetchSuccess = false;

    const OtaStatus status = fixture.updater.getUpdateStatus();

    // Check 0: status.querySucceeded must be false;
    TEST_ASSERT_FALSE(status.querySucceeded);

    // Check 1: getUpdateStatus failed; status.updateAvailable still at its default value (false)
    TEST_ASSERT_FALSE(status.updateAvailable);

    // Check 2: status.currentVersion was populated from .buildInfo member of OtaUpdater
    TEST_ASSERT_EQUAL_STRING(
        "1.2.3", // see firmwareVersion mocked into fixture.buildInfo
        status.currentVersion.c_str());

    // Check 3:  getUpdateStatus failed;
    // status.availableVersion was not manipulated beyond its default value ("")
    TEST_ASSERT_TRUE(status.availableVersion.empty());
}

// TEST 5:
void test_update_status_reports_matching_version()
{
    OtaUpdaterFixture fixture;

    fixture.versionSource.availableVersion = "1.2.3";

    const OtaStatus status = fixture.updater.getUpdateStatus();

    // Check 0: fetchAvailableVersion was called once
    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.versionSource.fetchCalls);

    // Check 1: status.querySucceeded must be true
    TEST_ASSERT_TRUE(status.querySucceeded);

    // Check 2: status.updateAvailable must be false since versionSouce has same version as in buildInfo
    TEST_ASSERT_FALSE(status.updateAvailable);

    // Check 3: status.currentVersion was populated from .buildInfo member of OtaUpdater
    TEST_ASSERT_EQUAL_STRING(
        "1.2.3", // see initialization of otaUpdater
        status.currentVersion.c_str());

    // Check 4: getUpdateStatus executed successfully
    TEST_ASSERT_EQUAL_STRING(
        "1.2.3",
        status.availableVersion.c_str());

    // Check 5:
    // all check went through, but no firmware upgrade was initiated since
    // currentVersion and availableVersion are identical
    // -> firmwareInstaller.install() was never hit
    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.firmwareInstaller.installCalls);
}

// TEST 6:
void test_update_status_reports_available_update()
{
    OtaUpdaterFixture fixture;

    fixture.versionSource.availableVersion = "1.2.4";

    const OtaStatus status = fixture.updater.getUpdateStatus();

    // Check 0: fetchAvailableVersion was called once
    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.versionSource.fetchCalls);

    // Check 1: status.querySucceeded must be true
    TEST_ASSERT_TRUE(status.querySucceeded);

    // Check 2: status.updateAvailable must be false since versionSouce has same version as in buildInfo
    TEST_ASSERT_TRUE(status.updateAvailable);

    // Check 3: status.currentVersion was populated from .buildInfo member of OtaUpdater
    TEST_ASSERT_EQUAL_STRING(
        "1.2.3", // see initialization of otaUpdater
        status.currentVersion.c_str());

    // Check 4: getUpdateStatus executed successfully
    TEST_ASSERT_EQUAL_STRING(
        "1.2.4",
        status.availableVersion.c_str());

    // Check 5:
    // all check went through, BUT this is just requesting the state!
    // it DOES NOT actually trigger a firmware upgrade (yet)
    // -> firmwareInstaller.install() was NOT called
    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.firmwareInstaller.installCalls);
}

// TEST 7:
void test_periodic_check_does_not_run_before_interval()
{
    OtaUpdaterFixture fixture;

    fixture.clock.currentMs = 99;

    fixture.updater.checkForUpdateIfDue();
    // since mocked elapsed time did not hit 100 ms
    // -> checkForUpdate is never called
    // -> fetchAvailableVersion is never called
    // -> MockVersionSource.fetchCalls was never incremented
    // -> and, naturally, MockFirmwareInstaller.install() was never hit
    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.versionSource.fetchCalls);

    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.firmwareInstaller.installCalls);
}

// TEST 8:
void test_periodic_check_does_not_run_at_exact_interval()
{
    OtaUpdaterFixture fixture;

    fixture.clock.currentMs = 100;

    fixture.updater.checkForUpdateIfDue();
    // since mocked elapsed time did not hit 100 ms
    // -> checkForUpdate is never called
    // -> fetchAvailableVersion is never called
    // -> MockVersionSource.fetchCalls was never incremented
    // -> and, naturally, MockFirmwareInstaller.install() was never hit
    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.versionSource.fetchCalls);

    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.firmwareInstaller.installCalls);
}

// TEST 9:
void test_periodic_check_runs_after_interval()
{
    OtaUpdaterFixture fixture;

    fixture.clock.currentMs = 101;
    fixture.versionSource.availableVersion = "1.2.3";

    fixture.updater.checkForUpdateIfDue();
    // since mocked elapsed exceeds 100 ms
    // -> checkForUpdate IS called
    // -> fetchAvailableVersion IS called
    // -> MockVersionSource.fetchCalls IS incremented
    // -> but since we mocked availableVersion to match have-version
    //  MockFirmwareInstaller.install() was never hit
    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.versionSource.fetchCalls);

    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.firmwareInstaller.installCalls);
}

// TEST 10:
void test_manual_check_bypasses_interval()
{
    OtaUpdaterFixture fixture;
    fixture.clock.currentMs = 99;
    fixture.versionSource.availableVersion = "1.2.3";

    fixture.updater.checkForUpdateNow();

    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.versionSource.fetchCalls);

    TEST_ASSERT_EQUAL_INT(
        0,
        fixture.firmwareInstaller.installCalls);
}

// TEST 11:
void test_periodic_check_preserves_rollover_safe_elapsed_time()
{
    OtaUpdaterFixture fixture;

    const uint32_t timeNearRollover = UINT32_MAX - 10u;

    // first version check occurs shortly before the uint32_t timer wraps around
    fixture.clock.currentMs = timeNearRollover;
    fixture.versionSource.availableVersion = "1.2.3";

    fixture.updater.checkForUpdateIfDue();

    // "now" (timeNearRollover) - _lastCheckMs (still at default 0) >> _checkIntervalMs
    // fetchAvailableVersion path is triggered
    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.versionSource.fetchCalls);

    // Mock that the clock has now wrapped. The elapsed time since timeNearRollover is
    // only 31 ms (although _lastCheckMs will be some huge number).
    // So, another check MUST NOT happen yet, i.e., fetchCalls did not grow any further!
    fixture.clock.currentMs = 20;

    fixture.updater.checkForUpdateIfDue();

    TEST_ASSERT_EQUAL_INT(
        1,
        fixture.versionSource.fetchCalls);

    // Mock that the clock has evolved further.
    // From timeNearRollover to 100 ms after rollover:
    // 100 - (UINT32_MAX - 10)
    // modulo(2^32)
    // = 111 ms
    // (which is greater than the 100 ms interval, so a second check should have occured)
    // NOTE: as so fa never the condition for checkForUpdate() was true, _lastCheckMs was never change so far
    fixture.clock.currentMs = 100;
    fixture.updater.checkForUpdateIfDue();
    TEST_ASSERT_EQUAL_INT(
        2,
        fixture.versionSource.fetchCalls);
}

// TEST 12
void test_installer_result_is_not_confused_with_version_decision()
{
    const FirmwareUpdateResult results[] = {
        FirmwareUpdateResult::Failed,
        FirmwareUpdateResult::NoUpdate,
        FirmwareUpdateResult::Succeeded};

    for (const FirmwareUpdateResult result : results)
    {
        OtaUpdaterFixture fixture;
        fixture.versionSource.availableVersion = "1.2.4"; // in principle update is available
        fixture.firmwareInstaller.result = result;

        fixture.updater.checkForUpdate();

        TEST_ASSERT_EQUAL_INT(
            1,
            fixture.versionSource.fetchCalls);

        TEST_ASSERT_EQUAL_INT(
            1,
            fixture.firmwareInstaller.installCalls);

        // firmware URL got passed down to installer in either cases
        TEST_ASSERT_EQUAL_STRING(
            FIRMWARE_URL,
            fixture.firmwareInstaller.lastFirmwareUrl);
    }
}

// Run test suite
int main(int, char **)
{
    UNITY_BEGIN();

    RUN_TEST(test_current_version_comes_from_build_info);                     // TEST 00
    RUN_TEST(test_check_for_updates_does_not_install_if_version_fetch_fails); // TEST 01
    RUN_TEST(test_check_for_update_does_not_install_if_versions_match);       // TEST 02
    RUN_TEST(test_check_for_update_triggers_install_if_version_mismatch);     // TEST 03
    RUN_TEST(test_update_status_reports_fetch_failure);                       // TEST 04
    RUN_TEST(test_update_status_reports_matching_version);                    // TEST 05
    RUN_TEST(test_update_status_reports_available_update);                    // TEST 06
    RUN_TEST(test_periodic_check_does_not_run_before_interval);               // TEST 07
    RUN_TEST(test_periodic_check_does_not_run_at_exact_interval);             // TEST 08
    RUN_TEST(test_periodic_check_runs_after_interval);                        // TEST 09
    RUN_TEST(test_manual_check_bypasses_interval);                            // TEST 10
    RUN_TEST(test_periodic_check_preserves_rollover_safe_elapsed_time);       // TEST 11
    RUN_TEST(test_installer_result_is_not_confused_with_version_decision);    // TEST 12

    return UNITY_END();
}