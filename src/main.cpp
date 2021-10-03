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
#include <Timelib.h>
#include <Timezone.h> // https://github.com/JChristensen/Timezone

#include ".settings.h"
#include <images.h>
#include <OpenWeatherId.h>

#define BUTTON_1 35
#define BUTTON_2 0

#define FONT_10PT 2
#define FONT_26PT 4
#define FONT_48PT 6
#define FONT_48PT_LCD 7

// Use hardware SPI
auto tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);

Button2 button1(BUTTON_1);
Button2 button2(BUTTON_2);
WiFiUDP ntpUDP;

// Synchronize time every hour and use UTC
NTPClient timeClient(ntpUDP, NTP_POOL, 0, NTP_UPDATE_MILLISECONDS);
TimeChangeRule dstBegin = DST_BEGIN;
TimeChangeRule dstEnd = DST_END;
Timezone timeZone(dstBegin, dstEnd);

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
#define MAIN_BAR_TEMPERATURE_X 40
#define MAIN_BAR_TEMPERATURE_Y MAIN_BAR_Y

#define MAIN_BAR_HUMIDITY_ICON_X 0
#define MAIN_BAR_HUMIDITY_X 40
#define MAIN_BAR_HUMIDITY_Y (MAIN_BAR_TEMPERATURE_Y + TEMPERATURE_HEIGHT)

#define MAIN_BAR_WEATHER_ICON_Y MAIN_BAR_Y
#define MAIN_BAR_WEATHER_ICON_X (TFT_HEIGHT - WEATHER_ICON_WIDTH)

#define MAIN_BAR_PRESSURE_Y (MAIN_BAR_Y + WEATHER_ICON_HEIGHT)
//#define MAIN_BAR_PRESSURE_X (TFT_HEIGHT - WEATHER_ICON_WIDTH)

void setup()
{
  tft.init();
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness
  tft.setRotation(1);
  tft.setTextDatum(TL_DATUM); // Top Left
  tft.setTextColor(TEXT_COLOR);

  // Show OpenWeather welcome screen / logo
  tft.pushImage(0, 0, image_openweather.width, image_openweather.height, image_openweather.data);

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
  tft.drawString(OPENWEATHERMAP_LOCATION, TOP_BAR_LOCATION_X, TOP_BAR_Y, FONT_10PT);
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
    // Take into account DST
    TimeChangeRule *tcr;
    auto local = timeZone.toLocal(timeClient.getEpochTime(), &tcr);
    char buff[9];
    sprintf(buff, "%02d:%02d:%02d", hour(local), minute(local), second(local));
    tft.drawString(buff, TOP_BAR_TIME_X, TOP_BAR_Y, FONT_26PT);
  }

  if (WiFi.isConnected())
  {
    if (loopCount % (WEATHER_UPDATE_MILLISECONDS / LOOP_MILLISECONDS) == 0)
    {
      tft.fillRect(0, MAIN_BAR_Y, TFT_HEIGHT, MAIN_BAR_HEIGHT, BACKGROUND_COLOR);
      tft.fillRect(0, BOTTOM_BAR_Y, TFT_HEIGHT, BOTTOM_BAR_HEIGHT, BACKGROUND_COLOR);

      // Draw the temperature and humidity icons
      tft.pushImage(MAIN_BAR_TEMPERATURE_ICON_X, MAIN_BAR_TEMPERATURE_Y, image_temperature.width, image_temperature.height, image_temperature.data, IMAGE_TRANSPARTENT_COLOR);
      // Fix for degrees symbol : ° is ` in library
      tft.drawCentreString("`C", MAIN_BAR_TEMPERATURE_ICON_X + image_temperature.width / 2, MAIN_BAR_TEMPERATURE_Y + image_temperature.height, FONT_10PT);
      tft.pushImage(MAIN_BAR_HUMIDITY_ICON_X, MAIN_BAR_HUMIDITY_Y, image_humidity.height, image_humidity.height, image_humidity.data, IMAGE_TRANSPARTENT_COLOR);
      tft.drawCentreString("%R", MAIN_BAR_HUMIDITY_ICON_X + image_humidity.width / 2, MAIN_BAR_HUMIDITY_Y + image_humidity.height, FONT_10PT);

      HTTPClient client;
      client.begin("http://api.openweathermap.org/data/2.5/weather?q=" OPENWEATHERMAP_LOCATION "&appid=" OPENWEATHERMAP_API_ID "&units=metric");
      auto code = client.GET();
      if (code == HTTP_CODE_OK)
      {
        const auto response = client.getString().c_str();
        DynamicJsonDocument doc(2048);

        // Parse JSON object
        const auto error = deserializeJson(doc, response);
        if (!error)
        {
          const auto main = doc["main"];
          auto temp = (const float)main["temp"];
          auto humidity = (const uint8_t)main["humidity"];

          tft.drawFloat(temp, 1, MAIN_BAR_TEMPERATURE_X, MAIN_BAR_TEMPERATURE_Y, FONT_48PT);
          tft.drawNumber(humidity, MAIN_BAR_HUMIDITY_X, MAIN_BAR_HUMIDITY_Y, FONT_48PT);

          auto dt = (const uint32_t)doc["dt"];
          const auto sys = doc["sys"];
          const auto isDay = dt > (const uint32_t)sys["sunrise"] && dt < (const uint32_t)sys["sunset"];

          const auto weather = doc["weather"];
          const auto id = (const uint16_t)weather[0]["id"];
          // Lookup weather code
          auto info = (const OpenWeatherIdt *)&openWeatherIds;
          while (info->id && info->id != id)
            ++info;

          if (info->id)
          {
            tft.drawCentreString(info->description, TFT_HEIGHT / 2, BOTTOM_BAR_Y, FONT_10PT);
            auto image = isDay ? info->imageDay : info->imageNight;
            tft.pushImage(MAIN_BAR_WEATHER_ICON_X, MAIN_BAR_WEATHER_ICON_Y, image->width, image->height, image->data, IMAGE_TRANSPARTENT_COLOR);
          }

          const auto pressure = (const float)main["pressure"];
          tft.drawRightString(String((const int)pressure) + " hpa", TFT_HEIGHT, MAIN_BAR_PRESSURE_Y, FONT_10PT);
        }
      }
      client.end();
    }
  }

  loopCount++;
  delayMicroseconds(1000 * LOOP_MILLISECONDS);
}