#pragma once

#include <Timezone.h> // https://github.com/JChristensen/Timezone

// The SSID and password of the accesspoint to connect to
constexpr auto wifi_ssid = "<fill in yours>";
constexpr auto wifi_password = "<fill in yours>";

// There must always be 3 NTP servers. Remove or replace by null if not required
constexpr auto ntp_server_1 = "europe.pool.ntp.org";
constexpr auto ntp_server_2 = "0.pool.ntp.org";
constexpr auto ntp_server_3 = "1.pool.ntp.org";

// DST start and end (Central European Standard Time)
constexpr TimeChangeRule dst_begin = {"CEST", Last, Sun, Mar, 2, 120};
constexpr TimeChangeRule dst_end = {"CET ", Last, Sun, Oct, 3, 60};

// Openweathermap location and subscription id. Create an account at www.openweathermap.org to obtain an id.
constexpr auto openweathermap_location = "Amsterdam";
constexpr auto openweathermap_api_id = "<fill in yours>";

// Every 2 minutes update the display
constexpr auto weather_update_milliseconds = 120000u;