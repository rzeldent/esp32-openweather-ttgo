#pragma once

#define APP_TITLE "OpenWeather"
#define APP_VERSION "1.0"

#define WIFI_SSID "OpenWeather"
#define WIFI_PASSWORD nullptr
#define CONFIG_VERSION "1.0"

// Time servers
#define NTP_SERVER_1 "nl.pool.ntp.org"
#define NTP_SERVER_2 "europe.pool.ntp.org"
#define NTP_SERVER_3 "time.nist.gov"
#define NTP_SERVERS NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3

#define DEFAULT_LOCATION "Amsterdam"
#define DEFAULT_TIMEZONE "Europe/Amsterdam"
#define DEFAULT_METRIC true
// Openweathermap location and subscription id. Create an account at www.openweathermap.org to obtain an id.
#define DEFAULT_OPENWEATHER_API_KEY  ""

#define WARM_TEMPERATURE_CENTIGRADE 20
#define WARM_TEMPERATURE_FAHRENHEIT 68

// Every 2 minutes update the display
constexpr auto weather_update_milliseconds = 120000ul;

// Every second update the time
constexpr auto time_update_milliseconds = 1000ul;
