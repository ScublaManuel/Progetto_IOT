#include "WebServerSetup.h"
#include <ESP8266HTTPClient.h>

// Funzione per recuperare i dati da un altro dispositivo ESP
String fetchDataFromOtherESP() {
  HTTPClient http;
  http.begin("http://other_esp_ip_address/data_endpoint"); // Sostituisci con l'URL effettivo dell'altro dispositivo ESP
  int httpCode = http.GET();
  String payload = "{}"; // Oggetto JSON vuoto predefinito

  if (httpCode > 0) {
    payload = http.getString();
  }
  http.end();

  return payload;
}

void handleHttpRoutes() {
  // Inizializza il file system LittleFS
  LittleFS.begin();

  // Route per servire il file HTML principale
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  // Route per servire il file CSS
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/styles.css", "text/css");
  });

  // Route per servire il file CSS di FontAwesome
  server.on("/fontawesome/css/all.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/fontawesome/css/all.min.css", "text/css");
  });

  // Route per gestire le richieste POST per recuperare i dati
  server.on("/getData", HTTP_POST, [](AsyncWebServerRequest *request) {
    String data = fetchDataFromOtherESP();
    request->send(200, "application/json", data);
  });
}
