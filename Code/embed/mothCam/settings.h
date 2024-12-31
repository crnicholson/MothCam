/*
settings.h, part of MothCam, for an automatic moth camera
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

#include "secrets.h" // This is my development secrets file with WiFi settings, you don't need this.

// Enables.
#define USE_WIFI // Disable and enable WiFi.
#define USE_SD   // Disable and enable local storage of SD card data.
#define USE_LDR // If WiFi isn't being used, enable surise/sunset data from 

// Pins.
#define CS 10       // Chip select for the SD card.
#define FET 9       // Pin for the MOSFET which turns on and off the lighting array.
#define SMALL_LED 8 // Pin for small LED.
#define LDR 7

// Other pins.
// SDO - pin 11
// SDI - pin 12
// CLK - pin 13

// WiFi.
#define WIFI_SSID SECRET_SSID         // Your WiFi SSID (name).
#define WIFI_PASSWORD SECRET_PASSWORD // Your WiFi password.

// Server.
#define SERVER_HOST "0.0.0.0"
#define SERVER_PORT 3000

// Image quality.
#define FRAMESIZE FRAMESIZE_VGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA.
#define JPEG_QUALITY 15;         // 10-63. Lower number means higher quality.

// Other.
#define BAUD_RATE 115200