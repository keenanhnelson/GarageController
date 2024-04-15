# Garage Controller

## Description

This project uses the extremely cheap esp8266 to control an old garage with a web browser from anywhere in the world with internet access

## Additional files required

Make sure to add `Code/MakefileUserSpecificInfo.mk` with local machine specific information. An example is provided below

```
# If SDK_LOC is changed then I might need to change $(SDK_LOC)/ld/eagle.rom.addr.v6.ld at the bottom of Makefile
SDK_LOC = C:/KeenanFiles/Libs/ESP8266_NONOS_SDK-2.2.1
XTENSA_LOC = C:/KeenanFiles/Libs/SysGCC/esp8266/opt/xtensa-lx106-elf/bin
FIND_CMD = C:/msys64/usr/bin/find.exe
MKDIR_CMD = C:/msys64/usr/bin/mkdir.exe

# Flash info
COM_PORT = COM3

# OTA info
ESP_IP_ADDRESS = 192.168.254.146
OTA_PORT = 8266
OTA_AUTH = BAD_OTA_PASSWORD
```

Next make sure to add `Code/PrivateCodes.hpp` and make sure to change the bad example passwords below

```
const char* WIFI_SSID = "MySsid";
const char* WIFI_PASSWORD = "MyWifiPassword";

const int SERVER_PORT = 1234;
const char* SERVER_COOKIE_NAME = "COOKIE_NAME";
const char* SERVER_COOKIE_PASSWORD = "COOKIE_PASSWORD";
const char* SERVER_USERNAME = "BAD_USER_NAME";
const char* SERVER_PASSWORD = "BAD_PASSWORD";

const int OTA_PORT = 8266;
const char* OTA_PASSWORD = "BAD_OTA_PASSWORD";

const char* TIMEZONE_API_KEY = "TIMEZONE_API_CODE";

const char* ANDROID_PASSWORD = "BAD_ANDROID_PASSWORD";
```