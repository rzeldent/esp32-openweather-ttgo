#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
// Setting for the display are defined in platformio.ini
#include <TFT_eSPI.h>
#include <Button2.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>

#include ".settings.h"
#include <icons.h>
#include <OpenWeatherId.h>

#define BUTTON_1 35
#define BUTTON_2 0

// Use hardware SPI
auto tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
Button2 button1(BUTTON_1);
Button2 button2(BUTTON_2);
WiFiUDP ntpUDP;
// Synchronize time every hour
NTPClient timeClient(ntpUDP, NTP_POOL, NTP_TIMEOFFSET, NTP_UPDATE_MILLISECONDS);

// Screen is 240 * 135 pixels (rotated)
#define BACKGROUND_COLOR TFT_BLACK
#define TEXT_COLOR TFT_WHITE

#define WEATHER_ICON_WIDTH 75
#define WEATHER_ICON_HEIGHT 75

// General Info bar => Location, WiFi
#define TOP_BAR_Y 0
#define TOP_BAR_HEIGHT 26
// Location
#define TOP_BAR_LOCATION_X 0
#define TOP_BAR_LOCATION_WIDTH 140
// Time
#define TOP_BAR_TIME_X TOP_BAR_LOCATION_WIDTH
#define TOP_BAR_TIME_WIDTH (TFT_HEIGHT - TOP_BAR_LOCATION_WIDTH)

// Bottom bar => Weather description
#define BOTTOM_BAR_Y (TFT_WIDTH - BOTTOM_BAR_HEIGHT)
#define BOTTOM_BAR_HEIGHT 16
// Weather description
#define BOTTOM_BAR_DESCRIPTION_X 0
#define BOTTOM_BAR_DESCRIPTION_WIDTH TFT_HEIGHT

// What remains is main screen
#define MAIN_BAR_Y TOP_BAR_HEIGHT
#define MAIN_BAR_HEIGHT (TFT_WIDTH - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT)

#define TEMPERATURE_HEIGHT 48
#define HUMIDITY_HEIGHT 48

#define MAIN_BAR_TEMPERATURE_ICON_X 0
#define MAIN_BAR_TEMPERATURE_X 50
#define MAIN_BAR_TEMPERATURE_Y MAIN_BAR_Y

#define MAIN_BAR_HUMIDITY_ICON_X 0
#define MAIN_BAR_HUMIDITY_X 50
#define MAIN_BAR_HUMIDITY_Y (MAIN_BAR_TEMPERATURE_Y + TEMPERATURE_HEIGHT)

#define MAIN_BAR_WEATHER_ICON_Y MAIN_BAR_Y
#define MAIN_BAR_WEATHER_ICON_X (TFT_HEIGHT - WEATHER_ICON_WIDTH)

#define MAIN_BAR_PRESSURE_Y (MAIN_BAR_Y + WEATHER_ICON_HEIGHT)
#define MAIN_BAR_PRESSURE_X (TFT_HEIGHT - WEATHER_ICON_WIDTH)

void setup()
{
  tft.init();
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness
  tft.setRotation(1);
  tft.setTextDatum(TL_DATUM); // Top Left
  tft.setTextColor(TEXT_COLOR);

  // Show OpenWeather welcome screen / logo
  tft.pushImage(0, 0, OpenWeather.width, OpenWeather.height, OpenWeather.data);

  // Wait 30 seconds for a connection, otherwise reset
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  auto i = 30;
  do
  {
    if (!i--)
      ESP.restart();

    sleep(1);
  } while (!WiFi.isConnected());

  // Start the timeclient
  timeClient.begin();

  // Clear the screen
  tft.fillRect(0, 0, TFT_HEIGHT, TFT_WIDTH, BACKGROUND_COLOR);
  // Set the location, Font(4) = 26px
  tft.drawString(OPENWEATHERMAP_LOCATION, TOP_BAR_LOCATION_X, TOP_BAR_Y, 4);
}

