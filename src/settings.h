#pragma once

// The SSID and password of the accesspoint to connect to 
#define WIFI_SSID       ""
#define WIFI_PASSWORD   ""

// NTP pool to use for the time synchronisation
#define NTP_POOL        "europe.pool.ntp.org"
// DST start and end (Central European Standard Time)
#define DST_BEGIN       {"CEST", Last, Sun, Mar, 2, 120}
#define DST_END         {"CET ", Last, Sun, Oct, 3, 60}
// Synchronize interval in milliseconds (every 30 minutes)
#define NTP_UPDATE_MILLISECONDS (30 * 60 * 1000)    

// Openweathermap location and subscription id. Create an account at www.openweathermap.org to obtain an id.
#define OPENWEATHERMAP_LOCATION "Amsterdam"
#define OPENWEATHERMAP_API_ID   ""