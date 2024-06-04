#include "WebServerSetup.h"

// server in ascolto sulla porta HTTP 80

// Replace with your network credentials


 
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


void setupWebServer(){
  connectWifi();
  handleHttpRoutes();
  server.begin();
  Serial.println("HTTP server started");
}

void handleHttpRoutes(){
  // Initialize LittleFS file system
  LittleFS.begin();
  // Read the HTML file from LittleFS
  // Routes for web page
  server.on("/", HTTP_GET, [](){
    File file =  LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });
  server.on("/index.html", HTTP_GET, [](){
    File file =  LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });
  server.on("/env_sensor.html", HTTP_GET, [](){
    File file =  LittleFS.open("/env_sensor.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });
  server.on("/soil_sensor.html", HTTP_GET, [](){
    File file =  LittleFS.open("/soil_sensor.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });
  server.on("/history.html", HTTP_GET, [](){
    File file =  LittleFS.open("/history.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });
  // Route to load style.css file
  server.on("/styles.css", HTTP_GET, [](){
    File file =  LittleFS.open("/styles.css", "r");
    server.streamFile(file, "text/css");
    file.close();
  });


// // Crea un oggetto con i dati da inviare
//             var data = {
//                 desiredHumidity: desiredHumidity,
//                 selectedSensorManual: selectedSensorManual,
//                 selectedSensorAuto: selectedSensorAuto,
//                 mode: mode
//             };
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

  
}