#define LOOP_MILLISECONDS 1000
#define TIME_UPDATE_MILLISECONDS 1000
#define WEATHER_UPDATE_MILLISECONDS 120000

uint loopCount;

void loop()
{
  if (!timeClient.update())
    timeClient.forceUpdate();

  // Update  time
  if (loopCount % (TIME_UPDATE_MILLISECONDS / LOOP_MILLISECONDS) == 0)
  {
    tft.fillRect(TOP_BAR_TIME_X, TOP_BAR_Y, TOP_BAR_TIME_WIDTH, TOP_BAR_HEIGHT, BACKGROUND_COLOR);
    // Font(4) = 26px
    tft.drawString(timeClient.getFormattedTime(), TOP_BAR_TIME_X, TOP_BAR_Y, 4);
  }

  if (WiFi.isConnected())
  {
    if (loopCount % (WEATHER_UPDATE_MILLISECONDS / LOOP_MILLISECONDS) == 0)
    {
      tft.fillRect(0, MAIN_BAR_Y, TFT_HEIGHT, MAIN_BAR_HEIGHT, BACKGROUND_COLOR);
      tft.fillRect(0, BOTTOM_BAR_Y, TFT_HEIGHT, BOTTOM_BAR_HEIGHT, BACKGROUND_COLOR);

      // Draw the temperature and Humidity icons
      tft.pushImage(MAIN_BAR_TEMPERATURE_ICON_X, MAIN_BAR_TEMPERATURE_Y, 48, 48, image_temperature.data, IMAGE_TRANSPARTENT_COLOR);
      tft.pushImage(MAIN_BAR_HUMIDITY_ICON_X, MAIN_BAR_HUMIDITY_Y, 48, 48, image_humidity.data, IMAGE_TRANSPARTENT_COLOR);

      HTTPClient client;
      client.begin("http://api.openweathermap.org/data/2.5/weather?q=" OPENWEATHERMAP_LOCATION "&appid=" OPENWEATHERMAP_ID "&units=metric");
      auto code = client.GET();
      if (code == HTTP_CODE_OK)
      {
        const auto response = client.getString().c_str();
        DynamicJsonDocument doc(2048);

        // Parse JSON object
        const auto error = deserializeJson(doc, response);
        if (!error)
        {
          const auto temp = (float)doc["main"]["temp"];
          // Round temperature to one digit
          char temperature[5];
          dtostrf(temp, 1, 1, temperature);
          const auto humidity = (uint8_t)doc["main"]["humidity"];

          // Font(6) = 48px
          tft.drawString(temperature, MAIN_BAR_TEMPERATURE_X, MAIN_BAR_TEMPERATURE_Y, 6);
          tft.drawString(String(humidity), MAIN_BAR_HUMIDITY_X, MAIN_BAR_HUMIDITY_Y, 6);

          const auto dt = (uint32_t)doc["dt"];
          const auto isDay = dt > (uint32_t)doc["sys"]["sunrise"] && dt < (uint32_t)doc["sys"]["sunset"];

          const auto id = (uint16_t)doc["weather"][0]["id"];
          // Lookup weather code
          auto info = (const OpenWeatherIdt *)&openWeatherIds;
          while (info->id && info->id != id)
            ++info;

          if (info->id)
          {
            // Font(2) = 16px
            tft.drawString(info->description, 0, BOTTOM_BAR_Y, 2);
            tft.pushImage(MAIN_BAR_WEATHER_ICON_X, MAIN_BAR_WEATHER_ICON_Y, WEATHER_ICON_WIDTH, WEATHER_ICON_HEIGHT, (isDay ? info->imageDay : info->imageNight)->data, IMAGE_TRANSPARTENT_COLOR);
          }

          const auto pressure = (float)doc["main"]["pressure"];
          // Font(2) = 16px
          tft.drawString(String((int)pressure) + " hpa", MAIN_BAR_PRESSURE_X, MAIN_BAR_PRESSURE_Y, 2);
        }
      }
      client.end();
    }
  }

  loopCount++;
  delayMicroseconds(1000 * LOOP_MILLISECONDS);
}