#include <stdint.h> 
#include <SoftwareSerial.h>

#ifndef ESPNOWSETUP_H
#define ESPNOWSETUP_H


// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    float temp;
    float hum;
    float bri;
    float bat;
    float soil;
    float water;
    float irr;
} struct_message;


extern struct_message incomingReadings;


void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len);



#endif
