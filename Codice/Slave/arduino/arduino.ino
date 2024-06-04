/*
Note:
- la stampa dei valori dei sensori va sul monitor seriale quindi viene letta dall'esp
- forse sono da togliere le à in output sul seriale
- le à sono decisamente da togliere nell'output seriale 
- controllare pin
*/

// Librerie
#include <RTClib.h> // Real Time Clock
#include <SoftwareSerial.h> // Comunicazione seriale Arduino-esp8266
#include <SD.h>
#include "DHT.h" // Temperatura e Umidità ambiente

// Costanti
#define LIVELLO_ACQUA_PIN A5
#define LUMINOSITA_PIN A0
#define DHT_PIN 2
#define DHTTYPE DHT11
#define IRRIGAZIONE_PIN 3
#define SD_CHIP_SELECT 4

DHT dht(DHT_PIN, DHTTYPE); // Inizializzo sensore
RTC_DS3231 rtc;
SoftwareSerial espSerial(10, 11); // Pines modificati per evitare conflitti con Serial

// Variabili
int valore_umidita_terreno = 0;
int slave_percentuale_umidita = 0;
int master_percentuale_umidita = 0;
DateTime data_ora;
int livello_acqua = 0;
int livello_batteria = 0;
int livello_luminosita = 0;
float livello_umidita_ambiente = 0;
float livello_temperatura_ambiente = 0;
char modalita_irrigazione = 'd'; // 'a' per automatica, 'm' per manuale, 'd' default

unsigned long durataIrrigazione = 0; // durata in millisecondi
unsigned long startIrrigazione = 0;

struct IrrigazioneManuale {
  int giornoSettimana;
  int ora;
  int minuto;
  unsigned long durata; // in millisecondi
};

IrrigazioneManuale orariIrrigazione[10];
int numOrariIrrigazione = 0;

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  
  setupRTC();
  setupDHT();
  setupIrrigazione();
  pinMode(LIVELLO_ACQUA_PIN, INPUT);
  pinMode(LUMINOSITA_PIN, INPUT);
  
  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("Errore inizializzazione SD");
    return;
  }

  // Inizializza i sensori e leggi i valori iniziali
  riceviValoriDaEsp();
  misuraLivelloAcqua();
  misuraLivelloBatteria();
  misuraLivelloLuminosita();
  misuraTempUmiAmbiente();
  misuraLivelloUmiditaTerreno();
}

void loop() {
  riceviValoriDaEsp();
  letturaSensori();
  modalitaIrrigazione();
  inviaDati();
  scritturaSD();
  delay(1000);
}

// ***********************************************************************
// Inizio letturaSensori
void letturaSensori() {
  misuraLivelloAcqua();
  stampaLivelloAcqua();
  
  misuraLivelloBatteria();
  stampaLivelloBatteria();
  
  misuraLivelloLuminosita();
  stampaLivelloLuminosita();
  
  misuraTempUmiAmbiente();
  stampaLivelloTemperaturaAmbiente();
  stampaLivelloUmiditaAmbiente();
  
  misuraLivelloUmiditaTerreno();
  stampaLivelloUmiditaTerreno();
}
// Fine letturaSensori
// ***********************************************************************

// ***********************************************************************
// Inizio RTC
void setupRTC() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void impostaDataOra(DateTime dt) {
  rtc.adjust(dt);
}

void stampaDataOra(DateTime dt) {
  Serial.print(dt.day(), DEC);
  Serial.print('/');
  Serial.print(dt.month(), DEC);
  Serial.print('/');
  Serial.print(dt.year(), DEC);
  Serial.print(" ");
  Serial.print(dt.hour(), DEC);
  Serial.print(':');
  Serial.print(dt.minute(), DEC);
  Serial.print(':');
  Serial.println(dt.second(), DEC);
}
// Fine RTC
// ***********************************************************************

// ***********************************************************************
// Inizio livello acqua
void misuraLivelloAcqua() {
  livello_acqua = analogRead(LIVELLO_ACQUA_PIN);
}

void stampaLivelloAcqua() {
  Serial.print("Livello acqua: ");
  Serial.println(livello_acqua);
}
// Fine livello acqua
// ***********************************************************************

// ***********************************************************************
// Inizio livello batteria
void misuraLivelloBatteria() {
  livello_batteria = analogRead(A0); // Leggi valore batteria
}

void stampaLivelloBatteria() {
  Serial.print("Livello Batteria: ");
  Serial.println(livello_batteria);
}
// Fine livello batteria
// ***********************************************************************

// ***********************************************************************
// Inizio luminosità
void misuraLivelloLuminosita() {
  int valore_ldr = analogRead(LUMINOSITA_PIN);
  livello_luminosita = map(valore_ldr, 20, 1005, 0, 100);
}

void stampaLivelloLuminosita() {
  Serial.print("Livello Luminosità: ");
  Serial.println(livello_luminosita);
}
// Fine luminosità
// ***********************************************************************

// ***********************************************************************
// Inizio DHT
void setupDHT() {
  dht.begin();
}

void misuraTempUmiAmbiente() {
  livello_umidita_ambiente = dht.readHumidity();
  livello_temperatura_ambiente = dht.readTemperature();
  
  if (isnan(livello_umidita_ambiente) || isnan(livello_temperatura_ambiente)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }
}

void stampaLivelloTemperaturaAmbiente() {
  Serial.print("Livello Temperatura ambiente: ");
  Serial.println(livello_temperatura_ambiente);
}

void stampaLivelloUmiditaAmbiente() {
  Serial.print("Livello Umidità ambiente: ");
  Serial.println(livello_umidita_ambiente);
}
// Fine DHT
// ***********************************************************************

