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

#include "driver/gpio.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "fb_gfx.h"
#include "img_converters.h"
#include "settings.h" // Change settings here.
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#include <ArduinoWebsockets.h>
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
uint8_t state = 0;

using namespace websockets;
WebsocketsClient client;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(BAUD_RATE);
  Serial.setDebugOutput(true);

  while (!Serial) {
    ;
  }

  Serial.println("MothCam starting...");

  initCamera();
  initWifi();
}

void loop() {
  if (client.available()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Image capture failed!");
      esp_camera_fb_return(fb);
      ESP.restart();
    }
    client.sendBinary((const char *)fb->buf, fb->len);
    Serial.println("Image sent.");
    esp_camera_fb_return(fb);
    client.poll();
  }
}

void onMessageCallback(WebsocketsMessage message) {
  Serial.print("Got message: ");
  Serial.println(message.data());
}

esp_err_t initCamera() {
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

  config.frame_size = FRAMESIZE_VGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA.
  config.jpeg_quality = 15;          // 10-63. Lower number means higher quality.
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("camera init FAIL: 0x%x", err);
    return err;
  }
  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_VGA);
  Serial.println("camera init OK");
  return ESP_OK;
};

esp_err_t initWifi() {
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Wifi init ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi OK");
  Serial.println("connecting to WS: ");
  client.onMessage(onMessageCallback);
  bool connected = client.connect(WEBSOCKETS_SERVER_HOST, WEBSOCKETS_SERVER_PORT, "/");
  if (!connected) {
    Serial.println("WS connect failed!");
    Serial.println(WiFi.localIP());
    state = 3;
    return ESP_FAIL;
  }
  if (state == 3) {
    return ESP_FAIL;
  }

  Serial.println("WS OK");
  client.send("hello from ESP32 camera stream!");
  return ESP_OK;
};
