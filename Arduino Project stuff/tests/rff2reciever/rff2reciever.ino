#include<SPI.h>
#include<RF24.h>


RF24 radio(10 , 19);
byte address  = 1 ;
char text[30] ;

void setup() {
  radio.begin() ;
  Serial.begin(9600);
  radio.setChannel(115) ;
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  delay(1000) ;
}

void loop(){
  if (radio.available()){
    radio.openReadingPipe(1 , address);
    radio.read(&text , sizeof(text));
    Serial.println(text);
  }
  
}
