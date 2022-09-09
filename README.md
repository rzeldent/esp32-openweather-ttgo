# esp32-openweather-TTGO

[![Platform IO CI](https://github.com/rzeldent/esp32-openweather-ttgo/actions/workflows/main.yml/badge.svg)](https://github.com/rzeldent/esp32-openweather-ttgo/actions/workflows/main.yml)

openweathermap.org interface for the [TTGO-T-Display](https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/TTGO-T-Display.ino) ESP32 Module

## Hardware
No additional hardware is required.

## Installation
Create an account at [www.openweathermap.org](www.openweathermap.org) to obtain an API key to query for the weather at your location.
I use Visual Studio Code and PlatformIO to edit and upload the sketches.
Install these, open the project folder and upload to the esp32-ttgo module using the USB-C connector. 

Current state: ![TTGO Display](assets/hardware.png)

## Configuration
Make a connection to the Accesspoint "OpenWeather". The first time you can connect without a wifi password.

Here, in the menu, update the settings:

- Open Weather API key. The key obtained from openweather (see above)
- Location. A name of the location. This should be supported as a location by OpenWeather.
- TimeZone. The timezone to display the correct (local) time and DST.
- Use metric units. The choice to use Imperial or Metric settings for the temperature etc...

There is also an option to use an alternative weather icon pack. This can be changed by the batch job to generate the icons.

## Current state
At the moment the basic functionality is working: 
 - Time is updated
 - API is queried and temperature, humidity, pressure, weather description and icons are shown.

However, there are still two buttons left on the TTGO-T-Display so room for improvement like:
- Setup using these buttons?
- Humidity and temperature icons have some dithering pixels. New images?
- Provide historical information about temperature / humidity / pressure and graph it.
- Provide a weather forecast?

## Collobaoration
New features / pull requests are welcome.
Please make a branch and, after checing it might be put into the main branch!

### Thanks
Used icons from: https://www.dovora.com/resources/weather-icons/
 - Modern Weather Icon Pack
 - Crystal Weather Icon Pack
