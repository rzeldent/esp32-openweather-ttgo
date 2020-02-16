# esp32-openweather-TTGO
openweathermap.org interface for the [TTGO-T-Display](https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/TTGO-T-Display.ino) ESP-32 Module.

**Work in progess** but fully functional but always open for suggestions / branches!

## Hardware
No additional hardware is required.

## Installation
Create an account at [www.openweathermap.org](www.openweathermap.org) to obtain an API key to query for the weather at your location.
Update the file .settings and:
 - enter the credentials for the WiFi (WIFI_SSID and WIFI_PASSWORD)
 - your location and id for the openeeathermap API (OPENWEATHERMAP_LOCATION and OPENWEATHERMAP_ID) 
Update these variables in .settings file (notice the dot). The settings.h file is a skeleton file for the configuration.

**Copy the file settings.h to .settings.h to keep your settings secret.**

I use Visual Studio Code and PlatformIO to edit and upload the sketches. Install these, open the project folder and upload to the esp32-ttgo module using the USB-C connector. 

Current state: ![hardware.png](https://github.com/rzeldent/esp32-openweather-TTGO/blob/develop/hardware.png)

At the moment the basic functionality is working: Time is updated, API is queried and temperature, humidity, pressure, weather description and icons are shown. So it is functioning and can be used immediately!

Update the settings.h with your wifi settings, location and openwaether api id.

However, there are still two buttons left on the TTGO-T-Display so room for improvement like:
- Setup using these buttons?
- Humidity and temperature icons have some dithering pixels. New images?
- Configure SSID/Password using some gui allowing to enter the text using the buttons
- Provide historical information about temperature / humidity / pressure and graph it.
- Provide a weather forecast?
