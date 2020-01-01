# TTGO-OpenWeathermap
openweathermap.org interface for the ![TTGO-T-Display](https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/TTGO-T-Display.ino) ESP-32 Module.

**Work in progess** but looking forward to suggestions / branches!

**Copy the file settings.h to .settings.h to keep your settings secret.**

Current state: ![hardware.png](https://github.com/rzeldent/TTGO-OpenWeather/blob/develop/hardware.png)

At the moment the basic functionality is working: Time is updated, API is queried and temperature, humidity, pressure, weather description and icons are shown. So it is functioning and can be used immediately!

Update the settings.h with your wifi settings, location and openwaether api id.

However, there are still two buttons left on the TTGO-T-Display so room for improvement like:

- Humidity and temperature icons have some dithering pixels. New images?
- Configure SSID/Password using some gui allowing to enter the text using the buttons
- Provide historical information about temperature / humidity / pressure and graph it.
- Provide a weather forecast?
