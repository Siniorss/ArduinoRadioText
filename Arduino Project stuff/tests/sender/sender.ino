#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>


RF24 radio(12 , 13) ;
byte myAddress = 1 ;
byte herAddress = 0 ;

void setup() {
  Serial.begin(9600) ;
  radio.begin() ;
  radio.openWritingPipe(herAddress) ;
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening() ;
}

void loop() {
  char input[] = "salam golabiiiii" ; 
  radio.write(&input , sizeof(input));
  Serial.println(input);
  
}
