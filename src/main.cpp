#include <Arduino.h>
#include <SPI.h>
#include <soc/rtc_cntl_reg.h>

#include <ttgo_display.h>

// Setting for the display are defined in platformio.ini
#include <TFT_eSPI.h>

#include <Button2.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <time.h>

#include <IotWebConf.h>
#include <IotWebConfTParameter.h>

#include <zimage.h>
#include <timezonedb.h>
#include <images.h>
#include <weather_types.h>

#include <format_duration.h>
#include <format_number.h>

#include <http_status.h>

#include <settings.h>

constexpr auto font_16pt = 2; // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
constexpr auto font_26pt = 4; // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
constexpr auto font_48pt = 6; // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm

// Use hardware SPI
auto tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
auto clock_sprite = TFT_eSprite(&tft);

Button2 button1(GPIO_BUTTON_TOP);
Button2 button2(GPIO_BUTTON_BOTTOM);

// Web server
DNSServer dnsServer;
WebServer server(80);
IotWebConf iotWebConf(WIFI_SSID, &dnsServer, &server, WIFI_PASSWORD, CONFIG_VERSION);

auto param_group = iotwebconf::ParameterGroup("openweather", "Open Weather");
auto iotWebParamOpenWeatherApiKey = iotwebconf::Builder<iotwebconf::TextTParameter<33>>("apikey").label("Open Weather API key").defaultValue(DEFAULT_OPENWEATHER_API_KEY).build();
auto iotWebParamLocation = iotwebconf::Builder<iotwebconf::TextTParameter<64>>("location").label("Location").defaultValue(DEFAULT_LOCATION).build();
auto iotWebParamTimeZone = iotwebconf::Builder<iotwebconf::SelectTParameter<sizeof(timezonelocation_t)>>("timezone").label("Choose timezone").optionValues((const char *)&timezonedb->location).optionNames((const char *)&timezonedb->location).optionCount(sizeof(timezonedb) / sizeof(timezonedb[0])).nameLength(sizeof(timezonelocation_t)).defaultValue(DEFAULT_TIMEZONE).build();
auto iotWebParamMetric = iotwebconf::Builder<iotwebconf::CheckboxTParameter>("metric").label("Use metric units").defaultValue(DEFAULT_METRIC).build();

// Screen is 240 * 135 pixels (rotated)
constexpr uint16_t background_color = TFT_BLACK;
constexpr uint16_t text_color = TFT_WHITE;
constexpr uint16_t image_color_transparent = TFT_WHITE;

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

bool time_valid()
{
  // Value of time_t for 2000-01-01 00:00:00, used to detect invalid SNTP responses.
  constexpr time_t epoch_2000_01_01 = 946684800;
  return time(nullptr) > epoch_2000_01_01;
}

void update_runtime_config()
{
  log_v("update_runtime_config");
  auto tz_definition = timezonedb_get_definition(iotWebParamTimeZone.value());
  setenv("TZ", iotWebParamTimeZone.value(), 1);
  tzset();
  log_i("Set timezone to %s (%s)", iotWebParamTimeZone.value(), tz_definition);
}

