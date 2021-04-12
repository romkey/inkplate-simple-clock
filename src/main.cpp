#include <Arduino.h>
#include <Wifi.h>
#include <time.h>

#include <Inkplate.h>

#include "config.h"

#include "fonts/Lato_Regular120pt7b.h"

static Inkplate display(INKPLATE_1BIT);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  display.begin();
  Serial.println("display.begin");

  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(20, 20);

  display.display();
  display.setFont(&Lato_Regular120pt7b);
  display.println("Hello world");
  display.print(WiFi.localIP());
  display.display();

#define GMT_OFFSET_SECS  -8 * 60 * 60
#define DAYLIGHT_SAVINGS_OFFSET_SECS 3600
  configTime(GMT_OFFSET_SECS, DAYLIGHT_SAVINGS_OFFSET_SECS, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
}


void loop() {
  static int64_t next_update = 0;

  if(next_update > esp_timer_get_time())
    return;

  next_update += 60 * 1000 * 1000;

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  if(timeinfo.tm_year == 0) {
    Serial.println("time not set");

    display.clearDisplay();
    display.print("No NTP");
    display.display();
  }

  display.clearDisplay();
  display.setCursor(20, 100);

  #define BUF_SIZE 6
  char buf[BUF_SIZE];

  snprintf(buf, 6, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

  int16_t  x1, y1;
  uint16_t w, h;

  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);

  display.setCursor((600 - h)/2 - 100, (800 - w)/2);
  display.print(buf);

  display.display();
}
