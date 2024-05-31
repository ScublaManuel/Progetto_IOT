#include "WebServerSetup.h"


// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

void setupWebServer(){
  // Initialize LittleFS file system
  initFS();
  setup_SDCard();
  handleHttpRoutes();
  server.begin();
  Serial.println("HTTP server started");
}




void handleHttpRoutes(){
  
  // Read the HTML file from LittleFS
  server.on("/", HTTP_GET, [](){
    File file =  LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });
  
// Dati ricevuti come body di /set-humidity
// var data = {
//     desiredHumidity: desiredHumidity,
//     selectedSensorManual: selectedSensorManual,
//     selectedSensorAuto: selectedSensorAuto,
//     mode: mode
// };
  server.on("/set-humidity", HTTP_POST, [](){    
    if (!server.hasArg("plain")){ //Check if body received
      server.send(400, "text/plain", "Body not received");
      return;
    }
    server.send(200, "text/plain", "ok");
    StaticJsonDocument<200> doc;
    String msg = server.arg("plain");
    deserializeJson(doc, msg);
    Serial.println(msg);
    String value = String(doc["desiredHumidity"]);
    Serial.println(value);
  });

  

  
  // Send sensor readings memorized in the SD Card
  server.on("/readings", HTTP_GET, [](){
    // char columnTypes[10] = "fffffff";
    // parseCSV(columnTypes,"data.csv");
    // Read CSV file from SD card
    File file = SD.open("/masterData.csv");
    if (!file) {
      Serial.println("Error opening file");
      server.send(400,"text/plain","Error opening file");
      return;
    }
    String csvData = csvToString(file);
    Serial.println(csvData);
    server.send(200,"text/csv",csvData);
  });


  server.on("/realTimeMode", HTTP_POST, [](){    
    if (!server.hasArg("plain")){ //Check if body received
      server.send(400, "text/plain", "Body not received");
      return;
    }
    server.send(200, "text/plain", "ok");
    StaticJsonDocument<200> doc;
    String msg = server.arg("plain");
    deserializeJson(doc, msg);
    Serial.println(msg);
    bool realtimeflag = doc["realTime"];
    unsigned long timeIntervalMillis = doc["timeIntervalMillis"];
    // aggiornamento timerDelay per il loop()
    timerDelay = timeIntervalMillis;
    // invia il nuovo timerDelay al esp slave tramite esp now
    //inviaTimerDelay(timerDelay);
    Serial.println("Realtime: "+String(realtimeflag));
    realTimeMode = realtimeflag;
  });

  // invia qualsiasi file all'interno della cartella /
  server.serveStatic("/", LittleFS, "/");  
}