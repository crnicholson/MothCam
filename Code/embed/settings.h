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

#include "secrets.h" // Development secrets file with WiFi settings.

// WiFi.
#define SSID SECRET_SSID
#define PASSWORD SECRET_PASSWORD

// Server.
#define WEBSOCKETS_SERVER_HOST "0.0.0.0"
#define WEBSOCKETS_SERVER_PORT 3000

// Other.
#define BAUD_RATE 115200