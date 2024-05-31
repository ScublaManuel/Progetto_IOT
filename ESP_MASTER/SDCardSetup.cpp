#include "SDCardSetup.h"




void setup_SDCard(){
  pinMode(PIN_SPI_CS, OUTPUT);
  digitalWrite(PIN_SPI_CS, HIGH);
  if (SD.begin(PIN_SPI_CS)) {
    Serial.println("SD card initialized.");
  } else {
    Serial.println("Initialization failed!");
    return;
  }
}



String csvToString(File file){
  String csvData = "";
  while (file.available()) {
    csvData += file.readStringUntil('\n');
    csvData += "\n";
  }
  file.close();
  return csvData;
}

// void parseCSV(char* format, char* filename){
//    CSV_Parser cp(/*format*/ format, true, ',');

//   // The line below (readSDfile) wouldn't work if SD.begin wasn't called before.
//   // readSDfile can be used as conditional, it returns 'false' if the file does not exist.
//   if (cp.readSDfile(filename)) {
//     float *temps =   (float*) cp["temp"];
//     float *hums =   (float*) cp["hum"];
//     float *bris =   (float*) cp["bri"];
//     float *bats =   (float*) cp["bat"];
//     float *soils =   (float*) cp["soil"];
//     float *waters =   (float*) cp["water"];
//     float *irrs =   (float*) cp["irr"];
    

//     // for(int row = 0; row < cp.getRowsCount(); row++) {
//     //   Serial.println("%d,%d,%d,%d,%d,%d,%d",temps[row],hums[row],bris[row],bats[row],soils[row],waters[row],irrs[row]);
//     // }
    
//     // output parsed values (allows to check that the file was parsed correctly)
//     cp.print(); // assumes that "Serial.begin()" was called before (otherwise it won't work)
    
//   } else {
//     Serial.println("ERROR: File called '/file.csv' does not exist...");
//   }
// }



void write_csv(String filename){
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    if (SD.exists(filename)) {
      dataFile.seek(dataFile.size()); // Move the file pointer to the end of the file
    }
    dataFile.println("Hello, world!");
    dataFile.close();
    Serial.println("Writing successfull");
  } else {
    Serial.println("Error opening the file.");
  }
}


