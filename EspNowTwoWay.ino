/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp8266-nodemcu/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/


#include <espnow.h>
#include "EspNowSetup.h"
#include "WifiSetup.h"
#include "WebServerSetup.h"

// VARIABILI GLOBALI

ESP8266WebServer server(80);

unsigned long timerDelay;
bool realTimeMode;

struct_message incomingReadings;


// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t slaveMacAddress[] = {0xDC, 0x4F, 0x22, 0x55, 0x15, 0x06};

// LASCIARE NEL .ino altrimenti crasha
void initEspNow(){
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP NOW Init Successfully");

}


 
void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  connectWifi();
  initEspNow();
  setupWebServer();
  
}
 
void loop() {
  server.handleClient();
}