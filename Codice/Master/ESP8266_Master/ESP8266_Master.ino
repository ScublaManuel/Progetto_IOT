#include <Arduino.h>
#include <PicoMQTT.h>

PicoMQTT::Server mqtt;

void loop() {
    // This will automatically handle client connections.  By default, all clients are accepted.
    mqtt.loop();

    if (random(1000) == 0)
        mqtt.publish("picomqtt/welcome", "Hello from PicoMQTT!");
}

// Import required libraries
#include <FS.h>
#include <ESP8266WiFi.h>
//#include <ESPAsyncWebServer.h> incluso gia in WebServerSetup.h
#include "WebServerSetup.h"
// Replace with your network credentials
const char* ssid = ""; //Replace with your own SSID
const char* password = ""; //Replace with your own password

AsyncWebServer  server(80);

void connectWifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println("Connected to WiFi successfully");
  Serial.println(WiFi.localIP());
}


void setup(){
    
  Serial.begin(9600);
  //------------INIZIO SETUP ACCESS POINT------------//  
  connectWifi();

  handleHttpRoutes();

  server.begin();
  Serial.println("HTTP server started");
  //------------FINE SETUP ACCESS POINT------------//
  //------------INIZIO SETUP BROKER------------//
  WiFi.mode(WIFI_STA);

    // Subscribe to a topic pattern and attach a callback
    mqtt.subscribe("#", [](const char * topic, const char * payload) {
        Serial.printf("Received message in topic '%s': %s\n", topic, payload);
    });

    // Start the broker
    mqtt.begin();
    //------------FINE SETUP BROKER------------//
}

void loop() {
    // This will automatically handle client connections.  By default, all clients are accepted.
    mqtt.loop();

    if (random(1000) == 0)
        mqtt.publish("picomqtt/welcome", "Hello from PicoMQTT!");
}