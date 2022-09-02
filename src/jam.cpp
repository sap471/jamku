#include "Arduino.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// LED Matrix MAX7219
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define CS_PIN 15
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// RTC
RTC_DS3231 rtc;

// WiFi
WiFiUDP wifiUDP;

// NTP
NTPClient ntpClient = NTPClient(wifiUDP, "id.pool.ntp.org", 25200);

uint8_t scrollSpeed = 25;
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 0;

const char daysOfTheWeek[7][7] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};
char runningText[120] = {"Ini adalah Text Berjalan"};

const char *ssid = "human";
const char *password = "akumanusia";
bool wifiConnected = false;

const int attemptConnectWiFi = 3;
unsigned long intervalConnectWifi = 60000; // 1 hours

float humidity;
float temperature_c;
float temperature_f;

unsigned int clkTime = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println(F("Halo Manusia"));

  delay(100);

  myDisplay.begin();
  myDisplay.setIntensity(0);
  myDisplay.displayClear();
  myDisplay.setTextAlignment(PA_CENTER);

  delay(100);

  if (!rtc.begin())
  {
    Serial.println(F("RTC Error!"));
    myDisplay.print(F("RTC Error!"));
    delay(2000);
    myDisplay.print("Restart in 3s!");
    delay(3000);

    ESP.reset();
  }

  if (rtc.lostPower())
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  myDisplay.print(F("PANGESTU"));

  delay(1500);

  myDisplay.displayClear();
  myDisplay.displayReset();

  delay(200);
}

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println(F("Connecting to WiFi ..."));

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(WiFi.localIP());
  }
}

void disconnectWiFi()
{
  WiFi.disconnect();
}

void syncTime()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    ntpClient.begin();
    ntpClient.update();

    unsigned long unixTime = ntpClient.getEpochTime();

    Serial.printf("Unix Time: %lu", unixTime);

    rtc.adjust(unixTime);
  }
}

void showText(const char *text)
{
  if (myDisplay.getTextAlignment() != PA_CENTER)
    myDisplay.setTextAlignment(PA_CENTER);

  myDisplay.print(text);
}

void showClock(DateTime timestamp)
{
  char time[16];
  sprintf(time, "%02d : %02d : %02d", timestamp.hour(), timestamp.minute(), timestamp.second());
  // Serial.print(time);
  showText(time);
}

void showDate(DateTime timestamp)
{
  char date[16];
  sprintf(date, "%02d-%02d-%4d", timestamp.day(), timestamp.month(), timestamp.year());
  // Serial.print(date);
  showText(date);
}

void loop()
{
  DateTime now = rtc.now();

  static unsigned long currentTime = millis();
  if ((millis() - currentTime) > 2000 && (millis() - currentTime) <= 4500)
  {
    showDate(now);
  }
  else if ((millis() - currentTime) > 4500 && (millis() - currentTime) <= 25000)
  {
    showClock(now);
  }
  else if ((millis() - currentTime) > 25000)
  {
    currentTime = millis();
  }
  else
  {
    showText(daysOfTheWeek[now.dayOfTheWeek()]);
  }
}
