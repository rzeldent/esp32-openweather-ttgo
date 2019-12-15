#pragma once

// The SSID and password of the accesspoint to connect to 
#define WIFI_SSID       ""
#define WIFI_PASSWORD   ""

// NTP pool to use for the time synchronisation
#define NTP_POOL        "europe.pool.ntp.org"
// The offset in seconds from UTC (+1)
#define NTP_TIMEOFFSET  3600
// Synchronize interval in milliseconds (every 30 minutes)
#define NTP_UPDATE_MILLISECONDS (30 * 60 * 1000)    

// Openweathermap location and subscription id. Create an account at www.openweathermap.org to obtain an id.
#define OPENWEATHERMAP_LOCATION "Amsterdam"
#define OPENWEATHERMAP_ID       ""