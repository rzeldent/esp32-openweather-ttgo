#include <Arduino.h>
#include <SPI.h>
// Setting for the display are defined in platformio.ini
#include <TFT_eSPI.h>

#include <Button2.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <time.h>

#include ".settings.h"
#include <images.h>
#include <OpenWeatherId.h>

constexpr byte button_top = 35;
constexpr byte button_bottom = 0;

constexpr auto font_10pt = 2; // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
constexpr auto font_26pt = 4; // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
constexpr auto font_48pt = 6; // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm

// Use hardware SPI
auto tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);

Button2 button1(button_top);
Button2 button2(button_bottom);

Timezone timeZone(dst_begin, dst_end);

// Screen is 240 * 135 pixels (rotated)
constexpr uint16_t background_color = TFT_BLACK;
constexpr uint16_t text_color = TFT_WHITE;

constexpr uint16_t color_transparent = TFT_BLACK;

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
#define MAIN_BAR_PRESSURE_X (TFT_HEIGHT - WEATHER_ICON_WIDTH)

void setup()
{
  tft.init();
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness
  tft.setRotation(1);
  tft.setTextDatum(TL_DATUM); // Top Left
  tft.setTextColor(text_color);

  // Show OpenWeather welcome screen / logo
  tft.pushImage(0, 0, image_open_weather.width, image_open_weather.height, image_open_weather.data);

  log_i("Connecting to access point: %s", wifi_ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  log_w("Waiting for WiFi connection");
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    log_w("Connection Failed! Rebooting...");
    // Show Dinosour / cactus image, wait an reset
    tft.pushImage(0, 0, image_no_internet.width, image_no_internet.height, image_no_internet.data);
    delay(10000);
    ESP.restart();
  }

  log_i("WiFi connected. IP: %s", WiFi.localIP().toString().c_str());

  // Set the time servers
  configTime(0, 0, ntp_server_1, ntp_server_2, ntp_server_3);

  // Clear the screen
  tft.fillRect(0, 0, TFT_HEIGHT, TFT_WIDTH, background_color);
  // Set the location, Font(4) = 26px
  tft.drawString(openweathermap_location, TOP_BAR_LOCATION_X, TOP_BAR_Y, font_10pt);
}

constexpr auto time_update_milliseconds = 1000ul;

uint loopCount;

void loop()
{
  tft.fillRect(TOP_BAR_TIME_X, TOP_BAR_Y, TOP_BAR_TIME_WIDTH, TOP_BAR_HEIGHT, background_color);
  // Take into account DST
  time_t now;
  time(&now);
  TimeChangeRule *tcr;
  auto local = timeZone.toLocal(now, &tcr);
  char buff[9];
  sprintf(buff, "%02d:%02d:%02d", hour(local), minute(local), second(local));
  tft.drawString(buff, TOP_BAR_TIME_X, TOP_BAR_Y, font_26pt);

  if (WiFi.isConnected())
  {
    if (loopCount % (weather_update_milliseconds / time_update_milliseconds) == 0)
    {
      tft.fillRect(0, MAIN_BAR_Y, TFT_HEIGHT, MAIN_BAR_HEIGHT, background_color);
      tft.fillRect(0, BOTTOM_BAR_Y, TFT_HEIGHT, BOTTOM_BAR_HEIGHT, background_color);

      // Draw the temperature and humidity icons
      tft.pushImage(MAIN_BAR_TEMPERATURE_ICON_X, MAIN_BAR_TEMPERATURE_Y, image_temperature.width, image_temperature.height, image_temperature.data, color_transparent);
      // Fix for degrees symbol : ° is ` in library (only 16pt font)
      tft.drawCentreString("`C", MAIN_BAR_TEMPERATURE_ICON_X + image_temperature.width / 2, MAIN_BAR_TEMPERATURE_Y + image_temperature.height, font_10pt);
      tft.pushImage(MAIN_BAR_HUMIDITY_ICON_X, MAIN_BAR_HUMIDITY_Y, image_humidity.height, image_humidity.height, image_humidity.data, color_transparent);
      tft.drawCentreString("%R", MAIN_BAR_HUMIDITY_ICON_X + image_humidity.width / 2, MAIN_BAR_HUMIDITY_Y + image_humidity.height, font_10pt);

      HTTPClient client;
      client.begin(String("http://api.openweathermap.org/data/2.5/weather?q=") + openweathermap_location + "&appid=" + openweathermap_api_id + "&units=metric");
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
          auto temperature = (const float)main["temp"];
          auto humidity = (const uint8_t)main["humidity"];

          auto temperatureString = String(temperature, 1);
          auto humidityString = String(humidity);
          tft.drawString(temperatureString, MAIN_BAR_TEMPERATURE_X, MAIN_BAR_TEMPERATURE_Y, font_48pt);
          tft.drawString(humidityString, MAIN_BAR_HUMIDITY_X, MAIN_BAR_HUMIDITY_Y, font_48pt);

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
            tft.drawCentreString(info->description, TFT_HEIGHT / 2, BOTTOM_BAR_Y, font_10pt);
            auto image = isDay ? info->imageDay : info->imageNight;
            tft.pushImage(MAIN_BAR_WEATHER_ICON_X, MAIN_BAR_WEATHER_ICON_Y, image->width, image->height, image->data, color_transparent);
          }

          const auto pressure = (const float)main["pressure"];
          tft.drawRightString(String((const int)pressure) + " hpa", TFT_HEIGHT, MAIN_BAR_PRESSURE_Y, font_10pt);
        }
      }
      client.end();
    }
  }

  loopCount++;
  delay(time_update_milliseconds);
}