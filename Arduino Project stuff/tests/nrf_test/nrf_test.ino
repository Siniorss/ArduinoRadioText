#include<Keypad.h>
#include<string.h>
#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include<stdlib.h>
#include<PCD8544.h>


//keyPad setup
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte colPins[COLS] = {5 , 4 , 3 ,2}; //connect to the row pinouts of the keypad
byte rowPins[ROWS] = {9 , 8 , 7 , 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//end of Keypad setup



//LCD setup
PCD8544 lcd(14 , 15 , 16 , 17 , 18);
//end LCD setup



char screen[100] ;
int screenptr = 0 ;


char msg[100] ;
int ptr = 0 ; 


//radio

  RF24 radio(10 , 19) ;

//end radio


void setup() {
  Serial.begin(9600) ;
  lcd.begin(84 , 48);

  for (int i = 0 ; i < 100 ; i++){
    msg[i] = '\0' ;
  }
  byte  address = 0 ;
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening() ;
}




void show(char m[]) {
  int i = 0 ;
  while(m[i] != '\0' && m[i] != NULL){
    screen[screenptr] = m[i] ;
    i++ ;
    screenptr++ ;
  }
  screen[screenptr] = '\0' ;
  Serial.println(screen);

  lcd.clear() ;
  lcd.print(screen);
  
}

void clearScreen(){
  for (int i = 0 ; i < 100 ; i++){
    screen[i] = '\0';
  }
  screenptr = 0 ;
  lcd.clear() ;
}


void send(){

  radio.write(&msg , sizeof(msg));
  
  clearScreen() ;
  show("SENT");
}


void loop() {
//  char key;
//  key = keypad.getKey() ;
//  if (key){
//    Serial.println("1");
//    if (key == '*') {
//      while(ptr > 0){
//        ptr-- ;
//        msg[ptr] = '\0' ;
//      }
//      clearScreen() ;
//    }
//    else if (key == 'D'){
//      send() ;
//    }
//    else {
//      Serial.println("2");
//      msg[ptr] = key ;
//      ptr++ ;
//      clearScreen() ;
//      show(msg) ;
//    }

  char text[] = "salam kalame" ;
  radio.write(&text , sizeof(text));
  delay(1000) ;
    
  }
