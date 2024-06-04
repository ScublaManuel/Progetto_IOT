#include <SoftwareSerial.h> // Comunicazione seriale Arduino-esp8266

SoftwareSerial espSerial(10, 11); // Pines modificati per evitare conflitti con Serial

#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x8c, 0xaa, 0xb5, 0x7c, 0x6a, 0x17};

// Define variables to store readings to be sent
float temperature;
float humidity;
float bright;
float batteryLevel;
float soilHumidity;
float waterLevel;
float irrigationMode;

// Updates readings every 10 seconds
const long interval = 10000; 
unsigned long previousMillis = 0;    // will store last time readings were updated 

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    float temp;
    float hum;
    float bri;
    float bat;
    float soil;
    float water;
    float irr;
} struct_message;

// Create a struct_message called DHTReadings to hold sensor readings
struct_message readings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void getReadings(){
  // legge i valori da trasmettere poi all'altro esp
  if (espSerial.available()) {
    String dati = Serial.readString();
    Serial.println(dati);
  }
  char dataArray[dati.length() + 1];
  dati.toCharArray(dataArray, dati.length() + 1);
  char* token = strtok(dataArray, ",");
  int i = 0;
  float floatArray[7]; // adjust the size according to your needs

  while (token!= NULL) {
    floatArray[i] = atof(token);
    token = strtok(NULL, ",");
    i++; 
  }
  readings.water = waterLevel;
  readings.bat = batteryLevel;
  readings.bri = bright;
  readings.temp = temperature;
  readings.hum = humidity;
  readings.soil = soilHumidity;
  readings.irr = irrigationMode;
}

void creaDatiFasulli() {
  readings.water = 1;
  readings.bat = 2;
  readings.bri = 3;
  readings.temp = 4;
  readings.hum = 5;
  readings.soil = 6;
  readings.irr = 7;
}


void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  espSerial.begin(9600);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

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
}
 
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    //Get DHT readings
    //getReadings();
    creaDatiFasulli();
    
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &sensorReadings, sizeof(DHTReadings));
  }
}