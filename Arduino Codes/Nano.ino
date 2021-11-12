// Date: 8.4.2018
// Drink: Nothing

#include <SPI.h>
#include "RF24.h"
#include <IRremote.h>

IRsend irsend;
RF24 radio(9, 10);
byte addresses[][6] = {"1Node", "2Node"};             // Radio pipe addresses for the 2 nodes to communicate.
uint8_t buff[16] = {0};
uint16_t code[400] = {0};
boolean sendToAC = false;
unsigned long timer = 0;
boolean flag = true;
uint8_t count = 0;
uint8_t counter = 0;
int j = 0;

void setup() {
  Serial.begin(115200);
  // Setup and configure radio

  radio.begin();

  radio.setAutoAck(true);
  radio.enableAckPayload();                     // Allow optional ack payloads
  radio.enableDynamicPayloads();                // Ack payloads are dynamic payloads

  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);

  radio.startListening();                       // Start listening

  radio.writeAckPayload(1, &counter, 1);        // Pre-load an ack-paylod into the FIFO buffer for pipe 1

  Serial.println("Begin");
}

void loop() {
  byte pipeNo, gotByte;                          // Declare variables for the pipe and the byte received

  while ( radio.available(&pipeNo)) {            // Read all available payloads
    uint8_t packSize = radio.getDynamicPayloadSize();
    radio.read( &buff, packSize );
    counter++;
    
    Serial.println();
    Serial.print("count: "); Serial.println(count);
    Serial.print("counter: "); Serial.println(counter);
    for (int i = 0; i < packSize; i++) {
      if(packSize != 1){
        //Serial.print(buff[i]); Serial.print(", ");
        radio.writeAckPayload(pipeNo,&gotByte, 1 );  // This can be commented out to send empty payloads.
        gotByte++;
        code[j] = (uint16_t)buff[i];
        code[j++] *= 100;
      }
      else{
        count = buff[0];
        Serial.print("count: "); Serial.println(count);
        radio.writeAckPayload(pipeNo,&gotByte, 1 );  // This can be commented out to send empty payloads.
        break;
      }
      Serial.print(code[j-1]); Serial.print(", ");

      if(counter == count+1)
        sendToAC = true;
      else
        sendToAC = false;
        
      }
    }
    if (sendToAC) {
      Serial.println();
      Serial.println();
      Serial.println("CODE");
      for (int i = 0; i < j; i++) {
        Serial.print(code[i]); Serial.print(", ");
      }
      irsend.sendRaw(code, sizeof(code) / sizeof(code[0]), 38); //Note the approach used to automatically calculate the size of the array.
      delay(50);
      irsend.sendRaw(code, sizeof(code) / sizeof(code[0]), 38); //Note the approach used to automatically calculate the size of the array.
      
      for (int i = 0; i < j; i++)
        code[i] = 0;

      Serial.print("j: "); Serial.println(j);
      counter = 0;
      j = 0;
      sendToAC = false;
    }

    /*while (Serial.available() > 0) {
      Serial.println();
      char letter = toupper(Serial.read());
      if (letter == 'T') {
        //      Serial.print("size: "); Serial.println(sizeof(code)/sizeof(code[0]));
        for (int i = 0; i < 400; i++) {
          Serial.print(code[i]); Serial.print(", ");
        }
        counter = 0;
      }
      if (letter == 'S') {
        irsend.sendRaw(code, sizeof(code) / sizeof(code[0]), 38); //Note the approach used to automatically calculate the size of the array.
        delay(50);
        irsend.sendRaw(code, sizeof(code) / sizeof(code[0]), 38); //Note the approach used to automatically calculate the size of the array.

        Serial.println("CODE");
        for (int i = 0; i < 400; i++) {
          Serial.print(code[i]); Serial.print(", ");
        }

        for (int i = 0; i < 400; i++)
          code[i] = 0;
        counter = 0;
      }
    }*/
}
