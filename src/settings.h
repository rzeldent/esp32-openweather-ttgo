#pragma once

// The SSID and password of the accesspoint to connect to 
#define WIFI_SSID       "<fill in your own>"
#define WIFI_PASSWORD   "<fill in your own>"

// There must always be 3 NTP servers. Remove or replace by null if not required
#define NTP_SERVERS     "europe.pool.ntp.org", "0.pool.ntp.org", "1.pool.ntp.org"
// DST start and end (Central European Standard Time) configure for your timezone
#define DST_BEGIN       {"CEST", Last, Sun, Mar, 2, 120}
#define DST_END         {"CET ", Last, Sun, Oct, 3, 60}

// Openweathermap location and subscription id. Create an account at www.openweathermap.org to obtain an id.
#define OPENWEATHERMAP_LOCATION "Amsterdam"
#define OPENWEATHERMAP_API_ID   "<fill in your own>"

// Every 2 minutes update the display
#define WEATHER_UPDATE_MILLISECONDS 120000