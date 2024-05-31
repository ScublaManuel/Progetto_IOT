//Librerie utili per gestione SD e comunicazione seriale 
#include <SD.h>
#include <SPI.h>
//Librerie utili per maneggiare i csv
#include <CSV_Parser.h>
#include <non_arduino_adaptations.h>


#ifndef SDCARDSETUP_H
#define SDCARDSETUP_H

#define PIN_SPI_CS D8

void setup_SDCard();
String csvToString(File file);
void write_csv();

#endif