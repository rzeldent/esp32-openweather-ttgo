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

#define BUTTON_1 35
#define BUTTON_2 0

// Use hardware SPI
auto tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
Button2 button1(BUTTON_1);
Button2 button2(BUTTON_2);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_POOL, 3600, 60000);

void setup()
{
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(4);

  tft.setTextColor(TFT_BLUE);
  tft.println("TTGO OpenWeathermap\n");

  tft.setTextFont(1);
  tft.setTextColor(TFT_WHITE);
  tft.print("Connecting to: " WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  auto i = 60;
  do
  {
    tft.print(".");
    if (!i--)
      ESP.restart();

    sleep(1);
  } while (!WiFi.isConnected());

  timeClient.begin();
}

void loop()
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);

  if (WiFi.isConnected())
  {
    tft.setTextFont(4);
    timeClient.update();
    tft.println(OPENWEATHERMAP_LOCATION "  " + timeClient.getFormattedTime());

    HTTPClient client;
    client.begin("http://api.openweathermap.org/data/2.5/weather?q=" OPENWEATHERMAP_LOCATION "&appid=" OPENWEATHERMAP_ID "&units=metric");
    auto code = client.GET();
    if (code == HTTP_CODE_OK)
    {
      auto response = client.getString().c_str();
      DynamicJsonDocument doc(2000);

      // Parse JSON object
      auto error = deserializeJson(doc, response);
      //      tft.println(error.c_str());
      if (!error)
      {
        tft.setTextFont(4);
        tft.println((const char *)doc["weather"][0]["description"]);
        tft.setTextFont(6);
        tft.println((float)doc["main"]["temp"]);
      }
    }
    client.end();
  }
  sleep(60);
}