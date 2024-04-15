#include "Wifi.hpp"
#include "ESP8266WiFi.h"
#include "PrivateCodes.hpp"

void wifiInit(){
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println(String("Connected to ") + WIFI_SSID);
	Serial.println("IP address: " + WiFi.localIP().toString());
}
