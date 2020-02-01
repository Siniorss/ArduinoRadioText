#include<Keypad.h>
#include<string.h>
#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include<stdlib.h>
#include<PCD8544.h>


RF24 radio(10 , 19);
//LCD setup
PCD8544 lcd(14 , 15 , 16 , 17 , 18);
//end LCD setup


 byte Address = 0 ;
 bool addressSet = false ;


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



const char failed[] = {"Failed"};
const char statChanger = 'A';
bool sender = false;
char msg[100] ;
int ptr= 0 ;
char package[100] ; 
char delivery[] = "Delivered\0";

char screen[100] ;
int screenptr = 0 ;
int line = 0 ; 

unsigned long sendTime ;
bool undeliveredMSG = false ;
const int timeOut = 1000 ; //ms
const int failLimit = 3 ; //times
char deliveryText[] = "Delivery" ;





void setup() {
  Serial.begin(9600);
  lcd.begin(84, 48);
  clearScreen() ;
  radio.begin() ;
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(115);
  radio.setDataRate(RF24_250KBPS);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////functions//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void show(char m[]) {
  
  int i = 0 ;
  while(m[i] != '\0' && m[i] != NULL){
    screen[screenptr] = m[i] ;
    i++ ;
    screenptr++ ;
  }
  screen[screenptr] = '\0' ;  

    lcd.clear() ;
    lcd.print(screen);
}

void clearScreen(){
  for (int i = 0 ; i < 100 ; i++){
    screen[i] = '\0';
  }
  screenptr = 0 ;
  lcd.clear();
}



void send(){
  
    clearScreen();
    char givead[] = {"Address ?"};
    show(givead) ;
  
        char* key2 = (char*) malloc(1);
        *key2 = keypad.getKey() ;
        while (! *key2 || *key2 - '0' > 9 || *key2 - '0' < 0) *key2 = keypad.getKey() ;
        clearScreen() ;
        *( key2 + 1) = '\0' ;
        show(key2) ;
        char* key3 = (char*) malloc(1);
        *key3 = keypad.getKey() ;
        while (! *key3  || *key3 - '0' > 9 || *key3 - '0' < 0) *key3 = keypad.getKey() ;
        *((key3) + 1) = '\0' ;
        show(key3) ;
        byte add = ((*key2 - '0') * 10) + (*key3 - '0') ;
        if (add > 32) add = 32 ;
        if (add < 0) add = 0 ;
  
     sendTime = millis() ;
     undeliveredMSG = true ;

     for (int i = 0 ; i < 3  &&undeliveredMSG; i++){  //tries to send the msg 3times 
       radio.stopListening() ;
       radio.openWritingPipe(add);
       Serial.println(msg);
       radio.write(&msg, sizeof(msg));
       sender=false;
       
       unsigned long currentTime = millis() ;
       while (millis() - currentTime < 500){ //waits 1 second and waits for the delivery msg
        radio.openReadingPipe(1 , add);
        radio.startListening() ;
        if (radio.available()){
          radio.read(&package , sizeof(package));
          Serial.println("got this after sending the msg from the receiver : ");
          Serial.println(package); 
          if (package[0]==delivery[0]){
            undeliveredMSG = false ;
            char success[] = "Sent" ;
            clearScreen() ;
            show(success);
            break ;
          }else {
            Serial.println("got something from the receiver but it wasn't delivery !");
            clearScreen();
            char idkstring[] = "IDK man ! it's weird ! but your msg was probably sent my bad !";
            show(idkstring);
          }
        }
       }
     }
     if (undeliveredMSG){
      char failed[] = "Failed to send the message";
      clearScreen() ;
      show (failed) ;
      undeliveredMSG = false ;
     }
     deleteMessage() ;
}

void deleteMessage(){
  while(ptr > 0){
      ptr--;
      msg[ptr] = '\0' ;
   }
   ptr = 0 ;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////// LOOP //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void loop() {
  char key = keypad.getKey();
  if (key){


//////////////////////////////////////////////////////////////////////////////////stat changer//////////////////////////////////////////////////////////////////////////////////
    
    if (key == statChanger && !undeliveredMSG) {   //make sender
      Serial.println();
      Serial.println("ok what's your message ? ");
      clearScreen() ;
      char string5[] = "what's the message ?\n";
      show(string5);
      lcd.setCursor(0, 1);
      if (addressSet){
        sender = true ;
       // radio.stopListening() ;
        return ;
      }else{
        char err[] ="set the Address first" ;
        show(err) ;
        
        return ;
      }
    }


///////////////////////////////////////////////////////////////////////////////set address//////////////////////////////////////////////////////////////////////////////////

    
    else if (key == '*'){     
      clearScreen() ;
      char string1[] = "setting address : \n" ;
      show(string1);
      char* key2 = (char*) malloc(1);
      *key2 = keypad.getKey() ;
      while (! *key2 || *key2 - '0' > 9 || *key2 - '0' < 0) *key2 = keypad.getKey() ;
      *( key2 + 1) = '\0' ;
      show(key2) ;
      char* key3 = (char*) malloc(1);
      *key3 = keypad.getKey() ;
      while (! *key3  || *key3 - '0' > 9 || *key3 - '0' < 0) *key3 = keypad.getKey() ;
      *((key3) + 1) = '\0' ;
      show(key3) ;
      byte ad = ((*key2 - '0') * 10) + (*key3 - '0') ;
      Address = ad ;

      if (Address > 32) {Address = 32 ; }
      if (Address < 0 ){ Address = 0 ; }
      addressSet = true ;
      clearScreen() ;
      char string2[] = "new address has been set : " ;
      show(string2);
      char thead[3] = {(char)(Address / 10 + '0') , (char)(Address % 10 + '0') , '\0'} ;
      char spaces[] = "      " ;
      show(spaces);
      show(thead) ;
      char string4[] = "          press A to write a message" ;
      show(string4);
      return ;  
    }


//////////////////////////////////////////////////////////////////////////////////sender//////////////////////////////////////////////////////////////////////////////////

    else if (sender){
      if (key == 'C'){    //clear 1 character
        if (ptr > 0){
          ptr--;
          msg[ptr] = '\0';
          clearScreen() ;
          show(msg);
        }
     }
     else if (key == 'B'){  //clear everything
        deleteMessage() ;
        clearScreen() ;
        show(msg);
     }
     else if (key == 'D'){  //send
       send() ;
      }
       else if (ptr < 100) { //type
        msg[ptr] = key ;
        ptr++ ;
        clearScreen() ;
        show(msg) ;
        
      }
    }
  }


/////////////////////////////////////////////////////////////////////////////////receiver//////////////////////////////////////////////////////////////////////////////////

    if (!sender){
        radio.openReadingPipe(1 , Address);
        radio.startListening() ;
        if (radio.available()){
          radio.read(&package , sizeof(package));
          char notif[] = "message received       \n    " ;
          Serial.println(notif);
          Serial.println(package);
          clearScreen() ;
          show(notif);
          show(package);
          radio.stopListening() ;
          radio.openWritingPipe(Address);
          radio.write(&delivery, sizeof(delivery));
          //undeliveredMSG=false;
        }
    }
    

}
