#pragma once

struct OtaConfig
{
    // static constexpr const char *VERSION_URL = "https://github.com/TobiU94/iot_esp8266_wifi_controlled_fairy_lights/releases/latest/download/version.txt";

    static constexpr const char *VERSION_URL =
        "https://github.com/TobiU94/iot_esp8266_wifi_controlled_fairy_lights/releases/download/v31/version.txt";

    static constexpr const char *FIRMWARE_URL = "https://github.com/TobiU94/iot_esp8266_wifi_controlled_fairy_lights/releases/latest/download/firmware.bin";
    static constexpr unsigned long CHECK_INTERVAL_MS = 10000; // 60000;
};