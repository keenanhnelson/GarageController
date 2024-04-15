#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "ESP8266WebServer.h"

extern ESP8266WebServer server;

void serverInit();
void handleServer();

#endif /* SERVER_HPP_ */