// ***********************************************************************
// Inizio irrigazione
void setupIrrigazione() {
  pinMode(IRRIGAZIONE_PIN, OUTPUT);
  digitalWrite(IRRIGAZIONE_PIN, LOW);
}

void misuraLivelloUmiditaTerreno() {
  valore_umidita_terreno = analogRead(A0);
  slave_percentuale_umidita = map(valore_umidita_terreno, 490, 1023, 100, 0);
}

void irrigazioneManuale() {
  DateTime now = rtc.now();
  for (int i = 0; i < numOrariIrrigazione; i++) {
    if (now.dayOfTheWeek() == orariIrrigazione[i].giornoSettimana &&
        now.hour() == orariIrrigazione[i].ora &&
        now.minute() == orariIrrigazione[i].minuto) {
      
      startIrrigazione = millis();
      durataIrrigazione = orariIrrigazione[i].durata;
      
      digitalWrite(IRRIGAZIONE_PIN, HIGH); // Accendi la pompa
      while (millis() - startIrrigazione < durataIrrigazione) {
        letturaSensori();
        inviaDati();
        delay(1000); // Attendi 1 secondo prima di rileggere i sensori
      }
      digitalWrite(IRRIGAZIONE_PIN, LOW); // Spegni la pompa
    }
  }
}

void irrigazioneAutomatica() {
  if (slave_percentuale_umidita < master_percentuale_umidita) {
    digitalWrite(IRRIGAZIONE_PIN, HIGH); // Accendi la pompa
  } else {
    digitalWrite(IRRIGAZIONE_PIN, LOW); // Spegni la pompa
  }
}

void modalitaIrrigazione() {
  if (modalita_irrigazione == 'a') {
    int new_modalita_irrigazione = 0;
    irrigazioneAutomatica();
  } else if (modalita_irrigazione == 'm') {
    int new_modalita_irrigazione = 1;
    irrigazioneManuale();
  } else {
    Serial.println("Errore modalità irrigazione.");
  }
}

void stampaLivelloUmiditaTerreno() {
  Serial.print("Livello Umidità terreno: ");
  Serial.println(slave_percentuale_umidita);
}
// Fine irrigazione
// ***********************************************************************

// ***********************************************************************
// Inizio comunicazione seriale
void inviaDati() {
  if (espSerial.available()) {
      String dati = String(livello_acqua) + "," + String(livello_batteria) + "," + String(livello_luminosita) + "," + String  (livello_temperatura_ambiente) + "," + String(livello_umidita_ambiente) + "," + String(slave_percentuale_umidita) + "," + new_modalita_irrigazione;
  espSerial.write(dati);
  }
}

void riceviValoriDaEsp() {
  while (espSerial.available()) {
    String input = espSerial.readStringUntil('\n');
    // Parse input and set variables accordingly
    // Example format: "2024-05-23T10:00:00,a,50,60000,1,14,30,20000;3,16,45,15000"
    // where each irrigation schedule is dayOfWeek,hour,minute,duration
    int separatorIndex = input.indexOf(',');
    DateTime dt = DateTime(input.substring(0, separatorIndex).c_str());
    impostaDataOra(dt);
    modalita_irrigazione = input.charAt(separatorIndex + 1);

    if (modalita_irrigazione == 'a') {
      master_percentuale_umidita = input.substring(separatorIndex + 3).toInt();
    } else if (modalita_irrigazione == 'm') {
      numOrariIrrigazione = 0;
      String scheduleString = input.substring(separatorIndex + 3);
      int scheduleStart = 0;
      while (scheduleStart < scheduleString.length()) {
        int scheduleEnd = scheduleString.indexOf(';', scheduleStart);
        if (scheduleEnd == -1) scheduleEnd = scheduleString.length();
        
        String schedule = scheduleString.substring(scheduleStart, scheduleEnd);
        int comma1 = schedule.indexOf(',');
        int comma2 = schedule.indexOf(',', comma1 + 1);
        int comma3 = schedule.indexOf(',', comma2 + 1);
        
        orariIrrigazione[numOrariIrrigazione].giornoSettimana = schedule.substring(0, comma1).toInt();
        orariIrrigazione[numOrariIrrigazione].ora = schedule.substring(comma1 + 1, comma2).toInt();
        orariIrrigazione[numOrariIrrigazione].minuto = schedule.substring(comma2 + 1, comma3).toInt();
        orariIrrigazione[numOrariIrrigazione].durata = schedule.substring(comma3 + 1).toInt();
        
        numOrariIrrigazione++;
        scheduleStart = scheduleEnd + 1;
      }
    }
  }
}
// Fine comunicazione seriale
// ***********************************************************************

// ***********************************************************************
// Inizio gestione SD
void scritturaSD() {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print("Data e Ora: ");
    dataFile.print(rtc.now().timestamp());
    dataFile.print(", Livello acqua: ");
    dataFile.print(livello_acqua);
    dataFile.print(", Livello batteria: ");
    dataFile.print(livello_batteria);
    dataFile.print(", Livello luminosità: ");
    dataFile.print(livello_luminosita);
    dataFile.print(", Temperatura ambiente: ");
    dataFile.print(livello_temperatura_ambiente);
    dataFile.print(", Umidità ambiente: ");
    dataFile.print(livello_umidita_ambiente);
    dataFile.print(", Umidità terreno: ");
    dataFile.println(slave_percentuale_umidita);
    dataFile.close();
  } else {
    Serial.println("Errore apertura file datalog.txt");
  }
}

void letturaSD() {
  File dataFile = SD.open("datalog.txt");
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  } else {
    Serial.println("Errore apertura file datalog.txt");
  }
}
// Fine gestione SD
// ***********************************************************************