/*
mothCam.ino, part of MothCam, for an automatic moth camera
Copyright (C) 2025 Charles Nicholson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// To-do:
// - Learn about esp_err_t.
// - Convert defines to upper snake case.
// - Find a better SD card library.
// - .write vs .println for SD card.
// - Print available size for SD card.
// - Find pin settings for OV5640.
// - Estimate images on SD and images left on SD.
// - Get sunrise and sunset to calculate when to turn on the LEDs.
// - Make a function to turn on the LEDs.
// - How many watts can a FET sink - heatsink needed?

#include "driver/gpio.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "fb_gfx.h"
#include "img_converters.h"
#include "settings.h" // Change settings and view pin defs here.
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#include <ArduinoWebsockets.h>
#include <SD.h>
#include <WiFi.h>

#define pwdnGpioNum 32
#define resetGpioNum -1
#define xclkGpioNum 0
#define siodGpioNum 26
#define siocGpioNum 27
#define y9GpioNum 35
#define y8GpioNum 34
#define y7GpioNum 39
#define y6GpioNum 36
#define y5GpioNum 21
#define y4GpioNum 19
#define y3GpioNum 18
#define y2GpioNum 5
#define vsyncGpioNum 25
#define hrefGpioNum 23
#define pclkGpioNum 22

camera_fb_t *fb = NULL;
size_t jpgBufLen = 0;
uint8_t *jpgBuf = NULL;

#ifdef USE_WIFI
using namespace websockets;
WebsocketsClient client;
#endif

// Optimize this!
#ifndef WIFI
#ifndef LDR
#error "LDR and WiFi are disabled, so there's no source of sunrise/sunset data! Please enable one or both to prevent lights from permanently being on."
#endif
#endif

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  pinMode(FET, OUTPUT);
  pinMode(SMALL_LED, OUTPUT);
  digitalWrite(FET, LOW);

  blink(SMALL_LED);
  blink(FET);

  Serial.begin(BAUD_RATE);
  // Serial.setDebugOutput(true);

  while (!Serial)
    blink(SMALL_LED);

  Serial.println("MothCam: Automated Photography of Nocturnal Lepidoptera for Urban Citizen Science");
  Serial.println("Initilization process starting.");

  cameraSetup();
#ifdef USE_WIFI
  wiFiSetup();
#endif
#ifdef USE_SD
  sdSetup();
#endif

  Serial.println("Everything has initialized. Sketch starting now.");
}

void loop() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Image capture failed!");
    esp_camera_fb_return(fb);
    ESP.restart();
  }
#ifdef USE_WIFI
  if (client.available()) {
    client.sendBinary((const char *)fb->buf, fb->len);

    Serial.println("Image sent.");

    client.poll();
  }
#endif
#ifdef USE_SD
  writeToSD(&fb);
#endif
  esp_camera_fb_return(fb);
}

void onMessageCallback(WebsocketsMessage message) {
  Serial.print("Received message from server: ");
  Serial.println(message.data());
}

esp_err_t cameraSetup() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = y2GpioNum;
  config.pin_d1 = y3GpioNum;
  config.pin_d2 = y4GpioNum;
  config.pin_d3 = y5GpioNum;
  config.pin_d4 = y6GpioNum;
  config.pin_d5 = y7GpioNum;
  config.pin_d6 = y8GpioNum;
  config.pin_d7 = y9GpioNum;
  config.pin_xclk = xclkGpioNum;
  config.pin_pclk = pclkGpioNum;
  config.pin_vsync = vsyncGpioNum;
  config.pin_href = hrefGpioNum;
  config.pin_sscb_sda = siodGpioNum;
  config.pin_sscb_scl = siocGpioNum;
  config.pin_pwdn = pwdnGpioNum;
  config.pin_reset = resetGpioNum;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE;
  config.jpeg_quality = JPEG_QUALITY;
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera initilization failed: 0x%x", err);
    return err;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_VGA);
  Serial.println("Camera succesfully initialized!");
  return ESP_OK;
};

esp_err_t wiFiSetup() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  long start = millis();

  Serial.println("WiFi initialization...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start > 15000) {
      Serial.println("There seems to be an issue with the WiFi.");
      Serial.println("Please confirm the SSID (" + WIFI_SSID + ") and the password (" + WIFI_PASSWORD ") are correct.")
    }
  }
  Serial.println("WiFi succesfully initialized!");
  Serial.println("Connecting to WebSocket: ws://" + SERVER_HOST + ":" + SERVER_PORT);

  client.onMessage(onMessageCallback);

  if (!client.connect(SERVER_HOST, SERVER_PORT, "/")) {
    Serial.println("Connecting to WebSocket failed!");
    Serial.println(WiFi.localIP());
    return ESP_FAIL;
  }

  Serial.println("WebSocket successfully initialized!");

  client.send("MothCam connection testing"); // This won't be mistaken for an image, don't worry!

  return ESP_OK;
};

void sdSetup() {
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed. Freezing.");
    while (1)
      ;
  }

  Serial.println("SD card successfully initialized!");
}

void writeToSD(camera_fb_t &fb) {
  long index = 0;

  File indexFile = SD.open("index.txt", FILE_READ);
  if (indexFile) {
    index = indexFile.read();
    indexFile.close();
  }

  File indexFile = SD.open("index.txt", FILE_WRITE);
  if (indexFile) {
    indexFile.write(index + 1);
    indexFile.close();
  }

  File images = SD.open(String(index) + ".jpg", FILE_WRITE);

  if (images) {
    images.write((const char *)fb->buf);
    images.close();
  } else {
    Serial.println("Error writing to the SD card.");
  }
}

void blink(int led) {
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
  delay(1000);
}

void flash(int led) {
  digitalWrite(led, HIGH);
  delay(250);
  digitalWrite(led, LOW);
  delay(250);
}

void pulse(int led) {
  digitalWrite(led, HIGH);
  delay(50);
  digitalWrite(led, LOW);
  delay(50);
}