#include "Arduino.h"
#include "Wifi.hpp"
#include "Server.hpp"
#include "GpioSetup.hpp"
#include "Ota.hpp"
#include "EventsAndLogs.hpp"

void setup(void) {
	Serial.begin(115200);

	wifiInit();
	serverInit();
	gpioInit();
	otaInit();
	eventsAndLogsInit();
}

void loop(void) {
	handleServer();
	handleOta();
	handleEventsAndLogs();
}