void handleRoot()
{
  log_v("Handle root");
  // Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
    return;

  struct tm timeinfo;
  getLocalTime(&timeinfo);
  char time_buffer[20];
  if (time_valid())
    strftime(time_buffer, sizeof(time_buffer), "%F %T", &timeinfo);
  else
    strcpy(time_buffer, "Time syncing...");

  String html;
  html += "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  html += "<title>" APP_TITLE " v" APP_VERSION "</title></head>";
  html += "<body>";
  html += "<h2>Status page for " + String(iotWebConf.getThingName()) + "</h2><hr />";
  html += "<h3>ESP32</h3>";
  html += "<ul>";
  html += "<li>CPU model: " + String(ESP.getChipModel()) + "</li>";
  html += "<li>CPU speed: " + String(ESP.getCpuFreqMHz()) + "Mhz</li>";
  html += "<li>Mac address: " + WiFi.macAddress() + "</li>";
  html += "<li>IPv4 address: " + WiFi.localIP().toString() + "</li>";
  html += "<li>IPv6 address: " + WiFi.localIPv6().toString() + "</li>";
  html += "</ul>";
  html += "<h3>Settings</h3>";
  html += "<ul>";
  html += "<li>Access point: " + String(iotWebConf.getWifiSsidParameter()->valueBuffer) + "</li>";
  html += "<li>Location: " + String(iotWebParamLocation.value()) + "</li>";
  html += "<li>API key: " + String(iotWebParamOpenWeatherApiKey.value()) + "</li>";
  html += "<li>Time zone: " + String(iotWebParamTimeZone.value()) + " (" + timezonedb_get_definition(iotWebParamTimeZone.value()) + ")" + "</li>";
  html += "<li>Units: " + String(iotWebParamMetric.value() ? "Metric" : "Imperial") + "</li>";
  html += "</ul>";
  html += "<h3>Diagnostics</h3>";
  html += "<ul>";
  html += "<li>Current time: " + String(time_buffer) + " (Local)</li>";
  html += "<li>Uptime: " + String(format_duration(millis() / 1000)) + "</li>";
  html += "<li>Free heap: " + format_memory(ESP.getFreeHeap()) + "</li>";
  html += "</ul>";
  html += "<br/>Go to <a href=\"config\">configure page</a> to change settings.";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup()
{
  // Disable brownout
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  //  ADC_EN is the ADC detection enable port
  //  If the USB port is used for power supply, it is turned on by default.
  //  If it is powered by battery, it needs to be set to high level
  pinMode(GPIO_ADC_EN, OUTPUT);
  digitalWrite(GPIO_ADC_EN, HIGH);

#ifdef CORE_DEBUG_LEVEL
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // esp_log_level_set("*", ESP_LOG_WARN);
#endif

  log_i("CPU Freq = %d Mhz", getCpuFrequencyMhz());
  log_i("Free heap: %d bytes", ESP.getFreeHeap());
  log_i("Starting " APP_TITLE "...");

  tft.init();
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness
  tft.setRotation(1);
  tft.setTextDatum(TL_DATUM); // Top Left
  tft.setTextColor(text_color);

  // Create sprite for clock. 1 bit depth
  clock_sprite.setColorDepth(1);
  clock_sprite.createSprite(TOP_BAR_TIME_WIDTH, TOP_BAR_HEIGHT);
  clock_sprite.setTextColor(text_color);

  // Initializing the configuration for web configuration
  param_group.addItem(&iotWebParamLocation);
  param_group.addItem(&iotWebParamOpenWeatherApiKey);
  param_group.addItem(&iotWebParamTimeZone);
  param_group.addItem(&iotWebParamMetric);
  iotWebConf.addParameterGroup(&param_group);

  iotWebConf.getApTimeoutParameter()->visible = true;
  iotWebConf.setWifiConnectionCallback(update_runtime_config);

  iotWebConf.init();

  // Set up required URL handlers on the web server.
  server.on("/", HTTP_GET, handleRoot);
  server.on("/config", []
            { iotWebConf.handleConfig(); });
  server.onNotFound([]()
                    { iotWebConf.handleNotFound(); });

  // Set the time servers
  configTime(0, 0, NTP_SERVERS);
  // Set the timezone
  auto tz_definition = timezonedb_get_definition(iotWebParamTimeZone.value());
  setenv("TZ", tz_definition, 1);
  tzset();

  // Clear the screen
  tft.fillRect(0, 0, TFT_HEIGHT, TFT_WIDTH, background_color);
  // Set the location, Font(4) = 26px
  tft.drawString(iotWebParamLocation.value(), TOP_BAR_LOCATION_X, TOP_BAR_Y, font_16pt);
}

void clear()
{
  log_d("Clear screen");
  tft.fillRect(0, 0, TFT_HEIGHT, TFT_WIDTH, background_color);
  tft.setCursor(0, 0);
}

void display_network_state(iotwebconf::NetworkState state)
{
  log_i("Network state: %d", state);
  unsigned short *image_data;
  switch (state)
  {
  case iotwebconf::NotConfigured:
  case iotwebconf::ApMode:
    // Show WiFi AP screen
    image_data = z_image_decode(&image_wifi);
    tft.pushImage(0, 0, image_wifi.width, image_wifi.height, image_data);
    delete[] image_data;
    tft.drawCentreString(state == iotwebconf::NotConfigured ? "No config. Connect to SSID:" : "To configure, connect to SSID:", TFT_HEIGHT / 2, TFT_WIDTH - 42, font_16pt);
    tft.drawCentreString(iotWebConf.getThingName(), TFT_HEIGHT / 2, TFT_WIDTH - 26, font_26pt);
    break;
  case iotwebconf::Connecting:
    // Show OpenWeather welcome screen / logo
    image_data = z_image_decode(&image_open_weather);
    tft.pushImage(0, 0, image_open_weather.width, image_open_weather.height, image_data);
    delete[] image_data;
    break;
  case iotwebconf::OffLine:
    // Show Dinosaur / cactus image
    image_data = z_image_decode(&image_no_internet);
    tft.pushImage(0, 0, image_no_internet.width, image_no_internet.height, image_data);
    delete[] image_data;
    break;
  case iotwebconf::OnLine:
    clear();
    // Set the location, Font(4) = 26px
    tft.drawString(iotWebParamLocation.value(), TOP_BAR_LOCATION_X, TOP_BAR_Y, font_16pt);
    break;
  }
}

void update_time()
{
  // Display time
  struct tm tm;
  getLocalTime(&tm);
  char time_buffer[20];
  if (time_valid())
    strftime(time_buffer, sizeof(time_buffer), "%T", &tm);
  else
    strcpy(time_buffer, "##:##:##");

  clock_sprite.fillSprite(background_color);
  clock_sprite.drawRightString(time_buffer, TOP_BAR_TIME_WIDTH, 0, font_26pt);
  clock_sprite.pushSprite(TOP_BAR_TIME_X, TOP_BAR_Y);
}

void display_error(const String &message)
{
  auto image_data = z_image_decode(&image_error);
  tft.pushImage(0, MAIN_BAR_Y, image_error.width, image_error.height, image_data);
  delete[] image_data;

  tft.fillRect(0, BOTTOM_BAR_Y, TFT_HEIGHT, BOTTOM_BAR_HEIGHT, background_color);
  tft.drawCentreString(message, TFT_HEIGHT / 2, BOTTOM_BAR_Y, font_16pt);
}

void update_weather()
{
  unsigned short *image_data;
  auto request_units = iotWebParamMetric.value() ? "metric" : "imperial";
  HTTPClient client;
  if (!client.begin(String("https://api.openweathermap.org/data/2.5/weather?q=") + iotWebParamLocation.value() + "&appid=" + iotWebParamOpenWeatherApiKey.value() + "&units=" + request_units))
  {
    auto message = "Failed to start HTTP client. DNS/TCP?";
    log_e("%s", message);
    display_error(message);
    client.end();
    return;
  }

  auto code = client.GET();
  // Check if request went OK
  if (code != HTTP_CODE_OK)
  {
    auto message = String(code) + " " + (code < 0 ? client.errorToString(code) : http_status_reason(code));
    log_e("OpenWeatherMap API error: %d (%s)", code, message.c_str());
    display_error(message);
    client.end();
    return;
  }

  // Check if deserialization worked
  DynamicJsonDocument doc(2048);
  // Parse JSON object
  const auto error = deserializeJson(doc, client.getString());
  if (error)
  {
    log_e("Deserialize JSon error: %s", error.c_str());
    auto message = error.c_str();
    display_error(message);
    client.end();
    return;
  }

  const auto main = doc["main"];
  const auto temperature = (const float)main["temp"];
  const auto humidity = (const uint8_t)main["humidity"];
  const auto datetime = (const uint32_t)doc["dt"];
  const auto sys = doc["sys"];
  const auto sunrise = (const uint32_t)sys["sunrise"];
  const auto sunset = (const uint32_t)sys["sunset"];
  const auto weather = doc["weather"];
  const auto id = (const uint16_t)weather[0]["id"];
  const auto pressure = (const float)main["pressure"];
  log_i("datetime=%d, sunrise=%d, sunset=%d, weather=%d, pressure=%d", datetime, sunrise, sunset, id, pressure);

  const auto isDay = datetime > sunrise && datetime < sunset;
  const auto isWarm = temperature > (iotWebParamMetric.value() ? WARM_TEMPERATURE_CENTIGRADE : WARM_TEMPERATURE_FAHRENHEIT);

  tft.fillRect(0, MAIN_BAR_Y, TFT_HEIGHT, MAIN_BAR_HEIGHT, background_color);
  tft.fillRect(0, BOTTOM_BAR_Y, TFT_HEIGHT, BOTTOM_BAR_HEIGHT, background_color);

  // Draw the temperature and humidity icons
  const auto temperature_icon = isWarm ? &image_temperature_warm : &image_temperature_cold;
  image_data = z_image_decode(temperature_icon);
  tft.pushImage(MAIN_BAR_TEMPERATURE_ICON_X, MAIN_BAR_TEMPERATURE_Y, temperature_icon->width, temperature_icon->height, image_data, image_color_transparent);
  delete[] image_data;
  // Fix for degrees symbol : ° is ` in library (only 16pt font)
  // const auto temperature_unit = iotWebParamMetric.value() ? "`C" : "`F";
  // tft.drawCentreString(temperature_unit, MAIN_BAR_TEMPERATURE_ICON_X + image_temperature.width / 2, MAIN_BAR_TEMPERATURE_Y + image_temperature.height, font_16pt);
  image_data = z_image_decode(&image_humidity);
  tft.pushImage(MAIN_BAR_HUMIDITY_ICON_X, MAIN_BAR_HUMIDITY_Y, image_humidity.width, image_humidity.height, image_data, image_color_transparent);
  delete[] image_data;
  // tft.drawCentreString("%R", MAIN_BAR_HUMIDITY_ICON_X + image_humidity.width / 2, MAIN_BAR_HUMIDITY_Y + image_humidity.height, font_16pt);

  // Celcius, accuracy 0.1, Imperial has no fraction
  auto temperatureString = iotWebParamMetric.value() ? String(temperature, 1) : String(temperature, 0);
  auto humidityString = String(humidity);
  tft.drawString(temperatureString, MAIN_BAR_TEMPERATURE_X, MAIN_BAR_TEMPERATURE_Y, font_48pt);
  tft.drawString(humidityString, MAIN_BAR_HUMIDITY_X, MAIN_BAR_HUMIDITY_Y, font_48pt);

  // Lookup weather code
  auto info = lookup_weather_code(id);
  if (info)
  {
    tft.drawCentreString(info->description, TFT_HEIGHT / 2, BOTTOM_BAR_Y, font_16pt);
    auto image = isDay ? info->imageDay : info->imageNight;
    auto image_data = z_image_decode(image);
    tft.pushImage(MAIN_BAR_WEATHER_ICON_X, MAIN_BAR_WEATHER_ICON_Y, image->width, image->height, image_data, image_color_transparent);
    delete[] image_data;
  }
  else
    log_e("Unknown weather code: %d", id);

  tft.drawRightString(String((const int)pressure) + " hpa", TFT_HEIGHT, MAIN_BAR_PRESSURE_Y, font_16pt);

  client.end();
}

// Variables for updating the display
unsigned long next_refresh_weather;
unsigned long next_update_clock;

void loop()
{
  iotWebConf.doLoop();

  static auto last_network_state = iotwebconf::NetworkState::Boot;

  auto network_state = iotWebConf.getState();
  if (network_state != last_network_state)
  {
    last_network_state = network_state;
    display_network_state(network_state);
  }

  auto now = millis();

  switch (network_state)
  {
  case iotwebconf::NetworkState::OffLine:
    next_refresh_weather = 0ul;
    break;

  case iotwebconf::NetworkState::OnLine:
    if (now > next_update_clock)
    {
      update_time();
      next_update_clock = now + time_update_milliseconds;
    }
    if (now > next_refresh_weather)
    {
      update_weather();
      next_refresh_weather = now + weather_update_milliseconds;
    }
    break;
  }

  yield();
}