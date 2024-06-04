#include <InfluxDbClient.h>
// Import required libraries
//#include <ESPAsyncWebServer.h> incluso gia in WebServerSetup.h
#include "WebServerSetup.h"



 
ESP8266WebServer server(80);


void setup(){
  Serial.begin(9600);
  setupWebServer();
}
 
void loop() {
  server.handleClient(); //Handling of incoming requests
}
 