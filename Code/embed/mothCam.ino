#include "esp_camera.h"
#include <HTTPClient.h>
#include <WiFi.h>

// Camera pin configuration (specific to ESP32-CAM with OV5640)
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 32
#define SIOD_GPIO_NUM 13
#define SIOC_GPIO_NUM 12

#define Y9_GPIO_NUM 39
#define Y8_GPIO_NUM 36
#define Y7_GPIO_NUM 23
#define Y6_GPIO_NUM 18
#define Y5_GPIO_NUM 15
#define Y4_GPIO_NUM 4
#define Y3_GPIO_NUM 14
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 27
#define HREF_GPIO_NUM 25
#define PCLK_GPIO_NUM 19

const char *ssid = "Crofut_house2";
const char *password = "BOS130HGK1020MEL";
const char *serverUrl = "http://localhost:5000/upload";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Initialize the camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Init with high specs to capture at max resolution
  config.frame_size = FRAMESIZE_UXGA; // UXGA for 1600x1200
  config.jpeg_quality = 10;           // Lower quality for smaller size
  config.fb_count = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  // Take and send photo
  sendPhoto();
}

void sendPhoto() {
  // Capture photo
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Send captured photo to the server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "image/jpeg");

    int httpResponseCode = http.POST(fb->buf, fb->len);

    if (httpResponseCode > 0) {
      String response = http.getString(); // Get the response to the request
      Serial.println(httpResponseCode);   // Print return code
      Serial.println(response);           // Print server response
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  // Return the frame buffer back to be reused
  esp_camera_fb_return(fb);
}

void loop() {
  // Nothing here
}
