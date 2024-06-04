
// #include <ESPAsyncWebServer.h>
// #include <ESPAsyncTCP.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#ifndef WEBSERVERSETUP_H
#define WEBSERVERSETUP_H

extern ESP8266WebServer  server;

void connectWifi();
void setupWebServer();
void handleHttpRoutes();


#endif
