
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <LittleFS.h>

#ifndef WEBSERVERSETUP_H
#define WEBSERVERSETUP_H

extern AsyncWebServer  server;

void handleHttpRoutes();

#endif
