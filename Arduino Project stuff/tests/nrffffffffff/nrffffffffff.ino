#include<SPI.h>
#include<RF24.h>


RF24 radio(10 , 19);
byte address  = 1 ;

void setup() {
  radio.begin() ;
  Serial.begin(9600);
  radio.setChannel(115) ;
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address) ;
  delay(1000) ;

}

char text[] = "olosposkolos" ;
char deliveryText[] = "Delivery";
void loop() {
 
  bool undeliveredMSG = true ;
  for (int i = 0 ; i < 3 && undeliveredMSG ; i++){
    radio.stopListening() ;
    radio.openWritingPipe(address) ;
    radio.write(&text , sizeof(text)) ;
    //deliery expecting !
    radio.openReadingPipe(1 , address) ;
    radio.startListening() ;
    unsigned long thisTime = millis(); 
    char package[20] ;
    
//    while(!radio.available() && millis() - thisTime < 1000);
//    if (radio.available()){
//      Serial.println("radio is available");
//      while(radio.available()){
//        radio.read(&package , sizeof(package));
//        if (strcmp(package , deliveryText)){
//          undeliveredMSG = false ;
//          break ; 
//        }
//      }
//    }
    delay(500) ;

    while(true){
      if (radio.available()){
         Serial.println("babooooo");
      }
    }

  }
  if (undeliveredMSG) Serial.println("failed");
  else Serial.println("success");

}
