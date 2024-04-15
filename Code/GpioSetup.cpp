#include "GpioSetup.hpp"
#include "Arduino.h"

const uint8_t CLOSED_DETECT_PIN = 13;
const uint8_t OPENED_DETECT_PIN = 12;
const uint8_t MOVE_GARAGE_DOOR_PIN = 14;

void gpioInit(){
	pinMode(MOVE_GARAGE_DOOR_PIN, OUTPUT); //Output to open/close garage door
	digitalWrite(MOVE_GARAGE_DOOR_PIN, 0); //Make sure garage door doesn't get triggered.

	pinMode(OPENED_DETECT_PIN, INPUT);
	pinMode(CLOSED_DETECT_PIN, INPUT);
}
