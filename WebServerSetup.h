#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include "SDCardSetup.h"

#ifndef WEBSERVERSETUP_H
#define WEBSERVERSETUP_H

// server in ascolto sulla porta HTTP 80
extern ESP8266WebServer server;
//extern AsyncEventSource events;
extern unsigned long timerDelay;
extern bool realTimeMode;

void setupWebServer();
void initFS();
void handleHttpRoutes();


#endif
