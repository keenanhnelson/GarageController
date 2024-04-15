#ifndef GPIOSETUP_HPP_
#define GPIOSETUP_HPP_

#include <cstdint>

extern const uint8_t CLOSED_DETECT_PIN;
extern const uint8_t OPENED_DETECT_PIN;
extern const uint8_t MOVE_GARAGE_DOOR_PIN;

void gpioInit();

#endif /* GPIOSETUP_HPP_ */
