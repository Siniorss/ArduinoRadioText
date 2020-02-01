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

void setup() {
  Serial.begin(9600) ;
  lcd.begin(84 , 48);
}




void show(char m[]) {
  
  Serial.println() ;
  Serial.println() ;
  Serial.print("m is :   ") ;
  Serial.println(m) ;
  Serial.println() ;
  
  int i = 0 ;
  while(m[i] != '\0' && m[i] != NULL && ( (m[i] - '0' < 10 && m[i] - '0' >= 0) || m[i] == '*' || m[i] == '#') ){
    screen[screenptr] = m[i] ;
    i++ ;
    screenptr++ ;
  }
  screen[screenptr] = '\0' ;
  
  Serial.println("this is shown :");
  Serial.print(screen) ;
  Serial.println() ;

  lcd.setCursor(0 , 0);
  lcd.print(screen);
  
}

void clearScreen(){
  for (int i = 0 ; i < 100 ; i++){
    screen[i] = '\0';
  }
  screenptr = 0 ;
  lcd.clear() ;
}





void loop() {

  
  char key[2];
  key[0]= keypad.getKey() ;
  key[1] = '\0' ;
  if (key[0]){
    if (key[0] == '*') clearScreen() ;
//    else show(key);
    else {
      Serial.println(key[0]);
      show(key) ;
    }
  }
  

  

}
